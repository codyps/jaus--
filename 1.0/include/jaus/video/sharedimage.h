////////////////////////////////////////////////////////////////////////////////////
///
///  \file sharedimage.h
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
#ifndef __JAUS_SHARED_VIDEO_H
#define __JAUS_SHARED_VIDEO_H

#include <cxutils/mappedmemory.h>
#include <cxutils/thread.h>
#include <cxutils/mutex.h>
#include "jaus/messages/address.h"
#include "jaus/video/image.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class SharedImage
    ///  \brief Shared memory buffer for storing image data for other JAUS components
    ///  on local host to bypass serializing image data for transfer.
    ///
    ///  Using the standard approach, a component wanting to access video data
    ///  from a JAUS Visual Sensor component would need to send a query or create
    ///  an Event or Service Connection to get video data.  This results in image
    ///  data being serialized into multiple JAUS packets or being compressed
    ///  using some type of compression method.  Although this approach
    ///  is fine (and necessary for communication with other computers), it is 
    ///  faster to just access the data directly when the sensor is on the
    ///  same node as the requesting component.
    ///
    ///  This class is used by the VisualSensor component class to provide an
    ///  alternative method for accessing video data on the same computer.  
    ///  The VisualSensorController component uses this video to just directly
    ///  get data from the Visual Sensor component without having to get a
    ///  bunch of JAUS packets or decompress image data.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_VIDEO_DLL SharedImage
    {
    public:
        SharedImage();
        ~SharedImage();
        int CreateSharedImage(const Address& src,
                              const unsigned int size = 2097152);
        int OpenSharedImage(const Address& src);
        int CloseSharedImage();
        int RegisterCallback(void (*func)(const Address& src, const Image& img, void* args), 
                             void *fargs);
        int ClearCallback();        
        int GetFrame(Image& img) const;
        int SetFrame(const Image& img);
        UInt GetFrameNumber() const;
        unsigned int GetBufferSize() const { return mImageBuffer.Size(); }
        bool IsOpen() const;
        inline Address GetSourceID() const { return mSourceID; }
    protected:
        static void SharedImageUpdate(void *args);
        Address mSourceID;                  ///<  Source ID of data.
        bool mWriteFlag;                    ///<  If true, writing to memory is allowed.
        Image mTempImage;                   ///<  Temporary image for storing data.
        CxUtils::MappedMemory mImageBuffer; ///<  Shared memory image buffer.
        CxUtils::Thread mCallbackThread;    ///<  Thread to poll for image data.
        CxUtils::Mutex mCallbackMutex;      ///<  Mutex for image callbacks.
        void (*mpCallback)(const Address& src, const Image& img, void *args);  ///<  Callback to perform when image updates.
        void *mpCallbackArgs;       ///<  Callback arguments.
    };
}

#endif
#endif
/*  End of File */
