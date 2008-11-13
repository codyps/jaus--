/*==================================================================================

    Filename:  cvimagecapture.h

    Copyright 2007 Brian C. Becker
                   Daniel Barber
                   Robotics Laboratory
                   University of Central Florida
                   http://robotics.ucf.edu

    Program Contents:
    -------------------------------------------------------------------------------
    Contains the ImageCapture wrapper class for cross platform video capture.
    If you get a compile error in windows, make sure you are including this
    header file before ALL other files.
    -------------------------------------------------------------------------------

    Modifications:
    -------------------------------------------------------------------------------
    8/10/2007 - Made imnprovements and code cleanup to interface.  This involved
    more tightly integrating with the OpenCV library, merging the 
    DirectShowCapture class with the CvImageCapture class.  Also switched to
    using a Callback in DirectShow so that we do not sample the same frame
    multiple times (also increases performance).

    9/18/2007 - Make sure to use CV_CAPTURE_IMPORTS to use the DLL version of
    the library built using this file.

    6/15/2007 - Added ability to disable use of OpenCV in Windows with the
    CV_NO_OPENCV preprocessor definition. (Only works in Windows).

    7/1/2008 - Made fixes which caused incompatibilities with other libraries and
               removed need to include DirectShow/Windows files in the
               header file.  Also changed pre-processor defintions.
               You must now disable OpenCV requirement with 
               CV_IMAGE_CAPTURE_NO_OPEN_CV.
    -------------------------------------------------------------------------------

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA

==================================================================================*/
#include "cv/cvimagecapture.h"
#include <iostream>
#include <assert.h>

#ifdef WIN32

#define _WIN32_WINNT 0x400

#include <windows.h>

#include <commctrl.h>
#include <commdlg.h>
#include <tchar.h>
#include <dbt.h>

#define __EDEVDEFS__    // Don't include edevdefs.h

#include <mmreg.h>
#include <mmstream.h>
#include <xprtdefs.h>   // Include this instead of edevdefs, per MS docs
#include <dshow.h>
#include <dshowasf.h>   // Using ASF streaming AVI files
#include <qedit.h>
#include <atlbase.h>
#include <streams.h>
#include <objbase.h>
#include <initguid.h>

using namespace std;

class DirectShowCapture : protected ISampleGrabberCB
{
public:
    DirectShowCapture();
    ~DirectShowCapture();
    // Starts capturing from video source.
    int Start(const char *videoSource,
              const int width = 0,
              const int height = 0,
              const bool interlace = false);
    // Stops image capture.
    int Stop();
    int GetFrame(CvImageCapture::Image* dest, const bool block = false);
#ifndef CV_IMAGE_CAPTURE_NO_OPEN_CV
    // Get frame from video source.
    int GetFrame(IplImage*& dest, const bool block = false);
#endif
    // Get height of image in pixels.
    int GetHeight() const;
    // Get width of image in pixels.
    int GetWidth() const;
    // Check if image capture is ready (incomming data).
    bool IsReady() const;
    const char* GetSource() const;
    static unsigned int ScanForSources(std::vector<std::string>& sources);
protected:
    int InitializeVideo();
    int InitializeFile();
    static HRESULT GetVideoInputFilter( IBaseFilter** gottaFilter, wchar_t* matchName);
    static HRESULT GetVideoInputFilter( IBaseFilter** gottaFilter, const int num);    
    static HRESULT GetPin(IBaseFilter* pFilter, PIN_DIRECTION dirrequired, int iNum, IPin** ppPin);
    STDMETHODIMP_(ULONG) AddRef();
    STDMETHODIMP_(ULONG) Release();
    STDMETHODIMP QueryInterface(REFIID riid, void ** ppv);
    virtual HRESULT STDMETHODCALLTYPE SampleCB(double sampleTime,
                                               IMediaSample *pSample);    
    virtual HRESULT STDMETHODCALLTYPE BufferCB(double sampleTime,
                                               BYTE *pBuffer,
                                               long bufferLen);
    IMediaControl*			pControl;   ///<  Used by direct show for media control.
	ISampleGrabber*			pGrabber;   ///<  Used to grab frames.
    HANDLE mImageMutex;                 ///<  Mutex for thread protection.
    volatile bool mStopFlag;            ///<  Flag to stop video capture.
    volatile bool mReadyFlag;           ///<  Flag to signal capture is ready.
    volatile bool mInterlacedFlag;      ///<  Should we de-interlace the frame?
    volatile unsigned int mFrameNumber; ///<  Frame number.
    long mDataSize;                     ///<  Size of image buffer in bytes.
    int mWidth;                         ///<  Width of the image in pixels.
    int mHeight;                        ///<  Height of the image in pixels.
    int mDesWidth;                      ///<  Desired width.
    int mDesHeight;                     ///<  Desired height.
    unsigned char *mpImageBuffer;       ///<  Actual image data.
    unsigned int mGetFrameNumber;       ///<  The last frame number retrieved.
    double mFrameTimeMs;                ///<  Time of frame capture.
    std::string mSourceName;            ///<  Name of image source.
};


