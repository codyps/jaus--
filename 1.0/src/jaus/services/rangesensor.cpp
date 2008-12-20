////////////////////////////////////////////////////////////////////////////////////
///
///  \file rangesensor.cpp
///  \brief This file contains software for creating a JAUS Range
///  Sensor Component (ID 50).
///
///  <br>Author(s): David Adams
///  <br>Created: 06 October 2008
///  <br>Last Modified: 06 October 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#include "jaus/services/rangesensor.h"
#include "jaus/messages/experimental/sick/querysicklidar.h"
#include <iostream>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
RangeSensor::RangeSensor()
{

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
RangeSensor::~RangeSensor()
{

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the Range Sensor component.
///
///   This method overrides the parent class Initialize function, filling in
///   the component name and component type number.  
///
///   \param subsystem Subsystem ID number.
///   \param node Node ID number.
///   \param instance Instance ID to use.  Set to 0 to find first available
///             instance.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RangeSensor::Initialize(const Byte subsystem,
                            const Byte node,
                            const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(InformComponent::Initialize("Range Sensor", 
                                           Address(subsystem, 
                                           node, 
                                           (Byte)(Service::RangeSensor), 
                                           i)))
            {
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        return InformComponent::Initialize("Range Sensor", 
                                           Address(subsystem, 
                                           node, 
                                           (Byte)(Service::RangeSensor), 
                                           instance));
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the component as supporting the Range Sensor service.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RangeSensor::SetupService()
{
    Service service;

    service.SetType(Service::RangeSensor);
    service.AddInputMessage(JAUS_QUERY_RELATIVE_OBJECT_POSITION, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_RELATIVE_OBJECT_POSITION));
    service.AddOutputMessage(JAUS_REPORT_RELATIVE_OBJECT_POSITION, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_RELATIVE_OBJECT_POSITION));
    service.AddInputMessage(JAUS_QUERY_SICK_LIDAR, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_SICK_LIDAR));
    service.AddOutputMessage(JAUS_REPORT_SICK_LIDAR, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_SICK_LIDAR));

    return AddService(service);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the current Range Sensor data for reporting.
