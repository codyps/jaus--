////////////////////////////////////////////////////////////////////////////////////
///
///  \file eventmanager.cpp
///  \brief This file contains a class for storing event information for a
///  service.  Use this to reduce the amount of code you need to write to
///  support events.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 26 January 2008
///  <br>Last Modified: 25 March 2008
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
#include "jaus/components/eventmanager.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/command/events/createeventrequest.h"
#include "jaus/messages/command/events/rejecteventrequest.h"
#include "jaus/messages/command/events/confirmeventrequest.h"
#include "jaus/messages/inform/events/eventmessage.h"
#include "jaus/components/connectionhandler.h"
#include "jaus/components/node/nodeconnectionhandler.h"
#include <iostream>

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
EventManager::EventManager()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
EventManager::~EventManager()
{
    Event::Map::iterator emap;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        delete emap->second;
    }
    for(emap = mEventSubscriptions.begin();
        emap != mEventSubscriptions.end();
        emap++)
    {
        delete emap->second;
    }
    mProducedEvents.clear();
    mEventSubscriptions.clear();
    mPeriodicSubscriptions.clear();
    mProducedPeriodicEvents.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID of the component the Event Manager is working for.
///
///   This value is used for internal sorting of events based on who is
///   generating them.
///
///   \param id Component ID.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::SetComponentID(const Address& id)
{
    if(id.IsValid() && !id.IsBroadcast())
    {
        mComponentID = id;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Add an event for the manager to store.
///
///   The event manager will take ownership of the pointer and store the
///   data internally.
///
///   \param e Event data to add.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::AddEvent(const Event& e)
{
    Event::Key key = e.GetKey();
    Event::Map::iterator emap;
    if(key.mSourceID == mComponentID)
    {
        emap = mProducedEvents.find(key);
        if(emap == mProducedEvents.end())
        {
            Event* newEvent = new Event(e);
            mProducedEvents[key] = newEvent;
            if(newEvent->IsPeriodic())
            {
                mProducedPeriodicEvents.insert(newEvent);
            }
            return JAUS_OK;
        }
        else
        {
            return SetJausError(ErrorCodes::DataExists);
        }
    }
    else
    {
        emap = mEventSubscriptions.find(key);
        if(emap == mEventSubscriptions.end())
        {
            Event* newEvent = new Event(e);
            mEventSubscriptions[key] = newEvent;
            if(newEvent->IsPeriodic())
            {
                mPeriodicSubscriptions.insert(newEvent);
            }
            return JAUS_OK;
        }
        else
        {
            return SetJausError(ErrorCodes::DataExists);
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Add an event for the manager to store.
///
///   The event manager will take ownership of the pointer and store the
///   data internally.
///
///   \param e Event pointer to add to manager.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::AddEvent(Event* e)
{
    Event::Key key = e->GetKey();
    Event::Map::iterator emap;
    if(key.mSourceID == mComponentID)
    {
        emap = mProducedEvents.find(key);
        if(emap == mProducedEvents.end())
        {
            mProducedEvents[key] = e;
            if(e->IsPeriodic())
            {
                mProducedPeriodicEvents.insert(e);
            }
            return JAUS_OK;
        }
        else
        {
            return SetJausError(ErrorCodes::DataExists);
        }
    }
    else
    {
        emap = mEventSubscriptions.find(key);
        if(emap == mEventSubscriptions.end())
        {
            mEventSubscriptions[key] = e;
            if(e->IsPeriodic())
            {
                mPeriodicSubscriptions.insert(e);
            }
            return JAUS_OK;
        }
        else
        {
            return SetJausError(ErrorCodes::DataExists);
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Delete an event from manager.
///
///   \param e Pointer to event data to delete.  Pointer will be reset to NULL
///            on success.
///   \param connectionHandler Pointer to connection handler if you want
///          automatic sending of Cancel Event messages. Set to NULL if not
///          wanted.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::DeleteEvent(Event*& e, ConnectionHandler* connectionHandler)
{
    if(e && DeleteEvent(e->GetKey(), connectionHandler))
    {
        e = NULL;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Delete an event from manager.
///
///   \param key Key structure to identify the event to delete.
///   \param connectionHandler Pointer to connection handler if you want
///          automatic sending of Cancel Event messages. Set to NULL if not
///          wanted.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::DeleteEvent(const Event::Key& key, ConnectionHandler* connectionHandler)
{
    Event::Map::iterator emap;
    if(key.mSourceID == mComponentID)
    {
        emap = mProducedEvents.find(key);
        if(emap == mProducedEvents.end())
        {
            return SetJausError(ErrorCodes::FieldNotPresent);
        }
        if(emap->second->IsPeriodic())
        {
            mProducedPeriodicEvents.erase( mProducedPeriodicEvents.find(emap->second) );
        }
        delete emap->second;
        mProducedEvents.erase(emap);
    }
    else
    {
        emap = mEventSubscriptions.find(key);
        if(emap == mEventSubscriptions.end())
        {
            return SetJausError(ErrorCodes::FieldNotPresent);
        }
        if(emap->second->IsPeriodic())
        {
            mPeriodicSubscriptions.erase( mPeriodicSubscriptions.find(emap->second) );
        }
        delete emap->second;
        mEventSubscriptions.erase(emap);

        // Tell the event provider that we want to cancel.
        if(connectionHandler)
        {
            Jaus::CancelEvent cancelEvent;
            Receipt receipt;
            cancelEvent.SetSourceID(mComponentID);
            cancelEvent.SetDestinationID(key.mSourceID);
            cancelEvent.SetEventID(key.mID);
            cancelEvent.SetRequestID(1);
            cancelEvent.SetMessageCode(key.mMessageCode);
            if(connectionHandler->Send(&cancelEvent, receipt) == JAUS_OK)
            {
#ifdef _DEBUG
                if(receipt.GetResponseMessage()->GetCommandCode() == JAUS_REJECT_EVENT_REQUEST)
                {
                    std::cout << "EventManager: Failed to cancel event.\n";
                }
#endif
            }
        }
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes all event subscriptions from a specific provider and removes
///          the source as a subscriber to events we are generating.
///
///   \param id ID of the componet to remove from management.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::DeleteComponent(const Address& id)
{
    Event::Map::iterator emap;

    emap = mEventSubscriptions.begin();
    while(emap != mEventSubscriptions.end())
    {
        if(emap->first.mSourceID == id )
        {
            if(emap->second->IsPeriodic())
            {
                mPeriodicSubscriptions.erase( mPeriodicSubscriptions.find( emap->second ) );
            }
            delete emap->second;
#ifdef WIN32
            emap = mEventSubscriptions.erase(emap);
#else
            mEventSubscriptions.erase(emap);
            emap = mEventSubscriptions.begin();
#endif

        }
        else
        {
            emap++;
        }
    }
    emap = mProducedEvents.begin();
    Address::Set *subscribers;
    while(emap != mProducedEvents.end())
    {
        subscribers = emap->second->GetEventSubscribers();
        if( subscribers->find( id ) != subscribers->end() )
        {
            subscribers->erase( subscribers->find(id) );
        }

        if(subscribers->size() == 0)
        {
            if(emap->second->IsPeriodic())
            {
                mProducedPeriodicEvents.erase( mProducedPeriodicEvents.find(emap->second));
            }
            delete emap->second;
#ifdef WIN32
            emap = mProducedEvents.erase(emap);
#else
            mProducedEvents.erase(emap);
            emap = mProducedEvents.begin();
#endif
        }
        else
        {
            emap++;
        }
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes all event subscriptions and subscribers that belong to
///          a subsystem.
///
///   \param sid Subsystem ID.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::DeleteComponentsFromSubsystem(const Byte sid)
{
    Event::Map::iterator emap;

    emap = mEventSubscriptions.begin();
    while(emap != mEventSubscriptions.end())
    {
        if(emap->first.mSourceID.mSubsystem == sid)
        {
            if(emap->second->IsPeriodic())
            {
                mPeriodicSubscriptions.erase( mPeriodicSubscriptions.find(emap->second));
            }
            delete emap->second;
#ifdef WIN32
            emap = mEventSubscriptions.erase(emap);
#else
            mEventSubscriptions.erase(emap);
            emap = mEventSubscriptions.begin();
#endif

        }
        else
        {
            emap++;
        }
    }
    emap = mProducedEvents.begin();
    Address::Set *subscribers;
    Address::Set::iterator subscriberID;
    while(emap != mProducedEvents.end())
    {
        subscribers = emap->second->GetEventSubscribers();
        subscriberID = subscribers->begin();
        while(subscriberID != subscribers->end())
        {
            if(subscriberID->mSubsystem == sid)
            {
#ifdef WIN32
                subscriberID = subscribers->erase(subscriberID);
#else
                subscribers->erase(subscriberID);
#endif
            }
            else
            {
                subscriberID++;
            }
        }
        if(subscribers->size() == 0)
        {
            if(emap->second->IsPeriodic())
            {
                mProducedPeriodicEvents.erase( mProducedPeriodicEvents.find(emap->second));
            }
            delete emap->second;
#ifdef WIN32
            emap = mProducedEvents.erase(emap);
#else
            mProducedEvents.erase(emap);
            emap = mProducedEvents.begin();
#endif
        }
        else
        {
            emap++;
        }
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes all event subscriptions and subscribers that belong to
///          a subsystem node.
///
///   \param sid Subsystem ID.
///   \param nid Node ID.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::DeleteComponentsFromNode(const Byte sid, const Byte nid)
{
    Event::Map::iterator emap;

    emap = mEventSubscriptions.begin();
    while(emap != mEventSubscriptions.end())
    {
        if(emap->first.mSourceID.mSubsystem == sid && emap->first.mSourceID.mNode == nid )
        {
            if(emap->second->IsPeriodic())
            {
                mPeriodicSubscriptions.erase( mPeriodicSubscriptions.find(emap->second));
            }
            delete emap->second;
#ifdef WIN32
            emap = mEventSubscriptions.erase(emap);
#else
            mEventSubscriptions.erase(emap);
            emap = mEventSubscriptions.begin();
#endif
        }
        else
        {
            emap++;
        }
    }
    emap = mProducedEvents.begin();
    Address::Set *subscribers;
    Address::Set::iterator subscriberID;
    while(emap != mProducedEvents.end())
    {
        subscribers = emap->second->GetEventSubscribers();
        subscriberID = subscribers->begin();
        while(subscriberID != subscribers->end())
        {
            if(subscriberID->mSubsystem == sid && subscriberID->mNode == nid)
            {
#ifdef WIN32
                subscriberID = subscribers->erase(subscriberID);
#else
                subscribers->erase(subscriberID);
                subscriberID = subscribers->begin();
#endif
            }
            else
            {
                subscriberID++;
            }
        }
        if(subscribers->size() == 0)
        {
            if(emap->second->IsPeriodic())
            {
                mProducedPeriodicEvents.erase( mProducedPeriodicEvents.find(emap->second));
            }
            delete emap->second;
#ifdef WIN32
            emap = mProducedEvents.erase(emap);
#else
            mProducedEvents.erase(emap);
            emap = mProducedEvents.begin();
#endif
        }
        else
        {
            emap++;
        }
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Terminates all events and deletes event data.
///
///   \param connectionHandler Pointer to connection handler for sending
///          Cancel Event messages for each event being deleted/canceled.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::CancelEvents(ConnectionHandler* connectionHandler)
{
    Event::Map::iterator emap;
    Jaus::CancelEvent cancelEvent;
    Receipt receipt;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        delete emap->second;
    }
    mProducedEvents.clear();
    mProducedPeriodicEvents.clear();

    for(emap = mEventSubscriptions.begin();
        emap != mEventSubscriptions.end();
        emap++)
    {
        // Tell the event provider that we want to cancel.
        if(connectionHandler)
        {
            Jaus::CancelEvent cancelEvent;
            Receipt receipt;
            cancelEvent.SetSourceID(mComponentID);
            cancelEvent.SetDestinationID(emap->second->GetEventProvider());
            cancelEvent.SetEventID(emap->second->GetEventID());
            cancelEvent.SetRequestID(1);
            cancelEvent.SetMessageCode(emap->second->GetMessageCode());
            if(connectionHandler->Send(&cancelEvent, receipt) == JAUS_OK)
            {
#ifdef _DEBUG
                if(receipt.GetResponseMessage()->GetCommandCode() == JAUS_REJECT_EVENT_REQUEST)
                {
                    std::cout << "EventManager: Failed to cancel event.\n";
                }
#endif
            }
        }

        delete emap->second;
    }
    mEventSubscriptions.clear();
    mPeriodicSubscriptions.clear();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Terminates all event subscriptions and deletes event data.
///
///   \param connectionHandler Pointer to connection handler for sending
///          Cancel Event messages for each event being deleted/canceled.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::CancelEventSubscriptions(ConnectionHandler* connectionHandler)
{
    Event::Map::iterator emap;
    Jaus::CancelEvent cancelEvent;
    Receipt receipt;

    if(connectionHandler == NULL)
    {
        return SetJausError(ErrorCodes::InvalidValue);
    }
    for(emap = mEventSubscriptions.begin();
        emap != mEventSubscriptions.end();
        emap++)
    {
        // Tell the event provider that we want to cancel.
        if(connectionHandler)
        {
            Jaus::CancelEvent cancelEvent;
            Receipt receipt;
            cancelEvent.SetSourceID(mComponentID);
            cancelEvent.SetDestinationID(emap->second->GetEventProvider());
            cancelEvent.SetEventID(emap->second->GetEventID());
            cancelEvent.SetRequestID(1);
            cancelEvent.SetMessageCode(emap->second->GetMessageCode());
            connectionHandler->Send(&cancelEvent, receipt);
        }

        delete emap->second;
    }
    mEventSubscriptions.clear();
    mPeriodicSubscriptions.clear();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Terminates all events being produced and deletes event data.
///
///   \param connectionHandler Pointer to connection handler for sending
///          Cancel Event messages for each event being deleted/canceled.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::CancelProducedEvents(ConnectionHandler* connectionHandler)
{
    Event::Map::iterator emap;
    Jaus::CancelEvent cancelEvent;
    Receipt receipt;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        delete emap->second;
    }
    mProducedEvents.clear();
    mProducedPeriodicEvents.clear();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Finds the event data that matches the cancel request, then
///          removes the message source as a subscriber and sends
///          confirmation or rejection of cancellation.
///
///   If after removal there are no more subscribers for a matching event, the
///   event is deleted.
///
///   \param cancelEvent Cancel Event message to use for event cancelation.
///   \param connectionHandler Pointer to connection handler for sending
///          Confirm or Reject Event Request message.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::CancelProducedEvent(const Jaus::CancelEvent& cancelEvent, ConnectionHandler* connectionHandler)
{
    int result = JAUS_FAILURE;

    Event::Map::iterator emap;
    Address::Set* subscribers = NULL;
    Byte eventID = 0;
    UShort messageCode = 0;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        subscribers = emap->second->GetEventSubscribers();
        if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::EventID) &&
            cancelEvent.IsFieldPresent(CancelEvent::VectorBit::MessageCode))
        {
            if(emap->first.mMessageCode == cancelEvent.GetMessageCode() &&
                emap->first.mID == cancelEvent.GetEventID() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                subscribers->erase(subscribers->find(cancelEvent.GetSourceID()));
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                if(subscribers->size() == 0)
                {
                    if(emap->second->IsPeriodic())
                    {
                        mProducedPeriodicEvents.erase(mProducedPeriodicEvents.find(emap->second));
                    }
                    delete emap->second;
                    mProducedEvents.erase(emap);
                }
                result = JAUS_OK;
                break;
            }
        }
        else if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::MessageCode))
        {
            if(emap->first.mMessageCode == cancelEvent.GetMessageCode() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                subscribers->erase(subscribers->find(cancelEvent.GetSourceID()));
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                if(subscribers->size() == 0)
                {
                    if(emap->second->IsPeriodic())
                    {
                        mProducedPeriodicEvents.erase(mProducedPeriodicEvents.find(emap->second));
                    }
                    delete emap->second;
                    mProducedEvents.erase(emap);
                }
                result = JAUS_OK;
                break;
            }
        }
        else if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::EventID))
        {
            if(emap->first.mID == cancelEvent.GetEventID() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                subscribers->erase(subscribers->find(cancelEvent.GetSourceID()));
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                if(subscribers->size() == 0)
                {
                    if(emap->second->IsPeriodic())
                    {
                        mProducedPeriodicEvents.erase(mProducedPeriodicEvents.find(emap->second));
                    }
                    delete emap->second;
                    mProducedEvents.erase(emap);
                }
                result = JAUS_OK;
                break;
            }
        }
        else
        {
            if(subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                subscribers->erase(subscribers->find(cancelEvent.GetSourceID()));
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                if(subscribers->size() == 0)
                {
                    if(emap->second->IsPeriodic())
                    {
                        mProducedPeriodicEvents.erase(mProducedPeriodicEvents.find(emap->second));
                    }
                    delete emap->second;
                    mProducedEvents.erase(emap);
                }
                result = JAUS_OK;
                break;
            }
        }
    }

    if(result == JAUS_OK && connectionHandler)
    {
        Jaus::ConfirmEventRequest confirmRequest;
        confirmRequest.SetSourceID(connectionHandler->GetID());
        confirmRequest.SetDestinationID(cancelEvent.GetSourceID());
        confirmRequest.SetRequestID(cancelEvent.GetRequestID());
        confirmRequest.SetMessageCode(messageCode);
        confirmRequest.SetEventID(eventID);
        connectionHandler->Send(&confirmRequest);
    }

    if(result == JAUS_FAILURE && connectionHandler)
    {
        Jaus::RejectEventRequest rejectRequest;
        rejectRequest.SetSourceID(connectionHandler->GetID());
        rejectRequest.SetDestinationID(cancelEvent.GetSourceID());
        rejectRequest.SetRequestID(cancelEvent.GetRequestID());
        rejectRequest.SetResponseCode(RejectEventRequest::InvalidEventSetup);
        connectionHandler->Send(&rejectRequest);
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Populates the event list of the Report Events message with
///   events being produced by component.
///
///   \param query The query message to filter results for.
///   \param msg Message to fill Event list for.
///
///   \return JAUS_OK on success, otherwise failure.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::PopulateReportEventsMessage(const Jaus::QueryEvents& query,
                                              Jaus::ReportEvents& msg) const
{
    Event::List* list = msg.GetEvents();
    Event::Map::const_iterator myEvent;
    list->clear();
    Byte pv = query.GetPresenceVector();
    for(myEvent = mProducedEvents.begin();
        myEvent != mProducedEvents.end();
        myEvent++)
    {
        if( BitVector::IsBitSet(pv, QueryEvents::VectorBit::MessageCode)  == 0 ||
            query.GetMessageCode() == myEvent->second->GetMessageCode())
        {
            if( BitVector::IsBitSet(pv, QueryEvents::VectorBit::EventType) == 0 ||
                query.GetEventType() == myEvent->second->GetEventType() )
            {
                if( BitVector::IsBitSet(pv, QueryEvents::VectorBit::EventID) == 0 ||
                    query.GetEventID() == myEvent->second->GetEventID())
                {
                    list->push_back(*myEvent->second);
                }
            }
        }
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes all event information stored in the manager.
///
////////////////////////////////////////////////////////////////////////////////////
void EventManager::DeleteAllEvents()
{
    Event::Map::iterator myEvent;

    for(myEvent = mProducedEvents.begin();
        myEvent != mProducedEvents.end();
        myEvent++)
    {
        if(myEvent->second)
        {
            delete myEvent->second;
            myEvent->second = NULL;
        }
    }
    for(myEvent = mEventSubscriptions.begin();
        myEvent != mEventSubscriptions.end();
        myEvent++)
    {
        if(myEvent->second)
        {
            delete myEvent->second;
            myEvent->second = NULL;
        }
    }
    mProducedEvents.clear();
    mEventSubscriptions.clear();
    mPeriodicSubscriptions.clear();
    mProducedPeriodicEvents.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Given event data, the inform message to include in the event, and
///   a connection handler to send with, this method creates an EventMessage
///   and sends it to all the events subscribers.
///
///   \param eventData Pointer to event information which includes subscribers,
///                    Event ID, Sequence Number, etc.  If method is successful,
///                    the eventData's sequence number and timestamp will be
///                    udpated.
///   \param eventMessage The message to use for the event.
///   \param connectionHandler The connection handler to send the message with.
///
///   \return JAUS_OK on success, otherwise failure.
///
////////////////////////////////////////////////////////////////////////////////////
int EventManager::GenerateEvent(const Event* eventData,
                                const Message* eventMessage,
                                ConnectionHandler* connectionHandler)
{
    if(eventData && connectionHandler)
    {
        EventMessage reportEvent;
        Stream *stream = NULL;
        Header header;
        NodeConnectionHandler* nodeConnectionHandler = NULL;
        Address::Set::const_iterator dest;
        Address::Set * destinations;

        // See if this is a node connection handler.
        nodeConnectionHandler = dynamic_cast<NodeConnectionHandler*>(connectionHandler);

        reportEvent.SetSourceID(connectionHandler->GetID());
        reportEvent.SetEventID(eventData->GetEventID());
        reportEvent.SetEventSequenceNumber(eventData->GetSequenceNumber());
        reportEvent.SetEventMessage(eventMessage, true);
        reportEvent.SetEventMessageCode(eventMessage->GetCommandCode());
        destinations = eventData->GetEventSubscribers();

        for(dest = destinations->begin();
            dest != destinations->end();
            dest++)
        {
            // Write the stream for the first time.
            if(stream == NULL)
            {
                stream = new Stream();
                reportEvent.SetDestinationID(*dest);
                if(reportEvent.Write(*stream) == JAUS_FAILURE)
                {
                    delete stream;
                    stream = NULL;
                    return JAUS_FAILURE;
                }
                stream->Read(header);
            }
            else
            {
                // Change destination ID
                header.mDestinationID = *dest;
                stream->Write(header, 0);
            }
            if(nodeConnectionHandler)
            {
                // SendStream is not thread safe for
                // the NodeConnectionHandler, so take this
                // into account and use the Send method.
                nodeConnectionHandler->Send(*stream);
            }
            else
            {
                connectionHandler->SendStream(*stream);
            }
        }
        if(stream) { delete stream; }
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Locks the EventManager with a mutex for thread protection.
///
///   If EventManager is used in a multi-threaded application (default) then
///   use the Lock and Unlock methods to avoid read/write errors.
///
////////////////////////////////////////////////////////////////////////////////////
void EventManager::Lock() const 
{ 
    mEventsMutex.Enter(); 
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Unlocks the EventManager from a mutex for thread protection.
///
///   If EventManager is used in a multi-threaded application (default) then
///   use the Lock and Unlock methods to avoid read/write errors.
///
////////////////////////////////////////////////////////////////////////////////////
void EventManager::Unlock() const 
{ 
    mEventsMutex.Unlock(); 
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to Event data.
///
///   \param source The source/provider of the event.
///   \param messageCode The event message type.
///   \param eventID The event ID.
///
///   \return Pointer to event if found, NULL if not present.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Address& source,
                              const UShort messageCode,
                              const Byte eventID) const
{
    Event::Map::const_iterator emap;
    if(source == mComponentID)
    {
        for(emap = mProducedEvents.begin();
            emap != mProducedEvents.end();
            emap++)
        {
            if( emap->first.mMessageCode == messageCode &&
                emap->first.mID == eventID)
            {
                return (Event*)emap->second;
            }
        }
    }
    else
    {
        for(emap = mEventSubscriptions.begin();
            emap != mEventSubscriptions.end();
            emap++)
        {
            if(emap->first.mSourceID == source &&
                emap->first.mMessageCode == messageCode &&
                emap->first.mID == eventID)
            {
                return (Event*)emap->second;
            }
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to the first Event data matching parameters.
///
///   \param source The source/provider of the event.
///   \param messageCode The event message type.
///
///   \return Pointer to event if found, NULL if not present.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Address& source,
                              const UShort messageCode) const
{
    Event::Map::const_iterator emap;
    if(source == mComponentID)
    {
        for(emap = mProducedEvents.begin();
            emap != mProducedEvents.end();
            emap++)
        {
            if( emap->first.mMessageCode == messageCode)
            {
                return (Event*)emap->second;
            }
        }
    }
    else
    {
        for(emap = mEventSubscriptions.begin();
            emap != mEventSubscriptions.end();
            emap++)
        {
            if(emap->first.mSourceID == source &&
                emap->first.mMessageCode == messageCode)
            {
                return (Event*)emap->second;
            }
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function looks through all events being produced by the
///   component, and gets an ID number that is not in use.
///
///   \param messageCode The type of event messages to look for a unique ID
///          within.
///
///   \return Unique ID number to use for event of type messageCode.
///
////////////////////////////////////////////////////////////////////////////////////
Byte EventManager::GenerateEventID(const UShort messageCode) const
{
    Event::Map::const_iterator emap;
    Byte eventID = 0;
    std::set<Byte> inUse;
    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        if(emap->first.mMessageCode == messageCode)
        {
            inUse.insert(emap->first.mID);
        }
    }
    // Generate an ID for this event that is unique.
    while(inUse.find(eventID) != inUse.end())
    {
        eventID++;
        // If we hit the end, exit regardless.
        if(eventID == 255)
            break;
    }

    return eventID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function will generate a random number between [0,255] for
///   use in Event Setup and Control messages.
///
///   \return Unique ID number to use for event request ID.
///
////////////////////////////////////////////////////////////////////////////////////
Byte EventManager::GenerateRequestID()
{
    return (Byte)(rand()*255.0/RAND_MAX);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to Event data.
///
///   \param key Event key data.
///
///   \return Pointer to event if found, NULL if not present.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Event::Key& key) const
{
    Event::Map::const_iterator emap;
    if(key.mSourceID == mComponentID)
    {
        emap = mProducedEvents.find(key);
        if(emap != mProducedEvents.end())
        {
            return (Event*)emap->second;
        }
    }
    else
    {
        emap = mEventSubscriptions.find(key);
        if(emap != mEventSubscriptions.end())
        {
            return (Event*)emap->second;
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to Event data.
///
///   \param ce Create Event message to find closest matching existing event to.
///
///   \return Pointer to event if found, NULL if not present.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Jaus::CreateEventRequest& ce) const
{
    Event::Map::const_iterator emap;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        if( emap->second->MatchesCreateRequest(ce))
        {
            return (Event*)emap->second;
        }
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to Event data.
///
///   \param command Update event message to use for lookup.
///
///   \return Pointer to event if found, NULL if not present.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Jaus::UpdateEvent& command) const
{
    Event::Map::const_iterator emap;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        if(emap->first.mID == command.GetEventID())
        {
            if(emap->first.mSourceID == command.GetDestinationID() &&
                emap->first.mMessageCode == command.GetMessageCode() &&
                emap->second->GetEventSubscribers()->find(command.GetSourceID())
                        != emap->second->GetEventSubscribers()->end() &&
                emap->second->GetEventType() == command.GetEventType())
            {
                return (Event*)emap->second;
            }

        }
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Finds the event that matches the cancel request, and returns
///   a pointer.
///
///   \param cancelEvent Cancel Event message to use for event lookup.
///
///   \return Pointer to matching event, NULL if not found.
///
////////////////////////////////////////////////////////////////////////////////////
Event* EventManager::GetEvent(const Jaus::CancelEvent& cancelEvent) const
{
    Event::Map::const_iterator emap;
    Event* eventInfo = NULL;
    Address::Set* subscribers = NULL;
    Byte eventID = 0;
    UShort messageCode = 0;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        subscribers = emap->second->GetEventSubscribers();
        if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::EventID) &&
            cancelEvent.IsFieldPresent(CancelEvent::VectorBit::MessageCode))
        {
            if(emap->first.mMessageCode == cancelEvent.GetMessageCode() &&
                emap->first.mID == cancelEvent.GetEventID() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                eventInfo = emap->second;
                break;
            }
        }
        else if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::MessageCode))
        {
            if(emap->first.mMessageCode == cancelEvent.GetMessageCode() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                eventInfo = emap->second;
                break;
            }
        }
        else if(cancelEvent.IsFieldPresent(CancelEvent::VectorBit::EventID))
        {
            if(emap->first.mID == cancelEvent.GetEventID() &&
                subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                eventInfo = emap->second;
                break;
            }
        }
        else
        {
            if(subscribers->find(cancelEvent.GetSourceID()) != subscribers->end())
            {
                eventID = emap->first.mID;
                messageCode = emap->first.mMessageCode;
                eventInfo = emap->second;
                break;
            }
        }
    }

    return eventInfo;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Finds all the events being produced that could possibly statisfy
///   the Create Event request.
///
///   The set returned will include all existing events being produced that
///   satisfy all requirements.  <b>You must do some additional comparisons of
///   periodic rate or any floating point values, since Event Manager only checks
///   for a difference of .1 when matching.</b>
///
///   \param ce The Create Event message to finding existing produced events
///             that match.
///
///   \return Set of Event pointers of the matching events.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set EventManager::GetMatchingEvents(const CreateEventRequest& ce) const
{
    Event::Map::const_iterator emap;
    Event::Set set;

    for(emap = mProducedEvents.begin();
        emap != mProducedEvents.end();
        emap++)
    {
        if( emap->second->MatchesCreateRequest(ce))
        {
            set.insert((Event*)emap->second);
        }
    }

    return set;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets all events for a specific message type.
///
///   \param messageType The message code/type to get Events for.
///
///   \return STL set of event pointers.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set EventManager::GetEventsOfType(const UShort messageType) const
{
    Event::Map::const_iterator e;
    Event::Set eventSet;

    for(e = mProducedEvents.begin();
        e != mProducedEvents.end();
        e++)
    {
        if(e->second->GetMessageCode() == messageType)
        {
            eventSet.insert(e->second);
        }
    }

    for(e = mEventSubscriptions.begin();
        e != mEventSubscriptions.end();
        e++)
    {
        if(e->second->GetMessageCode() == messageType)
        {
            eventSet.insert(e->second);
        }
    }

    return eventSet;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets event subscriptions for a specific type of event.
///
///   \param messageType The message code/type to get Events for.
///
///   \return STL set of event pointers.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set EventManager::GetEventSubscriptionsOfType(const UShort messageType) const
{
    Event::Map::const_iterator e;
    Event::Set eventSet;

    for(e = mEventSubscriptions.begin();
        e != mEventSubscriptions.end();
        e++)
    {
        if(e->second->GetMessageCode() == messageType)
        {
            eventSet.insert(e->second);
        }
    }

    return eventSet;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets event subscriptions for a specific type of event and specific
///          event provider.
///
///   \param messageType The message code/type to get Events for.
///   \param id The provider of the events of type messageType.
///
///   \return STL set of event pointers.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set EventManager::GetEventSubscriptionsOfType(const UShort messageType, const Address& id) const
{
    Event::Map::const_iterator e;
    Event::Set eventSet;

    for(e = mEventSubscriptions.begin();
        e != mEventSubscriptions.end();
        e++)
    {
        if(e->second->GetMessageCode() == messageType &&
            e->first.mSourceID == id)
        {
            eventSet.insert(e->second);
        }
    }

    return eventSet;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets produced events for a specific type of event.
///
///   \param messageType The message code/type to get Events for.
///
///   \return STL set of event pointers.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set EventManager::GetProducedEventsOfType(const UShort messageType) const
{
    Event::Map::const_iterator e;
    Event::Set eventSet;

    for(e = mProducedEvents.begin();
        e != mProducedEvents.end();
        e++)
    {
        if(e->second->GetMessageCode() == messageType)
        {
            eventSet.insert(e->second);
        }
    }

    return eventSet;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return A pointer to all events being produced by the Component.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Map* EventManager::GetProducedEvents() const
{
    return (Event::Map*)&mProducedEvents;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return A pointer to all events being subscribed to by the Component.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Map* EventManager::GetEventSubscriptions() const
{
    return (Event::Map*)&mEventSubscriptions;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return A pointer to all periodic events being produced by the Component.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set* EventManager::GetProducedPeriodicEvents() const
{
    return (Event::Set*)&mProducedPeriodicEvents;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return A pointer to all periodic events being subscribed to by the Component.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Set* EventManager::GetPeriodicEventSubscriptions() const
{
    return (Event::Set*)&mPeriodicSubscriptions;
}


/*  End of File */
