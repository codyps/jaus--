////////////////////////////////////////////////////////////////////////////////////
///
///  \file informcomponent.h
///  \brief This file contains software for creating JAUS components that
///  primarily act as informers.  An informer is typically a sensor or
///  component that provides information to other components.  The class
///  described here InformComponent is an advanced version of Component
///  with methods to simplify the creation of events and service connections
///  for notifying other services.
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
#ifndef __JAUS_INFORM_COMPONENT__H
#define __JAUS_INFORM_COMPONENT__H

#include "jaus/components/eventmanager.h"
#include "jaus/components/subscribercomponent.h"
#include "jaus/components/serviceconnection.h"
#include "jaus/messages/command/core/createserviceconnection.h"
#include "jaus/messages/command/core/suspendserviceconnection.h"
#include "jaus/messages/command/core/activateserviceconnection.h"
#include "jaus/messages/command/core/terminateserviceconnection.h"
#include "jaus/messages/command/core/confirmserviceconnection.h"
#include <cxutils/thread.h>
#include <cxutils/timer.h>

namespace Jaus
{
    typedef void (*UpdateServiceConnectionCallback)(const ServiceConnection& sc, void* args);

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class InformComponent
    ///   \brief Interface class for creating JAUS components that notify other
    ///   components through inform messages (i.e. sensor components).
    ///
    ///   This class extends the functionality for subscribing to events and service
    ///   connections found in the SubscriberComponent class by adding support
    ///   for managing events and service connections being produced by your component.
    ///
    ///   An InformComponent class is used when you want to provide events and
    ///   service connections to subscribing components.
    ///
    ///   In order to generate accurate periodic events/service connections this
    ///   class makes use of a High Performance Timer (HPT).  It is possible to
    ///   enable for all periodic events, but this will use a lot of computing
    ///   resources in Windows.  By default, any requested periodic rate above
    ///   30Hz will result in the use of a HPT for event generation.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL InformComponent : public SubscriberComponent
    {
    public:
        // Constructor.
        InformComponent();
        // Destructor.
        virtual ~InformComponent();
        // Shutsdown all events/subscriptions.
        virtual int Shutdown();
        // Sets up core services for component
        virtual int SetupCoreService();
        // Enable HPT for all periodic events.
        int UseHighPerformanceTimer(const bool on);
        // Set the maximum periodic rate before using HPT on periodic events.
        int SetHighPerformanceTimerThreshold(const double freq);      
        // Processes messages for creating events, service connections, etc.
        virtual int ProcessCommandMessage(const Message* msg, 
                                          const Byte commandAuthority);
        // Processes queries for event lists.
        virtual int ProcessQueryMessage(const Message* msg);
        // This method is called whenever an event needs to be generated.
        virtual int GenerateEvent(const Event* eventInfo);
        // This method is called whenever a service connection message needs to be sent.
        virtual int GenerateServiceConnectionMessage(const ServiceConnection& sc);
        // This method is called whenever a request for an event is made.
        virtual int ProcessEventRequest(const CreateEventRequest& command,
                                        Byte& responseValue,
                                        double& confirmedRate,
                                        std::string& errorMessage) const;
        // This method is called whenever a request for an event update is made.
        virtual int ProcessEventUpdateRequest(const UpdateEvent& command,
                                              Byte& responseValue,
                                              double& confirmedRate,
                                              std::string& errorMessage) const;
        // This method is called whenever a request for a service connection is made.
        virtual int ProcessServiceConnectionRequest(const Jaus::CreateServiceConnection& command,
                                                    Byte& responseValue,
                                                    double& confirmedRate) const;
        virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                          const SubscriberComponent::DiscoveryEvents eventType = SubsystemUpdate);
        // Checks to see if the HPT is enabled for all periodic events.
        bool IsHighPerformanceTimerOn() const;
        // Get the threshold for determining if the HPT should be used.
        double GetHighPerformanceTimerThreshold() const { return mTimerThresholdFrequency; }
        // Get the number of service connections being provided by component.
        virtual unsigned int GetNumServiceConnectionsProvided() const;
        // Get the number of service connections being requested by component.
        virtual unsigned int GetNumServiceConnectionsRequested() const;
    private:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class HighPerformanceTimer
        ///   \brief Timer class for service connections requring high frequencies.
        ///
        ///   A high update frequency is anything over 75Hz.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class HighPerformanceTimer : public CxUtils::TimerCallback
        {
        public:
            HighPerformanceTimer();
            ~HighPerformanceTimer();
            void ProcessTimerEvent(const unsigned int id);
            ServiceConnection* mpServiceConnection;  ///<  Service connection to use for events.
            Event::Key* mpEventKey;                  ///<  Event key to use for event generation.
            InformComponent* mpInformComponent;      ///<  Pointer to component to generate event.
            CxUtils::Mutex* mpScMutex;               ///<  Pointer to mutex for service connection.
            CxUtils::Timer mTimer;                   ///<  Timer generating events.
        };

        void GenerateServiceConnectionMessages(const bool lock = false);
        void GeneratePeriodicEvents(const bool lock = true);
        void CreateHighPerformanceTimer(const double rate,
                                        ServiceConnection* serviceConnection,
                                        Event::Key* eventKey);
        void StopHighPerformanceTimer(ServiceConnection* serviceConnection,
                                      Event::Key* eventKey);
        virtual void SubscriptionsLoop();       
        
        typedef std::set<HighPerformanceTimer*> HighPerformanceTimerSet;
        CxUtils::Mutex  mHighPerformanceTimersMutex;         ///<  Mutex for HPT's.
        volatile bool mUseHighPerformanceTimerFlag;          ///<  If true, always use high performance timer for SC's.
        double mTimerThresholdFrequency;                     ///<  Frequency for use of HP timer.        
        HighPerformanceTimerSet mHighPerformanceTimers;      ///<  All high performance timers for periodic events.
    };
}


#endif 
/*  End of File */
