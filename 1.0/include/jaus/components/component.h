////////////////////////////////////////////////////////////////////////////////////
///
///  \file component.h
///  \brief This file contains software for deriving/creating components
///  and services that support JAUS.  
///
///  The base class here (Component) contains all capability needed to
///  communicate with the JAUS Node Manager (NodeManager), and supports all of
///  the core messages defined in JAUS.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 9 February 2007
///  <br>Last Modified: 14 March 2008
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
#ifndef _JAUS_COMPONENT_BASE_H
#define _JAUS_COMPONENT_BASE_H

#ifdef __cplusplus

#include "connectionhandler.h"
#include "messagehandler.h"
#include "transport/jsharedmemory.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/inform/acknackmessage.h"
#include "jaus/messages/common/configuration/service.h"
#include <cxutils/mutex.h>
#include <string>

namespace Jaus
{    
    class ConnectionHandler;    // Forward declaration.
    class MessageHandler;       // Forward declaration.
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Component
    ///   \brief Interface class for creating a JAUS component.
    ///
    ///   This class is the base for creating any type of JAUS component.  It supports
    ///   connections to the Node Manager, and sending/receiving of messages.  As it
    ///   stands this class supports the core set of JAUS messages as defined by
    ///   the current JAUS Reference Architecture (RA).
    ///
    ///   There are multiple ways to receive incomming messages.  You can use 
    ///   callbacks, or you can inherit from Component and overload the different
    ///   virtual functions for processing messages (preferred method).  See the
    ///   JAUS++ documentation for creating new components, or look at the
    ///   examples that come with the library.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL Component : public ErrorHistory
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Status
        ///   \brief Class containing enumerations and constants associated with the
        ///          Primary status value of the message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL Status
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \enum Primary
            ///   \brief Allowed values for bitw 0-3 of Primary Status.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            enum Primary
            {
                Initialized = 0,
                Ready,
                Standby,
                Shutdown,
                Failure,
                Emergency
            };
        };
        // Constructor.
        Component();
        // Destructor.
        virtual ~Component();  
        // Initialize component for sending/receiving messages.
        virtual int Initialize(const std::string& name,
                               const Address& id,
                               const unsigned int mbsize = JAUS_SHARED_MEMORY_DEFAULT_SIZE); 
        // Shutdown component and communication.
        virtual int Shutdown();
        // Sends the serialized message stream data.
        virtual int Send(const Stream& msg) const;
        // Serializes, then sends the message.
        virtual int Send(const Message* msg) const;
        // Sends the message and waits for response message.
        virtual int Send(const Stream& msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT) const;
        // Sends the message and waits for response to arrive, and returns it.
        virtual int Send(const Message* msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT) const; 
        // Sets a flag to indicate if component is controllable.
        void SetControllable(const bool on);
        // Sends an acknowledge message to the source of the message passed.
        int SendAcknowledgeResponse(const Message* msg);
        // Sends a negative acknowledge message to the source of the message passed.
        int SendNegativeAcknowledgeResponse(const Message* msg);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(const UShort mcode, MessageCallback* cb);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(const UShort mcode, MessageFunctionCallback cb, void* args);
        // Removes a callback registered for a specific message.
        void RemoveCallback(const UShort mcode);
        // Ignore messages requesting acknowledge/negative acknowledge.
        // This is false by default.
        virtual void BlockAckNackMessages(const bool enable = true);
        // Set a list of accepted priories for messages, block all others.
        // By default, all levels of priority are accepted.
        virtual int SetAcceptedPriorityValues(const std::set<UShort> priorities);
        // Reset the component.
        virtual int Reset();
        // Sets the component in standy mode.
        virtual int Standby();
        // Tell the component to resume.
        virtual int Resume();
        // Sets up core component services.
        virtual int SetupCoreService();
        // Set up type of services/messages supported by component.
        virtual int SetupService();
        // Add a service that your component supports.
        int AddService(const Service& service);
        // Remove a serivice supported by your component.
        int RemoveService(const UShort type);
        // Get a service supported by the component.
        int GetService(const UShort type, Service& srvc);
        // General process for stream processing.  Called for unknown messages typically.
        virtual int ProcessMessage(const Stream& msg, const Header& header);
        // Generaal process message method.  Calls more specific process message methods.
        virtual int ProcessMessage(const Message* msg);
        // Process command messages.
        virtual int ProcessCommandMessage(const Message* msg);
        // Process command messages.
        virtual int ProcessCommandMessage(const Message* msg, const Byte commandAuthority);
        // Process query messages.
        virtual int ProcessQueryMessage(const Message* msg);
        // Process inform messages.
        virtual int ProcessInformMessage(const Message* msg);
        // Process experimental messages.
        virtual int ProcessExperimentalMessage(const Message* msg);
        // Process an Acknowledge/Negative Acknowledge message.
        virtual int ProcessAckNackMessage(const AckNackMessage* msg);        
        // Set the status of the component (standby, initialized, etc.)
        virtual int SetPrimaryStatus(const Component::Status::Primary status);    
        // Set the secondary status of component.
        virtual int SetSecondaryStatus(const UInt secondary);
        // Set emergency state for component.
        virtual int SetEmergency(const UShort code);
        // Clear emergency state for component.
        virtual int ClearEmergency(const UShort code);      
        // Request authority level of a component.
        int QueryComponentAuthorityLevel(const Address& dest, 
                                         Byte& auth,
                                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                                         const UShort tries = JAUS_MAX_SEND_COUNT);
        // Use this method to make sure some messages are not buffered within internal queue.
        int AddToDoNotBufferList(const UShort messageCode, const Address& source = Address());
        // Checks to see if connected to node manager.
        bool IsConnected() const; 
        // Check to see if component is initialized.
        bool IsInitialized() const;
        // Check to see if the component is controllable.
        bool IsControllable() const;
        // Check if the component is being controlled.
        bool IsControlled() const;
        // Check to see if the input message is supported.
        bool IsInputMessageSupported(const UShort messageCode, const UInt presenceVector) const;
        // Check to see if the output message is supported.
        bool IsOutputMessageSupported(const UShort messageCode, const UInt presenceVector) const;
        // Set the authority level of the component.
        void SetComponentAuthority(const Byte auth);
        // Reset the controller of this component.
        void ClearControllerID();
        // Get the primary status of component.
        Component::Status::Primary GetPrimaryStatus() const;
        // Get the secondary status of component.
        UInt GetSecondaryStatus() const { return mSecondaryStatus; }
        // Get the ID of component who has exclusive control over this component/service.
        Address GetControllerID() const { return mControllerID; }
        // Get the Authority of the component that has exclusive control over this component/service.
        Byte GetControllerAuthority() const { return mControllerAuthorityCode; }
        // Get the components ID.
        Address GetID() const { return mComponentID; }
        // Get the authority level of the component.
        Byte GetComponentAuthority() const { return mAuthorityCode; }
        // Get the name of the component/service.
        std::string GetName() const { return mComponentName; }   
        // Use this method to get a list of all Node Managers on machine.
        static int GetNodeManagerList(Address::List &id);
        // Use this to find the ID of a node running on the system.
        static bool IsNodeManagerPresent(Address* id = NULL);
        // Get a copy of services provided.
        Service::Map GetServices() const;
        // Provides a pointer to the Connection Handler used by the component (try not to use this).
        ConnectionHandler* GetConnectionHandler() const;
        // Provides a pointer to the Message Handler used by the component (try not to use this).
        MessageHandler* GetMessageHandler() const;
    private: 
        ///////////////////////////////////////////////////////////////////////
        ///
        ///  \class ComponentConnectionHandler
        ///  \brief Connection handler interface used by the Component
        ///         class to communicate with Node Manager.
        ///
        ///////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL ComponentConnectionHandler : public ConnectionHandler
        {
        public:
            ComponentConnectionHandler();
            virtual ~ComponentConnectionHandler();
            int Initialize(const Address& id, const unsigned int mbsize);
            virtual int Shutdown();
            virtual int SendStream(const Stream& msg);
            CxUtils::Mutex mPrioritiesMutex;
            Jaus::JSharedMemory mInbox;  ///<  Message box for incomming messages.
            Jaus::JSharedMemory mOutbox; ///<  Message box for outgoing messages.
            Jaus::JSharedMemory::Registry mRegistry; ///<  Node/Component registry.
        };
        static void StreamCallbackFunction(const Stream&, const Header*, const Byte, void*);
        std::string mComponentName;             ///<  Component name in ASCII format.
        Address mComponentID;                   ///<  Component address.
        Address mControllerID;                  ///<  Address of component authority.
        volatile Byte mAuthorityCode;           ///<  Authority level for component.
        volatile Byte mControllerAuthorityCode; ///<  Authority level of controller of component.        
        volatile Byte mPrimaryStatus;           ///<  Primary status code for component.
        volatile UInt mSecondaryStatus;         ///<  Secondary status code for component.
        volatile bool mIsControllableFlag;      ///<  Can the component be controlled?
        CxUtils::Mutex mComponentMutex;         ///<  Mutex to protect component specific content.
        MessageCallbackMap mMessageCallbacks;   ///<  Message callbacks for specific types of messages.
        Service::Map mComponentServices;        ///<  Services supported by your component.
        MessageHandler* mpMessageHandler;                   ///<  Message handler for component.
        ComponentConnectionHandler* mpConnectionHandler;    ///<  Connection handler for component.
    };
}

#endif
#endif
/*  End of File */
