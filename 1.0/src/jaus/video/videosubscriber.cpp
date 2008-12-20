////////////////////////////////////////////////////////////////////////////////////
///
///  \file videosubscriber.cpp
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
#include "jaus/video/videosubscriber.h"
#include "jaus/messages/query/configuration/queryconfiguration.h"
#include "jaus/messages/query/configuration/querysubsystemlist.h"
#include "jaus/messages/inform/configuration/reportsubsystemlist.h"
#include "jaus/messages/inform/configuration/reportconfiguration.h"
#include "jaus/messages/query/environment/querycameracapabilities.h"
#include "jaus/messages/query/environment/querycameracount.h"
#include "jaus/messages/query/environment/querycameraformatoptions.h"
#include "jaus/messages/query/environment/querycamerapose.h"
#include "jaus/messages/query/environment/queryselectedcamera.h"


using namespace Jaus;

const unsigned int gWaitTimeMs = 200;  // how long to wait for response in ms.


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
VideoSubscriber::VideoSubscriber()
{
    mpCallbackObject = NULL;
    mCompressionType = Image::JPEG;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
VideoSubscriber::~VideoSubscriber()
{
    CancelVideoSubscription();    
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Creates a video subscription to the sensor ID.  Using the methods
///  of VideoSubscriber, only one visual sensor can be subscribed to at a time.
///
///  \param[in] sensorID The ID of the visual sensor to subscribe to.
///
///  \return True on success, otherwise failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::CreateVideoSubscription(const Address& sensorID)
{
    bool result = false;
    SensorInfo::Map::iterator sensorInfo;
    sensorInfo = mVisualSensors.find(sensorID);
    if(sensorInfo == mVisualSensors.end())
    {
        if(QueryVisualSensorInfo(sensorID))
        {
            sensorInfo = mVisualSensors.find(sensorID);
        }
        else
        {
            return result;
        }
    }

    // Now that we have camera info, lets get the data,
    // but first, lets see if we are running on the host machine and if
    // so, use the shared memory buffer to get RAW data much faster from
    // the visual sensor because we don't have to compress/decompress image data.
    if(mSharedMemoryImage.OpenSharedImage(sensorID) && mSharedMemoryImage.IsActive(500))
    {
        mSharedMemoryImage.RegisterCallback(VideoSubscriber::SharedMemoryImageCallback, this);
        result = true;
    }
    else
    {
        // Create video subscription.
        CreateEventRequest createEvent;
        createEvent.SetSourceID(GetID());
        createEvent.SetDestinationID(sensorID);
        createEvent.SetMessageCode(JAUS_REPORT_IMAGE);
        createEvent.SetEventType(CreateEventRequest::EveryChange);

        if(RequestEvent(createEvent) == OK)
        {
            result = true;
        }
        else
        {
            // Try periodic events.
            createEvent.SetEventType(CreateEventRequest::Periodic);
            createEvent.SetRequestedPeriodicUpdateRate(30.0);
            if(RequestEvent(createEvent) == OK)
            {
                result = true;
            }
        }
        
        if(result)
        {
            AddSubsystemToDiscover(sensorID.mSubsystem);
        }
    }    

    if(result)
    {
        mVideoSourceID = sensorID;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a component is created.  It sets up
///   all core services supported by the component.
///
///   If a component ID is specified in the RA, it may report only one
///   service in beyond the core message support, and this ervice must be
///   equal to the component ID.  If a component ID is not listed in the
///   RA, it may report any number of services.  For example, an component
///   with ID 33 must provide only serive 33.  The exception to this rule
///   is component ID 1 (the Node Mnager) which may provide any number of
///   services in addition to core message support.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VideoSubscriber::SetupCoreService()
{
    // Setup extra service information.
    Service::Map services;
    Service::Map::iterator coreServices;

    Component::SetupCoreService();

    services = GetServices();

    coreServices = services.find(Service::CoreMessageSupport);
    // Add additional message to core set.
    if(coreServices != services.end())
    {
        // Dynamic configuration messages.
        coreServices->second.AddInputMessage(JAUS_REPORT_CONFIGURATION, 0);
        coreServices->second.AddInputMessage(JAUS_REPORT_SUBSYSTEM_LIST, 0);
        coreServices->second.AddInputMessage(JAUS_REPORT_SERVICES, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_SUBSYSTEM_LIST, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_CONFIGURATION, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_SERVICES, 0);

        // Add messages for events.
        coreServices->second.AddInputMessage(JAUS_CREATE_EVENT, 0xFF);
        coreServices->second.AddInputMessage(JAUS_UPDATE_EVENT, 0xFF);
        coreServices->second.AddInputMessage(JAUS_CANCEL_EVENT, 0x03);
        coreServices->second.AddInputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
        coreServices->second.AddInputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_CREATE_EVENT, 0xFF);
        coreServices->second.AddOutputMessage(JAUS_UPDATE_EVENT, 0xFF);
        coreServices->second.AddOutputMessage(JAUS_CANCEL_EVENT, 0x03);
        coreServices->second.AddOutputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_EVENT, 0);

        // Add video related messages
        coreServices->second.AddInputMessage(JAUS_REPORT_CAMERA_FORMAT_OPTIONS, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_FORMAT_OPTIONS));
        coreServices->second.AddInputMessage(JAUS_REPORT_CAMERA_CAPABILITIES, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_CAPABILITIES));
        coreServices->second.AddInputMessage(JAUS_REPORT_CAMERA_COUNT, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_COUNT));
        coreServices->second.AddInputMessage(JAUS_REPORT_CAMERA_POSE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_POSE));
        coreServices->second.AddInputMessage(JAUS_REPORT_SELECTED_CAMERA, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_SELECTED_CAMERA));
        coreServices->second.AddInputMessage(JAUS_REPORT_IMAGE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_IMAGE));
        
        coreServices->second.AddOutputMessage(JAUS_QUERY_CAMERA_FORMAT_OPTIONS, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_FORMAT_OPTIONS));
        coreServices->second.AddOutputMessage(JAUS_QUERY_CAMERA_CAPABILITIES, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_CAPABILITIES));
        coreServices->second.AddOutputMessage(JAUS_QUERY_CAMERA_COUNT, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_COUNT));
        coreServices->second.AddOutputMessage(JAUS_QUERY_CAMERA_POSE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_POSE));
        coreServices->second.AddOutputMessage(JAUS_QUERY_SELECTED_CAMERA, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_SELECTED_CAMERA));
        coreServices->second.AddOutputMessage(JAUS_QUERY_IMAGE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_IMAGE));

        // Add the updated version of the core services to component
        AddService(coreServices->second);
    }

    return JAUS_OK;
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method is called anytime a new Image has been received by the
///  subscriber.
///
///  \param[in] source The ID of the visual sensor source.
///  \param[in] rawImage Pointer to the raw image received.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::ProcessImage(const Address& source, const Image& rawImage)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method called whenever an inform message is received.  This method
///  adds ability to read ReportImage messages before passing to the parent
///  class to process.
///
///  \param[in] message Inform message to read.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VideoSubscriber::ProcessInformMessage(const Jaus::Message* message)
{
    int result = JAUS_FAILURE;
    if(message->GetCommandCode() == JAUS_REPORT_IMAGE)
    {
        const ReportImage* report = dynamic_cast<const ReportImage*>(message);
        if(report)
        {
            // Look up sensor data for decompression.
            SensorInfo::Map::iterator info;
            info = mVisualSensors.find(report->GetSourceID());
            if(info != mVisualSensors.end())
            {
                bool haveImage = true;
                if(mTempImage.Decompress(report->GetImageData(), 
                                         report->GetDataSize(), 
                                         mCompressionType) == FAILURE)
                {
                    haveImage = false;
                    // Try again with what the sensor supports.
                    if(mTempImage.Decompress(report->GetImageData(), 
                                             report->GetDataSize(),
                                             (Image::Format)info->second.mCameraFormatOptions.GetImageFormat1()))
                    {
                        mCompressionType = (Image::Format)info->second.mCameraFormatOptions.GetImageFormat1();
                        haveImage = true;
                    }
                    else if(mTempImage.Decompress(report->GetImageData(), 
                                             report->GetDataSize(),
                                             (Image::Format)info->second.mCameraFormatOptions.GetImageFormat2()))
                    {
                        mCompressionType = (Image::Format)info->second.mCameraFormatOptions.GetImageFormat2();
                        haveImage = true;
                    }
                    else if(mTempImage.Decompress(report->GetImageData(), 
                                             report->GetDataSize(),
                                             (Image::Format)info->second.mCameraFormatOptions.GetImageFormat3()))
                    {
                        mCompressionType = (Image::Format)info->second.mCameraFormatOptions.GetImageFormat3();
                        haveImage = true;
                    }
                    else if(mTempImage.Decompress(report->GetImageData(), 
                                             report->GetDataSize(),
                                             (Image::Format)info->second.mCameraFormatOptions.GetImageFormat4()))
                    {
                        mCompressionType = (Image::Format)info->second.mCameraFormatOptions.GetImageFormat4();
                        haveImage = true;
                    }
                }
                if(haveImage)
                {
                    SharedMemoryImageCallback(report->GetSourceID(), mTempImage, this);
                }
            }
        }
        result = JAUS_OK;
    }

    // Pass up the chain.
    if(result == OK)
    {
        SubscriberComponent::ProcessInformMessage(message);
    }
    else
    {
        result = SubscriberComponent::ProcessInformMessage(message);
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if a video subscriptions exists, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::HaveVideoSubscription() const
{
    if(mSharedMemoryImage.IsOpen())
    {
        return true;
    }
    return this->HaveEventSubscriptionsOfType(JAUS_REPORT_IMAGE);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return Stops any video subscription.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::CancelVideoSubscription()
{
    mSharedMemoryImage.CloseSharedImage();
    ClearVideoCallback();
    this->CancelEvents(mVideoSourceID);
    mCompressionType = Image::JPEG;
    mVideoSourceID(0, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method clears the current VisualSensor data stored internally
///  and replaces it with whatever visual sensor data can be found from the
///  specified subsystem. See GetVisualSensorInfo methods to retrieve data results
///  from this method.
///
///  \param[in] subsystem The desired subsystem to lookup data for.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::FindVisualSensors(const Byte subsystem)
{
    bool result = false;
    mVisualSensors.clear();

    // Query the node manager for the subsystem list.
    QuerySubsystemList querySubsystemList;
    Receipt receipt;
    querySubsystemList.SetSourceID(GetID());
    querySubsystemList.SetDestinationID(Address(GetID().mSubsystem, GetID().mNode, 1, 1));
    if(Send(&querySubsystemList, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        const ReportSubsystemList* reportSubsystemList = dynamic_cast<const ReportSubsystemList*>(receipt.GetResponseMessage());
        // Find the ID of the component in the subsystem list that can
        // provide configuration data for the subsystem we want.
        Address::Set::const_iterator discoveryID;
        for(discoveryID = reportSubsystemList->GetSubsystemList()->begin();
            discoveryID != reportSubsystemList->GetSubsystemList()->end();
            discoveryID++)
        {
            if(discoveryID->mSubsystem == subsystem)
            {
                break;
            }
        }
        if(discoveryID == reportSubsystemList->GetSubsystemList()->end())
        {
            // Could not find the information we need.
            return result;
        }
        QueryConfiguration queryConfiguration;
        queryConfiguration.SetSourceID(GetID());
        queryConfiguration.SetDestinationID(*discoveryID);
        queryConfiguration.SetQueryField(QueryConfiguration::Subsystem);
        if(Send(&queryConfiguration, receipt, 0, gWaitTimeMs, 1) == OK)
        {
            const ReportConfiguration* reportConfiguration = dynamic_cast<const ReportConfiguration*>(receipt.GetResponseMessage());
            // Find visual sensors.
            Address::List sensorList = reportConfiguration->GetConfiguration()->GetComponentsOfType((UShort)Service::VisualSensor);
            Address::List::iterator sensorID;
            
            for(sensorID = sensorList.begin();
                sensorID != sensorList.end();
                sensorID++)
            {
                result = true;
                QueryVisualSensorInfo(*sensorID, NULL);
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends queries to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[out] sensorInfoCopy A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QueryVisualSensorInfo(const Address& visualSensorID,
                                            VideoSubscriber::SensorInfo* sensorInfoCopy)
{
    bool result = false;
    SensorInfo::Map::iterator sensorInfo;

    // Delete old data.
    sensorInfo = mVisualSensors.find(visualSensorID);
    if(sensorInfo != mVisualSensors.end())
    {
        mVisualSensors.erase(sensorInfo);
    }
    if(QueryCameraCount(visualSensorID, NULL) && 
       QuerySelectedCamera(visualSensorID, NULL) )
    {
        sensorInfo = mVisualSensors.find(visualSensorID);
        result = true;
        // Try get other data.
        QueryCameraFormatOptions(visualSensorID, sensorInfo->second.mSelectedCamera.GetCameraID(), NULL);
        QueryCameraPose(visualSensorID, sensorInfo->second.mSelectedCamera.GetCameraID(), NULL);
        QueryCameraCapabilities(visualSensorID, sensorInfo->second.mSelectedCamera.GetCameraID(), NULL);
        // Get the copy if desired.
        if(sensorInfoCopy)
        {
            *sensorInfoCopy = sensorInfo->second;
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends a query to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[out] report A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QueryCameraCount(const Address& visualSensorID,
                                       ReportCameraCount* report)
{
    Jaus::QueryCameraCount query;
    Jaus::Receipt receipt;

    query.SetSourceID(GetID());
    query.SetDestinationID(visualSensorID);
    if(visualSensorID.mComponent == (Byte)Service::VisualSensor &&
        Send(&query, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        mVisualSensors[visualSensorID].mID = visualSensorID;
        mVisualSensors[visualSensorID].mCameraCount = *((ReportCameraCount*)(receipt.GetResponseMessage()));
        if(report)
        {
            *report = *((ReportCameraCount*)(receipt.GetResponseMessage()));
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends a query to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[out] report A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QuerySelectedCamera(const Address& visualSensorID,
                                          ReportSelectedCamera* report)
{
    Jaus::QuerySelectedCamera query;
    Jaus::Receipt receipt;

    query.SetSourceID(GetID());
    query.SetDestinationID(visualSensorID);
    if(visualSensorID.mComponent == (Byte)Service::VisualSensor &&
        Send(&query, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        mVisualSensors[visualSensorID].mID = visualSensorID;
        mVisualSensors[visualSensorID].mSelectedCamera = *((ReportSelectedCamera*)(receipt.GetResponseMessage()));
        if(report)
        {
            *report = *((ReportSelectedCamera*)(receipt.GetResponseMessage()));
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends a query to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[in] cameraID The ID of the camera on the sensor to get the data for.
///  \param[out] report A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QueryCameraFormatOptions(const Address& visualSensorID,
                                               const Byte cameraID,
                                               ReportCameraFormatOptions* report)
{
    Jaus::QueryCameraFormatOptions query;
    Jaus::Receipt receipt;

    query.SetSourceID(GetID());
    query.SetDestinationID(visualSensorID);
    query.SetCameraID(cameraID);
    query.SetPresenceVector((Jaus::Byte)query.GetPresenceVectorMask());

    if(visualSensorID.mComponent == (Byte)Service::VisualSensor &&
        Send(&query, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        mVisualSensors[visualSensorID].mID = visualSensorID;
        mVisualSensors[visualSensorID].mCameraFormatOptions = *((ReportCameraFormatOptions*)(receipt.GetResponseMessage()));
        if(report)
        {
            *report = *((ReportCameraFormatOptions*)(receipt.GetResponseMessage()));
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends a query to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[in] cameraID The ID of the camera on the sensor to get the data for.
///  \param[out] report A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QueryCameraPose(const Address& visualSensorID,
                                      const Byte cameraID,
                                      ReportCameraPose* report)
{
    Jaus::QueryCameraPose query;
    Jaus::Receipt receipt;

    query.SetSourceID(GetID());
    query.SetDestinationID(visualSensorID);
    query.SetCameraID(cameraID);
    query.SetPresenceVector((Jaus::UShort)query.GetPresenceVectorMask());

    if(visualSensorID.mComponent == (Byte)Service::VisualSensor &&
        Send(&query, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        mVisualSensors[visualSensorID].mID = visualSensorID;
        mVisualSensors[visualSensorID].mCameraPose = *((ReportCameraPose*)(receipt.GetResponseMessage()));
        if(report)
        {
            *report = *((ReportCameraPose*)(receipt.GetResponseMessage()));
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method sends a query to the visual sensor to get 
///  information about it.  Results are saved to internal SensorInfo data
///  members.
///
///  \param[in] visualSensor The ID of a Visual Sensor component to query.
///  \param[in] cameraID The ID of the camera on the sensor to get the data for.
///  \param[out] report A copy of the data retrieved, set to NULL if
///                             you don't want a copy.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoSubscriber::QueryCameraCapabilities(const Address& visualSensorID,
                                      const Byte cameraID,
                                      ReportCameraCapabilities* report)
{
    Jaus::QueryCameraCapabilities query;
    Jaus::Receipt receipt;

    query.SetSourceID(GetID());
    query.SetDestinationID(visualSensorID);
    query.SetCameraID(cameraID);
    query.SetPresenceVector((Jaus::UShort)query.GetPresenceVectorMask());

    if(visualSensorID.mComponent == (Byte)Service::VisualSensor &&
        Send(&query, receipt, 0, gWaitTimeMs, 1) == OK)
    {
        mVisualSensors[visualSensorID].mID = visualSensorID;
        mVisualSensors[visualSensorID].mCameraCapabilities = *((ReportCameraCapabilities*)(receipt.GetResponseMessage()));
        if(report)
        {
            *report = *((ReportCameraCapabilities*)(receipt.GetResponseMessage()));
        }
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Registers a callback to get image data as it arrives from a
///  visual sensor subscription.  The second method to get data is to inherit
///  from this class and overload ProcessImage method.
///
///  \param[in] callback Pointer to callback object.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::RegisterVideoCallback(VideoSubscriber::Callback* callback)
{
    mVideoCallbacksMutex.Enter();
    mpCallbackObject = callback;
    mVideoCallbacksMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Registers a callback to get image data as it arrives from a
///  visual sensor subscription.  The second method to get data is to inherit
///  from this class and overload ProcessImage method.
///
///  \param[in] callback Pointer to function callback.
///  \param[in] args Pointer to any additional arguments for function callback.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::RegisterVideoCallback(CallbackFunction callback, void* args)
{
    mVideoCallbacksMutex.Enter();
    mFunctionCallback.mpFunction = callback;
    mFunctionCallback.mpFunctionArgs = args;
    mVideoCallbacksMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears any registered callbacks.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::ClearVideoCallback()
{
    mVideoCallbacksMutex.Enter();
    mFunctionCallback.mpFunction = NULL;
    mFunctionCallback.mpFunctionArgs = NULL;
    mpCallbackObject = NULL;
    mVideoCallbacksMutex.Leave();
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets information about a visual sensor in the list of known
///  sensors based on its index.
///
///  \param[in] i The index of the visual sensor in the internal list to get
///               information for.
///
///  \return Copy of sensor information.  If none availalbe, a blank copy is
///          returned with invalid sensor ID.
///
////////////////////////////////////////////////////////////////////////////////////
VideoSubscriber::SensorInfo VideoSubscriber::GetVisualSensorInfo(const unsigned int i) const
{
    SensorInfo::Map::const_iterator data;
    data = mVisualSensors.begin();
    for(unsigned int v = 0;  v < mVisualSensors.size(); v++, data++)
    {
        if(v == i)
        {
            return data->second;
        }
    }
    
    return SensorInfo();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets information about a visual sensor in the list of known
///  sensors based on its index.
///
///  \param[in] i The index of the visual sensor in the internal list to get
///               information for.
///
///  \return Copy of sensor information.  If none availalbe, a blank copy is
///          returned with invalid sensor ID.
///
////////////////////////////////////////////////////////////////////////////////////
VideoSubscriber::SensorInfo VideoSubscriber::GetVisualSensorInfo(const Address& id) const
{
    SensorInfo::Map::const_iterator data;
    data = mVisualSensors.find(id);
    if(data != mVisualSensors.end())
    {
        return data->second;
    }
    
    return SensorInfo();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This method is called whenever Shared Memory gets a new image.
///
///  \param[in] source Source of the image data (JAUS ID).
///  \param[in] img Decompressed image data.
///  \param[in] args Pointer to VideoSubscriber instance.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoSubscriber::SharedMemoryImageCallback(const Address& source, const Image& img, void* args)
{
    VideoSubscriber* videoSubscriber = (VideoSubscriber*)(args);
    if(source == videoSubscriber->mVideoSourceID)
    {
        videoSubscriber->ProcessImage(source, img);
        // Check for callbacks.
        videoSubscriber->mVideoCallbacksMutex.Enter();
        if(videoSubscriber->mpCallbackObject)
        {
            videoSubscriber->mpCallbackObject->ProcessImage(source, img);
        }
        if(videoSubscriber->mFunctionCallback.mpFunction)
        {
            videoSubscriber->mFunctionCallback.mpFunction(source, img, videoSubscriber->mFunctionCallback.mpFunctionArgs);
        }
        videoSubscriber->mVideoCallbacksMutex.Leave();
    }
}


/*  End of File */
