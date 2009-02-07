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
#ifndef _CV_IMAGE_CAPTURE_H
#define _CV_IMAGE_CAPTURE_H


#if WIN32

#ifdef CV_IMAGE_CAPTURE_EXPORT
    #define CV_IMAGE_CAPTURE_DLL __declspec(dllexport)
    #pragma warning( disable : 4251)
#elif defined(CV_IMAGE_CAPTURE_IMPORT)
    #define CV_IMAGE_CAPTURE_DLL __declspec(dllimport)
    #pragma warning( disable : 4251)
    #ifdef _DEBUG
    #pragma comment( lib, "cvcapture_1.1_dll_d.lib" )
    #else
    #pragma comment( lib, "cvcapture_1.1_dll.lib" )
    #endif
#else
    #ifdef _DEBUG
    #pragma comment( lib, "cvcapture_1.1_d.lib" )
    #else
    #pragma comment( lib, "cvcapture_1.1.lib" )
    #endif
    #define CV_IMAGE_CAPTURE_DLL
#endif

#include <vector>

#ifndef CV_IMAGE_CAPTURE_NO_OPEN_CV
#include <cv.h>
#pragma comment( lib, "cv.lib" )
#pragma comment( lib, "cxcore.lib" )
#endif

class DirectShowCapture;

////////////////////////////////////////////////////////////////////////////////////
///
///   \class CvImageCapture
///   \brief Video Capture interface that supports USB/Firewire DV cameras and
///   video files.
///
///   Required Libraries:
///   Windows: DirectX 9 SDK, Windows Platform SDK (DirectShow).
///            To disable support for OpenCV use the preprocessor definition
///            CV_IMAGE_CAPTURE_NO_OPEN_CV
///   Linux: Intel OpenCV
///
////////////////////////////////////////////////////////////////////////////////////
class CV_IMAGE_CAPTURE_DLL CvImageCapture
{
public:
    class CV_IMAGE_CAPTURE_DLL Image
    {
    public:
        Image();
        Image(const Image& img);
        ~Image();
        Image& operator=(const Image& img);
        void Destroy();
        void Create(const unsigned short width,
                    const unsigned short height,
                    const unsigned short channels);
        unsigned short mWidth;    ///<  Image width.
        unsigned short mHeight;   ///<  Image height.
        unsigned short mChannels; ///<  Number of channels.
        unsigned char* mpData;    ///<  Image data.
    };
    CvImageCapture();
    ~CvImageCapture();
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
private:
    DirectShowCapture* mpDirectShow; ///< DirectShow interface class.
};

#else // WIN32

#include <cv.h>
#include <highgui.h>
#include <string>
#include <vector>

////////////////////////////////////////////////////////////////////////////////////
///
///   \class CvImageCapture
///   \brief Wrapper class for cross platform capturing of cameras and video
///   files.
///
////////////////////////////////////////////////////////////////////////////////////
class CvImageCapture
{
public:
    class Image
    {
    public:
        Image();
        Image(const Image& img);
        ~Image();
        Image& operator=(const Image& img);
        void Destroy();
        void Create(const unsigned short width,
                    const unsigned short height,
                    const unsigned short channels);
        unsigned short mWidth;    ///<  Image width.
        unsigned short mHeight;   ///<  Image height.
        unsigned short mChannels; ///<  Number of channels.
        unsigned char* mpData;    ///<  Image data.
    };
    CvImageCapture();
    ~CvImageCapture();
    // Starts capturing from video source.
    int Start(const char *videoSource,
              const int width = 0,
              const int height = 0,
              const bool interlace = false);
    // Stops image capture.
    int Stop();
    // Gets frame from video source.
    int GetFrame(CvImageCapture::Image* dest, const bool block = false);
    // Get frame from video source.
    int GetFrame(IplImage*& dest, const bool block = false);
    // Get height of image in pixels.
    int GetHeight() const;
    // Get width of image in pixels.
    int GetWidth() const;
    // Check if image capture is ready (incomming data).
    bool IsReady() const;
    const char* GetSource() const { return mSourceName.c_str(); }
    //  Get vector of sources on computer system. Not currently supported in linux.
    static unsigned int ScanForSources(std::vector<std::string>& sources);
protected:
    IplImage *mImage;
    CvCapture *mCapture;
    std::string mSourceName;    ///<  Name of video source.
    unsigned int mSources;      ///<  Number of sources on system.
    bool mInterlacedFlag;       ///<  If true, images from camera are interlaced.
};


#endif // LINUX

#endif // _CV_IMAGE_CAPTURE_H

/*  End of File */
