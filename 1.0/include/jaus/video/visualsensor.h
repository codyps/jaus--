////////////////////////////////////////////////////////////////////////////////////
///
///  \file visualsensor.h
///  \brief This file contains software for creating a JAUS Visual Sensor 
///  Component capable of sending video data.
///  
///  <br>Author(s): Sergey Leontyev, Daniel Barber
///  <br>Created: 27 July 2007
///  <br>Last Modified: 11 June 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: sleontye@ist.ucf.edu, dbarber@ist.ucf.edu
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
#ifndef __JAUS_VISUAL_SENSOR_H
#define __JAUS_VISUAL_SENSOR_H

#include "jaus/video/image.h"
#include "jaus/video/sharedimage.h"
#include "jaus/components/informcomponent.h"
#include "jaus/messages/query/environment/queryimage.h"
#include "jaus/messages/query/environment/querycameraformatoptions.h"
#include "jaus/messages/inform/environment/reportimage.h"
#include "jaus/messages/inform/environment/reportcameraformatoptions.h"
#include <cxutils/mutex.h>

#ifdef __cplusplus

namespace Jaus
{
    const unsigned int JAUS_VISUAL_SENSOR_MESSAGE_BOX_SIZE  = 2097152;  ///<  Default size for message inbox buffer.
    
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class VisualSensor
    ///  \brief Visual Sensor component class.  Supports sharing of image/video
    ///  data using image formats supported in Image.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_VIDEO_DLL VisualSensor : public InformComponent
    {
    public:
        VisualSensor();
        ~VisualSensor();
        // Initialize JAUS component.
        int Initialize(const Byte subsystem, 
                       const Byte node, 
                       const Byte instance,
                       const unsigned int mbsize = JAUS_VISUAL_SENSOR_MESSAGE_BOX_SIZE);  
        // Shutdown JAUS component.
        virtual int Shutdown();
        // Setup service for the Visual Sensor
        virtual int SetupService();
        //  Set raw uncompressed image data.
        int SetCurrentFrame(const Image &frame,
                            const UShort maxWidth = 0,
                            const UShort maxHeight = 0);
        //  Set raw uncompressed image data.
        int SetCurrentFrame(const Byte * frame,       
                            const UShort width, 
                            const UShort height, 
                            const Byte channels,
                            const bool vflip,
                            const UShort maxWidth = 0,
                            const UShort maxHeight = 0);
        //  Set compressed image data.
        int SetCurrentFrameCompressed(const Byte* image,
                                      const unsigned int size,
                                      const Image::Format format,
                                      const bool decompress = false);
        virtual int ProcessCommandMessage(const Message* message, const Byte commandAuthority);
        // Method to process incomming query messages.
        virtual int ProcessQueryMessage(const Message* message);
        // This method is called whenever a request for an event is made.
        virtual int ProcessEventRequest(const CreateEventRequest& command,
                                        Byte& responseValue,
                                        double& confirmedRate,
                                        std::string& errorMessage) const;
        // This method is called whenever a request for a service connection is made.
        virtual int ProcessServiceConnectionRequest(const Jaus::CreateServiceConnection& command,
                                                    Byte& responseValue,
                                                    double& confirmedRate) const;
        // This method is called whenever an event needs to be generated.
        virtual int GenerateEvent(const Event* eventInfo);
        // This method is called whenever a service connection message needs to be sent.
        virtual int GenerateServiceConnectionMessage(const ServiceConnection& sc);
        //virtual int ReportCameraCapabilities(const QueryCameraCapabilities* msg) const;
        virtual int ReportCameraFormatOptions(const QueryCameraFormatOptions* msg) const;
        // Set the default format to use for image compression.
        int SetImageFormat( const Image::Format format );
        // Sets the update rate of images from visual sensor.
        int SetFrameRate(const UShort fps);
        // Sets the ID of the Camera.
        int SetCameraID(const Byte id);
        // Gets the current image format being used.
        Image::Format GetImageFormat() const { return mImageFormat; }
        // Gets the frame rate of the visual sensor.
        UShort GetFrameRate() const { return mFrameRate; }
        // Gets the camera ID
        Byte GetCameraID() const { return mCameraID; }
        // Gets the number of the frame set.
        unsigned int GetFrameNumber() const { return mFrameNumber; }
        // If using JPEG compression, this sets the
        // quality of it.
        void SetJPEGQuality(const int quality);
    protected:       
        CxUtils::Mutex mImageMutex;         ///< Mutex to protect RAW image data.
        Image mRawImage;                    ///< The internal Image object to compress/decompress
        SharedImage mSharedImage;           ///< Shared memory image.
        Byte * mpCompressedImage;           ///< Buffer for the compressed data.
        Image::Format   mImageFormat;       ///< Image format set for transmission.
        Byte mCameraID;                     ///< ID of the camera.
        UShort mFrameRate;                  ///< Frame rate of visual sensor.
        UShort mSequenceNumber;             ///< Sequence Number used for service connections.
        UShort mCompressionNumber;          ///< Sequence Number of the compressed image data (used to reduce number of times data is compressed).
        unsigned int mCompressedBufferSize; ///< Size of the buffer used for compression results.
        unsigned int mCompressedSize;       ///< Size of compressed image data.
        unsigned int mFrameNumber;          ///< Counter to know what the current frame number is.
        volatile bool mEnableSharedImageFlag; ///<  If true, shared memory image is created.
        int mJPEGQuality;                     ///<  Quality level for JPEG.
        JPEG::Compressor mJPEGCompressor;     ///<  JPEG Compressor to re-use as much memory as possible.
    };
}



#endif // __cplusplus

#endif // __JAUS_VISUAL_SENSOR_H
/*  End of File */
