////////////////////////////////////////////////////////////////////////////////////
///
///  \file sharedimage.cpp
///  \brief This file contains the main data structure for storing raw image
///  data within shared memory for transfering between components on the
///  same machine.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 October 2007
///  <br>Last Modified: 11 June 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dbarber@ist.ucf.edu
///  <br>Web:  http://active.ist.ucf.edu
///
///  Redistribution and use in source and binary forms, with or without
///  modification, are permitted provided that the following conditions are met:
///      * Redistributions of source code must retain the above copyright
///        notice, this list of conditions and the following disclaimer.
///      * Redistributions in binary form must reproduce the above copyright
///        notice, this list of conditions and the following disclaimer in the
///        documentation and/or other materials provided with the distribution.
///      * Neither the name of the ACTIVE LAB, IST, UCF, nor the
///        names of its contributors may be used to endorse or promote products
///        derived from this software without specific prior written permission.
/// 
///  THIS SOFTWARE IS PROVIDED BY THE ACTIVE LAB''AS IS'' AND ANY
///  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
///  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
///  DISCLAIMED. IN NO EVENT SHALL UCF BE LIABLE FOR ANY
///  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
///  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
///  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
///  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
///  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
///  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
///
////////////////////////////////////////////////////////////////////////////////////
#include "jaus/video/sharedimage.h"
#include "jaus/messages/types.h"
#include "jaus/messages/time.h"
#include <string.h>

using namespace CxUtils;
using namespace Jaus;

