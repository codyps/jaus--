////////////////////////////////////////////////////////////////////////////////////
///
///  \file visualsensor.cpp
///  \brief This file contains software for creating a JAUS Visual Sensor 
///  Component capable of sending video data.
///  
///  <br>Author(s): Sergey Leontyev, Daniel Barber
///  <br>Created: 27 July 2007
///  <br>Last Modified: 12 June 2008
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
#include "jaus/video/visualsensor.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/query/querymessages.h"
#include "jaus/messages/inform/informmessages.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
VisualSensor::VisualSensor()
{
    mpCompressedImage = NULL;
    mCompressedBufferSize = 0;
    mCompressedSize = 0;
    mFrameNumber = 0;
    mFrameRate = 0;
    mImageFormat = Image::JPEG;
    mSequenceNumber = 0;
    mCompressionNumber = 0;
    mCameraID = 1;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
VisualSensor::~VisualSensor()
{
    Shutdown();
    if (mpCompressedImage)
    {
        delete[] mpCompressedImage;
        mpCompressedImage = NULL; 
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initialize the Visual Sensor
///
///   \param subsystem Subsystem ID [1-254].
///   \param node Node ID [1-254].
///   \param instance The instance ID of the visual sensor [1-254].
///   \param mbsize How large of a buffer to have in shared memory for
///                 storing messages from this component.  This should be
///                 at least the size of 2 RAW/Uncompressed images from
///                 sensor.
///
///   \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::Initialize(const Byte subsystem, 
                             const Byte node, 
                             const Byte instance,
                             const unsigned int mbsize)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(InformComponent::Initialize("Visual Sensor", 
                                           Address(subsystem, 
                                           node, 
                                           (Byte)(Service::VisualSensor), 
                                           i)) &&
               mSharedImage.CreateSharedImage( GetID(), mbsize))
            {
                this->EnableSubsystemDiscovery(true);
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        if(InformComponent::Initialize("Visual Sensor", 
                                       Address(subsystem, 
                                       node, 
                                       (Byte)(Service::VisualSensor), 
                                       instance)) &&
           mSharedImage.CreateSharedImage( GetID(), mbsize))
        {
            this->EnableSubsystemDiscovery(true);
            return JAUS_OK;
        }
    }

    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Performs visual sensor shutdown procedures.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::Shutdown()
{
    mSharedImage.CloseSharedImage();       // Close shared image buffer out.
    
    mImageMutex.Enter();
    if (mpCompressedImage)
    {
        delete[] mpCompressedImage;
    }
    mpCompressedImage = NULL;
    mCompressedBufferSize = 0;
    mCompressedSize = 0;
    mFrameNumber = 0;
    mFrameRate = 0;
    mImageFormat = Image::JPEG;
    mSequenceNumber = 0;
    mCompressionNumber = 0;
    mImageMutex.Leave();

    return InformComponent::Shutdown();    // Shutdown component.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds service capabilities for Visual Sensor for component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetupService()
{
    Service service;
    service.SetType(Service::VisualSensor);

    UInt presenceVector = 0;

    presenceVector = QueryCameraFormatOptions::VectorMask::ImageFormat1 |
                     QueryCameraFormatOptions::VectorMask::ImageFormat2 |
                     QueryCameraFormatOptions::VectorMask::ImageFormat3;

    service.AddOutputMessage(JAUS_REPORT_IMAGE, 0);
    service.AddOutputMessage(JAUS_REPORT_CAMERA_FORMAT_OPTIONS, presenceVector);
    service.AddInputMessage(JAUS_QUERY_IMAGE, 0);
    service.AddInputMessage(JAUS_QUERY_CAMERA_FORMAT_OPTIONS, presenceVector);

    presenceVector = 0;
    presenceVector = SetCameraFormatOptions::VectorMask::ImageFormat;
    service.AddInputMessage(JAUS_SET_CAMERA_FORMAT_OPTIONS, presenceVector);

    AddService(service);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method copies the uncompressed frame to internal Image structure
///          so that it can be shared with subscribers.
///  
///   \param frame Raw uncompressed image data.
///   \param maxWidth Set the maximum width you want the image stored within.
///                   This will force image scaling to occur.  Set to 0 to use
///                   the original image size.
///   \param maxHeight Set the maximum height you want the image stored within.
///                   This will force image scaling to occur.  Set to 0 to use
///                   the original image size.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetCurrentFrame(const Image& frame,
                                  const UShort maxWidth,
                                  const UShort maxHeight)
{
    return SetCurrentFrame(frame.ImageData(), 
                           frame.Width(), 
                           frame.Height(), 
                           frame.Channels(), 
                           false, 
                           maxWidth, 
                           maxHeight);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method copies the uncompressed frame to internal Image structure
///          so that it can be shared with subscribers.
///  
///   \param frame Pointer to the raw image data.
///   \param width Width of the image in pixels.
///   \param height Height of the image in pixels.
///   \param channels Number of color channels in image data.
///   \param vflip If true the image data is flipped vertically.
///   \param maxWidth Set the maximum width you want the image stored within.
///                   This will force image scaling to occur.  Set to 0 to use
///                   the original image size.
///   \param maxHeight Set the maximum height you want the image stored within.
///                   This will force image scaling to occur.  Set to 0 to use
///                   the original image size.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetCurrentFrame(const Byte * frame, 
                                  const UShort width, 
                                  const UShort height, 
                                  const Byte channels,
                                  const bool vflip,
                                  const UShort maxWidth,
                                  const UShort maxHeight)
{
    int result = JAUS_FAILURE;
    
    mImageMutex.Enter();

    // Copy the raw image data and perform any needed scaling.
    result = mRawImage.Create(width, height, channels, frame, maxWidth, maxHeight, vflip);
    // Increase the frame count.
    mFrameNumber++;
    // Copy the raw image data to the shared image buffer.
    if(mSharedImage.SetFrame(mRawImage) == JAUS_FAILURE)
    {
        mSharedImage.CloseSharedImage();
        CxUtils::SleepMs(500); // Wait for subscribers to exit.
        mSharedImage.CreateSharedImage(GetID(), mRawImage.DataSize()*2);
        mSharedImage.SetFrame(mRawImage);
    }

    mImageMutex.Leave();

    // If we have data, generate events.
    if(result == JAUS_OK)
    {
        Event::Set myEvents;
        Event::Set::iterator e;

        mEventManager.Lock();
        
        myEvents = mEventManager.GetEventsOfType(JAUS_REPORT_IMAGE);
        for(e = myEvents.begin();
            e != myEvents.end();
            e++)
        {
            if( (*e)->GetEventType() == Event::EveryChange)
            {
                GenerateEvent(*e);
                (*e)->SetSequenceNumber( (*e)->GetSequenceNumber() + 1);
                (*e)->SetTimeStampMs(Time::GetUtcTimeMs());
            }
        }

        mEventManager.Unlock();
    }


    return result;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes command messages.
///
///   \param message Command message to process.
///   \param commandAuthority Authority level of component sending request.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::ProcessCommandMessage(const Message* message, const Byte commandAuthority)
{
    int processed = JAUS_FAILURE;
    if(commandAuthority >= GetComponentAuthority())
    {
        processed = JAUS_OK;
        switch(message->GetCommandCode())
        {
        case JAUS_SET_CAMERA_FORMAT_OPTIONS:
            {
                const SetCameraFormatOptions *command = 
                    dynamic_cast<const SetCameraFormatOptions*>(message);
                if(command)
                {
                    if(BitVector::IsBitSet(command->GetPresenceVector(), SetCameraFormatOptions::VectorBit::ImageFormat))
                    {
                        SetImageFormat((Image::Format)(command->GetImageFormat()));
                    }
                }
            }
            break;
        default:
            processed = InformComponent::ProcessCommandMessage( message, commandAuthority);
            break;
        }

        return processed;
    }
    return InformComponent::ProcessCommandMessage( message, commandAuthority);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes query messages.
///
///   \param message Query message to process.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::ProcessQueryMessage(const Message* message)
{
    int processed = JAUS_FAILURE;

    switch(message->GetCommandCode())
    {
    case JAUS_QUERY_IMAGE:
        {
            const QueryImage* query = dynamic_cast<const QueryImage*>(message);
            if(query) 
            {
                mImageMutex.Enter();
                if(mFrameNumber > 0)
                {
                    ReportImage report;
                    report.SetSourceID(GetID());
                    report.SetDestinationID(query->GetSourceID());

                    // If we have not compressed the image data yet for
                    // transport, do so now.
                    if(mCompressionNumber != mFrameNumber)
                    {
                        if(mCompressedBufferSize < mRawImage.DataSize() + 256)
                        {
                            if(mpCompressedImage)
                            {
                                delete[] mpCompressedImage;
                                mpCompressedImage = NULL;
                            }
                            mCompressedBufferSize = mRawImage.DataSize() + 256;
                            mpCompressedImage = new Byte[mCompressedBufferSize];
                            memset(mpCompressedImage, 0, mCompressedBufferSize);
                            mCompressedSize = 0;
                        }
                        // Now compress the image data.
                        mRawImage.Compress(&mpCompressedImage, mCompressedBufferSize, mCompressedSize, mImageFormat);
                        mCompressionNumber = (UShort)mFrameNumber;
                    }

                    if(mpCompressedImage != NULL && mCompressedBufferSize > 0)
                    {
                        report.SetImageData(mpCompressedImage, mCompressedSize);
                    }
                   
                    Send(&report);
                }
                mImageMutex.Leave();
                processed = JAUS_OK;
            }
        }
        break;
    case JAUS_QUERY_CAMERA_FORMAT_OPTIONS:
        {
            processed = ReportCameraFormatOptions((QueryCameraFormatOptions *)message);
        }
        break;
    default:
        processed = InformComponent::ProcessQueryMessage(message);
        break;
    };

    return processed;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a request is received to Create an Event, this function is
///          called.  This function then determines if an event will be created for
///          the requesting component.
///
///   If this function return JAUS_OK, then an event is created and added to
///   the EventManager.  If the event is periodic, then the InformComponent
///   class will generate the events by called the UpdatePeriodicEvent function.
///   In all other cases, it is up to this child class of InformComponent
///   to generate the events.
///
///   \param command The Create Event request message.
///   \param responseValue The response to the request. See 
///          ConfirmEventRequest::ResponseValues for values. This value must
///          be set no matter what.
///   \param confirmedRate The periodic rate that can be supported (if the
///                        event is periodic).
///   \param errorMessage If event not supported, the error message is copied
///                       to this variable.
///
///   \return JAUS_OK if the class supports the event (responeValue should be
///   set to ConfirmEventRequest::ResponseCode::Successful in this case).  
///   otherwise return JAUS_FAILURE if the event is not supported/refused.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                      Byte& responseValue,
                                      double& confirmedRate,
                                      std::string& errorMessage) const
{
    int result = JAUS_FAILURE;
   
    // Currently only supports events for global pose.
    if(command.GetMessageCode() == JAUS_REPORT_IMAGE)
    {
        // Initialize the response value to something.
        responseValue = RejectEventRequest::MessageNotSupported;
        // This implementation of a Global Pose sensor only supports Periodic, EveryChange, or
        // OneTime events, so check for unsupported types.
        if(command.GetEventType() != CreateEventRequest::FirstChange &&
            command.GetEventType() != CreateEventRequest::FirstChangeBoundaries)
        {
            Byte pv = command.GetPresenceVector();
            // This implementation of a Visual Sensor will only support events when any
            // value changes, not on boundaries or lower limites, etc.  If you want to do this, 
            // feel free to change the code. :)
            if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State) == false)
            {
                result = JAUS_OK;

                // If the event requested is periodic, then we need
                // to verify we can support the periodic update rates being requested.
                if(command.GetEventType() == CreateEventRequest::Periodic ||
                    command.GetEventType() == CreateEventRequest::PeriodicWithoutReplacement)
                {
                    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedPeriodicRate) &&
                        BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedMinimumPeriodicRate))
                    {
                        if(mFrameRate >= command.GetRequestedPeriodicUpdateRate())
                            
                        {
                            confirmedRate = command.GetRequestedPeriodicUpdateRate();
                        }
                        else if(mFrameRate >= command.GetMinimumPeriodicRate())
                        {
                            confirmedRate = mFrameRate;
                        }
                        else
                        {
                            result = JAUS_FAILURE;
                            responseValue = RejectEventRequest::ConnectionRefused;
                            errorMessage = "Periodic Rate Not Supported";
                        }
                    }                    
                }
            }
        }
        else
        {
            errorMessage = "Only One Time, Every Change, or Periodic Events Supported";
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Whenever a Create Service Connection message is sent to the
///          component, the parent class (InformComponent) will call this
///          function to determine if the the SC will be created or not.
///
///   This method should not modify any internal values of your class.
///
///   \param command The Service Connection being requested.
///   \param responseValue The components response to the request.  See
///          ConfirmServiceConnection message for possible values.
///   \param confirmedRate If SC is supported, this output value will be
///          given the periodic rate that is supported/confirmed.
///
///   \return JAUS_OK if the service connection being requested is supported.
///           If supported, the responseValue will be set to
///           ConfirmServiceConnection::CreatedSuccessfully.  JAUS_FAILURE
///           if the SC cannot be created, and responseValue will reflect
///           the reason.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::ProcessServiceConnectionRequest(const Jaus::CreateServiceConnection& command,
                                                  Byte& responseValue,
                                                  double& confirmedRate) const
{
    int result = JAUS_FAILURE;

    responseValue = ConfirmServiceConnection::MessageNotSupported;

    if(command.GetMessageCode() == JAUS_REPORT_IMAGE) 
    {
        result = JAUS_OK;
        responseValue = ConfirmServiceConnection::CreatedSuccessfully;
        // If the desired rate is greater than the maximum
        // update rate of the sensor, then we'll set the confirmed
        // rate to the maximum update rate.  
        if (command.GetPeriodicUpdateRate() > mFrameRate)
        {
            confirmedRate = mFrameRate;
        }
        else
        {
            confirmedRate = command.GetPeriodicUpdateRate();
        }
    }
    else
    {
        result = InformComponent::ProcessServiceConnectionRequest(command, responseValue, confirmedRate);
    }
    
    return result;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a periodic or one time event needs to be generated, the
///   parent class (InformComponent) calls this function.  It then attempts
///   to generate event messages for the event specified.
///
///   This method should not modify any internal values of your class.
///
///   \param eventInfo The event that needs to be generated.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::GenerateEvent(const Event* eventInfo)
{
    int result = JAUS_FAILURE;
    if(eventInfo->GetMessageCode() == JAUS_REPORT_IMAGE)
    {
        ReportImage report;

        mImageMutex.Enter();
        // If we have not compressed the image data yet for
        // transport, do so now.
        if(mCompressionNumber != mFrameNumber)
        {
            if(mCompressedBufferSize < mRawImage.DataSize() + 256)
            {
                if(mpCompressedImage)
                {
                    delete[] mpCompressedImage;
                    mpCompressedImage = NULL;
                }
                mCompressedBufferSize = mRawImage.DataSize() + 256;
                mpCompressedImage = new Byte[mCompressedBufferSize];
                memset(mpCompressedImage, 0, mCompressedBufferSize);
                mCompressedSize = 0;
            }
            // Now compress the image data.
            mRawImage.Compress(&mpCompressedImage, mCompressedBufferSize, mCompressedSize, mImageFormat);
            mCompressionNumber = (UShort)mFrameNumber;
        }

        if(mpCompressedImage && mCompressedSize > 0)
        {
            report.SetImageData(mpCompressedImage, mCompressedSize, true);
        }
        mImageMutex.Leave();
       
        // Send event message to everyone.
        EventManager::GenerateEvent(eventInfo, &report, GetConnectionHandler());
    }
    else
    {
        // See if the parent class supports the event.
        result = InformComponent::GenerateEvent(eventInfo);
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function gets called every time a Service Connection 
///   (Subscription) needs to be updated.  The role of this function is to
///   check what type of subscription it is, generate the approprate report
///   message and send it.  This function gets called based on the periodic
///   update rate of each Service Connection.
///
///   <b>Make sure to set the Service Connection flag for your messages
///   since these are service connection messages!</b>
///
///   By default this function can handle JAUS_REPORT_IMAGE
///   connections and those of the InformComponent parent class.
///
///   Overload this virtual function to support additional types of 
///   Service Connections (SC)s/Subscriptions.
///
///   \param sc The service connection description.  Contains the type of
///             service connection, sequence number to use, etc.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::GenerateServiceConnectionMessage(const ServiceConnection& sc)
{
    int result = JAUS_FAILURE;
    if(sc.mMessageCode == JAUS_REPORT_IMAGE) 
    {
        ReportImage report; 
        //  Set the ID values for the message.
        report.SetSourceID(GetID());
        report.SetDestinationID(sc.mRequestorID);
        // Mark that the message is part of a Service Connection.
        report.SetServiceConnectionFlag(JAUS_SERVICE_CONNECTION);
        // Put the sequence number so the requesting components
        // can check that they are not oversampling the data.
        report.SetSequenceNumber(sc.mSequenceNumber);
        // Copy global pose data.
        mImageMutex.Enter();
        if(mpCompressedImage && mCompressedSize > 0)
        {
            report.SetImageData(mpCompressedImage, mCompressedSize, true);
        }
        mImageMutex.Leave();

        // Now if the presence vector matches, send the response.
        if(Send(&report))
        {
            return JAUS_OK;       
        }
    }
    else
    {
        result = InformComponent::GenerateServiceConnectionMessage(sc);
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Responds to the query message indicating the type of image
///   formats supported by this class.  Currently, this defaults to JPEG, PNG,
///   and PPM.  To change the format options, overload this method and choose 
///   what you wish to support.
///
///   \param msg Query message for camera format options.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::ReportCameraFormatOptions(const QueryCameraFormatOptions* msg) const
{
    if(msg->GetCameraID() == mCameraID)
    {
        Jaus::ReportCameraFormatOptions report;

        report.SetSourceID(GetID());
        report.SetDestinationID(msg->GetSourceID());
        report.SetCameraID(mCameraID);
        report.SetImageFormat1((Byte)Image::JPEG);
        report.SetImageFormat2((Byte)Image::PNG);
        report.SetImageFormat3((Byte)Image::PPM);

        return Send(&report);
    }
    // Wrong camera ID.
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the image format to report image data in.  Based on this 
///   value, different compression formats are used.
///
///   \param format Image compression format (see jimageformats.h) for
///                 options.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetImageFormat(const Image::Format format)
{
    mImageFormat = format;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the frame rate of the visual sensor.
///
///   \param fps Frame rate of the camera.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetFrameRate(const UShort fps)
{
    if(fps > 0 && fps <= 1092)
    {
        mFrameRate = fps;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the camera.
///
///   \param id Camera ID.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VisualSensor::SetCameraID(const Byte id)
{
    if(id >= 1)
    {
        mCameraID = id;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

/*  End of File */