////////////////////////////////////////////////////////////////////////////////////
///
///   \macro SAFE_RELEASE
///   \brief Safely releases COM related pointers for DirectShow.
///
////////////////////////////////////////////////////////////////////////////////////
#define SAFE_RELEASE(pUnk)      \
        if (pUnk)               \
        {                       \
            pUnk->Release();    \
            pUnk = NULL;        \
        }	


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
DirectShowCapture::DirectShowCapture() 
{
    // Image data values.
    mWidth = mHeight = 0;
    mDataSize = 0;
    mpImageBuffer = NULL;
    mFrameNumber = 0;
    mFrameTimeMs = 0;

    mDesHeight = mDesWidth = 0;

    mImageMutex = NULL;
    mImageMutex = CreateMutex(NULL, FALSE, NULL);
    assert(mImageMutex);

    //  Flags.
    mStopFlag = true;
    mReadyFlag = false;
    mInterlacedFlag = false;

    pControl = NULL;
    pGrabber = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
DirectShowCapture::~DirectShowCapture()
{
    Stop();

    if (mImageMutex)
    {
        ReleaseMutex(mImageMutex);
        CloseHandle(mImageMutex);
        mImageMutex = 0;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Start capture from a video source.
///
///   If width or height <= 0 the largest formats upported from the video
///   source is used.  Otherwise an attempt to select the size will be
///   tried, and if the desired resolution fails, a default resolution will be
///   used.  This feature may not be supported outside of Windows and does
///   not work with video files (only cameras).
///
///   \param videoSource The String/Friendly name of the video source.
///                      If left blank ( "" ), then the first device on the
///                      system will be used.  This value is the name of a
///                      camera or a video file.  You can also use an ascii 
///                      number representing the device.  For example "0" for
///                      camera 0, "1" for camera 1, etc.
///   \param width Desired image width resolution if supported.  If <= 0 then
///                first working resolution is chosen from camera.
///   \param height Desired image height resolutoin if supported. If <= 0 then
///                first working resolution is chosen from camera.
///   \param iterlace If the video is interlaced, set to true and it will be
///                   automatically de-interlaced.
///
///   \return 1 if started, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::Start(const char *videoSource, 
                             const int width, 
                             const int height, 
                             const bool interlace) 
{
    int result = 0;

    Stop();

    USES_CONVERSION;

    CoInitializeEx( NULL, COINIT_MULTITHREADED);
    mSourceName = videoSource;
    mInterlacedFlag = interlace;
    if( mSourceName.empty() == false && GetFileAttributes( A2W(mSourceName.c_str()) ) != - 1 ) {
        result = InitializeFile();
    }
    //  Camera
    else {
        if( width > 0 && height > 0 ) {
            this->mDesHeight = height;
            this->mDesWidth = width;
        }
        else {
            mDesHeight = mDesWidth = 0;
        }
        result = InitializeVideo();
    }
    if( result == 0 ) {
        Stop();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Stops capture from video source.
///
///   \return 1 if stopped, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::Stop()
{
    mStopFlag = true;    

    if(pControl) {
        pControl->Stop();
        pControl = NULL;
    }
    if(pGrabber) {
        pGrabber = NULL;
    }

    //  Now release everything and cleanup.
    CoUninitialize();

    mSourceName.clear();
    mHeight = mWidth = 0;
    mDataSize = 0;
    mFrameNumber = 0;
    mGetFrameNumber = 0;
    mFrameTimeMs = 0;

    WaitForSingleObject( mImageMutex, INFINITE );
    if( mpImageBuffer ) {
        delete[] mpImageBuffer;
        mpImageBuffer = NULL;
    }
    ReleaseMutex( mImageMutex );

    return 1;
}

int DirectShowCapture::GetHeight() const { return mHeight; }
int DirectShowCapture::GetWidth() const { return mWidth; }
bool DirectShowCapture::IsReady() const { return mReadyFlag; }
const char *DirectShowCapture::GetSource() const { return mSourceName.c_str(); }


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the next frame from the image source.
///
///   An internal counter is kept that keeps track of what frame number was
///   last retrieved from the image source.  This counter prevents you from
///   get the same frame multiple times (over sampling).  If you set the
///   blocking flag to false, then if the next frame hasn't arrived yet, the 
///   function will return 0.
///
///   \param dest The destination (where to save image to).
///   \param block If true, then this function will block until the next
///                image arrives, otherwise not.
///
///   \return 1 if image retrieved, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::GetFrame(CvImageCapture::Image* dest, const bool block)
{
    int result = 0;
	int i, j;
	unsigned char * ptr, * dRowUp, * dRowDown;
    unsigned char *iplPtr = NULL;
    CvImageCapture::Image *image;
    
    //  Check to see if this frame has already
    //  been captured.
    if( mGetFrameNumber >= mFrameNumber ) {
        //  If block is true, wait for the next frame
        //  to arrive, otherwise exit.
        if( block ) {
            unsigned int count = 0;
            while( mGetFrameNumber >= mFrameNumber && !mStopFlag ) {
                Sleep(1);
            }
        }
        else {
            return 0;
        }
    }

    // Can't copy if input is null
    if( dest->mHeight != mHeight ||
        dest->mWidth != mWidth ||
        dest->mChannels != 3)
    {
        dest->Create((unsigned short)mWidth, (unsigned short)mHeight, 3);
    }

    image = dest;

	// pGrabber is our handle to the DirectShow "controller" if you will
	if (pGrabber && mImageMutex)
	{
        WaitForSingleObject(mImageMutex, INFINITE);

        // OK, so get the image

		if (mpImageBuffer && mDataSize)
		{	
			ptr = mpImageBuffer;
            iplPtr = (unsigned char *)(image->mpData);
            
            if( !mInterlacedFlag ) 
            {
                // Copy the data so that the top of the image is at
                // row 0.
                for(unsigned short i = 0, r = (unsigned short)(mHeight - 1); i < (unsigned short)(mHeight) && r >= 0; i++, r--)
                {
                    memcpy(&image->mpData[i*mWidth*3],
                           &mpImageBuffer[r*mWidth*3],
                           mWidth*3);
                }
            }
            else {
			    for (i = mHeight - 1; i > 0; i--)
			    {
                    ptr = (unsigned char *)(mpImageBuffer + (mHeight - i)*image->mWidth*image->mChannels);
                    iplPtr = (unsigned char *)(image->mpData + i*image->mWidth*image->mChannels);
    				
                    //  Perform interlacing so that the video appears clear.  This is
                    //  only needed if the video is DV in most cases, which arrives
                    //  interlaced. 
				    //For lines that are very top or very bottom we just copy or it is a scanline to use
                    if ((i - 1 < 0) || (i + 1 >= mHeight) || ((unsigned int)i % 2 == 0) || i==1) {
					    //Just copy
                        for (j=0; j < mWidth; j++) {
                            *iplPtr++ = *ptr++; // B
                            *iplPtr++ = *ptr++; // G
                            *iplPtr++ = *ptr++; // R
					    }
				    }
				    else
				    {
					    dRowUp = ptr - image->mWidth*image->mChannels;
					    dRowDown = ptr + image->mWidth*image->mChannels;

					    //Interpolate!!
                        for (j=0; j < mWidth; j++) {
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  B
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  G
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  R
					    }
				    }							
			    } 
            }
			result = true;	
            mGetFrameNumber = mFrameNumber;
		}

        ReleaseMutex(mImageMutex);
	}

	return result;
}

#ifndef CV_IMAGE_CAPTURE_NO_OPEN_CV
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the next frame from the image source.
///
///   An internal counter is kept that keeps track of what frame number was
///   last retrieved from the image source.  This counter prevents you from
///   get the same frame multiple times (over sampling).  If you set the
///   blocking flag to false, then if the next frame hasn't arrived yet, the 
///   function will return 0.
///
///   \param dest The destination (where to save image to).
///   \param block If true, then this function will block until the next
///                image arrives, otherwise not.
///
///   \return 1 if image retrieved, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::GetFrame(IplImage*& dest, const bool block)
{
    int result = 0;
	int i, j;
	unsigned char * ptr, * dRowUp, * dRowDown;
    unsigned char *iplPtr = NULL;
    IplImage *image;
    
    //  Check to see if this frame has already
    //  been captured.
    if( mGetFrameNumber >= mFrameNumber ) {
        //  If block is true, wait for the next frame
        //  to arrive, otherwise exit.
        if( block ) {
            unsigned int count = 0;
            while( mGetFrameNumber >= mFrameNumber && !mStopFlag ) {
                Sleep(1);
            }
        }
        else {
            return 0;
        }
    }

    // Can't copy if input is null
    if( dest == NULL ||
        dest->height != mHeight ||
        dest->width != mWidth ||
        dest->depth != IPL_DEPTH_8U ||
        dest->nChannels != 3)
    {
        if(dest)
            cvReleaseImage(&dest);
        dest = cvCreateImage(cvSize(mWidth, mHeight), IPL_DEPTH_8U, 3);
    }

    image = dest;
    image->origin = IPL_ORIGIN_TL;   
    if(image->roi)
        cvResetImageROI(dest);

	// pGrabber is our handle to the DirectShow "controller" if you will
	if (pGrabber && mImageMutex)
	{
        WaitForSingleObject(mImageMutex, INFINITE);

        // OK, so get the image

		if (mpImageBuffer && mDataSize)
		{	
			ptr = mpImageBuffer;
            iplPtr = (unsigned char *)(image->imageData);
            
            if( !mInterlacedFlag ) {
                memcpy( image->imageData, mpImageBuffer, mDataSize );
                cvFlip( image, image, 0 );
                image->origin = IPL_ORIGIN_TL;
            }
            else {
			    for (i = mHeight - 1; i > 0; i--)
			    {
                    ptr = (unsigned char *)(mpImageBuffer + (mHeight - i)*image->widthStep);
                    iplPtr = (unsigned char *)(image->imageData + i*image->widthStep);
    				
                    /*  Perform interlacing so that the video appears clear.  This is
                        only needed if the video is DV in most cases, which arrives
                        interlaced. */
				    //For lines that are very top or very bottom we just copy or it is a scanline to use
                    if ((i - 1 < 0) || (i + 1 >= mHeight) || ((unsigned int)i % 2 == 0) || i==1) {
					    //Just copy
                        for (j=0; j < mWidth; j++) {
                            *iplPtr++ = *ptr++; // B
                            *iplPtr++ = *ptr++; // G
                            *iplPtr++ = *ptr++; // R
					    }
				    }
				    else
				    {
					    dRowUp = ptr - image->widthStep;
					    dRowDown = ptr + image->widthStep;

					    //Interpolate!!
                        for (j=0; j < mWidth; j++) {
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  B
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  G
						    *iplPtr++ = (*dRowUp++ + *dRowDown++) / 2;  //  R
					    }
				    }							
			    } 
            }
			result = true;	
            mGetFrameNumber = mFrameNumber;
		}

        ReleaseMutex(mImageMutex);
	}

	return result;
}

#endif

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes capture from a video source, (USB or Firewire DV Camera).
///
///   \return 1 if success, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::InitializeVideo()
{
    int result = 0;

    USES_CONVERSION;

    // COM variables to capture live video
	CComPtr <ICaptureGraphBuilder2>		pCaptureGraph;
	CComPtr <IGraphBuilder>				pGraph;
	CComPtr <IBaseFilter>				pVideoInputFilter;
	CComPtr <ISampleGrabber>			pGrabber;
	CComPtr <IAMStreamConfig>			pConfig;
	CComPtr <IAMVideoProcAmp>			pProcAmp;
    HRESULT hr; //  Used to store results of different operations.

    //  Create the filter graph manager and query for video interfaces
    hr = CoCreateInstance( CLSID_CaptureGraphBuilder2, 
                           NULL, 
                           CLSCTX_INPROC_SERVER,
                           IID_ICaptureGraphBuilder2, 
                           (void **)&pCaptureGraph );
    if( FAILED(hr) ) {
        return result;
    }

    //  Now create a filter graph (this will do all encoding/decoding of data from camera.
    CoCreateInstance( CLSID_FilterGraph, 
                      NULL, 
                      CLSCTX_INPROC_SERVER,
                      IID_IFilterGraph,
                      (void **)&pGraph );
    if( !pGraph ) {
        return result;
    }

    //  Now get the fitler graph manager, and give the filter graph.
    hr = pCaptureGraph->SetFiltergraph( pGraph );
    if( FAILED(hr) ) {
        return result;
    }

    //  Using QueryInterface on the graph builder, get the Media Control object.
    hr = pGraph->QueryInterface(IID_IMediaControl, (void **)&pControl);
    if( FAILED(hr) ) {
        return result;
    }
    
    //  If the string is a number, then use it instead.
    bool isNumber = true;
    for(unsigned int i = 0; i < mSourceName.size(); i++) {
        if( isdigit( mSourceName.c_str()[i] ) == 0 ) {
            isNumber = false;
            break;
        }
    }
    if( isNumber )  {
        hr = GetVideoInputFilter( &pVideoInputFilter, atoi( mSourceName.c_str() ) );
    }
    else  {
        // Now create the video input filter from the camera name.
        hr = GetVideoInputFilter( &pVideoInputFilter, A2W( mSourceName.c_str() ) );
    }
    if( FAILED( hr ) ) {
        return result;
    }

    //  If we succeeded, add filter to our input filter graph.
    hr = pGraph->AddFilter(pVideoInputFilter, A2W( mSourceName.c_str() ) );
   
    //  Create the frame grabber.
    hr = pGrabber.CoCreateInstance( CLSID_SampleGrabber );
    if( !pGrabber ) {
        return result;
    }

    //  Get the frame grabber and add to the grabber base.
    CComQIPtr <IBaseFilter, &IID_IBaseFilter> pGrabberBase(pGrabber);
    hr = pGraph->AddFilter( pGrabberBase, L"Grabber" );

    //  Tell the grabber to grab 24-bit video.  Must do this before connecting.
    CMediaType GrabType;
    GrabType.SetType( &MEDIATYPE_Video );
    GrabType.SetSubtype( &MEDIASUBTYPE_RGB24 );
    hr = pGrabber->SetMediaType( &GrabType );
    if( FAILED(hr) ) {
        return result;
    }
    //  Tell the grabber to grab certain resolution.  Must do this before connecting.
    hr = pCaptureGraph->FindInterface(&PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video,
                                      pVideoInputFilter,
                                      IID_IAMStreamConfig,
                                      (void **)&pConfig);
    if( FAILED( hr ) ) {
        return result;
    }

    AM_MEDIA_TYPE *theFormat = NULL;
    pConfig->GetFormat( &theFormat );

    VIDEOINFOHEADER *pvi = NULL;
    //  Create the format type and resolution
    if( theFormat->formattype == FORMAT_VideoInfo ) {
        
        pvi = (VIDEOINFOHEADER*)theFormat->pbFormat;
        hr = !S_OK;

        if( this->mDesWidth > 0 && this->mDesHeight > 0 ) {
            pvi->bmiHeader.biWidth = mDesWidth;
            pvi->bmiHeader.biHeight = mDesHeight;
            hr = pConfig->SetFormat(theFormat);
        }

        if( hr != S_OK ) {
            //  Figure out the resolution of the video data.
            int widthArray[] = {1920, 1680, 1600, 1440, 1280, 1024, 800, 720, 640, 320, 160 };
            int heightArray[] = {1200, 1080, 1050, 1200, 1024, 900, 800, 720, 768, 600, 480, 480, 240, 120 };
            for(int i = 0; i < 11 && hr != S_OK; i++) {
                for(int j = 0; j < 14 && hr != S_OK; j++) {
                    pvi->bmiHeader.biWidth = widthArray[i];
	                pvi->bmiHeader.biHeight = heightArray[j];
                    hr = pConfig->SetFormat(theFormat);
                    if(hr == S_OK)
                        break;
                }
            }
        }
        if (hr != S_OK) {
            mReadyFlag = false;
            Stop();
            return result;
		}
    }

    // Save height/width data.
    mHeight = pvi->bmiHeader.biHeight;
    mWidth = pvi->bmiHeader.biWidth;

    //  Allocate memory, but do it within critical section.
    WaitForSingleObject( mImageMutex, INFINITE );
    if( mpImageBuffer ) {
        delete[] mpImageBuffer;
        mpImageBuffer = NULL;
    }
    mDataSize = mHeight*mWidth*3;
    mpImageBuffer = new unsigned char[ mDataSize ];
    //  Leave critical section of memory.
    ReleaseMutex( mImageMutex );

    hr = pCaptureGraph->FindInterface(&PIN_CATEGORY_CAPTURE,
                                      &MEDIATYPE_Video,
                                      pVideoInputFilter,
                                      IID_IAMVideoProcAmp,
                                      (void **)&pProcAmp);

    //  Attempt to connect the pins.        
    CComPtr<IPin> pSourcePin=0;
    CComPtr<IPin> pGrabPin;
    
    GetPin( pVideoInputFilter,PINDIR_OUTPUT, 0, &pSourcePin );
    GetPin( pGrabberBase, PINDIR_INPUT, 0, &pGrabPin );

    hr = pGraph->Connect( pSourcePin, pGrabPin );
    if( FAILED(hr) ) {
        return result;
    }

    //  Turn AutoShow off.
    CComQIPtr <IVideoWindow, &IID_IVideoWindow> pWindow = pGraph;
    if (pWindow) {
        hr = pWindow->put_AutoShow(OAFALSE);
	}

	// Ask for the connection media type so we know its size
	AM_MEDIA_TYPE mmt;
	hr = pGrabber->GetConnectedMediaType(&mmt);

	VIDEOINFOHEADER * mvih = (VIDEOINFOHEADER*) mmt.pbFormat;
	FreeMediaType(mmt);

	hr = pGrabber->SetBufferSamples(FALSE);
	
	// Grab as a sequence of frames, not just one single image
	hr = pGrabber->SetOneShot(FALSE);

    pGrabber->SetCallback(this, 1); // 1 as second arg indicates BufferCB(),
                                    // 2 indicates SampleCB().

    // If all the above junk went through, we are about ready to actually start capturing
    if( SUCCEEDED( hr ) ) {
        // Activate the threads
		CComQIPtr <IMediaControl, &IID_IMediaControl> pControl(pGraph);
        // Run the graph filter.
        hr = pControl->Run();
        if( SUCCEEDED( hr ) ) {
            
            //  Wait for the graph to settle
            CComQIPtr<IMediaEvent, &IID_IMediaEvent> pEvent( pGraph );
            long EvCode = 0;
            //  Don't rush video capture.
            hr = pEvent->WaitForCompletion( INFINITE , &EvCode );

            mStopFlag = false;
            //  Wait until ready, but don't wait forever.
            for(unsigned int i = 0; i < 5000; i++) {
                if(mReadyFlag) 
                    break;
                Sleep(1);
            }

            this->pGrabber = pGrabber;
            result = 1;
        }
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes capture from a file video source.
///
///   \return 1 if success, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int DirectShowCapture::InitializeFile()
{
    int result = 0;

    CComPtr <IGraphBuilder>				pGraph;
    CComPtr <IBaseFilter>               pGrabberFilter;
    CComPtr <IBaseFilter>               pSrcFilter;
    CComPtr <IBaseFilter>               pNullRenderer;
    //CComPtr <IMediaControl>             pControl;
    CComPtr <IMediaEventEx>             pEvent;
    CComPtr <IMediaSeeking>             pSeeking;
    CComPtr <IMediaFilter>              pMediaFilter;
    CComPtr <ISampleGrabber>            pGrabber;
    

    HRESULT hr;

    //  Initialize interfaces
    hr = CoCreateInstance(CLSID_FilterGraph, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,
                          IID_IGraphBuilder, 
                          reinterpret_cast<void **>(&pGraph));
    if(FAILED(hr)) {
        return result;
    }
    
    hr = CoCreateInstance(CLSID_SampleGrabber, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,
                          IID_IBaseFilter, 
                          reinterpret_cast<void **>(&pGrabberFilter));
    
    hr = pGrabberFilter->QueryInterface(IID_ISampleGrabber,
                                       (void **)(&pGrabber));
    if(FAILED(hr)) {
        return result;
    }

    hr = pGraph->AddFilter(pGrabberFilter, L"SampleGrabber");
    if(FAILED(hr)) {
        return result;
    }
    
    //  Set the media type
    AM_MEDIA_TYPE mt;
    ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
    mt.formattype = FORMAT_VideoInfo;
    mt.majortype = MEDIATYPE_Video;
    mt.subtype = MEDIASUBTYPE_RGB24;    //  Only accept 24-bit video bitmaps
    hr = pGrabber->SetMediaType(&mt);
    if(FAILED(hr)) {
        return result;
    }

    //  Create the src filter
    wchar_t *strFilename;
    strFilename = new wchar_t[mSourceName.size() + 1];
    memset( strFilename, 0, sizeof(wchar_t)*(mSourceName.size() + 1));
    MultiByteToWideChar(CP_ACP, 0, mSourceName.c_str(), -1, strFilename, 256);
    
    hr = pGraph->AddSourceFilter( strFilename, L"Source", &pSrcFilter);
    delete[] strFilename;
    if(FAILED(hr)) {
        return result;
    }

    //  Attempt to connect the pins.        
    CComPtr<IPin> pGrabPin;
    CComPtr<IPin> pSourcePin;
    
    GetPin( pSrcFilter, PINDIR_OUTPUT, 0, &pSourcePin );
    GetPin( pGrabberFilter, PINDIR_INPUT, 0, &pGrabPin );
    hr = pGraph->Connect( pSourcePin, pGrabPin );
    if(FAILED(hr)) {
        return result;
    }
    
    // Create the NULL renderer and connect
	hr = CoCreateInstance(CLSID_NullRenderer, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,
		                  IID_IBaseFilter, 
                          (void **)(&pNullRenderer));
	hr = pGraph->AddFilter(pNullRenderer, L"NullRenderer");

    //  Attempt to connect the pins.        
    CComPtr<IPin> pGrabPin2;
    CComPtr<IPin> pNullPin;

    GetPin( pGrabberFilter, PINDIR_OUTPUT, 0, &pNullPin );
    GetPin( pNullRenderer, PINDIR_INPUT, 0, &pGrabPin2 );
    pGraph->Connect( pNullPin, pGrabPin2 );
    if( FAILED (hr ) ) {
        return result;
    }

    // Necessary interfaces for controlling
	pGraph->QueryInterface(IID_IMediaFilter, (void **)(&pMediaFilter));
	pGraph->QueryInterface(IID_IMediaControl, (void **)(&this->pControl));
	pGraph->QueryInterface(IID_IMediaEvent, (void **)(&pEvent));
	pGraph->QueryInterface(IID_IMediaSeeking, (void **)(&pSeeking));

	// Retrieve the actual media type
	ZeroMemory(&mt, sizeof(mt));
	hr = pGrabber->GetConnectedMediaType(&mt);
	VIDEOINFOHEADER *pVih;
	if (mt.formattype == FORMAT_VideoInfo) 
		pVih = reinterpret_cast<VIDEOINFOHEADER*>(mt.pbFormat);
    else {
		return result; // Something went wrong, perhaps not appropriate media type
	}
    //  Get height and width information for video
    mHeight = pVih->bmiHeader.biHeight;
    mWidth = pVih->bmiHeader.biWidth;

    //  Allocate memory for image capture buffer.
    WaitForSingleObject(mImageMutex, INFINITE);
    if(mpImageBuffer) {
        delete[] mpImageBuffer;
        mpImageBuffer = NULL;
    }
    mDataSize = mHeight*mWidth*3;
    mpImageBuffer = new unsigned char[mDataSize];
    ReleaseMutex(mImageMutex);


    // Free the media type
    FreeMediaType(mt);

    hr = pGrabber->SetBufferSamples(TRUE);
    hr = pGrabber->SetOneShot(FALSE);

    pGrabber->SetCallback(this, 1); // 1 as second arg indicates BufferCB(),
                                    // 2 indicates SampleCB().
    if(SUCCEEDED(hr))
    {
        CComQIPtr <IMediaControl, &IID_IMediaControl> pControl(pGraph);
        
        hr = this->pControl->Run(); // Run the graph to start the analyzing process!
        if( SUCCEEDED(hr) )
        {
            this->pGrabber = pGrabber;
            pGrabber = NULL;

            long evCode = 0;
            mStopFlag = false;
            mReadyFlag = true;
            result = 1;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Enumerate all of the video input devices.  
///
///  \param gottaFilter Input filter pointer.
///  \param matchName The friendly name of the device (camera name) to
///         match to.
///
///  \return The filter with a matching friendly name.
///
////////////////////////////////////////////////////////////////////////////////////
HRESULT DirectShowCapture::GetVideoInputFilter(IBaseFilter** gottaFilter, wchar_t* matchName)
{
	BOOL done = false;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
                                  NULL, 
                                  CLSCTX_INPROC_SERVER,
		                          IID_ICreateDevEnum, 
                                  (void **)&pSysDevEnum);
    if (FAILED(hr)) {
		return hr;
	}

	// Obtain a class enumerator for the video input category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
                                            &pEnumCat, 
                                            0);    
	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while ((pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) && (!done))
		{
			// Bind the first moniker to an object
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					// Do a comparison, find out if it's the right one
					if (wcsncmp(varName.bstrVal, matchName, 
						wcslen(matchName)) == 0) {

						// We found it, so send it back to the caller
						hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**) gottaFilter);
						done = true;
					}
				}
				VariantClear(&varName);	
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	if (done) {
		return hr;	// found it, return native error
	} 
    else {
		return VFW_E_NOT_FOUND;	// didn't find it error
	}
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Enumerate all of the video input devices.  
///
///  \param gottaFilter Input filter pointer.
///  \param num The video device number to get input filter for.  This is
///             [0, num video sources].
///
///  \return The filter with a matching friendly name.
///
////////////////////////////////////////////////////////////////////////////////////
HRESULT DirectShowCapture::GetVideoInputFilter(IBaseFilter** gottaFilter, const int num)
{
	BOOL done = false;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	HRESULT hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
                                  NULL, 
                                  CLSCTX_INPROC_SERVER,
		                          IID_ICreateDevEnum, 
                                  (void **)&pSysDevEnum);
    if (FAILED(hr)) {
		return hr;
	}

	// Obtain a class enumerator for the video input category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
                                            &pEnumCat, 
                                            0);    
    int count = 0;
	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while ((pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK) && (!done))
		{
			// Bind the first moniker to an object
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, 
				(void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr) && count == num)
				{
                    hr = pMoniker->BindToObject(NULL, NULL, IID_IBaseFilter, (void**) gottaFilter);
					done = true;
				}
                count++;
				VariantClear(&varName);	
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release();
	if (done) {
		return hr;	// found it, return native error
	} 
    else {
		return VFW_E_NOT_FOUND;	// didn't find it error
	}
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Get a pin in the the graph.
///
///  \param pFilter The filter.
///  \param dirrequired The pin directoin required.
///  \param iNum Input number.
///  \param ppPin Pointer to pin.
///
///  \return Result < 0 is failure, >= 0 is success.
///
////////////////////////////////////////////////////////////////////////////////////
HRESULT DirectShowCapture::GetPin(IBaseFilter* pFilter, PIN_DIRECTION dirrequired, int iNum, IPin **ppPin)
{
    CComPtr< IEnumPins > pEnum;
    *ppPin = NULL;

    HRESULT hr = pFilter->EnumPins(&pEnum);
    if(FAILED(hr)) 
        return hr;

    ULONG ulFound;
    IPin *pPin;
    hr = E_FAIL;

    while(S_OK == pEnum->Next(1, &pPin, &ulFound))
    {
        PIN_DIRECTION pindir = (PIN_DIRECTION)3;

        pPin->QueryDirection(&pindir);
        if(pindir == dirrequired)
        {
            if(iNum == 0)
            {
                *ppPin = pPin;  // Return the pin's interface
                hr = S_OK;      // Found requested pin, so clear error
                break;
            }
            iNum--;
        } 

        pPin->Release();
    } 

    return hr;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function is part of the callbacks used by ISampleGrabberCB.
///
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) DirectShowCapture::AddRef() 
{ 
    return 2;   //  Return default value.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function is part of the callbacks used by ISampleGrabberCB.
///
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP_(ULONG) DirectShowCapture::Release() 
{ 
    return 1;   //  Return default value.
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function is part of the callbacks used by ISampleGrabberCB.
///
////////////////////////////////////////////////////////////////////////////////////
STDMETHODIMP DirectShowCapture::QueryInterface(REFIID riid, void ** ppv)
{
    if( ppv == NULL ) {
        return E_POINTER;
    }
    
    if( riid == IID_ISampleGrabberCB || riid == IID_IUnknown ) 
    {
        *ppv = (void *) static_cast<ISampleGrabberCB*> ( this );
        return NOERROR;
    }    

    return E_NOINTERFACE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function is part of the callbacks used by ISampleGrabberCB.
///
////////////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE DirectShowCapture::SampleCB(double SampleTime,
                                                   IMediaSample *pSample)
{
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
///
///   \brief The implemented callback function.  Gets latest frame
///   from the video feed.
///
//////////////////////////////////////////////////////////////////////////////
HRESULT STDMETHODCALLTYPE DirectShowCapture::BufferCB(double sampleTime,
                                                   BYTE *pBuffer,
                                                   long bufferLen)
{
    //  Verify we have received valid image data.
    if(pBuffer && bufferLen &&  mImageMutex) {
        //  Enter critical section 
        WaitForSingleObject(mImageMutex, INFINITE);
        // Make sure we can fit the data into our buffer
        if(bufferLen >= mDataSize) {
            if(!mpImageBuffer) {
                mpImageBuffer = new unsigned char[mDataSize];
                assert(mpImageBuffer);
            }
            //  Copy data.
            memcpy(mpImageBuffer, pBuffer, mDataSize);
            mFrameNumber++;
            mFrameTimeMs= (unsigned int)(sampleTime*1000.0);
            mReadyFlag = true;
        }
        //  Leave critical section
        ReleaseMutex(mImageMutex);
        //cout << "Frame\n";
    }
    return S_OK;
}


//////////////////////////////////////////////////////////////////////////////
///
///   \brief Get a list of video sources connected to the computer (cameras).
///
///   \param sources Copy of source names.
///
///   \return The number of video sources connected to the computer.
///
//////////////////////////////////////////////////////////////////////////////
unsigned int DirectShowCapture::ScanForSources(std::vector<std::string>& sources)
{
    if(sources.size() > 0 )
        sources.clear();

    // Initialize DirectShow enough so we can enumerate the video soruces
	CoInitializeEx(NULL, COINIT_MULTITHREADED);

	USES_CONVERSION;
	
	// COM junk variables to capture live video
	CComPtr <ICaptureGraphBuilder2>		pCaptureGraph;
	CComPtr <IGraphBuilder>				pGraph;
	CComPtr <IMediaControl>				pControl;
	CComPtr <IBaseFilter>				pVideoInputFilter;
	CComPtr <ISampleGrabber>			pGrabber;
	CComPtr <IAMStreamConfig>			pConfig;
	CComPtr <IAMVideoProcAmp>			pProcAmp;

	HRESULT hr;

	// Create the filter graph manager and query for interfaces.
	hr = CoCreateInstance(CLSID_CaptureGraphBuilder2, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,
						  IID_ICaptureGraphBuilder2, 
                          (void **)&pCaptureGraph);

	if (FAILED(hr))
	    return 0;

	// Create filter graph itself
	CoCreateInstance(CLSID_FilterGraph, 
                     NULL, 
                     CLSCTX_INPROC_SERVER, 
                     IID_IFilterGraph, 
                     (void **)&pGraph );
	if (!pGraph)
        return 0;

	// Now get the filter graph manager. That's part of the capture graph builder
	hr = pCaptureGraph->SetFiltergraph(pGraph);

	if (FAILED(hr))
	    return 0;

	// Using QueryInterface on the graph builder, get the Media Control object.
	hr = pGraph->QueryInterface(IID_IMediaControl, 
                                (void **)&pControl);

	// No syncronization needed so disable clock
	IMediaFilter *pMediaFilter = 0;
	pGraph->QueryInterface(IID_IMediaFilter, 
                          (void**)&pMediaFilter);
	pMediaFilter->SetSyncSource(NULL);
	pMediaFilter->Release();

	if (FAILED(hr))
	    return 0;

	// Create the System Device Enumerator.
	ICreateDevEnum *pSysDevEnum = NULL;
	hr = CoCreateInstance(CLSID_SystemDeviceEnum, 
                          NULL, 
                          CLSCTX_INPROC_SERVER,
		                  IID_ICreateDevEnum, 
                          (void **)&pSysDevEnum);
    if (FAILED(hr)) {
		return 0;
	}

	// Obtain a class enumerator for the video input category.
	IEnumMoniker *pEnumCat = NULL;
	hr = pSysDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory, 
                                            &pEnumCat, 
                                            0);
	if (hr == S_OK) 
	{
		// Enumerate the monikers.
		IMoniker *pMoniker = NULL;
		ULONG cFetched;
		while ((pEnumCat->Next(1, &pMoniker, &cFetched) == S_OK))
		{
			// Bind the first moniker to an object
			IPropertyBag *pPropBag;
			hr = pMoniker->BindToStorage(0, 
                                         0, 
                                         IID_IPropertyBag, 
                                         (void **)&pPropBag);
			if (SUCCEEDED(hr))
			{
				// To retrieve the filter's friendly name, do the following:
				VARIANT varName;
				VariantInit(&varName);
				hr = pPropBag->Read(L"FriendlyName", &varName, 0);
				if (SUCCEEDED(hr))
				{
					USES_CONVERSION;
					sources.push_back(W2A(varName.bstrVal));
				}
				VariantClear(&varName);	
				pPropBag->Release();
			}
			pMoniker->Release();
		}
		pEnumCat->Release();
	}
	pSysDevEnum->Release(); 

	return (unsigned int)(sources.size());
}


CvImageCapture::Image::Image() : mWidth(0),
                                 mHeight(0),
                                 mChannels(0),
                                 mpData(0)
{

}

CvImageCapture::Image::Image(const Image& img) : mWidth(0),
                                                 mHeight(0),
                                                 mChannels(0),
                                                 mpData(0)
{
    *this = img;
}


CvImageCapture::Image::~Image()
{
    if(mpData)
    {
        delete[] mpData;
        mpData = NULL;
    }
}

CvImageCapture::Image& CvImageCapture::Image::operator =(const CvImageCapture::Image& img)
{
    if(this != &img)
    {
        if(mpData)
        {
            delete[] mpData;
            mpData = NULL;
        }
        mWidth = img.mWidth;
        mHeight = img.mHeight;
        mChannels = img.mChannels;
        if(mWidth > 0 && mHeight > 0 && mChannels > 0)
        {
            mpData = new unsigned char[mWidth*mHeight*mChannels];
            memcpy(mpData, img.mpData, mWidth*mHeight*mChannels);
        }
    }
    return *this;
}

void CvImageCapture::Image::Destroy()
{
    if(mpData)
    {
        delete[] mpData;
        mpData = NULL;
    }
    mWidth = mHeight = mChannels = 0;
}

void CvImageCapture::Image::Create(const unsigned short width,
                                   const unsigned short height,
                                   const unsigned short channels)
{
    if(mpData)
    {
        delete[] mpData;
        mpData = NULL;
    }
    mWidth = width;
    mHeight = height;
    mChannels = channels;
    if(mWidth > 0 && mHeight > 0 && mChannels > 0)
    {
        mpData = new unsigned char[mWidth*mHeight*mChannels];
        memset(mpData, 0, mWidth*mHeight*mChannels);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CvImageCapture::CvImageCapture()
{
    mpDirectShow = new DirectShowCapture();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CvImageCapture::~CvImageCapture()
{
    if(mpDirectShow)
    {
        delete mpDirectShow;
        mpDirectShow = NULL;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Starts capture.
///
///   If width or height <= 0 the largest formats upported from the video
///   source is used.  Otherwise an attempt to select the size will be
///   tried, and if the desired resolution fails, a default resolution will be
///   used.  This feature may not be supported outside of Windows and does
///   not work with video files (only cameras).
///
///   \param videoSource The name video source.  If this is a camera, use the
///                      ASCII equivalent of the source number:  ("0", "1", etc.).
///                      For video simply put the filename.
///   \param width Desired image width resolution if supported.  If <= 0 then
///                first working resolution is chosen from camera.
///   \param height Desired image height resolutoin if supported. If <= 0 then
///                first working resolution is chosen from camera.
///   \param interlace Has no affect in linux.
///
///   \return 1 on OK, 0 on fail.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::Start(const char *videoSource,
                          const int width,
                          const int height,
                          const bool interlace)
{
    return mpDirectShow->Start(videoSource, width, height, interlace);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Stops capture.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::Stop()
{
    return mpDirectShow->Stop();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the latest frame from video source.
///   
///   \param image Where to copy image to.
///   \param block If true, blocks until next image retrieved.
///
///   \return 1 on OK, 0 on fail.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetFrame(CvImageCapture::Image* dest, const bool block)
{
    return mpDirectShow->GetFrame(dest, block);
}

#ifndef CV_IMAGE_CAPTURE_NO_OPEN_CV
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the latest frame from video source.
///   
///   \param image Where to copy image to.
///   \param block If true, blocks until next image retrieved.
///
///   \return 1 on OK, 0 on fail.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetFrame(IplImage*& dest, const bool block)
{
    return mpDirectShow->GetFrame(dest, block);
}

#endif

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Size of images height in pixels.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetHeight() const
{
    return mpDirectShow->GetHeight();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Size of images width in pixels.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetWidth() const
{
    return mpDirectShow->GetWidth();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if capture from video source is ready.
///
////////////////////////////////////////////////////////////////////////////////////
bool CvImageCapture::IsReady() const
{
    return mpDirectShow->IsReady();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Camera/Visual Sensor source.
///
////////////////////////////////////////////////////////////////////////////////////
const char* CvImageCapture::GetSource() const
{
    return mpDirectShow->GetSource();
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Get a list of video sources connected to the computer (cameras).
///
///   \param sources Copy of source names.
///
///   \return The number of video sources connected to the computer.
///
//////////////////////////////////////////////////////////////////////////////
unsigned int CvImageCapture::ScanForSources(std::vector<std::string>& sources)
{
    return DirectShowCapture::ScanForSources(sources);
}

#else

#include <stdio.h>
#include <string.h> 


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CvImageCapture::CvImageCapture()
{
    mImage = NULL;
    mCapture = NULL;
    mSources = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CvImageCapture::~CvImageCapture()
{
    Stop();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Starts capture.
///
///   If width or height <= 0 the largest formats upported from the video
///   source is used.  Otherwise an attempt to select the size will be
///   tried, and if the desired resolution fails, a default resolution will be
///   used.  This feature may not be supported outside of Windows and does
///   not work with video files (only cameras).
///
///   \param videoSource The name video source.  If this is a camera, use the
///                      ASCII equivalent of the source number:  ("0", "1", etc.).
///                      For video simply put the filename.
///   \param width Desired image width resolution if supported.  If <= 0 then
///                first working resolution is chosen from camera.
///   \param height Desired image height resolutoin if supported. If <= 0 then
///                first working resolution is chosen from camera.
///   \param interlace Has no affect in linux.
///
///   \return 1 on OK, 0 on fail.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::Start(const char *videoSource, 
                          const int width, 
                          const int height, 
                          const bool interlace) 
    Stop();

    if( videoSource && (mCapture = cvCaptureFromFile(videoSource)) > 0) {
        mSourceName = videoSource;
        return 1;
    }
    else if(videoSource && (mCapture = cvCreateCameraCapture(atoi(videoSource))) > 0) {
        mSourceName = videoSource;
        return 1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Stops capture.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::Stop()
{
    if(mCapture)
    {
        cvReleaseCapture(&mCapture);
        mSourceName.clear();
        return 1;
    }

    return 1;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the latest frame from video source.
///   
///   \param image Where to copy image to.
///   \param block If true, blocks until next image retrieved.
///
///   \return 1 on OK, 0 on fail.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetFrame(IplImage *&image, const bool block)
{
    if(!mCapture)
        return false;

    if( (mImage = cvQueryFrame(mCapture)) )
    {
        if( !image                          ||
            image->width != mImage->width   ||
            image->height != mImage->height ||
            image->depth != mImage->depth   ||
            image->roi != NULL              ||
            image->nChannels != mImage->nChannels)
        {
            cvReleaseImage(&image);
            image = cvCreateImage(cvSize(mImage->width, mImage->height),
                                  mImage->depth,
                                  mImage->nChannels);
            if(!image)
            {
                return 0;
            }
        }
        //  Make sure origin is in the top left corner
        if(mImage->origin != IPL_ORIGIN_TL)
        {
            cvFlip(mImage, image, 0);
        }
        else
        {
            cvCopyImage(mImage, image);
        }
        return 1;
    }
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Size of images height in pixels.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetHeight() const
{
    return mImage ? mImage->height : 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Size of images width in pixels.
///
////////////////////////////////////////////////////////////////////////////////////
int CvImageCapture::GetWidth() const
{
    return mImage ? mImage->width : 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if capture from video source is ready.
///
////////////////////////////////////////////////////////////////////////////////////
bool CvImageCapture::IsReady() const
{
    return mCapture ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Scans for camera sources to connect to, not fully supported in Linux
///   as it is in Windows.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int CvImageCapture::ScanForSources(std::vector<std::string>& sources)
{
    return 0;
}
#endif

/*  End of File */