const UInt JAUS_SHARED_IMAGE_HEADER_SIZE = JAUS_UINT_SIZE*2 + JAUS_USHORT_SIZE*2 + JAUS_BYTE_SIZE;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.  
///
////////////////////////////////////////////////////////////////////////////////////
SharedImage::SharedImage()
{
    mWriteFlag = false;
    mpCallback = NULL;
    mpCallbackArgs = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.  
///
////////////////////////////////////////////////////////////////////////////////////
SharedImage::~SharedImage()
{
    CloseSharedImage();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Created a shared image memory space.  
///
///  \param src Source ID of the provider of the data.
///  \param size How many bytes needed to store image.
///
///  \return JAUS_OK if created, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::CreateSharedImage(const Address& src, const unsigned int size)
{
    CloseSharedImage();

    if( src.IsValid() && !src.IsBroadcast() && size > JAUS_SHARED_IMAGE_HEADER_SIZE )
    {
        char sname[256];
        sprintf(sname, "SharedImage_%d.%d.%d.%d", src.mSubsystem,
                                                  src.mNode,
                                                  src.mComponent,
                                                  src.mInstance);
        if( mImageBuffer.CreateMappedMemory(sname, size + JAUS_SHARED_IMAGE_HEADER_SIZE) ||
            mImageBuffer.OpenMappedMemory(sname, CX_MAPPED_MEMORY_READ_WRITE, size + JAUS_SHARED_IMAGE_HEADER_SIZE))
        {
            UInt tstamp, number;
            UShort width = 0, height = 0;
            Byte channels = 0;
            tstamp = Time::GetUtcTimeMs();
            number = 0;
            mImageBuffer.Lock();
            mImageBuffer.SetWritePos(0);
            mImageBuffer.Write(tstamp);
            mImageBuffer.Write(number);
            mImageBuffer.Write(width);
            mImageBuffer.Write(height);
            mImageBuffer.Write(channels);
            mImageBuffer.Unlock();

            mWriteFlag = true;
            mSourceID = src;

            return JAUS_OK;
        }
    }

    CloseSharedImage();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Created a shared image memory space.  
///
///  \param src Source ID of the provider of the data.
///
///  \return JAUS_OK if created, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::OpenSharedImage(const Address& src)
{
    if(src.IsValid() && !src.IsBroadcast())
    {
        char sname[256];
        sprintf(sname, "SharedImage_%d.%d.%d.%d", src.mSubsystem,
                                                  src.mNode,
                                                  src.mComponent,
                                                  src.mInstance);
        if( mImageBuffer.OpenMappedMemory(sname) )
        {
            mWriteFlag = false;
            mSourceID = src;
            return JAUS_OK;
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Closes shared image.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::CloseSharedImage()
{
    mCallbackThread.StopThread(500);

    mCallbackMutex.Enter();
    mpCallback = NULL;
    mpCallbackArgs = NULL;
    mCallbackMutex.Leave();

    if( mWriteFlag )
    {
        mImageBuffer.Lock();
        memset((void *)mImageBuffer.pGetMemory(), 0, mImageBuffer.Size());
        mImageBuffer.Unlock();
    }
    mImageBuffer.CloseMappedMemory();

    mWriteFlag = false;

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Registers a callback to be done every time the shared image
///  updates.
///
///  \param func Callback function (arguments are source of image data and
///              a raw image, plus additional arguments).
///  \param fargs Arguments to function callback.
///
///  \return JAUS_OK if registered, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::RegisterCallback(void (*func)(const Address& src, const Image& img, void* args), void *fargs)
{
    if( func )
    {
        mCallbackMutex.Enter();
        mpCallback = func;
        mpCallbackArgs = fargs;
        mCallbackMutex.Leave();

        if( mCallbackThread.IsThreadActive() == false )
        {
            mCallbackThread.CreateThread(&SharedImage::SharedImageUpdate, this);
        }
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears the callback.
///
///  \return JAUS_OK if cleared, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::ClearCallback()
{
    mCallbackMutex.Enter();
    mpCallback = NULL;
    mpCallbackArgs = NULL;
    mCallbackMutex.Leave();

    mCallbackThread.StopThread();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the value of the image data.
///
///  Image data can only be set if you create the shared image using
///  the CreateSharedImage method.
///
///  \param img The updated image. Must be small enough in bytes to fit
///             in created shared image space.
///
///  \return JAUS_OK if created, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::SetFrame(const Image& img)
{
    int result = JAUS_FAILURE;

    if( mWriteFlag && 
        mImageBuffer.IsOpen() &&
        img.DataSize() <= mImageBuffer.Size() - JAUS_SHARED_IMAGE_HEADER_SIZE)
    {
        UInt imgNumber, imgTimeStamp;
        UShort width, height;
        Byte channels;

        width = img.Width();
        height = img.Height();
        channels = img.Channels();
        imgTimeStamp = Time::GetUtcTimeMs();
        mImageBuffer.Lock();
        mImageBuffer.Read(imgNumber, JAUS_UINT_SIZE);
        mImageBuffer.SetWritePos(0);
        mImageBuffer.Write(imgTimeStamp);
        mImageBuffer.Write(imgNumber + 1);
        mImageBuffer.Write(width);
        mImageBuffer.Write(height);
        mImageBuffer.Write(channels);
        mImageBuffer.Write( img.ImageData(), width*height*channels );
        mImageBuffer.Unlock();
        result = JAUS_OK;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the image from shared memory.
///
///  \param img Current image data.
///
///  \return JAUS_OK if created, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SharedImage::GetFrame(Image& img) const
{
    int result = JAUS_FAILURE;
    UInt tstamp, number;
    UShort width, height;
    Byte channels;

    if( mImageBuffer.IsOpen() )
    {
        mImageBuffer.Lock();
        mImageBuffer.SetReadPos(0);
        if( mImageBuffer.Read( tstamp ) &&
            mImageBuffer.Read( number ) &&
            mImageBuffer.Read( width ) &&
            mImageBuffer.Read( height ) &&
            mImageBuffer.Read( channels ) )
        {
            if( width && height && channels &&
                mImageBuffer.Size() - JAUS_SHARED_IMAGE_HEADER_SIZE == (unsigned int)((unsigned int)(width)*height*channels))
            {
                result = img.Create(width, height, channels, mImageBuffer.GetMemory() + JAUS_SHARED_IMAGE_HEADER_SIZE );
            }
        }
        mImageBuffer.Unlock();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Get the sequence number for the image.  
///
///  \return Image sequence number (0) if no image data.
///
////////////////////////////////////////////////////////////////////////////////////
UInt SharedImage::GetFrameNumber() const
{
    int result = JAUS_FAILURE;
    UInt number = 0;

    if( mImageBuffer.IsOpen() )
    {
        mImageBuffer.Lock();
        if( mImageBuffer.Read( number, JAUS_UINT_SIZE ) )
            result = JAUS_OK;
        mImageBuffer.Unlock();
    }

    if( result )
        return number;
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if shared image is open/active, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool SharedImage::IsOpen() const
{
    if( mImageBuffer.IsOpen() ||
        mCallbackThread.IsThreadActive() )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method checks to see if the shared image buffer is being updated or
///  not.
///
///  \return True if active, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool SharedImage::IsActive(const unsigned int timeout) const
{
    if(mImageBuffer.IsOpen())
    {
        UInt imgTimeStamp = 0;
        mImageBuffer.Lock();
        mImageBuffer.SetReadPos(0);
        mImageBuffer.Read(imgTimeStamp);
        mImageBuffer.Unlock();
        UInt currentTime = Jaus::Time::GetUtcTimeMs();
        if(currentTime > imgTimeStamp && currentTime - imgTimeStamp < timeout)
        {
            return true;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called by thread which keeps trying to get the latest
///  data from shared memory image.
///
////////////////////////////////////////////////////////////////////////////////////
void SharedImage::SharedImageUpdate(void *args)
{
    SharedImage *img = (SharedImage*)args;
    UInt imgNumber = 0, prevImgNumber = 0;
    UInt imgTimeStamp = 0, currentTime = 0;
    UShort width, height;
    Byte channels;

    bool haveCallback  = true;
    while( img && !img->mCallbackThread.QuitThreadFlag() && haveCallback)
    {
        if( img->mImageBuffer.IsOpen() )
        {
            bool haveImage = false;
            width = height = channels = 0;
            img->mImageBuffer.Lock();
            img->mImageBuffer.SetReadPos(0);
            if( img->mImageBuffer.Read(imgTimeStamp) &&
                img->mImageBuffer.Read(imgNumber) &&
                img->mImageBuffer.Read(width) &&
                img->mImageBuffer.Read(height) &&
                img->mImageBuffer.Read(channels) )
                {
                    if(imgTimeStamp && 
                        imgNumber != prevImgNumber && 
                        width > 0 && height > 0 && channels >= 1 &&
                        width*height*((unsigned int)channels) <= img->mImageBuffer.Size() - JAUS_SHARED_IMAGE_HEADER_SIZE)
                    {
                        if( img->mTempImage.Create(width, 
                                                   height,
                                                   channels,
                                                   img->mImageBuffer.GetMemory() + JAUS_SHARED_IMAGE_HEADER_SIZE))
                        {
                            haveImage = true;
                            img->mTempImage.SetImageNumber(imgNumber);
                        }
                    }
                    prevImgNumber = imgNumber;
                }
            img->mImageBuffer.Unlock();

            if(haveImage)
            {
                img->mCallbackMutex.Enter();
                if( img->mpCallback )
                {
                    img->mpCallback(img->mSourceID, img->mTempImage, img->mpCallbackArgs);
                }
                else
                {
                    haveCallback = false;
                }
                img->mCallbackMutex.Leave();
            }

            currentTime = Time::GetUtcTimeMs();
            if(imgTimeStamp == 0 || (imgTimeStamp < currentTime && currentTime - imgTimeStamp > 10000))
            {
                img->mImageBuffer.CloseMappedMemory();
                // Wait a while for others to close there
                // view of shared memory.
                for(unsigned int i = 0; i < 1000; i++)
                {
                    if( img->mCallbackThread.QuitThreadFlag() )
                        break;

                    CxUtils::SleepMs(1);
                }
            }
        }
        else
        {
            img->mCallbackMutex.Enter();
            if( img->mpCallback == NULL )
            {
                haveCallback = false;
            }
            img->mCallbackMutex.Leave();

            //  Try open shared memory.
            img->OpenSharedImage(img->mSourceID);
        }

        CxUtils::SleepMs(1);
    }
}

/*  End of File */
