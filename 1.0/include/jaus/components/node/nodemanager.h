////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodemanager.h
///  \brief This file contains software used to create a JAUS Node Manager, and
///  is one of the most critical parts of the JAUS library.
///
///  The JAUS Node Manager (NodeManager) handles all connections to
///  JAUS components on a node (device with physical IP address "ex: computer"),
///  and processing incomming data from the network.  It routes all messages
///  within a node.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 6 April 2008
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
#ifndef __JAUS_NODE_MANAGER_H
#define __JAUS_NODE_MANAGER_H

#ifdef __cplusplus

#include "jaus/messages/common/configuration/service.h"
#include "jaus/messages/common/configuration/configuration.h"
#include "jaus/components/eventmanager.h"
#include "jaus/components/receipt.h"
#include "jaus/components/node/communicator.h"

namespace Jaus
{
    class NodeConnectionHandler;
    const unsigned int JAUS_NODE_SM_BUFFER_DEFAULT_SIZE    = 4194304;   ///<  Default Shared Memory size for the node to recieve messages.
    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class NodeManager
    ///   \brief Handles all transportation of messages accross different
    ///   transport layers.
    ///
    ///   This class uses the NodeConnectionHandler, ServiceConnectionManager, and
    ///   the MessageHandler classes.  The collection of these 3 pieces allows it to
    ///   transfer messages between components on a single system, talk to
    ///   other Node Manager applications, and process and respond to messages
    ///   sent to the node.
    ///
    ///   If a Node Manager is not running on a system, no communication will
    ///   take place between components.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL NodeManager : public ErrorHistory
    {
    public:
        // Constructor.
        NodeManager();
        // Destructor.
        ~NodeManager();
        // Initialize the node manager.
        int Initialize(const Byte sid, 
                       const Byte nid,
                       const bool tcp = false,
                       const UInt smSize = JAUS_NODE_SM_BUFFER_DEFAULT_SIZE);
        // Initialize from settings file.
        int Initialize(const std::string& settingsXML);
        // Shutdown the node manager.
        int Shutdown();
        // Restart the node manager.
        int Restart();
        // Sends the serialized message stream data.
        int Send(const Stream& msg);
        // Serializes, then sends the message.
        int Send(const Message* msg);
        // Sends the message and waits for response message.
        int Send(const Stream& msg,
                 Receipt& receipt,
                 const UShort responseCode = 0,
                 const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                 const UShort tries = JAUS_MAX_SEND_COUNT);
        // Sends the message and waits for response to arrive, and returns it.
        int Send(const Message* msg,
                 Receipt& receipt,
                 const UShort responseCode = 0,
                 const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                 const UShort tries = JAUS_MAX_SEND_COUNT); 
        // Sends an acknowledge message to the source of the message passed.
        int SendAcknowledgeResponse(const Message* msg);
        // Sends a negative acknowledge message to the source of the message passed.
        int SendNegativeAcknowledgeResponse(const Message* msg);
        // Process command messages.
        int ProcessCommandMessage(const Message* msg);
        // Process query messages.
        int ProcessQueryMessage(const Message* msg);
        // Process inform messages.
        int ProcessInformMessage(const Message* msg);
        // Process experimental message.
        int ProcessExperimentalMessage(const Message* msg);
        // Set the address to use for multicasting.
        int SetMulticastAddress(const std::string& multicast, const unsigned char ttl);
        // Set the network interface to use for receiving network traffic.
        bool SetNetworkInterface(const int num);
        // Set the network interface to use for receiving network traffic.
        bool SetNetworkInterface(const std::string& address);
        // Create a log file by generating a unique file name.
        int CreateLogFile();
        // Create a log file using specified file name.
        int CreateLogFile(const std::string& logfileXML);
        // Close log file.
        int CloseLogFile();
        // Sets the subsystem identification.
        int SetSubsystemIdentification(const Identification& ident);
        // Enables discovery of subsystem configuration data (send heartbeat pulse messages to 255.255.1.1)
        int EnableSubsystemConfigDiscovery(const bool enable);
        // Sets the system configuration.
        int SetSystemConfiguration(const Configuration& config);
        // Check if a log file is open.
        bool IsLogOpen() const;
        // Check if node has been initialized.
        bool IsInitialized() const;    
        // Check if broadcasting to 255.255.1.1 is enabled or disabled (subsystem discovery).
        bool IsSubsystemConfigDiscoveryEnabled() const { return mSubsystemDiscoveryFlag; } 
        // Gets the subsystem identification the node knows about.
        Identification GetSubsystemIdentification() const;
        // Gets known system configuration data
        Configuration GetSystemConfiguration() const;
        // Gets subsystem configuration.
        Configuration::Subsystem GetSubsystemConfiguration() const;
        // Toggles use of UDP multicast/broadcast for delivering heartbeat messages.
        void ToggleBroadcastUDP(const bool on);
        // Get the node ID.
        inline Address GetNodeID() const { return mNodeID; }
        // Get the time of last connection or subsystem list update.
        unsigned int GetConnectionEventTimeMs() const { return mConnectionEventTimeMs; }
        // Gets the list of components broadcasting hearbeats to 255.255.1.1.
        Address::Set GetSubsystemList() const;
        // Gets a pointer to the connection manager.
        NodeConnectionHandler* GetConnectionHandler() const;
        // Get a pointer to the communicator.
        Communicator* GetCommunicator() const;
        // Use this to find the ID of a node running on the system.
        static bool IsNodeManagerPresent(Address* id = NULL);
        static int GetNodeManagerAddresses(Address::List &id);
        void CheckSubsystemList();                          ///<  Checks to see if subsystem list is up to date.
    protected:
        void ReportNodeConfiguration(const Address& dest, const bool lockNode = true);
        void ReportNodeConfiguration(const Event* eventInfo, const bool lockNode = true);
        void ReportSubsystemConfiguration(const Address& dest, const bool lockNode = true);
        void ReportSubsystemConfiguration(const Event* eventInfo, const bool lockNode = true);
        void ReportSubsystemList(const Address& dest, const bool lockNode = true);
        void ReportSubsystemList(const Event* eventInfo, const bool lockNode = true);
        void ReportNodeConfigurationEvent(const bool lockNode = true, const bool lockEvents = true);
        void ReportSubsystemConfigurationEvent(const bool lockNode = true, const bool lockEvents = true);
        void ReportSubsystemListEvent(const bool lockNode = true, const bool lockEvents = true);
        static void ProcessConnectionEvent(const Byte type, const Address& id, void *args);
        static void MessageCallbackFunction(const Message*, void *);               
        bool mInitializedFlag;                              ///<  Indicates node manager is initialized.
        CxUtils::Mutex mNodeMutex;                          ///<  Mutex to protect node manager data.
        Address mNodeID;                                    ///<  Node ID.
        Address::Set mNoEventSupport;                       ///<  List of nodes that do not support events.
        EventManager mEvents;                               ///<  Node events.
        Address::Set mSubsystemList;                        ///<  Subsystem list.
        Service mServices;                                  ///<  Node services.
        Identification mSubsystemIdentification;            ///<  Subsystem identification.
        Configuration mSystemConfiguration;                 ///<  System configuration information.
        Configuration::Subsystem mSubsystemConfigFromXML;   ///<  Subsystem configuration data found in XML file.
        NodeConnectionHandler* mpConnectionHandler;         ///<  Handles all connections.
        MessageHandler *mpMessageHandler;                   ///<  Handles all message processing for node.
        volatile bool mSubsystemDiscoveryFlag;              ///<  Enable subsystem configuration discovery.
        volatile unsigned int mConnectionEventTimeMs;       ///<  Time of last connection event in ms.
        std::map<Byte, unsigned int> mSubsystemHeartbeatTimes;///<  The times when heart pulse messages were received (used to maintain subsystem list).
        std::map<Byte, unsigned int> mNodeRequestTimeMs;      ///<  The last time the manager requested events/configuration from node in subsystem.
    };
}


#endif
#endif
/*  End of File */
