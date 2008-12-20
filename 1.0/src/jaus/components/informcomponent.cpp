////////////////////////////////////////////////////////////////////////////////////
///
///  \file informcomponent.cpp
///  \brief This file contains software for creating JAUS components that
///  primarily act as informers.  An informer is typically a sensor or
///  component that provides information to other components.  The class
///  described here InformComponent is an advanced version of Component
///  with methods to simplify the creation of events and service connections
///  for notifying other services.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 July 2007
///  <br>Last Modified: 4 April 2008
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
#include "jaus/components/informcomponent.h"
#include "jaus/messages/msglib.h"
#include <cxutils/timer.h>
#include <iostream>
#ifndef WIN32
#include <unistd.h>
#endif

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
InformComponent::HighPerformanceTimer::HighPerformanceTimer() : mpServiceConnection(0),
                                                                mpEventKey(0),
                                                                mpInformComponent(0),
                                                                mpScMutex(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, stops the timer.
///
////////////////////////////////////////////////////////////////////////////////////
InformComponent::HighPerformanceTimer::~HighPerformanceTimer()
{
    mTimer.StopPeriodicTimer();
    if(mpEventKey)
    {
        delete mpEventKey;
        mpEventKey = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Generates a periodic event.
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::HighPerformanceTimer::ProcessTimerEvent(const unsigned int id)
{
    if(mpInformComponent && mpServiceConnection)
    {
        if(mpScMutex) { mpScMutex->Enter(); }
        mpInformComponent->GenerateServiceConnectionMessage(*mpServiceConnection);
        mpServiceConnection->mUpdateTimeMs = Time::GetUtcTimeMs();
        mpServiceConnection->mSequenceNumber++;
        if(mpScMutex) { mpScMutex->Leave(); }
    }
    else if(mpInformComponent && mpEventKey)
    {
        EventManager* events;
        Event* eventInfo;
        events = mpInformComponent->GetEvents();
        events->Lock();
        eventInfo = events->GetEvent(*mpEventKey);
        if(eventInfo)
        {
            Byte sequenceNumber = eventInfo->GetSequenceNumber();
            mpInformComponent->GenerateEvent(eventInfo);
            // Update event stats.
            eventInfo->SetSequenceNumber(sequenceNumber + 1);
            eventInfo->SetTimeStampMs(Time::GetUtcTimeMs());
        }
        events->Unlock();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
InformComponent::InformComponent() : mUseHighPerformanceTimerFlag(false),
                                     mTimerThresholdFrequency(30)
{

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
InformComponent::~InformComponent()
{
    TerminateServiceConnections();
    CancelRequestedEvents();

    HighPerformanceTimerSet::iterator hpt;
    // Stop all high frequency timers.
    mHighPerformanceTimersMutex.Enter();
    for(hpt = mHighPerformanceTimers.begin();
        hpt != mHighPerformanceTimers.end();
        hpt++)
    {
        bool scMutex = false;
        // Use the correct mutex based on
        // the kind of subscription being produced.
        if((*hpt)->mpServiceConnection)
        {
            scMutex = true;
            mServiceConnectionsMutex.Enter();
        }
        else
        {
            mEventManager.Lock();
        }

        (*hpt)->mTimer.StopPeriodicTimer();
        delete (*hpt);

        if(scMutex)
        {
            scMutex = true;
            mServiceConnectionsMutex.Unlock();
        }
        else
        {
            mEventManager.Unlock();
        }
    }
    mHighPerformanceTimers.clear();
    mHighPerformanceTimersMutex.Leave();

    mPeriodicEventThread.StopThread();
    SubscriberComponent::Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Performs additional Shutdown procedures before calling
///   the parent class Shutdown method.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::Shutdown()
{
    TerminateServiceConnections();
    CancelRequestedEvents();

    HighPerformanceTimerSet::iterator hpt;
    // Stop all high frequency timers.
    mHighPerformanceTimersMutex.Enter();
    for(hpt = mHighPerformanceTimers.begin();
        hpt != mHighPerformanceTimers.end();
        hpt++)
    {
        bool scMutex = false;
        // Use the correct mutex based on
        // the kind of subscription being produced.
        if((*hpt)->mpServiceConnection)
        {
            scMutex = true;
            mServiceConnectionsMutex.Enter();
        }
        else
        {
            mEventManager.Lock();
        }

        (*hpt)->mTimer.StopPeriodicTimer();
        delete (*hpt);

        if(scMutex)
        {
            scMutex = true;
            mServiceConnectionsMutex.Unlock();
        }
        else
        {
            mEventManager.Unlock();
        }
    }
    mHighPerformanceTimers.clear();
    mHighPerformanceTimersMutex.Leave();

    mPeriodicEventThread.StopThread();
    return SubscriberComponent::Shutdown();
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
int InformComponent::SetupCoreService()
{
    Service::Map services;
    Service::Map::iterator coreServices;

    // Initialize core services from parent.
    SubscriberComponent::SetupCoreService();

    services = GetServices();

    coreServices = services.find(Service::CoreMessageSupport);
    // Add additional message to core set.
    if(coreServices != services.end())
    {
        // Add additional messages for events.
        coreServices->second.AddInputMessage(JAUS_QUERY_EVENTS, 0);
        coreServices->second.AddOutputMessage(JAUS_REPORT_EVENTS, 0);

        // Add the updated version of the core services to component
        AddService(coreServices->second);
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enable/disable use of high performance timer for generation of
///   periodic events and service connection messages.
///
///   The high performance timer allows the generation of messages and very
///   precise frequencies, but with a trade off of higher CPU requirements.
///   Therefore, unless you require precise periodic events, do not enable
///   this feature.
///
///   Any periodic events or service connections that have a frequency higher
///   than that set using the SetHighFrequencyTimerThreshold function (default
///   value is 50 Hz) automatically uses the High Performance Timer regardless
///   of the value of this flag because events cannot be generated at those
///   speeds with traditional methods.
///
///   \param on If true HPT is used by default, otherwise only when above
///             preset threshold.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::UseHighPerformanceTimer(const bool on)
{
    mUseHighPerformanceTimerFlag = on;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the frequency threshold used to determine if the
///   high performance timer (HPT) should be used for periodic event and
///   service connection message generation.
///
///   \param freq The frequency (Hz) that if any service connection or
///               periodic event is above this value the HPT is used. Must
///               be a value larger than 5 Hz since this rate can be
///               supported easily without an HPT (save processing power).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::SetHighPerformanceTimerThreshold(const double freq)
{
    if(freq > 5)
    {
        mHighPerformanceTimersMutex.Enter();
        mTimerThresholdFrequency = freq;
        mHighPerformanceTimersMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing command messages.  This method is called
///   whenever a command message is received.  Overload this method to
///   add support for other input message types.
///
///   This version (which overloads the one in Component) has additional
///   functionality for dealing with the creation/suspension/termination, etc. of
///   service connections.  It also has methods for dealing with requests
///   to create/cancel/update events.
///
///   \param msg Command message to process.
///   \param commandAuthority The authority code/level of the source of the message.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::ProcessCommandMessage(const Message* msg, const Byte commandAuthority)
{
    int processed = JAUS_OK;

    // If component does not have minimum authority to
    // use this component, ignore command.  If from Node Manager
    // then allow.
    if(commandAuthority >= GetComponentAuthority() ||
        msg->GetSourceID() == Address(GetID().mSubsystem, GetID().mNode, 1, 1))
    {
        switch(msg->GetCommandCode())
        {
        case JAUS_CREATE_EVENT:
            {
                const Jaus::CreateEventRequest* command = dynamic_cast<const Jaus::CreateEventRequest*>(msg);
                Byte responseValue = RejectEventRequest::ConnectionRefused;
                double confirmedRate = 0.0;
                std::string errorMessage;

                if(command == NULL) { return processed; }

                if(ProcessEventRequest(*command, responseValue, confirmedRate, errorMessage) == JAUS_OK)
                {
                    ConfirmEventRequest response;
                    Jaus::Event* eventInfo = NULL;

                    // Send confirmation message for event creation.
                    response.SetSourceID(GetID());
                    response.SetDestinationID(command->GetSourceID());
                    response.SetMessageCode(command->GetMessageCode());
                    response.SetRequestID(command->GetRequestID());
                    if(command->GetEventType() == Event::Periodic ||
                        command->GetEventType() == Event::PeriodicWithoutReplacement)
                    {
                        response.SetConfirmedPeriodicUpdateRate(confirmedRate);
                    }

                    // Create the event!
                    mEventManager.Lock();
                    eventInfo = mEventManager.GetEvent(*command);
                    // If event already exists, then add the subscriber.
                    if(eventInfo)
                    {
                        response.SetEventID(eventInfo->GetEventID());
                        Send(&response);
                        eventInfo->GetEventSubscribers()->insert(command->GetSourceID());
                    }
                    else
                    {
                        // Create a new event
                        eventInfo = new Event(*command);
                        eventInfo->SetEventProvider(GetID());
                        eventInfo->GetEventSubscribers()->insert(command->GetSourceID());
                         // Get an event ID value.
                        eventInfo->SetEventID(mEventManager.GenerateEventID(command->GetMessageCode()));
                        eventInfo->SetTimeStampMs(0);
                        response.SetEventID(eventInfo->GetEventID());

                        if(eventInfo->IsPeriodic())
                        {
                            eventInfo->SetPeriodicRate(confirmedRate);
                            // If the SC operates at a rate higher than some frequency, we need to use
                            // a high performance timer to generate events fast enough.
                            if(mUseHighPerformanceTimerFlag || confirmedRate >= mTimerThresholdFrequency)
                            {
                                Event::Key key = eventInfo->GetKey();
                                CreateHighPerformanceTimer(confirmedRate, NULL, &key);
                            }
                            else
                            {
                                if(mPeriodicEventThread.IsThreadActive() == false)
                                {
                                    mPeriodicEventThread.CreateThread(InformComponent::PeriodicEventThreadFunction, this);
                                    mPeriodicEventThread.SetThreadName("Periodic Events");
                                }
                            }
                        }
                        // If this is a one time event, generate the data
                        // then finish.
                        if(eventInfo->GetEventType() == Event::OneTime)
                        {
                            Send(&response);
                            // Now generate the event, followed by data deletion.
                            GenerateEvent(eventInfo);
                            delete eventInfo;
                        }
                        else
                        {
                            Send(&response);
                            mEventManager.AddEvent(eventInfo);
                        }
                    }

                    mEventManager.Unlock();
                }
                else
                {
                    // Reject the event.
                    RejectEventRequest response;
                    response.SetSourceID(GetID());
                    response.SetDestinationID(command->GetSourceID());
                    response.SetRequestID(command->GetRequestID());
                    response.SetResponseCode((RejectEventRequest::ResponseCode)responseValue);
                    if(errorMessage.empty() == false)
                    {
                        response.SetErrorMessage(errorMessage);
                    }
                    Send(&response);
                }
            }
            break;
        case JAUS_CANCEL_EVENT:
            {
                const Jaus::CancelEvent* command = dynamic_cast<const Jaus::CancelEvent*>(msg);
                if(command)
                {
                    Event::Key key;
                    Event* eventInfo;
                    mEventManager.Lock();
                    // First delete any HPT if used
                    eventInfo = mEventManager.GetEvent(*command);
                    if(eventInfo)
                    {
                        key = eventInfo->GetKey();
                        StopHighPerformanceTimer(NULL, &key);
                        mEventManager.CancelProducedEvent(*command, this->GetConnectionHandler());
                    }
                    mEventManager.Unlock();
                }
            }
            break;
        case JAUS_UPDATE_EVENT:
            {
                const Jaus::UpdateEvent* command = dynamic_cast<const Jaus::UpdateEvent*>(msg);
                if(command)
                {
                    // See if the component will support the new event.
                    bool eventExits = false;
                    Byte responseValue;
                    double confirmedRate;
                    responseValue = RejectEventRequest::ConnectionRefused;
                    std::string errorMessage;

                    Event::Key key;

                    mEventManager.Lock();
                    // First see if the event exists
                    if(mEventManager.GetEvent(*command) != NULL)
                    {
                        eventExits = true;
                    }
                    mEventManager.Unlock();

                    // If changes supported by the component, and event exits.
                    if(eventExits &&
                        ProcessEventUpdateRequest(*command, responseValue, confirmedRate, errorMessage) == JAUS_OK)
                    {
                        ConfirmEventRequest response;
                        Jaus::Event* eventInfo = NULL;

                        response.SetSourceID(GetID());
                        response.SetDestinationID(command->GetSourceID());
                        response.SetMessageCode(command->GetMessageCode());
                        response.SetRequestID(command->GetRequestID());
                        response.SetEventID(command->GetEventID());

                        // Update the event.
                        mEventManager.Lock();
                        eventInfo = mEventManager.GetEvent(*command);
                        // If event already exists, then add the subscriber.
                        if(eventInfo)
                        {
                            // The first thing we must do is check and see if there
                            // are other subscribers to this event, and if so, we
                            // must create a new event so that we don't disrupt
                            // the event for other subscribers.
                            if(eventInfo->GetEventSubscribers()->size() > 1)
                            {
                                // Remove the current subscriber from the previous event.
                                eventInfo->GetEventSubscribers()->erase(eventInfo->GetEventSubscribers()->find(command->GetSourceID()));
                                // Make a copy, then get a new event ID.
                                eventInfo = new Jaus::Event(*eventInfo);
                                eventInfo->SetEventID(mEventManager.GenerateEventID(eventInfo->GetMessageCode()));
                                mEventManager.AddEvent(eventInfo);
                                key = eventInfo->GetKey();
                            }
                            else
                            {
                                // Stop any HPT if one is running, since we'll probably
                                // not need it anymore, or need to change the update interval.
                                key = eventInfo->GetKey();
                                StopHighPerformanceTimer(NULL, &key);
                            }

                            // Copy new values from Update Message.
                            eventInfo->Update(*command);
                            eventInfo->GetEventSubscribers()->insert(command->GetSourceID());
                            eventInfo->SetEventProvider(GetID());

                            // If periodic we may need to create an HPT.
                            if(eventInfo->IsPeriodic())
                            {
                                eventInfo->SetPeriodicRate(confirmedRate);
                                response.SetConfirmedPeriodicUpdateRate(confirmedRate);
                                Send(&response);
                                // If the SC operates at a rate higher than some frequency, we need to use
                                // a high performance timer to generate events fast enough.
                                if(mUseHighPerformanceTimerFlag || confirmedRate >= mTimerThresholdFrequency)
                                {
                                    CreateHighPerformanceTimer(confirmedRate, NULL, &key);
                                }

                            }
                            // If this is a one time event, generate the data
                            // then finish.
                            else if(eventInfo->GetEventType() == Event::OneTime)
                            {
                                // First confirm event creation.
                                Send(&response);
                                // Now generate the event, followed by data deletion.
                                GenerateEvent(eventInfo);
                                mEventManager.DeleteEvent(eventInfo);
                            }
                            else
                            {
                                Send(&response);
                            }
                        }
                        mEventManager.Unlock();
                    }
                    else
                    {
                        RejectEventRequest response;
                        response.SetSourceID(GetID());
                        response.SetDestinationID(command->GetSourceID());
                        response.SetRequestID(command->GetRequestID());
                        response.SetResponseCode((RejectEventRequest::ResponseCode)responseValue);
                        if(errorMessage.empty() == false)
                        {
                            response.SetErrorMessage(errorMessage);
                        }
                        Send(&response);
                    }
                }
            }
            break;
        case JAUS_CREATE_SERVICE_CONNECTION:
            {
                const Jaus::CreateServiceConnection* command = dynamic_cast<const Jaus::CreateServiceConnection*>(msg);
                ConfirmServiceConnection response;

                response.SetSourceID(GetID());
                response.SetDestinationID(msg->GetSourceID());
                response.SetMessageCode(command->GetMessageCode());

                if(command && IsInformMessage(command->GetMessageCode()))
                {
                    Byte responseValue = ConfirmServiceConnection::Refused;
                    double confirmedRate = 0.0;
                    if(ProcessServiceConnectionRequest(*command, responseValue, confirmedRate) == JAUS_OK)
                    {
                        response.SetResponseCode(responseValue);
                        response.SetConfirmedPeriodicUpdateRate(confirmedRate);
                        // If response is success, then add to our SC map.
                        if(responseValue == ConfirmServiceConnection::CreatedSuccessfully)
                        {
                            ServiceConnectionMap::iterator sc;
                            ServiceConnection* scPtr = NULL;
                            std::set<Byte> instanceValues;
                            bool match = false;
                            Byte instanceID = 0;
                            bool rateIncrease = false;

                            mServiceConnectionsMutex.Enter();
                            for(sc = mServiceConnections.begin();
                                sc != mServiceConnections.end();
                                sc++)
                            {
                                if(sc->second.mMessageCode == response.GetMessageCode() &&
                                    sc->second.mPresenceVector == command->GetPresenceVector() &&
                                    sc->second.mProviderID == GetID())
                                {
                                    match = true;
                                    scPtr = &sc->second;
                                    response.SetInstanceID( sc->second.mInstanceID );
                                    // If the new confirmed rate is greater than the
                                    // previous confirmed rate, then we need to update to the
                                    // faster rate.
                                    if(confirmedRate > sc->second.mPeriodicRate )
                                    {
                                        rateIncrease = true;
                                        sc->second.mPeriodicRate = confirmedRate;
                                        sc->second.mUpdateIntervalMs = 1000.0/(confirmedRate + JAUS_EPSILON);
                                    }
                                    break;
                                }

                                // Generate a unique instance ID by finding out how many
                                // other SC's for this message type already exist.
                                if(sc->second.mMessageCode == response.GetMessageCode())
                                {
                                    // Add the instance ID of the SC to the
                                    // set of all instance ID's in use.  We will use
                                    // this set to generate a new ID.
                                    instanceValues.insert(sc->second.mInstanceID);
                                }
                            }
                            // If an exisiting SC does not exist, then create one.
                            if(match == false)
                            {
                                ServiceConnection::Key key;
                                key.mProviderID = GetID();
                                key.mMessageCode = command->GetMessageCode();
                                key.mPresenceVector = command->GetPresenceVector();

                                mServiceConnections[key].mPresenceVector = key.mPresenceVector;
                                mServiceConnections[key].mProviderID = key.mProviderID;
                                mServiceConnections[key].mMessageCode = key.mMessageCode;
                                mServiceConnections[key].mRequestorID = command->GetSourceID();
                                mServiceConnections[key].mPeriodicRate = confirmedRate;
                                mServiceConnections[key].mUpdateIntervalMs = 1000.0/(confirmedRate + JAUS_EPSILON);
                                // Generate a unique instance ID.
                                while(instanceValues.find(instanceID) != instanceValues.end())
                                {
                                    instanceID++;
                                    // Safety check (we should never get here, unless
                                    // JAUS changes.
                                    if(instanceID == 255)
                                        break;
                                }
                                mServiceConnections[key].mInstanceID = instanceID;
                                response.SetInstanceID(instanceID);
                                mServiceConnections[key].mPrimaryStatus = ServiceConnection::Active;
                                scPtr = &mServiceConnections[key];
                            }

                            // If the SC operates at a rate higher than some frequency, we need to use
                            // a high performance timer to generate events fast enough.
                            if((mUseHighPerformanceTimerFlag || confirmedRate >= mTimerThresholdFrequency) && scPtr)
                            {
                                CreateHighPerformanceTimer(confirmedRate, scPtr, NULL);
                            }

                            mServiceConnectionsMutex.Leave();
                        }
                    }
                    else
                    {
                        response.SetResponseCode(ConfirmServiceConnection::Refused);
                    }
                }
                else
                {
                    response.SetResponseCode(ConfirmServiceConnection::Refused);
                }
                // Create thread for periodic events if needed.
                if(response.GetResponseCode() == ConfirmServiceConnection::CreatedSuccessfully)
                {
                    if(mPeriodicEventThread.IsThreadActive() == false)
                    {
                        mPeriodicEventThread.CreateThread(InformComponent::PeriodicEventThreadFunction, this);
                        mPeriodicEventThread.SetThreadName("Periodic Events");
                    }
                }
                Send(&response);
            }
            break;
        case JAUS_SUSPEND_SERVICE_CONNECTION:
            {
                const Jaus::SuspendServiceConnection* command = dynamic_cast<const Jaus::SuspendServiceConnection*>(msg);

                if(command)
                {
                    ServiceConnectionMap::iterator sc;
                    mServiceConnectionsMutex.Enter();
                    for(sc = mServiceConnections.begin();
                        sc != mServiceConnections.end();
                        sc++)
                    {
                        if(sc->second.mMessageCode == command->GetMessageCode() &&
                            sc->second.mInstanceID == command->GetInstanceID())
                        {
                            sc->second.mPrimaryStatus = ServiceConnection::Suspended;
                            StopHighPerformanceTimer(&sc->second, NULL);
                            break;
                        }
                    }
                    mServiceConnectionsMutex.Leave();
                }
            }
            break;
        case JAUS_ACTIVATE_SERVICE_CONNECTION:
            {
                const Jaus::ActivateServiceConnection* command = dynamic_cast<const Jaus::ActivateServiceConnection*>(msg);

                if(command)
                {
                    ServiceConnectionMap::iterator sc;
                    mServiceConnectionsMutex.Enter();
                    for(sc = mServiceConnections.begin();
                        sc != mServiceConnections.end();
                        sc++)
                    {
                        if(sc->second.mMessageCode == command->GetMessageCode() &&
                            sc->second.mInstanceID == command->GetInstanceID())
                        {
                            sc->second.mPrimaryStatus = ServiceConnection::Active;
                            if(mUseHighPerformanceTimerFlag || sc->second.mPeriodicRate >= mTimerThresholdFrequency)
                            {
                                CreateHighPerformanceTimer(sc->second.mPeriodicRate, &sc->second, NULL);
                            }
                            break;
                        }
                    }
                    mServiceConnectionsMutex.Leave();
                }
            }
            break;
        case JAUS_TERMINATE_SERVICE_CONNECTION:
            {
                const Jaus::TerminateServiceConnection* command = dynamic_cast<const Jaus::TerminateServiceConnection*>(msg);

                if(command)
                {
                    ServiceConnectionMap::iterator sc;
                    mServiceConnectionsMutex.Enter();
                    for(sc = mServiceConnections.begin();
                        sc != mServiceConnections.end();
                        sc++)
                    {
                        // Only allow terminate messages to cancel service connections
                        // we are providing.
                        if(IsInformMessage(sc->second.mMessageCode) &&
                            sc->second.mMessageCode == command->GetMessageCode() &&
                            sc->second.mInstanceID == command->GetInstanceID() &&
                            sc->second.mProviderID == GetID())
                        {
                            StopHighPerformanceTimer(&sc->second, NULL);
                            mServiceConnections.erase(sc);
                            break;
                        }
                    }
                    mServiceConnectionsMutex.Leave();
                }
            }
            break;
        default:
            processed = SubscriberComponent::ProcessCommandMessage(msg, commandAuthority);
            break;
        }
    }

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing query messages.  This method is called
///   whenever a query message is received.  Overload this method to
///   add support for other input message types.
///
///   \param msg Query message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::ProcessQueryMessage(const Message* msg)
{
    int processed = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_EVENTS:
        {
            const QueryEvents* query = dynamic_cast<const QueryEvents*>(msg);
            if(query)
            {
                ReportEvents report;
                report.SetSourceID(GetID());
                report.SetDestinationID(msg->GetSourceID());
                mEventManager.Lock();
                mEventManager.PopulateReportEventsMessage(*query, report);
                mEventManager.Unlock();
                Send(&report);
            }
        }
        break;
    default:
        processed = SubscriberComponent::ProcessQueryMessage(msg);
        break;
    };

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a periodic or one time event needs to be generated, this
///   function is called.
///
///   It is up to the developer/child class to support the event messages that
///   need to be generated using this function.
///
///   \param eventInfo Information about the event that needs to be generated.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::GenerateEvent(const Event* eventInfo)
{
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a service connection (SC) message needs to be generated
///          this function is called.
///
///   \param sc The service connection that needs to be updated (needs an
///             SC message generated).  It contains the type of SC and
///             sequence number, etc.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::GenerateServiceConnectionMessage(const ServiceConnection& sc)
{
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a request is received to Create a Service Connection,
///   (Create Service Connection Message), this function is called.  This function
///   determines if the SC will be created or not.
///
///   If this function return JAUS_OK, then a service connection is created
///   with the periodic rate confirmed.  Anytime the service connection needs
///   a new message generated, the GenerateServiceConnectionMessage function is called.
///
///   \param command The Create Service Connection request.
///   \param responseValue The response to the request. See
///          ConfirmServiceConnection::ResponseValues for values.
///   \param confirmedRate The periodic rate that can be supported for the SC.
///
///   \return JAUS_OK if the service connection can be created, otherwise
///   JAUS_FAILURE.  If JAUS_OK is returned, the responseValue <b>must</b> be
///   set to ConfirmServiceConnection::CreatedSuccessfully.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::ProcessServiceConnectionRequest(const Jaus::CreateServiceConnection& command,
                                                     Byte& responseValue,
                                                     double& confirmedRate) const
{
    responseValue = ConfirmServiceConnection::Refused;
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a request is received to Create an Event, this function is
///          called.  It is up to the derived class to determine if the
///          event can be supported or not, if so then the InformComponent
///          class will create an event.
///
///   If this function return JAUS_OK, then an event is created and added to
///   the EventManager.  If the event is periodic, then the InformComponent
///   class will generate the events by called the UpdatePeriodicEvent function.
///   In all other cases, it is up to the developer/child class of InformComponent
///   to generate the events.
///
///   Child classes may access the Event Manager member (mEventManager) because it
///   is protected, otherwise use the GetEventManager function to get a pointer.
///
///   See the GlobalPoseSensor class in the JAUS++ Services Library for
///   a good example of how to use this class.
///
///   \param command The Create Event request message.
///   \param responseValue The response to the request. See
///          ConfirmEventRequest::ResponseValues for values. This value must
///          be set no matter what.
///   \param confirmedRate The periodic rate that can be supported (if the
///                        event is periodic).
///   \param errorMessage If the event is refused, save the error message to
///                       this variable.
///
///   \return JAUS_OK if the class supports the event (responeValue should be
///   set to ConfirmEventRequest::ResponseCode::Successful in this case).
///   otherwise return JAUS_FAILURE if the event is not supported/refused.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                         Byte& responseValue,
                                         double& confirmedRate,
                                         std::string& errorMessage) const
{
    responseValue = RejectEventRequest::ConnectionRefused;
    errorMessage = "Events Not Supported";
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Anytime a request is received to Update an Event, this function is
///          called.  It is up to the derived class to determine if the
///          event can be supported or not, if so then the InformComponent
///          class will create an event.
///
///   If this function return JAUS_OK, then an event is updated.
///
///   See the GlobalPoseSensor class in the JAUS++ Services Library for
///   a good example of how to use this class.
///
///   \param command The Update Event request message.
///   \param responseValue The response to the request. See
///          ConfirmEventRequest::ResponseValues for values. This value must
///          be set no matter what.
///   \param confirmedRate The periodic rate that can be supported (if the
///                        event is periodic).
///   \param errorMessage If event refused, error message is copied to parameter.
///
///   \return JAUS_OK if the class supports the event (responeValue should be
///   set to ConfirmEventRequest::ResponseCode::Successful in this case).
///   otherwise return JAUS_FAILURE if the event is not supported/refused.
///
////////////////////////////////////////////////////////////////////////////////////
int InformComponent::ProcessEventUpdateRequest(const Jaus::UpdateEvent& command,
                                               Byte& responseValue,
                                               double& confirmedRate,
                                               std::string& errorMessage) const
{
    responseValue = RejectEventRequest::ConnectionRefused;
    errorMessage = "Events Not Supported";
    return JAUS_FAILURE;
}

int InformComponent::ProcessDiscoveryEvent(const Platform& subsystem,
                                           const SubscriberComponent::DiscoveryEvents eventType)
{
    if(eventType == SubscriberComponent::SubsystemDisconnect)
    {
        mEventManager.Lock();
        mEventManager.DeleteComponentsFromSubsystem(subsystem.GetSubsystemID());
        mEventManager.Unlock();
    }
    if(eventType == SubscriberComponent::SubsystemUpdate)
    {
        // Go through each event being produced, and delete
        // delete any missing components.
        Event::Map* myEvents;
        Event::Map::iterator ev;
        Address::Set* subscribers;
        Address::Set::iterator id;
        mEventManager.Lock();
        myEvents = mEventManager.GetProducedEvents();
        ev = myEvents->begin();
        while(ev != myEvents->end())
        {
            subscribers = ev->second->GetEventSubscribers();
            if(subscribers == NULL || subscribers && subscribers->empty())
            {
                ev++;
                continue;
            }
            id = subscribers->begin();
            while(id != subscribers->end())
            {
                if(id->mSubsystem == subsystem.GetSubsystemID())
                {
                    if(subsystem.GetConfiguration()->HaveComponent(*id) == false)
                    {
#ifdef WIN32
                        id = subscribers->erase(id);
#else
                        subscribers->erase(id);
#endif
                        continue;
                    }
                }
                id++;
            }
            if(subscribers->size() == 0)
            {
#ifdef WIN32
                ev = myEvents->erase(ev);
#else
                myEvents->erase(ev);
#endif
            }
            else
            {
                ev++;
            }
        }
        mEventManager.Unlock();
    }

    return SubscriberComponent::ProcessDiscoveryEvent(subsystem, eventType);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if high performance timer is always used for periodic events.
///
////////////////////////////////////////////////////////////////////////////////////
bool InformComponent::IsHighPerformanceTimerOn() const { return mUseHighPerformanceTimerFlag; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Number of service connections where this component is the provider.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int InformComponent::GetNumServiceConnectionsProvided() const
{
    unsigned int count = 0;
    ServiceConnectionMap::const_iterator scIter;

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();

    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mProviderID == GetID()) { count++; }

        scIter++;
    }

    mServiceConnectionsMutex.Leave();

    return count;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Number of service connections requested by this component.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int InformComponent::GetNumServiceConnectionsRequested() const
{
    unsigned int count = 0;
    ServiceConnectionMap::const_iterator scIter;

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();

    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mProviderID != GetID()) { count++; }

        scIter++;
    }

    mServiceConnectionsMutex.Leave();

    return count;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Goes through all active service connections, and if a message
///          needs to be generated, it is.  Then SC sequence numbers and update
///          times are updated.
///
///   \param lock If true the ServiceConnectionsMutex is used.
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::GenerateServiceConnectionMessages(const bool lock)
{
    ServiceConnectionMap::iterator sc;

    // If HPT is used, do nothing.
    if(mUseHighPerformanceTimerFlag) { return; }

    if(lock) { mServiceConnectionsMutex.Enter(); }

    for(sc = mServiceConnections.begin();
        sc != mServiceConnections.end();
        sc++)
    {
        if(sc->second.mProviderID != GetID()) { continue; }

        if(mUseHighPerformanceTimerFlag || sc->second.mPeriodicRate >= mTimerThresholdFrequency)
        {
            continue;
        }

        // If the service is being provided by the
        // component, and it is an inform message, check and
        // see if we need to generate an event.
        if(IsInformMessage(sc->second.mMessageCode) &&
            sc->second.mPrimaryStatus == ServiceConnection::Active)
        {
            if( Time::GetUtcTimeMs() - sc->second.mUpdateTimeMs >= sc->second.mUpdateIntervalMs)
            {
                GenerateServiceConnectionMessage(sc->second);
                sc->second.mUpdateTimeMs = Time::GetUtcTimeMs();
                sc->second.mSequenceNumber++;
            }
        }
        if(IsCommandMessage(sc->second.mMessageCode) &&
            sc->second.mPrimaryStatus == ServiceConnection::Active)
        {
            double timeInterval = 1000.0/(sc->second.mPeriodicRate + JAUS_EPSILON);
            if( Time::GetUtcTimeMs() - sc->second.mUpdateTimeMs >= timeInterval )
            {
                GenerateServiceConnectionMessage(sc->second);
                sc->second.mUpdateTimeMs = Time::GetUtcTimeMs();
                sc->second.mSequenceNumber++;
            }
        }
    }
    if(lock) { mServiceConnectionsMutex.Leave(); }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Goes through all events that are periodic, and if needed,
///          generates the event.
///
///   \param lock If true locks the event manager.
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::GeneratePeriodicEvents(const bool lock)
{
    // If HPT is used, do nothing.
    if(mUseHighPerformanceTimerFlag) { return; }

    if(lock) { mEventManager.Lock(); }

    Event::Map::iterator e;
    Event::Map* producedEvents = mEventManager.GetProducedEvents();
    for(e = producedEvents->begin();
        e != producedEvents->end();
        e++)
    {
        // If the event is not periodic, skip it.
        if(e->second->GetEventType() != Event::Periodic &&
            e->second->GetEventType() != Event::PeriodicWithoutReplacement)
        {
            continue;
        }

        // If above the timer threshold, or flag is HPT flag, then the event
        // is being generated by the HPT.
        if(mUseHighPerformanceTimerFlag || e->second->GetPeriodicRate() >= mTimerThresholdFrequency)
        {
            continue;
        } 

        if( Time::GetUtcTimeMs() - e->second->GetTimeStampMs() >= 1000.0/(e->second->GetPeriodicRate() + JAUS_EPSILON))
        {
            if(e->first.mMessageCode == JAUS_REPORT_IMAGE)
            {
                int x;
                x = 3;
            }
            GenerateEvent(e->second);
            e->second->SetTimeStampMs(Time::GetUtcTimeMs());
            e->second->SetSequenceNumber(e->second->GetSequenceNumber() + 1);
        }
    }
    if(lock) { mEventManager.Unlock(); }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a high performance timer for event generation for
///   service connections and events.
///
///   \param rate The update frequency.
///   \param serviceConnection Pointer to SC to use for events. (pointer will be
///          copied).
///   \param eventKey Event key to use for SC events. (a copy will be made).
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::CreateHighPerformanceTimer(const double rate,
                                                 ServiceConnection* serviceConnection,
                                                 Event::Key* eventKey)
{
    HighPerformanceTimerSet::iterator hpt;
    bool match = false;
    mHighPerformanceTimersMutex.Enter();
    for(hpt = mHighPerformanceTimers.begin();
        hpt != mHighPerformanceTimers.end();
        hpt++)
    {
        if((*hpt)->mpServiceConnection == serviceConnection)
        {
            (*hpt)->mTimer.StopPeriodicTimer();
            (*hpt)->mpServiceConnection = serviceConnection;
            (*hpt)->mpInformComponent = this;
            (*hpt)->mpScMutex = &this->mServiceConnectionsMutex;
            if((*hpt)->mpEventKey)
            {
                delete (*hpt)->mpEventKey;
                (*hpt)->mpEventKey = NULL;
            }
            (*hpt)->mTimer.StartPeriodicTimer((unsigned int)(1000.0/(rate + JAUS_EPSILON)), (*hpt), 0);
            match = true;
            break;
        }
        else if((*hpt)->mpEventKey && eventKey &&
                (*hpt)->mpEventKey->mEventType == eventKey->mEventType &&
                (*hpt)->mpEventKey->mID == eventKey->mID &&
                (*hpt)->mpEventKey->mMessageCode == eventKey->mMessageCode &&
                (*hpt)->mpEventKey->mSourceID == eventKey->mSourceID)
        {
            (*hpt)->mTimer.StopPeriodicTimer();
            (*hpt)->mpServiceConnection = NULL;
            (*hpt)->mpInformComponent = this;
            (*hpt)->mpScMutex = &this->mServiceConnectionsMutex;
            if((*hpt)->mpEventKey)
            {
                delete (*hpt)->mpEventKey;
                (*hpt)->mpEventKey = new Event::Key(*eventKey);
            }
            (*hpt)->mTimer.StartPeriodicTimer((unsigned int)(1000.0/(rate + JAUS_EPSILON)), (*hpt), 0);
            match = true;
            break;
        }
    }
    if(match == false)
    {
        HighPerformanceTimer *timer = new HighPerformanceTimer();
        if(serviceConnection)
        {
            timer->mpServiceConnection = serviceConnection;
        }
        else if(eventKey)
        {
            timer->mpEventKey = new Event::Key(*eventKey);
        }
        timer->mpInformComponent = this;
        timer->mpScMutex = &this->mServiceConnectionsMutex;
        timer->mTimer.StartPeriodicTimer((unsigned int)(1000.0/(rate + JAUS_EPSILON)), timer, 0);
        mHighPerformanceTimers.insert(timer);
    }
    mHighPerformanceTimersMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Stops a high performance timer for event generation for
///   service connections and events.
///
///   \param serviceConnection Pointer to SC to use for events.
///   \param eventKey Event key info to use for SC events.
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::StopHighPerformanceTimer(ServiceConnection* serviceConnection,
                                               Event::Key* eventKey)
{
    HighPerformanceTimerSet::iterator hpt;
    mHighPerformanceTimersMutex.Enter();
    for(hpt = mHighPerformanceTimers.begin();
        hpt != mHighPerformanceTimers.end();
        hpt++)
    {
        if((*hpt)->mpServiceConnection == serviceConnection)
        {
            (*hpt)->mTimer.StopPeriodicTimer();
            delete (*hpt);
            mHighPerformanceTimers.erase(hpt);
            break;
        }
        else if((*hpt)->mpEventKey && eventKey &&
                (*hpt)->mpEventKey->mEventType == eventKey->mEventType &&
                (*hpt)->mpEventKey->mID == eventKey->mID &&
                (*hpt)->mpEventKey->mMessageCode == eventKey->mMessageCode &&
                (*hpt)->mpEventKey->mSourceID == eventKey->mSourceID)
        {
            (*hpt)->mTimer.StopPeriodicTimer();
            delete (*hpt);
            mHighPerformanceTimers.erase(hpt);
            break;
        }
    }
    mHighPerformanceTimersMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function that is run in a thread which is used to
///   generate periodic and service connection events, and checks to see if
///   service connections and periodic events requested have stopped updating, and
///   attempts to re-establish the connections.
///
////////////////////////////////////////////////////////////////////////////////////
void InformComponent::SubscriptionsLoop()
{
#ifdef WIN32
    unsigned int loopCounter = 0;
    unsigned int loopCountLimit = 50;
#endif
    unsigned int lastCheckTimeMs = 0;   // Last time that we checked for lost services/events.
    unsigned int numServiceConnections = 0, numEvents = 0;

    while(mPeriodicEventThread.QuitThreadFlag() == false)
    {
        // Only check once a second for lost/non-updating connections
        if(Time::GetUtcTimeMs() - lastCheckTimeMs > mDiscoveryTTL)
        {
            CheckServiceConnections();
            if(mPeriodicEventThread.QuitThreadFlag() == false)
            {
                CheckEvents();
            }
            if(mPeriodicEventThread.QuitThreadFlag() == false)
            {
                CheckDiscoveryEvents();
            }
            lastCheckTimeMs = Time::GetUtcTimeMs();
        }

        if(mPeriodicEventThread.QuitThreadFlag() == false)
        {
            GeneratePeriodicEvents();
            GenerateServiceConnectionMessages();
        }

        mServiceConnectionsMutex.Enter();
        numServiceConnections = (unsigned int)mServiceConnections.size();
        mServiceConnectionsMutex.Leave();
        if(mPeriodicEventThread.QuitThreadFlag() == false)
        {
            mEventManager.Lock();
            numEvents = (unsigned int)mEventManager.GetEventSubscriptions()->size();
            numEvents = (unsigned int)mEventManager.GetProducedEvents()->size();
            mEventManager.Unlock();
        }
        // If we have no service connections or
        // periodic events, exit thread.
        if(numServiceConnections == 0 &&
            numEvents == 0 &&
            mLostEvents.size() == 0 &&
            mSystemDiscoveryFlag == false)
        {
            break;
        }
#ifdef WIN32
        // Try not to max out the CPU
        if(++loopCounter == loopCountLimit)
        {
            loopCounter = 0;
            CxUtils::SleepMs(1);
        }
#else
        usleep(500);
#endif
    }

}



/*  End of File */
