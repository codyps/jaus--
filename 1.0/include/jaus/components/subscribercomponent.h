////////////////////////////////////////////////////////////////////////////////////
///
///  \file subscribercomponent.h
///  \brief This file contains a more advanced version of the JAUS++ Component
///  class.  This child class of Component adds additional functionality for
///  creating requesting Inform Service Connections and Events from service
///  providers.  It also performs discovery capabilities by gathering 
///  system configuration data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 July 2007
///  <br>Last Modified: 25 April 2008
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
#ifndef __JAUS_SUBSCRIBER_COMPONENT__H
#define __JAUS_SUBSCRIBER_COMPONENT__H

#include "jaus/components/eventmanager.h"
#include "jaus/components/component.h"
#include "jaus/components/serviceconnection.h"
#include "jaus/messages/common/platform/platform.h"
#include "jaus/messages/command/core/createserviceconnection.h"
#include "jaus/messages/command/core/suspendserviceconnection.h"
#include "jaus/messages/command/core/activateserviceconnection.h"
#include "jaus/messages/command/core/terminateserviceconnection.h"
#include "jaus/messages/command/core/confirmserviceconnection.h"
#include "jaus/messages/inform/configuration/reportsubsystemlist.h"
#include <cxutils/thread.h>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SubscriberComponent
    ///   \brief Advanced Component based class that has extra capabilities for
    ///   requesting events and inform service connections from components and
    ///   keeping track of them.
    ///
    ///   If a developer wants to quickly create a component that just gathers
    ///   data from other components, this is the interface to use.  It automatically
    ///   keeps track of the configurations of different subsystems, and 
    ///   can automatically create service connections and events for you.
    ///
    ///   When shutdown, this component will also automatically cancel/terminate
    ///   all events and service connections requested.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL SubscriberComponent : public Component
    {
        friend class InformComponent;
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief If Dynamic System Discovery is enabled for the component, then
        ///   any time a subsystem is discovered, lost, or updated, an event is generated.
        ///   This enumeration lists the types of events.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum JAUS_CMP_DLL DiscoveryEvents
        {
            SubsystemConnected = 0,
            SubsystemUpdate = 1,
            SubsystemDisconnect = 2
        };
        // Constructor.
        SubscriberComponent();
        // Destructor.
        virtual ~SubscriberComponent();
        // Initialize component for sending/receiving messages.
        virtual int Initialize(const std::string& name,
                               const Address& id,
                               const unsigned int mbsize = JAUS_SHARED_MEMORY_DEFAULT_SIZE); 
        // Shutdown the component interface.  (overload to shutdown your device also).
        virtual int Shutdown();
        // Resets the Subscriber Component (terminates all subscriptions, etc.)
        virtual int Reset();
        // Sets up the core services provided by the component.
        virtual int SetupCoreService();
        // Enables automatic discovery of subsystems and there configurations.
        virtual int EnableSubsystemDiscovery(const bool on = true, 
                                             const std::set<Byte>* subsystems = NULL);
        // Add subsystem to discovery to discovery list.
        virtual int AddSubsystemToDiscover(const Byte subsystemID);
        virtual int QuerySubsystemServices(const Byte subsystemID);
        // Process inform message.
        virtual int ProcessInformMessage(const Message* msg);
        // Request an event from a providing component.
        virtual int RequestEvent(const CreateEventRequest& request,
                                 Event* eventInfo = NULL,
                                 const UShort timeoutMs = JAUS_RESPONSE_TIMEOUT_TIME,
                                 const UShort tries = JAUS_MAX_SEND_COUNT);
        // Request an update to an existing event you are subscribing to.
        virtual int RequestEventUpdate(const UpdateEvent& request,
                                       const UShort timeoutMs = JAUS_RESPONSE_TIMEOUT_TIME,
                                       const UShort tries = JAUS_MAX_SEND_COUNT);
        // Create an Inform Service Connection subscription.
        virtual int CreateInformServiceConnection(const Address& provider,
                                                  const UShort messageCode,
                                                  const UInt presenceVector,
                                                  const double desiredPeriodicRate,
                                                  const double minPeriodicRate = -1,
                                                  const UShort timeoutMs = JAUS_RESPONSE_TIMEOUT_TIME,
                                                  const UShort tries = JAUS_MAX_SEND_COUNT);
        // If discovery is enabled, this function is called when an event happens.
        virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                          const DiscoveryEvents eventType = SubsystemUpdate);
        // Cancel all events being subscribed to.
        virtual int CancelRequestedEvents();
        // Cancel events from specific source.
        virtual int CancelEvents(const Address& id);
        // Suspend a service connection subscription.
        virtual int SuspendServiceConnection(const ServiceConnection& sc);
        // Activate a suspended service connection subscription.
        virtual int ActivateServiceConnection(const ServiceConnection& sc);
        // Terminate a service connection subscription.
        virtual int TerminateServiceConnection(const ServiceConnection& sc);
        // Terminate all service connection subscriptions.
        virtual int TerminateServiceConnections();
        // Terminate all service connections from a specific service provider.
        virtual int TerminateServiceConnections(const Address& provider);
        // Check if the component has event subscriptions of a specific type.
        virtual bool HaveEventSubscription(const Address& id,
                                           const UShort messageCode) const;
        // Check if the component has event subscriptions of a specific type.
        virtual bool HaveEventSubscriptionsOfType(const UShort messageCode) const;        
        // Check if the component has a service connection subscription.
        virtual bool HaveServiceConnection(const Address& provider,
                                           const UShort messageCode) const;
        // Check if the component has a service connection subscription, and get info.
        virtual bool HaveServiceConnection(const Address& provider,
                                           const UShort messageCode,
                                           ServiceConnection& scInfo) const;
        // If class detects a service connection has been lost, this function is called.
        virtual bool ReEstablishServiceConnection(const ServiceConnection& sc) const;
        // If class detects event has been lost, this function is called.
        virtual bool ReEstablishEvent(const Event* ev) const;
        // Check if system discovery is enabled.
        virtual bool IsSubsystemDiscoveryEnabled() const { return mSystemDiscoveryFlag; }
        // Get the number of service connections.
        virtual unsigned int GetNumServiceConnections() const;
        // Gets a copy of all service connections.
        virtual ServiceConnectionMap GetServiceConnections() const;
        // Get all event information (subscriptions and produced events).
        virtual EventManager* GetEvents() const { return (EventManager*)(&mEventManager); }
        // Gets the platform information if available.
        int GetPlatformInfo(Platform& info, const Byte subsystemID) const;
        // Gets a copy of the known system configuration (only valid if subsystem discovery is on).
        virtual Platform::Map GetSystemConfiguration() const;
        // Get the configuration of a platform.
        Platform GetPlatformInfo(const Byte subsystemID) const;
    protected:   
        EventManager mEventManager;      ///<  Stores all events for component.
    private:
        static void StreamCallbackFunction(const Stream&, const Header*, const Byte, void*);
        static void PeriodicEventThreadFunction(void *args); 
        virtual void SubscriptionsLoop();
        void CheckServiceConnections();
        void CheckEvents();
        void CheckDiscoveryEvents();
        void UpdateSubsystemList(const ReportSubsystemList* report);
        volatile bool mSystemDiscoveryFlag;         ///<  If true, component acquires system configuration.
        Event::List mLostEvents;                    ///<  Events that we lost and need to re-acquire.
        Platform::Map mSubsystems;                  ///<  Known subsystems.
        Address::Set    mSubsystemList;             ///<  Subsystem list.
        std::set<Byte>* mSubsystemsToDiscover;      ///<  Set of subsystems to discover.
        CxUtils::Mutex mSubsystemsMutex;            ///<  Mutex for thread protection of configuration data.
        CxUtils::Mutex  mServiceConnectionsMutex;   ///<  Mutex for protection of service connection data.
        CxUtils::Thread mPeriodicEventThread;       ///<  Thread for checking subscriptions, etc.
        ServiceConnectionMap mServiceConnections;   ///<  Map of all active service connections.
    };
}

#endif
/*  End of File */
