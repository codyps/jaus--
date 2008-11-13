////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodeconnectionhandler.h
///  \brief Contains the NodeConnectionHandler class, which manages all 
///  incomming and outgoing connections to components and nodes for the
///  Node Manager.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 15 November 2007
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
#ifndef __JAUS_NODE_CONNECTION_HANDLER_H
#define __JAUS_NODE_CONNECTION_HANDLER_H

#include "jaus/components/messagehandler.h"
#include "jaus/components/connectionhandler.h"
#include "jaus/components/node/nodeconnection.h"
#include "jaus/components/node/nodelogger.h"
#include "jaus/components/node/communicator.h"

#include <cxutils/mutex.h>
#include <map>

namespace Jaus
{
    typedef void (*NodeConnectionCallback)(const Byte type, const Address& id, void *args);
    const unsigned int JAUS_NODE_SM_DEFAULT_SIZE    = 4194304;   ///<  Default Shared Memory size for the node to recieve messages.
    class ServiceConnectionManager;

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \class NodeConnectionHandler
    ///  \brief Maintains all connections to components and other nodes for the
    ///         Node Manager.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL NodeConnectionHandler : public ConnectionHandler
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \enum ConnectionEvents
        ///  \brief Enumeration of different types of connection events.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum ConnectionEvents
        {
            ComponentConnect = 0,
            NodeConnect,
            ComponentDisconnect,
            NodeDisconnect
        };
        // Constructor.
        NodeConnectionHandler();
        // Destructor.
        ~NodeConnectionHandler();
        int Initialize(const Byte sid,
                       const Byte nid,
                       Jaus::MessageHandler* msgHandler,
                       const bool tcp = false,
                       const std::string& netAddress = "-1",
                       const unsigned int inboxSize = JAUS_NODE_SM_DEFAULT_SIZE);
        // Shutsdown all connections.
        virtual int Shutdown();
        // Resets all connections.
        virtual int Restart();
        // Sends the byte array data to destination.
        virtual int Send(const Stream& msg);
        // Serializes, then sends the message.
        virtual int Send(const Message* msg);
        // Sends the message and waits for response message.
        virtual int Send(const Stream& msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT);
        // Sends the message and waits for response to arrive, and returns it.
        virtual int Send(const Message* msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT); 
        // Creates a connection with the given ID.
        int CreateConnection(const Address& id,
                             const bool discoverFlag,
                             const std::string* host);
        // Create a serial port connection.
        int CreateSerialConnection(const Address& id,
                                   const std::string& port,                             
                                   const unsigned int baud,
                                   const unsigned int bits = 8,
                                   const unsigned int parity = 0,
                                   const unsigned int stop = 1);
        int CloseConnection(const Address& id);
        // Set the address to use for multicasting.
        int SetMulticastAddress(const std::string& multicast = "224.1.0.1",
                                const unsigned char ttl = 1);
        // Check if a connection exists to the given ID.
        bool HaveConnection(const Address& id) const;
        // Toggles subsystem hearbeat broadcasting versus node only broadcasting.
        void ToggleSubsystemHeartbeatBroadcast(const bool on);
        // Toggles use of UDP multicast/broadcast for delivering heartbeat messages.
        void ToggleBroadcastUDP(const bool on);
        // Register Connection Event Callback
        int SetConnectionEventCallback(NodeConnectionCallback cb, void *args);
        // Get a list of all node connections.
        Address::Set GetNodeConnections() const;
        // Get a list of all component connections.
        Address::Set GetComponentConnections() const;
        // Get a pointer to the node logger.
        NodeLogger* GetNodeLogger() const { return (NodeLogger*)(&mLogger); }
        // Get a pointer to the communicator.
        Jaus::Communicator* GetCommunicator() const { return (Jaus::Communicator*)(&mCommunicator); }
    protected: 
        // Sends the byte array data to destination.
        virtual int SendStream(const Stream& msg);
        virtual void ProcessStreamCallback(const Stream& msg,
                                           const Header* info,
                                           const StreamCallback::Transport transport);
        static void DiscoveryThread(void *args);
        void CheckComponents(Address::Set& newConnections, Address::Set& lostConnections);
        void CheckNodes(Address::Set& newConnections, Address::Set& lostConnections);
        typedef std::map<Address, JSharedMemory*> ComponentsMap;  ///<  STL map of component connections.
        typedef std::map<Address, NodeConnection*> NodesMap;      ///<  STL map of node connections.  
        // Handles SC management
        ServiceConnectionManager* mpServiceManager; ///<  SC management.
        // Flags
        volatile bool mTCPFlag;                     ///<  If true, TCP server and client connections are used.
        volatile bool mBroadcastFlag;               ///<  If true, heartbeat messages are broadcast instead of multicast.
        volatile bool mSubsystemDiscoverFlag;       ///<  If true, broadcast heartbeats to 255.255.1.1 instead of subsystem.255.1.1
        volatile bool mAddressConflictFlag;         ///<  If true, then there is an address conflict.
        volatile bool mInitializingFlag;            ///<  Flag to indicate the handler is initializing.
        // Callback
        void* mpConnectionEventCallbackArgs;             ///<  Arguments for connection event callback.
        NodeConnectionCallback mConnectionEventCallback; ///<  Callback for connection events.
        // Logging
        NodeLogger mLogger;                         ///<  Node traffic logger.
        // Thread objects
        CxUtils::Thread mDiscoveryThread;           ///<  Thread for discovering/removing connections.
        Address::Set mDiscoveredComponents;         ///<  Set of components we do not yet have connections for.
        // Thread protection.
        CxUtils::Mutex mComponentsMutex;            ///<  Mutex for component connections.
        CxUtils::Mutex mNodesMutex;                 ///<  Mutex for node connections.
        CxUtils::Mutex mDiscoveryMutex;             ///<  Mutex used by the discovery thread.
        CxUtils::Mutex mConnectionEventMutex;       ///<  Mutex used for connection event processing.
        // Registries for nodes and components in shared memory.
        JSharedMemory::Registry mNodeRegistry;      ///<  Shared Memory list of nodes on machine.
        JSharedMemory::Registry mComponentRegistry; ///<  Shared Memory list of components on node.
        std::string mMulticastAddress;              ///<  Address to use for multicast sending/receiving.
        unsigned char mMulticastTTL;                ///<  TTL for Multicast.
        // Outgoing connections.
        JUDPClient mMulticastUDP;                   ///<  For transmission to multicast address.
        JUDPClient mBroadcastUDP;                   ///<  If broadcast is enabled, broadcast discovery message.
        ComponentsMap mComponents;                  ///<  Component connections.
        NodesMap      mNodes;                       ///<  Node connections.
        // Incomming message transports
        JSharedMemory mSharedMemoryInput;           ///<  Message inbox for shared memory messages.
        JUDPServer    mUdpInput;                    ///<  Inbox for UDP messages.
        JTCPServer    mTcpInput;                    ///<  Inbox for TCP messages.
        // Communicator
        Jaus::Communicator mCommunicator;           ///<  Communicator to use for transmission to other subsystems.
    };
}

#endif

/*  End of File */