///
///   The presence vector of the message data provided must compatible with
///   the presence vector in the services list for your Range Sensor.
///
///   \param state The current relative object position information.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RangeSensor::SetRelativeObjectPosition(const ReportRelativeObjectPosition& state)
{
    int result = JAUS_FAILURE;
    ReportRelativeObjectPosition prevState;

    mRangeSensorMutex.Enter();
    if(IsOutputMessageSupported(JAUS_REPORT_RELATIVE_OBJECT_POSITION, state.GetPresenceVector()))
    {
        prevState = mRelativeObjectPosition;
        mRelativeObjectPosition = state;
        result = JAUS_OK;
    }
    mRangeSensorMutex.Leave();

    if(result == JAUS_OK)
    {
        // Now that we've updated our internal data, lets see if we
        // need to generate any events.

        Event::Set myEvents;
        Event::Set::iterator e;

        mEventManager.Lock();
        myEvents = mEventManager.GetProducedEventsOfType(JAUS_REPORT_RELATIVE_OBJECT_POSITION);
        for(e = myEvents.begin();
            e != myEvents.end();
            e++)
        {
            // Periodic events and one time events are generated
            // for us by the parent class (InformComponent), so don't
            // double generate.  Also, this implementation of
            // a Range Sensor only supports Periodic, OneTime and
            // EveryChange events of this type.
            if((*e)->GetEventType() == Event::EveryChange)
            {
                GenerateEvent(*e);
                // Update sequence number and timestamp.
                (*e)->SetSequenceNumber((*e)->GetSequenceNumber() + 1);
                (*e)->SetTimeStampMs(Time::GetUtcTimeMs());
            }
        }

        mEventManager.Unlock();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the current LIDAR scan data for reporting.
///
///   \param lidarScan LIDAR Scan data to report.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RangeSensor::SetSickLidarData(const ReportSickLidar& lidarScan)
{   
    if(lidarScan.GetScanData()->size() == 0)
    {
        return JAUS_FAILURE;
    }

    mRangeSensorMutex.Enter();
    mSickLidarData = lidarScan;
    mRangeSensorMutex.Leave();

    // Now that we've updated our internal data, lets see if we
    // need to generate any events.

    Event::Set myEvents;
    Event::Set::iterator e;

    mEventManager.Lock();
    myEvents = mEventManager.GetProducedEventsOfType(JAUS_REPORT_SICK_LIDAR);
    for(e = myEvents.begin();
        e != myEvents.end();
        e++)
    {
        // Periodic events and one time events are generated
        // for us by the parent class (InformComponent), so don't
        // double generate.  Also, this implementation of
        // a Range Sensor only supports Periodic, OneTime and
        // EveryChange events of this type.
        if((*e)->GetEventType() == Event::EveryChange)
        {
            GenerateEvent(*e);
            // Update sequence number and timestamp.
            (*e)->SetSequenceNumber((*e)->GetSequenceNumber() + 1);
            (*e)->SetTimeStampMs(Time::GetUtcTimeMs());
        }
    }

    mEventManager.Unlock();

    return OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function processes a query message.
///
///   Checks to see if the message is a query for relative object position and sends
///   the appropriate response.  If not, the default ProcessQueryMessage of
///   Component is used.
///
///   \param msg The query message to respond to.
///
///   \return JAUS_OK if message processed.  JAUS_FAILURE if the message 
///   cannot be processed.
///
////////////////////////////////////////////////////////////////////////////////////
int RangeSensor::ProcessQueryMessage(const Message* msg)
{
    int processed = JAUS_FAILURE;

    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_RELATIVE_OBJECT_POSITION:
        {
            const QueryRelativeObjectPosition* query = dynamic_cast<const QueryRelativeObjectPosition*>(msg);
            if(query)
            {
                ReportRelativeObjectPosition report;    
                mRangeSensorMutex.Enter();
                report = mRelativeObjectPosition;
                mRangeSensorMutex.Leave();
                //  Set the ID values for the message.
                report.SetSourceID(GetID());
                report.SetDestinationID(query->GetSourceID());
                // Remove the fields not being requested.
                report.ClearFields( ~query->GetPresenceVector() );
                // Now if the presence vector matches, send the response.
                Send(&report);
                processed = JAUS_OK;
            }
        }
        break;
    case JAUS_QUERY_SICK_LIDAR:
        {
            ReportSickLidar report = GetSickLidarData();
            if(report.GetScanData()->size() > 0)
            {
                report.SetSourceID(GetID());
                report.SetDestinationID(msg->GetSourceID());
                Send(&report);
            }
            processed = JAUS_OK;
        }
        break;
    default:
        // Let parent class process message.  That way we
        // respond to Core Messages, etc.
        processed = InformComponent::ProcessQueryMessage(msg);
        break;
    }

    return processed;
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
int RangeSensor::GenerateEvent(const Event* eventInfo)
{
    int result = JAUS_FAILURE;
    if(eventInfo->GetMessageCode() == JAUS_REPORT_RELATIVE_OBJECT_POSITION)
    {
        ReportRelativeObjectPosition report;
        mRangeSensorMutex.Enter();
        report = mRelativeObjectPosition;
        mRangeSensorMutex.Leave();

        if(eventInfo->GetQueryMessage())
        {
            const QueryRelativeObjectPosition* query = dynamic_cast<const QueryRelativeObjectPosition*>(eventInfo->GetQueryMessage());          
            if(query)
            {
                // Remove fields not being requested.
                report.ClearFields(~query->GetPresenceVector());
            }            
        }
        // Send event message to everyone.
        EventManager::GenerateEvent(eventInfo, &report, GetConnectionHandler());
        result = JAUS_OK;
    }
    else if(eventInfo->GetMessageCode() == JAUS_REPORT_SICK_LIDAR)
    {
        ReportSickLidar report = GetSickLidarData();
        if(report.GetScanData()->size() > 0)
        {
            EventManager::GenerateEvent(eventInfo, &report, GetConnectionHandler());
            result = JAUS_OK;
        }
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
int RangeSensor::ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                     Byte& responseValue,
                                     double& confirmedRate,
                                     std::string& errorMessage) const
{
    int result = JAUS_FAILURE;
   
    // Currently only supports events for global pose.
    if(command.GetMessageCode() == JAUS_REPORT_RELATIVE_OBJECT_POSITION ||
        command.GetMessageCode() == JAUS_REPORT_SICK_LIDAR)
    {
        // Initialize the response value to something.
        responseValue = RejectEventRequest::MessageNotSupported;
        // This implementation of a Global Pose sensor only supports Periodic, EveryChange, or
        // OneTime events, so check for unsupported types.
        if(command.GetEventType() == CreateEventRequest::EveryChange ||
            command.GetEventType() == CreateEventRequest::OneTime)
        {
            Byte pv = command.GetPresenceVector();
            // This implementation of a Range Sensor will only support events when any
            // value changes, not on boundaries or lower limites, etc.  If you want to do this, 
            // feel free to change the code. :)
            if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit) == false &&
                BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State) == false)
            {
                // Finally, check and if there is a query message associated with
                // the Event.  If there is, we must make sure that our sensor
                // supplies all the data being requested by check our presence vector.
                if(command.GetQueryMessage())
                {
                    const QueryRelativeObjectPosition* query = dynamic_cast<const QueryRelativeObjectPosition*>(command.GetQueryMessage());
                    if(query)
                    {
                        if(IsOutputMessageSupported(MessageCreator::GetQueryResponseType(query->GetCommandCode()), 
                                                    query->GetPresenceVector()))
                        {
                            result = JAUS_OK;
                        }
                    }
                }
                // If on query, then go ahead and say we can support the event.
                else
                {
                    result = JAUS_OK;
                }                
            }
        }
        else
        {
            errorMessage = "Only One Time and Every Change Events Supported";
        }
        
        return result;
    }

    return InformComponent::ProcessEventRequest(command,
                                                responseValue,
                                                confirmedRate,
                                                errorMessage);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Returns a copy of the relative object position.
///
////////////////////////////////////////////////////////////////////////////////////
ReportRelativeObjectPosition RangeSensor::GetRelativeObjectPosition() const
{
    mRangeSensorMutex.Enter();
    ReportRelativeObjectPosition ret = mRelativeObjectPosition;
    mRangeSensorMutex.Leave();
    return ret;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Returns a copy of the SICK LIDAR message data.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSickLidar RangeSensor::GetSickLidarData() const
{
    mRangeSensorMutex.Enter();
    ReportSickLidar ret = mSickLidarData;
    mRangeSensorMutex.Leave();
    return ret;
}


/*  End of File */
