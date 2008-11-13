////////////////////////////////////////////////////////////////////////////////////
///
///  \file eventmanager.h
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
#ifndef __JAUS_EVENT_MANAGER__H
#define __JAUS_EVENT_MANAGER__H

#include <map>
#include <set>
#include <cxutils/mutex.h>
#include "jaus/components/callback.h"
#include "jaus/messages/errorhistory.h"
#include "jaus/messages/query/events/queryevents.h"
#include "jaus/messages/inform/events/reportevents.h"
#include "jaus/messages/common/events/event.h"
#include "jaus/messages/command/events/cancelevent.h"
#include "jaus/messages/command/events/createeventrequest.h"
#include "jaus/messages/command/events/updateevent.h"

namespace Jaus
{
    class ConnectionHandler; // Forward declaration.
    class CreateEventRequest;// Forward declaration.

    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class EventManager
    ///  \brief Event manager is used to keep track of events for a component.
    ///
    ///  The main role of this class is a repository for all events that a component
    ///  is requesting or generating.
    ///
    ///  <b>Lock and Unlock methods are provided to protect the data stored by
    ///  EventManager when used in multi-threaded applications.  If you have
    ///  more than one thread access or use the data stored in EventManager make
    ///  sure you use Lock and Unlock.  This should be the default behavior
    ///  since this class will be used in multi-threaded applications.</b>
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL EventManager : public ErrorHistory
    {
    public:
        // Constructor.
        EventManager();
        // Destructor.
        virtual ~EventManager();
        // Set the address of the component using manager.
        int SetComponentID(const Address& id);
        // Add an event to be managed.
        int AddEvent(Event* e);
        // Add an event to be managed (makes copy of data).
        int AddEvent(const Event& e);
        // Removes and deletes event from manager.
        int DeleteEvent(Event*& e, ConnectionHandler* connectionHandler = NULL);
        // Removes and deletes the event from the manager.
        int DeleteEvent(const Event::Key& key, ConnectionHandler* connectionHandler = NULL);
        // Delete all events from a provider.
        int DeleteComponent(const Address& provider);
        // Delete all components from subsystem.
        int DeleteComponentsFromSubsystem(const Byte sid);
        // Delete all events from a subsystem node.
        int DeleteComponentsFromNode(const Byte sid, const Byte nid);
        // Terminates all events by deleting data.
        int CancelEvents(ConnectionHandler* connectionHandler);
        // Terminates all event subscriptions.
        int CancelEventSubscriptions(ConnectionHandler* connectionHandler);
        // Terminates all event subscriptions.
        int CancelProducedEvents(ConnectionHandler* connectionHandler = NULL);
        // Processes the cancel message automatically.
        int CancelProducedEvent(const Jaus::CancelEvent& ce, ConnectionHandler* connectionHandler = NULL);
        // Populates the events list for the Report Events message.
        int PopulateReportEventsMessage(const Jaus::QueryEvents& query,
                                        Jaus::ReportEvents& msg) const;
        // Deletes all event data
        void DeleteAllEvents();
        // Generates an event using data and connection
        static int GenerateEvent(const Event* eventData, 
                                 const Message* eventMessage, 
                                 ConnectionHandler* connectionHandler);
        // Locks the event manager with Mutex (used for thread protection).
        void Lock() const;
        // Unlocks the event manager from Mutex (used for thread protection).
        void Unlock() const;
        // Generates a unique event ID for the type of event (based on previously created events).
        Byte GenerateEventID(const UShort messageCode) const;
        // Generate a unique local request ID for event messages.
        static Byte GenerateRequestID();
        // Finds an event based on source, type, and local request ID
        Event* GetEvent(const Address& source,
                        const UShort messageCode,
                        const Byte eventID) const;
        // Finds an event based on source, type, and local request ID
        Event* GetEvent(const Address& source,
                        const UShort messageCode) const;
        // Looks up an event if present.
        Event* GetEvent(const Event::Key& key) const;
        // Looks up an event if present.
        Event* GetEvent(const Jaus::CreateEventRequest& ce) const;
        // Looks up an event if present.
        Event* GetEvent(const Jaus::UpdateEvent& ce) const;
        // Looks up an event if present.
        Event* GetEvent(const Jaus::CancelEvent& ce) const;
        // Looks up all events being produced that match the create request.
        Event::Set GetMatchingEvents(const CreateEventRequest& ce) const;
        // Gets all events of for a specific message type.
        Event::Set GetEventsOfType(const UShort messageType) const;
        // Gets all events of for a specific message type you are subscribing to.
        Event::Set GetEventSubscriptionsOfType(const UShort messageType) const;
        // Gets all events of for a specific message type you are subscribing to.
        Event::Set GetEventSubscriptionsOfType(const UShort messageType, const Address& source) const;
        // Gets all events of for a specific message type you are generating.
        Event::Set GetProducedEventsOfType(const UShort messageType) const;
        // Gets the events being generated by your component/service.
        Event::Map* GetProducedEvents() const;
        // Gets the events being subscribed to (generated by other components/services).
        Event::Map* GetEventSubscriptions() const;
        // Gets a pointer to all periodic events being subscribed to.
        Event::Set* GetPeriodicEventSubscriptions() const;
        // Gets a pointer to all periodic events being produced.
        Event::Set* GetProducedPeriodicEvents() const;
    protected:
        CxUtils::Mutex mEventsMutex;            ///<  Mutex for protecting event data.
        Address mComponentID;                   ///<  Component ID.
        Event::Map mProducedEvents;             ///<  Map of all events to generate.
        Event::Map mEventSubscriptions;         ///<  Events being subscribed to.
        Event::Set mPeriodicSubscriptions;      ///<  Periodic event subscriptions.
        Event::Set mProducedPeriodicEvents;     ///<  Periodic events to generate. 
    };
}


#endif
/*  End of File */
