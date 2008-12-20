////////////////////////////////////////////////////////////////////////////////////
///
///  \file videosubscriber.h
///  \brief Contains a component for creating video subscriptions to Visual Sensors.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 December 2008
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
#ifndef __JAUS_VIDEO_SUBSCRIBER__H
#define __JAUS_VIDEO_SUBSCRIBER__H

#include "jaus/video/videolibdll.h"
#include "jaus/video/image.h"
#include "jaus/video/sharedimage.h"
#include "jaus/components/subscribercomponent.h"
#include "jaus/messages/inform/environment/reportcameracapabilities.h"
#include "jaus/messages/inform/environment/reportcameracount.h"
#include "jaus/messages/inform/environment/reportcameraformatoptions.h"
#include "jaus/messages/inform/environment/reportcamerapose.h"
#include "jaus/messages/inform/environment/reportselectedcamera.h"
#include "jaus/messages/inform/environment/reportimage.h"

namespace Jaus
{
    const unsigned int JAUS_VIDEO_SUBSCRIBER_MESSAGE_BOX_SIZE  = 2097152;  ///<  Default size for message inbox buffer.

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class VideoSubscriber
    ///   \brief Video Subscriber component class.  Only functionality supported is
    ///   for acquiring visual sensor data (in addition to core message set).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_VIDEO_DLL VideoSubscriber : public SubscriberComponent
    {
    public:
        typedef void (*CallbackFunction)(const Address& source, const Image& rawImage, void* args);
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Callback
        ///   \brief Callback class for registering callbacks for when image data is
        ///          available.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_VIDEO_DLL Callback
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \class Data
            ///   \brief Structure for storing function callback information.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            class JAUS_VIDEO_DLL Data
            {
            public:
                Data() : mpFunction(0), mpFunctionArgs(0) {}
                ~Data() {}
                CallbackFunction mpFunction;
                void* mpFunctionArgs;
            };
            virtual void ProcessImage(const Address& source, const Image& rawImage) = 0;
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class SensorInfo
        ///   \brief Structure for storing information about a Visual Sensor component.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_VIDEO_DLL SensorInfo
        {
        public:
            typedef std::map<Address,SensorInfo> Map;           ///<  STL map of sensors.
            Address                     mID;                    ///<  Visual sensor ID.
            ReportCameraCapabilities    mCameraCapabilities;    ///<  Camera capabilities for selected camera.
            ReportCameraFormatOptions   mCameraFormatOptions;   ///<  Camera format options for selected camera.
            ReportCameraCount           mCameraCount;           ///<  Number of cameras available to visual sensor.
            ReportCameraPose            mCameraPose;            ///<  Camera pose of selected camera.
            ReportSelectedCamera        mSelectedCamera;        ///<  The current selected camera on visual sensor.
        };
        VideoSubscriber();
        virtual ~VideoSubscriber();    
        bool CreateVideoSubscription(const Address& sensorID);
        bool HaveVideoSubscription() const;
        void CancelVideoSubscription();
        virtual int SetupCoreService();
        virtual void ProcessImage(const Address& source, const Image& rawImage);
        virtual int ProcessInformMessage(const Message* message);
        bool FindVisualSensors(const Byte subsystem);
        bool QuerySelectedCamera(const Address& visualSensorID, ReportSelectedCamera* report = NULL);
        bool QueryCameraCount(const Address& visualSensorID, ReportCameraCount* report = NULL);
        bool QueryCameraFormatOptions(const Address& visualSensorID, 
                                      const Byte cameraID, 
                                      ReportCameraFormatOptions* report = NULL);
        bool QueryCameraPose(const Address& id,
                             const Byte cameraID,
                             ReportCameraPose* report = NULL);
        bool QueryCameraCapabilities(const Address& id, 
                                     const Byte cameraID,
                                     ReportCameraCapabilities* report = NULL);
        bool QueryVisualSensorInfo(const Address& visualSensorID,
                                   VideoSubscriber::SensorInfo* sensorInfoCopy = NULL);
        void RegisterVideoCallback(Callback* callback);
        void RegisterVideoCallback(CallbackFunction callback, void* args);
        void ClearVideoCallback();
        SensorInfo GetVisualSensorInfo(const unsigned int i) const;
        SensorInfo GetVisualSensorInfo(const Address& id) const;
        SensorInfo::Map GetVisualSensorInfo() const { return mVisualSensors; }
    protected:
        static void SharedMemoryImageCallback(const Address& source, const Image& img, void* args);
        SensorInfo::Map mVisualSensors;     ///<  List of available visual sensors on subsystem.
        Address mVideoSourceID;             ///<  Source of video data.
        Image mTempImage;                   ///<  Re-usable temporary image.
        Image::Format mCompressionType;     ///<  Type of image compression being used.
        CxUtils::Mutex mVideoCallbacksMutex;///<  Callbacks mutex.
        Callback::Data mFunctionCallback;   ///<  Function callback data.
        Callback* mpCallbackObject;         ///<  Pointer to callback object.
        SharedImage mSharedMemoryImage;     ///<  Connection to video data in shared memory.
    };
}

#endif
/* End of File */
