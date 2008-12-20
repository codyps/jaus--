////////////////////////////////////////////////////////////////////////////////////
///
///  \file velocitystatesensor.cpp
///  \brief This file contains software for creating a JAUS Velocity State
///  Sensor component (ID 42).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 18 May 2008
///  <br>Last Modified: 18 May 2008
///  <br>Copyright (c) 2007
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
#include "jaus/services/velocitystatesensor.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   \param updateRate Sensor update rate (Hz).
///
////////////////////////////////////////////////////////////////////////////////////
VelocityStateSensor::VelocityStateSensor(const double updateRate)
{
    mMaxUpdateRate = 0;
    mTravelSpeed = 0;
    if(updateRate > 0) { mMaxUpdateRate = updateRate; }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
VelocityStateSensor::~VelocityStateSensor()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the Velocity State Sensor component.
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
int VelocityStateSensor::Initialize(const Byte subsystem,
                                    const Byte node,
                                    const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(InformComponent::Initialize("Velocity State Sensor",
                                           Address(subsystem,
                                           node,
                                           (Byte)(Service::VelocityStateSensor),
                                           i)))
            {
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        return InformComponent::Initialize("Velocity State Sensor",
                                           Address(subsystem,
                                           node,
                                           (Byte)(Service::VelocityStateSensor),
                                           instance));
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the maximum update rate of the sensor generating data. This
///          value is used to deal with automatic creation of Service Connections
///          and Periodic Events.
///
///   \param r Max update rate [0.0166, 1092] Hz.  See limit values for
///            Service connections and Periodic Events.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VelocityStateSensor::SetSensorUpdateRate(const double r)
{
    if (r >= CreateServiceConnection::Limits::MinPeriodicRate &&
        r <= CreateServiceConnection::Limits::MaxPeriodicRate )
    {
        mMaxUpdateRate = r;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the current Velocity State data for reporting.
///
///   The presence vector of the Velocity State data provided must match
///   the presence vector in the services list for the Velocity State Sensor.
///
///   \param state The current velocity state information.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VelocityStateSensor::SetVelocityState(const ReportVelocityState& state)
{
    int result = JAUS_FAILURE;
    ReportVelocityState prevState;

    mVelocityStateMutex.Enter();
    if(IsOutputMessageSupported(JAUS_REPORT_VELOCITY_STATE, state.GetPresenceVector()))
    {
        prevState = mVelocityState;
        mVelocityState = state;
        result = JAUS_OK;
    }
    mVelocityStateMutex.Leave();

    if(result == JAUS_OK)
    {
        // Now that we've set the pose, lets see if we need
        // to generate any events.
        Event::Set myEvents;
        Event::Set::iterator e;

        mEventManager.Lock();
        myEvents = mEventManager.GetProducedEventsOfType(JAUS_REPORT_VELOCITY_STATE);
        for(e = myEvents.begin();
            e != myEvents.end();
            e++)
        {
            // Periodic events and one time events are generated
            // for us by the parent class (InformComponent), so don't
            // double generate.  Also, this implementation of
            // a Velocity State Sensor only supports Periodic, OneTime and
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
///   \brief Set the current Travel Speed of the platform so it can be
///          reported to subscribing components.
///
///   \param travelSpeed Current travel speed of the platform [0, 10000] m/s.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VelocityStateSensor::SetTravelSpeed(const double travelSpeed)
{
    int result = JAUS_FAILURE;

    if(travelSpeed < ReportTravelSpeed::Limits::MinSpeed ||
        travelSpeed > ReportTravelSpeed::Limits::MaxSpeed)
    {
        return result;
    }

    mVelocityStateMutex.Enter();
    if(IsOutputMessageSupported(JAUS_REPORT_VELOCITY_STATE, 0))
    {
        mTravelSpeed = travelSpeed;
        result = JAUS_OK;
    }
    mVelocityStateMutex.Leave();

    if(result == JAUS_OK)
    {
        // Now that we've set the pose, lets see if we need
        // to generate any events.
        Event::Set myEvents;
        Event::Set::iterator e;
        ReportTravelSpeed reportTravelSpeed;

        mEventManager.Lock();
        myEvents = mEventManager.GetProducedEventsOfType(JAUS_REPORT_TRAVEL_SPEED);
        for(e = myEvents.begin();
            e != myEvents.end();
            e++)
        {
            // Periodic events and one time events are generated
            // for us by the parent class (InformComponent), so don't
            // double generate.  Also, this implementation of
            // a Velocity State Sensor only supports Periodic, OneTime and
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
///   \return The most current travel speed data set.
///
////////////////////////////////////////////////////////////////////////////////////
double VelocityStateSensor::GetTravelSpeed() const
{
    double value = 0;
    mVelocityStateMutex.Enter();
    value = mTravelSpeed;
    mVelocityStateMutex.Leave();
    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The currently set velocity state data.
///
////////////////////////////////////////////////////////////////////////////////////
ReportVelocityState VelocityStateSensor::GetVelocityState() const
{
    ReportVelocityState value;
    mVelocityStateMutex.Enter();
    value = mVelocityState;
    mVelocityStateMutex.Leave();
    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function processes an written query message.
///
///   Checks to see if the message is a query for global pose and sends
///   the appropriate response.  If not, the default ProcessQueryMessage of
///   Component is used.
///
///   \param msg The written message to processes.
///
///   \return JAUS_OK if message processed.  JAUS_FAILURE if the message
///   cannot be processed.
///
////////////////////////////////////////////////////////////////////////////////////
int VelocityStateSensor::ProcessQueryMessage(const Message* msg)
{
    int result = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_VELOCITY_STATE:
        {
            const QueryVelocityState* query = dynamic_cast<const QueryVelocityState*>(msg);
            if(query)
            {
                result = RespondToQuery(query);
            }
        }
        break;
    case JAUS_QUERY_TRAVEL_SPEED:
        {
            const QueryTravelSpeed* query = dynamic_cast<const QueryTravelSpeed*>(msg);
            if(query)
            {
                ReportTravelSpeed report;

                report.SetSourceID(GetID());
                report.SetDestinationID(query->GetSourceID());

                mVelocityStateMutex.Enter();
                report.SetSpeed(mTravelSpeed);
                mVelocityStateMutex.Leave();

                result = Send(&report);
            }
        }
        break;
    default:
        // Let parent class process message.  That way we
        // respond to Core Messages, etc.
        result = InformComponent::ProcessQueryMessage(msg);
        break;
    }

    // Still let parent class process (in case dynamic discovery is
    // enabled and parent class needs this data too.
    if(result == JAUS_FAILURE)
    {
        result = InformComponent::ProcessQueryMessage(msg);
    }
    else
    {
        // Always run parent process command in case it needs the data too.
        InformComponent::ProcessQueryMessage(msg);
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
int VelocityStateSensor::GenerateEvent(const Event* eventInfo)
{
    int result = JAUS_FAILURE;
    if(eventInfo->GetMessageCode() == JAUS_REPORT_VELOCITY_STATE)
    {
        ReportVelocityState report;
        mVelocityStateMutex.Enter();
        report = mVelocityState;
        mVelocityStateMutex.Leave();

        if(eventInfo->GetQueryMessage())
        {
            const QueryVelocityState* query = dynamic_cast<const QueryVelocityState*>(eventInfo->GetQueryMessage());
            if(query)
            {
                // Remove fields not being requested.
                report.ClearFields(~query->GetPresenceVector());
            }
            // Send event message to everyone.
            EventManager::GenerateEvent(eventInfo, &report, GetConnectionHandler());
        }
        else
        {
            // Send event message to everyone.
            EventManager::GenerateEvent(eventInfo, &report, GetConnectionHandler());
        }
    }
    else if(eventInfo->GetMessageCode() == JAUS_REPORT_TRAVEL_SPEED)
    {
        ReportTravelSpeed report;
        mVelocityStateMutex.Enter();
        report.SetSpeed(mTravelSpeed);
        mVelocityStateMutex.Leave();
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
int VelocityStateSensor::ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                             Byte& responseValue,
                                             double& confirmedRate,
                                             std::string& errorMessage) const
{
    int result = JAUS_FAILURE;

    // Currently only supports events for global pose.
    if(command.GetMessageCode() == JAUS_REPORT_VELOCITY_STATE ||
        command.GetMessageCode() == JAUS_REPORT_TRAVEL_SPEED)
    {
        // Initialize the response value to something.
        responseValue = RejectEventRequest::MessageNotSupported;
        // This implementation of a Global Pose sensor only supports Periodic, EveryChange, or
        // OneTime events, so check for unsupported types.
        if(command.GetEventType() != CreateEventRequest::FirstChange &&
            command.GetEventType() != CreateEventRequest::FirstChangeBoundaries)
        {
            Byte pv = command.GetPresenceVector();
            // This implementation of a Global Pose Sensor will only support events when any
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
                    const QueryVelocityState* query = dynamic_cast<const QueryVelocityState*>(command.GetQueryMessage());
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
                // If the event requested is periodic, then we need
                // to verify we can support the periodic update rates being requested.
                if(command.GetEventType() == CreateEventRequest::Periodic ||
                    command.GetEventType() == CreateEventRequest::PeriodicWithoutReplacement)
                {
                    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedPeriodicRate) &&
                        BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedMinimumPeriodicRate))
                    {
                        if(mMaxUpdateRate >= command.GetRequestedPeriodicUpdateRate())

                        {
                            confirmedRate = command.GetRequestedPeriodicUpdateRate();
                        }
                        else if(mMaxUpdateRate >= command.GetMinimumPeriodicRate())
                        {
                            confirmedRate = mMaxUpdateRate;
                        }
                        else
                        {
                            result = JAUS_FAILURE;
                            responseValue = RejectEventRequest::ConnectionRefused;
                            errorMessage = "Periodic Rate Not Supported";
                        }
                    }
                    else if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedPeriodicRate))
                    {
                        if(mMaxUpdateRate >= command.GetRequestedPeriodicUpdateRate())

                        {
                            confirmedRate = command.GetRequestedPeriodicUpdateRate();
                        }
                        else
                        {
                            confirmedRate = mMaxUpdateRate;
                        }
                    }
                    else if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedMinimumPeriodicRate))
                    {
                        if(mMaxUpdateRate >= command.GetMinimumPeriodicRate())
                        {
                            confirmedRate = mMaxUpdateRate;
                        }
                        else
                        {
                            result = JAUS_FAILURE;
                            responseValue = RejectEventRequest::ConnectionRefused;
                            errorMessage = "Minimum Requested Periodic Rate Greater Than Max Update Rate";
                        }
                    }
                    else
                    {
                        result = JAUS_FAILURE;
                        responseValue = RejectEventRequest::InvalidEventSetup;
                        errorMessage = "Event Configuration Invalid";
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
///   \brief Sends a report velocity state message in response to the query.
///
///   \param query The query for velocity state.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int VelocityStateSensor::RespondToQuery(const QueryVelocityState* query)
{
    ReportVelocityState report;

    mVelocityStateMutex.Enter();
    report = mVelocityState;
    mVelocityStateMutex.Leave();
    //  Set the ID values for the message.
    report.SetSourceID(GetID());
    report.SetDestinationID(query->GetSourceID());
    // Remove the fields not being requested.
    report.ClearFields( ~query->GetPresenceVector() );
    // Now if the presence vector matches, send the response.
    if(Send(&report))
    {
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


/*  End of File */
