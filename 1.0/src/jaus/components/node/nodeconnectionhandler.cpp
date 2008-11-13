////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodeconnectionhandler.cpp
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
#include "jaus/components/node/nodeconnectionhandler.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/inform/informcodes.h"
#include "jaus/messages/inform/communications/reportheartbeatpulse.h"
#include "jaus/messages/common/configuration/service.h"
#include "jaus/components/node/serviceconnectionmanager.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor, initializes default values.
///
////////////////////////////////////////////////////////////////////////////////////
NodeConnectionHandler::NodeConnectionHandler()
{
    mTCPFlag = false;
    mSubsystemDiscoverFlag = true;
    mAddressConflictFlag = false;
    mInitializingFlag = false;
    mpConnectionEventCallbackArgs = NULL;
    mConnectionEventCallback = NULL;
    mpServiceManager = new ServiceConnectionManager();
    mMulticastAddress = "224.1.0.1";
    mMulticastTTL = 1;
    mBroadcastFlag = false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, shuts down the handler.
///
////////////////////////////////////////////////////////////////////////////////////
NodeConnectionHandler::~NodeConnectionHandler()
{
    Shutdown();
    delete mpServiceManager;
    mpServiceManager = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the connection handler.
///
///   \param sid The subsystem ID of the Node Manager.
///   \param nid The node ID of the Node Manager.
///   \param msgHandler Pointer to the message handler for the Node Manager. This
///                     is what processes messages sent to the Node Manager
///                     (messages not routed through it).
///   \param tcp If true, TCP communication is used for communication between
///              nodes.  If false, then UDP is used for communication between
///              nodes.  (UDP is still used for dynamic discovery of nodes).
///   \param netAddress The IP address to receive on.  If -1 any is used the first
///                     available is used.  The value can be an IP address, or
///                     the network device numer to use.
///   \param inboxSize Size of the shared memory buffer for components to use
///                    for routing messages through the node manager.  This
///                    size should be varied based on the number of components,
///                    and the amount/size of the message traffic you expect.
///                    By default this size is 2 MB which should be more than
///                    enough (event for video streaming).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Initialize(const Byte sid,
                                      const Byte nid,
                                      Jaus::MessageHandler* msgHandler,
                                      const bool tcp,
                                      const std::string& netAddress,
                                      const unsigned int inboxSize)
{
    int result = JAUS_FAILURE;

    Shutdown();

    if(sid != 0 && sid != 255 && nid != 0 && nid != 255 &&
        msgHandler &&
        inboxSize >= JAUS_MAX_PACKET_SIZE*2)
    {
        SetComponentID(Address(sid, nid, 1, 1));
#ifdef JAUS_DEBUG
        cout << "Initializing Shared Memory Input...";
#endif
        // Create an inbox in shared memory for the Node Manager
        if(mSharedMemoryInput.CreateInbox(GetID(), NULL, inboxSize) ||
            mSharedMemoryInput.OpenInbox(GetID()))
        {
            #ifdef JAUS_DEBUG
            cout << "Success!\n";
            #endif
            // Wait and see if another node manager is already using
            // this ID and shared memory.

            // Create a UDP server, and try to receive any heartbeat pulse
            // messages from other nodes running on the network.  If any are
            // found with the same ID as the one we want to use, then we must
            // exit with an error.
            mInitializingFlag = true;
            #ifdef JAUS_DEBUG
            cout << "Initializing UDP Server...";
            #endif
            if(mUdpInput.Initialize(this, mMulticastAddress) || CxUtils::Socket::GetNumInterfaces() == 0)
            {
                #ifdef JAUS_DEBUG
                cout << "Success!\n";
                #endif
                // Wait 100 ms for UDP server to start.
                CxUtils::SleepMs(100);
                unsigned int startTimeMs = Time::GetUtcTimeMs();

                // Wait up to 2.5 seconds to check for ID conflicts.
                // This should be long enough because any other node
                // out there that is broadcasting a heartbeat pulse
                // message is doing so at a rate of 1Hz.
                while(Time::GetUtcTimeMs() - startTimeMs <= 1750)
                {
                    if(mAddressConflictFlag == true)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "\n\n===============================================================\n";
                        cout << "ERROR: Node Manager With Same ID Running On Network!\n";
                        cout << "===============================================================\n";
                        #endif
                        SetJausError(ErrorCodes::AddressConflict);
                        break;
                    }

                    CxUtils::SleepMs(1);
                }

                if(mAddressConflictFlag == false)
                {
                    mTCPFlag = tcp;

                    if(mTCPFlag)
                    {
                        if(mTcpInput.Initialize(this) == JAUS_FAILURE && CxUtils::Socket::GetNumInterfaces() > 0)
                        {
                            #ifdef JAUS_DEBUG
                            cout << "\n\n===============================================================\n";
                            cout << "ERROR: Unable to initialize TCP server (is another server running?)!\n";
                            cout << "===============================================================\n";
                            #endif
                            SetJausError(ErrorCodes::ConnectionFailure);
                            return JAUS_FAILURE;
                        }
                    }
                    // Initialize remaining input streams and
                    // threads.
                    #ifdef JAUS_DEBUG
                    cout << "Initializing Component Registry...";
                    #endif
                    int initializedComponentRegistry = mComponentRegistry.OpenComponentRegistry(sid, nid);
                    if(initializedComponentRegistry)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }
                    #ifdef JAUS_DEBUG
                    cout << "Initializing Node Registry...";
                    #endif
                    int initializedNodeRegistry = mNodeRegistry.OpenNodeRegistry(sid, nid);
                    if(initializedNodeRegistry)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }
                    #ifdef JAUS_DEBUG
                    cout << "Initializing UDP Broadcast...";
                    #endif
                    int initializedBroadcast = mBroadcastUDP.Initialize("255.255.255.255");
                    if(initializedBroadcast)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }
                    #ifdef JAUS_DEBUG
                    cout << "Initializing UDP Multicast Client for Discovery...";
                    #endif
                    int initializedMulticast = mMulticastUDP.InitializeMulticast(mMulticastAddress, mMulticastTTL);
                    if(initializedMulticast)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }
#ifdef JAUS_DEBUG
                    cout << "Initializing Component Discovery...";
#endif
                    int initializedThread = mDiscoveryThread.CreateThread(NodeConnectionHandler::DiscoveryThread, this);
                    if(initializedThread)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }
#ifdef JAUS_DEBUG
                    cout << "Initializing Service Connection Manager...";
#endif
                    int initializedServiceManager = mpServiceManager->Initialize(sid, nid, this);
                    if(initializedServiceManager)
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Success!\n";
                        #endif
                    }
                    else
                    {
                        #ifdef JAUS_DEBUG
                        cout << "Failure!\n";
                        #endif
                    }

                    // Create a communicator.
#ifdef JAUS_DEBUG
                    cout << "Initialzing Communicator for Subsystem Communication...";
#endif
                    if(mCommunicator.Initialize("Communicator", Address(sid, nid, (Byte)(Service::Communicator), 1)))
                    {
#ifdef JAUS_DEBUG
                        cout << "Success!\n";
#endif
                        mCommunicator.DeleteDataLink(0);
                        Communicator::DefaultDataLink* defaultLink = new Communicator::DefaultDataLink();
                        defaultLink->SetMulticastAddress(mMulticastAddress, mMulticastTTL);
                        mCommunicator.SetNodeConnectionHandlerCallback(this);
                        mCommunicator.AddDataLink(defaultLink);
                        mCommunicator.SetSelectedDataLinkState(defaultLink->GetID(), Communicator::DataLink::On);
                    }
                    else
                    {
#ifdef JAUS_DEBUG
                        cout << "Failure.\n";
#endif
                    }

                    if( initializedComponentRegistry &&
                        initializedNodeRegistry &&
                        initializedBroadcast &&
                        initializedThread &&
                        initializedServiceManager &&
                        mCommunicator.IsInitialized())
                    {
                        mSharedMemoryInput.RegisterCallback(this);
                        mNodeRegistry.Register(mComponentID);
                        mDiscoveryThread.SetThreadName("NodeDiscovery");
                        // Signal we are no longer in the process of initializain
                        // to active threads.
                        mInitializingFlag = false;
                        // Set the message handler.
                        SetMessageHandler(msgHandler);
                        result = JAUS_OK;
#ifdef JAUS_DEBUG
                        cout << "Node Manager Initialized!\n";
#endif

                    } // If able to initialize everything.
                    else
                    {
                        SetJausError(ErrorCodes::ConnectionFailure);
                    }
                } // If Node ID is available.
            } // If initialized UDP input
        }

    } // If function inputs are valid
    else
    {
        SetJausError(ErrorCodes::InvalidValue);
    }

    // Make sure everything is closed on failure.
    if(result == JAUS_FAILURE)
    {
        Shutdown();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown all connections.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Shutdown()
{
    mCommunicator.Shutdown();
    //  Stop discovery, etc.  This should
    //  close faster after our incomming connections
    //  have been closed.
    mDiscoveryThread.StopThread();
    mpServiceManager->Shutdown();
    ConnectionHandler::ClearPendingReceipts();
    // Unregister the node manager.
    mNodeRegistry.UnRegister(mComponentID);
    // Shutdown any incomming data streams. This
    // will stop us from getting new messages
    // to process.
    mSharedMemoryInput.Close();
    mUdpInput.Shutdown();

    // Now close all outgoing streams.

    // Close component connections.
    ComponentsMap::iterator component;

    mComponentsMutex.Enter();
    for(component = mComponents.begin();
        component != mComponents.end();
        component++)
    {
        if(component->second)
        {
            component->second->Close();
            delete component->second;
        }
    }
    mComponents.clear();
    mComponentsMutex.Leave();

    // Close node connections.
    NodesMap::iterator node;

    mNodesMutex.Enter();
    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        if(node->second)
        {
            node->second->Shutdown();
            delete node->second;
        }
    }
    mNodes.clear();
    mNodesMutex.Leave();

    mTCPFlag = false;
    mAddressConflictFlag = false;
    mInitializingFlag = false;
    mpConnectionEventCallbackArgs = NULL;
    mConnectionEventCallback = NULL;

    mDiscoveryMutex.Enter();
    mDiscoveredComponents.clear();
    mDiscoveryMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown all connections, but doesn't close any active threads or
///          stop the connection handler. (Flushes all connections out basically).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Restart()
{
    // Clear input streams
    mSharedMemoryInput.Empty();
    mUdpInput.Shutdown();
    mUdpInput.Initialize(this);

    // Remove all active connections, so we
    // can rediscover them.

    // Components first
    ComponentsMap::iterator component;

    mComponentsMutex.Enter();
    for(component = mComponents.begin();
        component != mComponents.end();
        component++)
    {
        component->second->Close();
        delete component->second;
    }
    mComponents.clear();
    mComponentsMutex.Leave();

    // Close node connections, but only
    // those we discovered dynamically.  Those
    // node discovered dynamically are permanent
    // connections that can only be removed through
    // Shutodwn.
    NodesMap::iterator node;

    mNodesMutex.Enter();
    for(node = mNodes.begin();
        node != mNodes.end();)
    {
        if(node->second->mDiscoveryFlag == true)
        {
            node->second->Shutdown();
            delete node->second;
            mNodes.erase(node);
            node = mNodes.begin();
            /*
#ifdef WIN32
            node = mNodes.erase(node);
#else
            mNodes.erase(node);
#endif
            */
        }
        else
        {
            node++;
        }
    }
    mNodesMutex.Leave();

    mDiscoveryMutex.Enter();
    mDiscoveredComponents.clear();
    mDiscoveryMutex.Leave();

    mpServiceManager->Shutdown();
    mpServiceManager->Initialize(mComponentID.mSubsystem, mComponentID.mNode, this);

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the serialized JAUS message.
///
///   \param msg The JAUS message to send.
///
///   \return JAUS_OK if sent, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Send(const Stream& msg)
{
    int result;

    mNodesMutex.Enter();
    mComponentsMutex.Enter();

    result = SendStream(msg);

    mComponentsMutex.Leave();
    mNodesMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Serializes, then sends the message.
///
///   \param msg The JAUS message to send.
///
///   \return JAUS_OK if sent, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Send(const Message* msg)
{
    Stream stream;
    if(msg->Write(stream))
    {
        return Send(stream);
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a serialized message, and waits for a desired response message.
///
///   \param msg The serialized JAUS message to send.
///   \param receipt Receipt structure to store response information.
///   \param responseCode The message code of the response to the message sent.
///                       Set to 0 (default) to let the Send method automatically
///                       lookup the response code for you.
///   \param tms Total time to wait in milliseconds before timeout waiting for
///              response.
///   \param tries How many attempts to make.  The max allowed is
///                JAUS_MAX_SEND_COUNT.  Example:  If the value is 2, then at
///                most the your message will be sent 2 times within the
///                total timeout time (tms).
///
///   \return JAUS_OK if response message received successfully, otherwise
///           JAUS_FAILURE.  You may look at the Error History and the
///           status value of the receipt to identify reason for failure.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Send(const Stream& msg,
                                Receipt& receipt,
                                const UShort responseCode,
                                const UShort tms,
                                const UShort tries)
{
    int result;
    Header header;
    if(msg.Read(header, 0) == JAUS_FAILURE)
    {
        return JAUS_FAILURE;
    }
    if(header.mDestinationID.IsBroadcast())
    {
        mNodesMutex.Enter();
        mComponentsMutex.Enter();
        result = ConnectionHandler::Send(msg, receipt, responseCode, tms, tries);
        mComponentsMutex.Leave();
        mNodesMutex.Leave();
    }
    else
    {
        if(header.mDestinationID.mSubsystem != mComponentID.mSubsystem ||
            header.mDestinationID.mNode != mComponentID.mNode)
        {
            mNodesMutex.Enter();
            result = ConnectionHandler::Send(msg, receipt, responseCode, tms, tries);
            mNodesMutex.Leave();
        }
        else
        {
            mComponentsMutex.Enter();
            result = ConnectionHandler::Send(msg, receipt, responseCode, tms, tries);
            mComponentsMutex.Leave();
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a message, and waits for a desired response message.
///
///   \param msg The JAUS message to serialize and send.
///   \param receipt Receipt structure to store response information.
///   \param responseCode The message code of the response to the message sent.
///                       Set to 0 (default) to let the Send method automatically
///                       lookup the response code for you.
///   \param tms Total time to wait in milliseconds before timeout waiting for
///              response.
///   \param tries How many attempts to make.  The max allowed is
///                JAUS_MAX_SEND_COUNT.  Example:  If the value is 2, then at
///                most the your message will be sent 2 times within the
///                total timeout time (tms).
///
///   \return JAUS_OK if response message received successfully, otherwise
///           JAUS_FAILURE.  You may look at the Error History and the
///           status value of the receipt to identify reason for failure.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::Send(const Message* msg,
                                Receipt& receipt,
                                const UShort responseCode,
                                const UShort tms,
                                const UShort tries)
{
    Stream stream;
    if(msg->Write(stream))
    {
        return Send(stream, receipt, responseCode, tms, tries);
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a connection to the ID.
///
///   This method first attempts to connect to the ID through shared memory, if
///   this fails, then either UDP or TCP is used based on how the handler has
///   been initialized.  If the ID is for a component on the node, then only
///   shared memory connections are attempted.
///
///   \param id The ID of the component to connect to.
///   \param discoveryFlag If true, then this connection is subject to
///                        dynamic discovery conditions and will be removed if
///                        the connection is not updated regularly.  For example
///                        if a connection to a node, and the node isn't sending
///                        a hearbeat pulse message for more than 5 seconds, the
///                        connection is closed.  If discoveryFlag is set to false
///                        then the connection is never closed until Shutdown.
///   \param host The hostname/address to connect to (used only for UDP/TCP)
///               connections.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::CreateConnection(const Address& id,
                                            const bool discoveryFlag,
                                            const std::string* host)
{
    int result = JAUS_FAILURE;

    if(id == mComponentID) { return result; }

    //  If the destination we are creating a connection to is not
    //  on this node, then we are creating a connection to another node,
    //  so change the ID for the connection.
    if( id.mSubsystem != mComponentID.mSubsystem ||
        id.mNode != mComponentID.mNode )
    {
        NodesMap::iterator node;
        Address nodeID(id.mSubsystem, id.mNode, 1, 1);

        mNodesMutex.Enter();
        // If we don't already have a connection
        node = mNodes.find(nodeID);
        if(node == mNodes.end())
        {
            NodeConnection* connection = new NodeConnection();
            if(connection->CreateConnection(nodeID, this, host, mTCPFlag))
            {
                // Mark that whether or not we discovered this
                // connection dynamically, and mark the time
                // when data was last recieved (which is now).
                connection->mDiscoveryFlag = discoveryFlag;
                connection->mRecvTimeMs = Time::GetUtcTimeMs();
                mNodes[nodeID] = connection;
                result = JAUS_OK;
            }
            else
            {
                delete connection;
            }
        }
        else
        {
            result = JAUS_OK;
        }
        mNodesMutex.Leave();
    }
    // Create a component connection.
    else
    {
        ComponentsMap::iterator component;

        mComponentsMutex.Enter();
        component = mComponents.find(id);
        // If we don't have a connection already, make one
        if(component == mComponents.end())
        {
            JSharedMemory* connection = new JSharedMemory();
            if(connection->OpenInbox(id))
            {
                mComponents[id] = connection;
                result = JAUS_OK;
            }
            else
            {
                delete connection;
                connection = NULL;
            }
        }
        else
        {
            result = JAUS_OK;
        }
        mComponentsMutex.Leave();
    }

    // Signal a new connection has been made.
    if(result == JAUS_OK)
    {
        mDiscoveryMutex.Enter();
        mDiscoveredComponents.insert(id);
        mDiscoveryMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a connection to a node through a serial port (RS232, etc.)
///
///   Connections of this type made are only closed through the CloseConnection
///   or Shutdown methods.
///
///   \param id The node ID.
///   \param port The port name ("COM1", "dev/ttys0", etc.).
///   \param baud Bits per second rate.
///   \param bits Bits per byte.
///   \param parity The type of parity (CX_SERIAL_NO_PARITY,
///                                     CX_SERIAL_EVEN_PARITY,
///                                     CX_SERIAL_ODD_PARITY)
///   \param stop Number of stop bits.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::CreateSerialConnection(const Address& id,
                                                  const std::string& port,
                                                  const unsigned int baud,
                                                  const unsigned int bits,
                                                  const unsigned int parity,
                                                  const unsigned int stop)
{
    int result = JAUS_FAILURE;

    if(id == mComponentID) { return result; }

    //  If the destination we are creating a connection to is not
    //  on this node, then we are creating a connection to another node,
    //  so change the ID for the connection.
    if( id.mComponent == 1 && id.mInstance == 1)
    {
        NodesMap::iterator node;
        mNodesMutex.Enter();
        // If we don't already have a connection
        node = mNodes.find(id);
        if(node != mNodes.end())
        {
            NodeConnection* connection = new NodeConnection();
            if(connection->CreateConnection(id, this, port, baud, bits, parity, stop))
            {
                // Mark that whether or not we discovered this
                // connection dynamically, and mark the time
                // when data was last recieved (which is now).
                connection->mDiscoveryFlag = false;
                connection->mRecvTimeMs = Time::GetUtcTimeMs();
                mNodes[id] = connection;
                result = JAUS_OK;
            }
            else
            {
                delete connection;
            }
        }
        else
        {
            result = JAUS_OK;
        }
        mNodesMutex.Leave();
    }

    // Signal a new connection has been made.
    if(result == JAUS_OK)
    {
        mDiscoveryMutex.Enter();
        mDiscoveredComponents.insert(id);
        mDiscoveryMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Closes a connection.
///
///   \param id ID of connection to close.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::CloseConnection(const Address& id)
{
    int result = JAUS_FAILURE;
    if(id.mSubsystem == mComponentID.mSubsystem &&
        id.mNode == mComponentID.mNode)
    {
        ComponentsMap::iterator component;
        mComponentsMutex.Enter();
        component = mComponents.find(id);
        if(component != mComponents.end())
        {
            component->second->Close();
            delete component->second;
            mComponents.erase(component);
            result = JAUS_OK;
        }
        mComponentsMutex.Leave();
    }
    else
    {
        NodesMap::iterator node;
        mNodesMutex.Enter();
        node = mNodes.find(Address(id.mSubsystem, id.mNode, 1, 1));
        if(node != mNodes.end())
        {
            node->second->Shutdown();
            delete node->second;
            mNodes.erase(node);
            result = JAUS_OK;
        }
        mNodesMutex.Leave();
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the multicast address to use for sending/receiving messages.
///
///   This method will also update the multicast address and TTL for the
///   DefaultDataLink of the Communicator component.
///
///   \param multicast The multicast ID to use.  Value must be in the range of
///                    "224.0.0.0" - "239.255.255.255".
///   \param ttl   Time To Live for Multicast transmission.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::SetMulticastAddress(const std::string& multicast,
                                               const unsigned char ttl)
{
    int a, b, c, d;
    if(sscanf(multicast.c_str(), "%d.%d.%d.%d", &a, &b, &c, &d) > 0 && ttl > 0)
    {
        if(a >= 224 && a <= 239)
        {
            mMulticastAddress = multicast;
            mMulticastTTL = ttl;

            Communicator::DataLink::Map* dataLinks;
            Communicator::DataLink::Map::iterator dl;
            Communicator::DefaultDataLink* defaultDataLink;
            mCommunicator.LockDataLinks();
            dataLinks = mCommunicator.GetDataLinks();
            for(dl = dataLinks->begin();
                dl != dataLinks->end();
                dl++)
            {
                defaultDataLink = dynamic_cast<Communicator::DefaultDataLink*>(dl->second);
                if(defaultDataLink)
                {
                    defaultDataLink->SetMulticastAddress(multicast, ttl);
                }
            }
            mCommunicator.UnlockDataLinks();
            return JAUS_OK;
        }
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks if a connection is available.
///
///   \param id ID of connection to check for.
///
///   \return True if connection exists, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeConnectionHandler::HaveConnection(const Address& id) const
{
    bool result = false;
    if(id.mSubsystem == mComponentID.mSubsystem &&
        id.mNode == mComponentID.mNode)
    {
        ComponentsMap::const_iterator component;
        mComponentsMutex.Enter();
        component = mComponents.find(id);
        if(component != mComponents.end())
        {
            result = true;
        }
        mComponentsMutex.Leave();
    }
    else
    {
        NodesMap::const_iterator node;
        mNodesMutex.Enter();
        node = mNodes.find(Address(id.mSubsystem, id.mNode, 1, 1));
        if(node != mNodes.end())
        {
            result = true;
        }
        mNodesMutex.Leave();
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If you want the Node Manager to share subsystem configuration data
///   with other nodes/subsystems, then you can enable broadcasting to
///   255.255.1.1 of Report Heartbeat Pulse messages.  This will tell other
///   nodes that your node supports messages for Identification, Services, and
///   Configuration for the Subsystem and not just the Node.  By default
///   the Connection Handler for the node broadcasts to SubsystemNumber.255.1.1,
///   for only discovery of nodes.
///
///   \param on If true, heartbeat messages are sent to 255.255.1.1 instead of
///             the SubsystemNumber.255.1.1.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::ToggleSubsystemHeartbeatBroadcast(const bool on)
{
    mSubsystemDiscoverFlag = on;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief The Node Manager uses a hearbeat pulse message for dynamic discovery
///   of nodes.  This method allows the toggle between sending the hearbeat
///   message using UDP broadcast vs. UDP multicast.
///
///   \param on If true, heartbeat messages are sent using UDP broadcast. If false
///             then they are sent using UDP multicast.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::ToggleBroadcastUDP(const bool on)
{
    mBroadcastFlag = on;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the callback to use for notifications of new or lost connections.
///
///   \param cb The callback pointer.
///   \param args Additional arguments for callback.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::SetConnectionEventCallback(NodeConnectionCallback cb, void* args)
{
    mConnectionEventMutex.Enter();
    mConnectionEventCallback = cb;
    mpConnectionEventCallbackArgs = args;
    mConnectionEventMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Set of all node connections addresses.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set NodeConnectionHandler::GetNodeConnections() const
{
    Address::Set list;
    NodesMap::const_iterator node;
    mNodesMutex.Enter();
    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        list.insert(node->first);
    }
    mNodesMutex.Leave();

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Set of all component connections addresses.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set NodeConnectionHandler::GetComponentConnections() const
{
    Address::Set list;
    ComponentsMap::const_iterator component;
    mComponentsMutex.Enter();
    for(component = mComponents.begin();
        component != mComponents.end();
        component++)
    {
        list.insert(component->first);
    }
    mComponentsMutex.Leave();

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sends the message data to the appropriate destination.  <b>Warning: this
///         method is NOT thread safe.</b>
///
///  This method at times needs to call itself recursively, or calls methods
///  from the parent class (ConnectionHandler) which will in tern call the
///  SendStream method.  Therefore it is difficult to use a Mutex to protect
///  the connections used by this method.  Therefore this method is designed
///  without the use of Mutexes, and is <b>NOT</b> thread safe.
///
///  If you need a thread safe method to send a message use any of the 4
///  overloaded Send functions.  Those methods enable the appropriate
///  mutexes and then call the SendStream method directly or the the
///  parent classes Send methods.
///
///  \param msg The serialized JAUS message to route to a destination.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnectionHandler::SendStream(const Stream& msg)
{
    int result = JAUS_FAILURE;
    Header header;

    LargeDataSet *mpstream = NULL;  // Multi-packet stream data.

    if(msg.Read(header, 0) == JAUS_FAILURE)
    {
        SetJausError(ErrorCodes::BadPacket);
        return JAUS_FAILURE;
    }

    // Check to see if this message needs to be split
    // into a large data set.
    if(msg.Length() > JAUS_MAX_PACKET_SIZE)
    {
        // If going to a component on this node, then copy
        // right to shared memory.
        if(header.mDestinationID.mSubsystem == mComponentID.mSubsystem &&
            header.mDestinationID.mNode == mComponentID.mNode)
        {
            ComponentsMap::iterator component;
            component = mComponents.find(header.mDestinationID);
            if(component != mComponents.end())
            {
                result = component->second->EnqueueMessage(msg) > 0 ? JAUS_OK : JAUS_FAILURE;
            }
            // Maybe we haven't discovered the
            // component on this node yet, if this is
            // the case, then try create a connection on the fly.
            else
            {
                JSharedMemory *connection = new JSharedMemory();
                if(connection->OpenInbox(header.mDestinationID))
                {
                    // Signal that a new connection has been made.
                    mDiscoveryMutex.Enter();
                    mDiscoveredComponents.insert(header.mDestinationID);
                    mDiscoveryMutex.Leave();

                    mComponents[header.mDestinationID] = connection;
                    connection = NULL;
                    component = mComponents.find(header.mDestinationID);
                    if(component != mComponents.end())
                    {
                        result = component->second->EnqueueMessage(msg) > 0 ? JAUS_OK : JAUS_FAILURE;
                    }
                }
                if(connection) { delete connection; }
            }
        }
        if(result == JAUS_FAILURE)
        {
            LargeDataSet dataSet;
            const Stream::List* sequence;
            Stream::List::const_iterator packet;
            bool failure = false;
            Receipt receipt;
            dataSet.CreateLargeDataSet(msg, &header);

            sequence = dataSet.GetDataSet();
            for(packet = sequence->begin();
                packet != sequence->end() && !failure;
                packet++)
            {
                // If this message request ACK/NACK, then
                // get a receipt.  However, use the ConnectionHandler
                // version of Send because it doesn't use the same mutex
                // the NodeConectionHandler::Send method uses.
                if(header.mAckNack == Header::AckNack::Request && !header.mDestinationID.IsBroadcast())
                {
                    if(ConnectionHandler::Send(*packet,
                                               receipt,
                                               header.mCommandCode) == JAUS_FAILURE)
                    {
                        failure = true;
                    }
                }
                else
                {
                    if(SendStream(*packet) == JAUS_FAILURE)
                    {
                        // If we are sending to a component
                        // on this node, we may be sending it the
                        // sequence to quickly, so wait a moment, then
                        // try one more time.
                        if(header.mDestinationID.mSubsystem == mComponentID.mSubsystem &&
                           header.mDestinationID.mNode == mComponentID.mNode)
                        {
                            CxUtils::SleepMs(50);
                            if(SendStream(*packet) == JAUS_FAILURE)
                            {
                                failure = true;
                            }
                        }
                        else
                        {
                            failure = true;
                        }
                    }
                }
            }

            if(!failure)
            {
                result = JAUS_OK;
            }
        }
    }
    // Perform normal send/routing operation for message.
    else
    {
        // If broadcast, extra work must be done.
        if(header.mDestinationID.IsBroadcast())
        {
            // Only broadcast messages that originate from this node to other nodes/subsystem.  Failure
            // to check this will result in possible infinite loop/echoing back of
            // broadcast messages.
            if(header.mSourceID.mSubsystem != mComponentID.mSubsystem && header.mSourceID.mNode != mComponentID.mNode)
            {
                if(header.mDestinationID.mSubsystem == 255)
                {
                    mCommunicator.Transmit(msg);
                }

                // Send to nodes on this subsystem.
                NodesMap::iterator node;
                for(node = mNodes.begin();
                    node != mNodes.end();
                    node++)
                {
                    if( (header.mDestinationID.mSubsystem == mComponentID.mSubsystem || header.mDestinationID.mSubsystem == 255) &&
                        (header.mDestinationID.mNode == 255 || header.mDestinationID.mNode != node->first.mNode) )
                    {
                        node->second->SendStream(msg);
                    }
                }
            }

            // Send to components on this node if necessary
            ComponentsMap::iterator component;
            for(component = mComponents.begin();
                component != mComponents.end();
                component++)
            {
                // If destination matches, and the component is not
                // the one who send the message, send to component.
                if(Address::DestinationMatch(header.mDestinationID, component->first) &&
                    component->first != header.mSourceID)
                {
                    result = component->second->EnqueueMessage(msg);
                } // If the component is a matching destination.
            } // Check each component connection on this node to see if we must send to it.
        }
        // Non-broadcast message, and not destined for
        // the node manager specifically
        else if(header.mDestinationID != mComponentID)
        {
            // If the destination is not on this node, then check to
            // see if we have a direct connection to it, or use the
            // Communicator if needed.
            if(header.mDestinationID.mSubsystem != mComponentID.mSubsystem ||
                header.mDestinationID.mNode != mComponentID.mNode)
            {
                // First see if we have a direct connection
                // to the node, even if it is on another subsystem
                // and send directly to it using unicast.
                NodesMap::iterator node;
                node = mNodes.find(Address(header.mDestinationID.mSubsystem,
                                           header.mDestinationID.mNode,
                                           1,
                                           1));
                if(node != mNodes.end()) // Have direct connection.
                {
                    result = node->second->SendStream(msg);
                }
                else
                {
                    // If on a different subsystem use the communicator,
                    // otherwise send using multicast or broadcast if we don't have
                    // a direct connection to the node.
                    if(header.mDestinationID.mSubsystem != mComponentID.mSubsystem)
                    {
                        if(mCommunicator.Transmit(msg))
                        {
                            result = JAUS_OK;
                        }
                    }
                }
            }
            // Send to component on this node.
            else
            {
                ComponentsMap::iterator component;
                component = mComponents.find(header.mDestinationID);
                if(component != mComponents.end())
                {
                    result = component->second->EnqueueMessage(msg) > 0 ? JAUS_OK : JAUS_FAILURE;
                }
                // Maybe we haven't discovered the
                // component on this node yet, if this is
                // the case, then try create a connection on the fly.
                else
                {
                    JSharedMemory *connection = new JSharedMemory();
                    if(connection->OpenInbox(header.mDestinationID))
                    {
                        // Signal that a new connection has been made.
                        mDiscoveryMutex.Enter();
                        mDiscoveredComponents.insert(header.mDestinationID);
                        mDiscoveryMutex.Leave();

                        mComponents[header.mDestinationID] = connection;
                        connection = NULL;
                        component = mComponents.find(header.mDestinationID);
                        if(component != mComponents.end())
                        {
                            result = component->second->EnqueueMessage(msg) > 0 ? JAUS_OK : JAUS_FAILURE;
                        }
                    }
                    if(connection) { delete connection; }
                }
            }
        }
    }

    // Send NACK if needed.  This will tell the
    // sending component that the destination is not on
    // this node.
    if( result == JAUS_FAILURE &&
        header.mSourceID.mSubsystem == mComponentID.mSubsystem &&
        header.mSourceID.mNode == mComponentID.mNode &&
        header.mAckNack == Header::AckNack::Request &&
        header.mDestinationID.IsBroadcast() == false )
    {
        Header nack;
        Stream nackPacket;
        nack = header;
        nack.mDataSize = 0;
        nack.mDataFlag = Header::DataControl::Single;
        nack.mAckNack = Header::AckNack::Nack;
        nack.SwapSourceAndDestination();
        nackPacket.Write(nack);
        SendStream(nackPacket);
    }

    if(mLogger.IsLogOpen())
    {
        if(result == JAUS_OK)
        {
            // Update Log File
            mLogger.WriteSentDataToLog(msg, header, true);
        }
        else
        {
            // Update Log File
            if(header.mDestinationID == mComponentID ||
                Address::DestinationMatch(header.mDestinationID, mComponentID))
            {
                mLogger.WriteSentDataToLog(msg, header, true);
            }
            else
            {
                mLogger.WriteSentDataToLog(msg, header, false);
            }
        }
    }

    // Delete allocated memory if needed.
    if(mpstream)
    {
        delete mpstream;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Main entry point for all messages receives by the NodeConnectionHandler
///   over all transport layers.
///
///   This method receives all incomming data, and then routes it to the
///   appropriate destination (makes new connections as needed) or passes to
///   service connection managemer if needed.
///
///   \param msg The message to process (header and message body).
///   \param info Header information for message.
///   \param transport The transport layer the data came from.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::ProcessStreamCallback(const Stream &msg,
                                                  const Header *info,
                                                  const StreamCallback::Transport transport)
{
    Header header;
    // Read header information if not provided.
    if( info )
    {
        header = *info;
    }
    else
    {
        if( !msg.Read(header, 0) )
        {
            SetJausError(ErrorCodes::InvalidHeader);
            if(mLogger.IsLogOpen())
            {
                mLogger.WriteReceivedDataDumped(msg, header, transport, NodeLogger::InvalidHeader);
            }
            return;
        }
    }

    //  If we receive a message with the same ID as
    //  the node, and it is within our initialization wait
    //  window, then signal to the Initialize function that there
    //  is a conflict.
    if(mInitializingFlag)
    {
        if(header.mSourceID == mComponentID)
        {
            mAddressConflictFlag = true;
        }
        return;
    }

    //  If a message was received over UDP or the Communicator, and it was
    //  sent by this node, ignore it.  If we do not do this
    //  then we may keep re-processing our own messages over
    //  and over.  This is a result of using Multicast/Broadcast
    //  capabilities of the UDP Transport Layer typically.
    if( (transport == StreamCallback::UDP || transport == StreamCallback::Communicator) &&
        header.mSourceID.mSubsystem == mComponentID.mSubsystem &&
        header.mSourceID.mNode == mComponentID.mNode )
    {
        if(mLogger.IsLogOpen())
        {
            mLogger.WriteReceivedDataDumped(msg, header, transport, NodeLogger::ReceivedByLoopback);
        }
        return;
    }

    //if(header.mCommandCode == JAUS_CREATE_EVENT && header.mSourceID.mSubsystem != mComponentID.mSubsystem)
    //{
    //    int x;
    //    x = 3;
    //}

    // If message is for this node, then give it to the
    // message handler.
    if(header.mDestinationID == mComponentID ||
        Address::DestinationMatch(header.mDestinationID, mComponentID))
    {
        // If the message is part of a multi-packet stream sequence
        // then we must collect the pieces for merging.
        if(header.mDataFlag != Header::DataControl::Single)
        {
            Stream* merged = NULL;
            // Process multi-sequence data
            if(ConnectionHandler::ProcessMultiPacketStream(msg, header, &merged))
            {
                // If we finally recieved the last message in the
                // stream sequcne (got merged stream) pass it to
                // the receipt handler or message handler.
                if(merged)
                {
                    merged->Read(header, 0);
                    if(CheckForReceipt(*mpMergedStream, header) == JAUS_FAILURE && mpMessageHandler)
                    {
                        mpMessageHandler->AddToMessageQueue(*merged, &header);
                    }
                }
            }
            if(merged) { delete merged; }
        }
        // If it is just a normal message, then pass to receipt
        // or message handler.
        else if( CheckForReceipt(msg, header) == JAUS_FAILURE && mpMessageHandler)
        {
            mpMessageHandler->AddToMessageQueue(msg, &header);
        }

        // If the message is not broadcast, then we are done,
        // otherwise we may need to send it to others.
        if(header.mDestinationID.IsBroadcast() == false)
        {
            // Update logger.
            mLogger.WriteReceivedDataToLog(msg, header, transport);
            return;
        }
    }

    // Update logger.
    mLogger.WriteReceivedDataToLog(msg, header, transport);

    // If SC message, we must do additional processing.
    if( header.mServiceConnectionFlag == JAUS_SERVICE_CONNECTION ||
        header.mCommandCode == JAUS_CREATE_SERVICE_CONNECTION ||
        header.mCommandCode == JAUS_CONFIRM_SERVICE_CONNECTION ||
        header.mCommandCode == JAUS_SUSPEND_SERVICE_CONNECTION ||
        header.mCommandCode == JAUS_TERMINATE_SERVICE_CONNECTION ||
        header.mCommandCode == JAUS_ACTIVATE_SERVICE_CONNECTION)
    {
        if(mpServiceManager)
        {
            mpServiceManager->RouteServiceConnectionMessage(msg, header);
        }
        else
        {
            Send(msg);
        }
    }
    else
    {
        // Send the message onto its destination!
        Send(msg);
    }

    // If this is a Heartbeat message from another node or subsystem
    // see if we have a connection or not.
    if( header.mCommandCode == JAUS_REPORT_HEARTBEAT_PULSE &&
        (transport == StreamCallback::UDP || transport == StreamCallback::Communicator) &&
        (header.mSourceID.mSubsystem != mComponentID.mSubsystem || header.mSourceID.mNode != mComponentID.mNode))
    {
        // Create a connection to the node manager if we do
        // not already have one.
        Address nid( header.mSourceID.mSubsystem,
                      header.mSourceID.mNode,
                      1,
                      1 );
        if( !HaveConnection( nid ) )
        {
            //  See if this is a network connection.
            std::string host;
            if( mUdpInput.GetHostname(header.mSourceID, host) )
            {
                CreateConnection( nid, true, &host );
            }
            else
            {
                //  Try create a connection using shared memory.
                CreateConnection( nid, true, NULL );
            }
        }
        mNodesMutex.Enter();
        NodesMap::iterator node = mNodes.find(nid);
        if(node != mNodes.end())
        {
            // Update the time for latest message received from source.
            node->second->mRecvTimeMs = Time::GetUtcTimeMs();
        }
        mNodesMutex.Leave();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function performed in a thread with the job of looking for
///   new connections, and removing invalid ones.  It also generates a heartbeat
///   message which is sent using UDP broadcast so that other nodes can discover
///   this node manager.
///
///   \param arg Pointer to NodeConnectionHandler.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::DiscoveryThread(void *arg)
{
    unsigned int heartbeatTime = 0;
    unsigned int componentCheckTime = 0;
    unsigned int nodeCheckTime = 0;
    bool haveCallback = false;

    Address::Set::iterator id;
    Address::Set newNodeConnections, newComponentConnections;
    Address::Set lostNodeConnections, lostComponentConnections;
    Address::Set newConnections;
    ReportHeartbeatPulse heartBeatMessage;
    Stream heartBeatStream;

    // Wait for node manager to get ready.
    CxUtils::SleepMs(500);

    NodeConnectionHandler *handler = (NodeConnectionHandler*)(arg);

    heartBeatMessage.SetSourceID(handler->mComponentID);
    heartBeatMessage.SetDestinationID(Address(handler->mComponentID.mSubsystem, 255, 1, 1));

    while( handler &&
           !handler->mDiscoveryThread.QuitThreadFlag() )
    {
        //  Check to see if we need to send out a heartbeat pulse message.
        if( Time::GetUtcTimeMs() - heartbeatTime >= 1000 )
        {
            if(handler->mSubsystemDiscoverFlag)
            {
                heartBeatMessage.SetDestinationID(Address(255, 255, 1, 1));
            }
            else
            {
                heartBeatMessage.SetDestinationID(Address(handler->mComponentID.mSubsystem, 255, 1, 1));
            }
            heartBeatMessage.SetSequenceNumber(heartBeatMessage.GetSequenceNumber() + 1);
            heartBeatMessage.Write(heartBeatStream);

            if(handler->mSubsystemDiscoverFlag) // If going to other subsystems, give to communicator.
            {
                handler->mCommunicator.Transmit(heartBeatStream);
            }

            if(handler ->mSubsystemDiscoverFlag == false ||
                (handler->mSubsystemDiscoverFlag == true && handler->mCommunicator.IsDefaultDataLinkSelected(handler->mMulticastAddress) == false))
            {
                if(handler->mBroadcastFlag)
                {
                    handler->mBroadcastUDP.Send(heartBeatStream);
                }
                else
                {
                    handler->mMulticastUDP.Send(heartBeatStream);
                }
            }
            heartbeatTime = Time::GetUtcTimeMs();
        }

        // Check for changes in component connections.
        if( Time::GetUtcTimeMs() - componentCheckTime > 100 )
        {
            handler->CheckComponents(newComponentConnections,
                                     lostComponentConnections);
            componentCheckTime = Time::GetUtcTimeMs();
        }

        // Check for changes in node connections.
        if( Time::GetUtcTimeMs() - nodeCheckTime > 100 )
        {
            handler->CheckNodes(newNodeConnections,
                                lostNodeConnections);
            nodeCheckTime = Time::GetUtcTimeMs();
        }

        // Get a copy of any new components that may have
        // been discovered.
        handler->mDiscoveryMutex.Enter();
        newConnections = handler->mDiscoveredComponents;
        handler->mDiscoveredComponents.clear();
        handler->mDiscoveryMutex.Leave();

        // Go through all newly discovered or lost
        // connections, and generated a connection event callback.
        handler->mConnectionEventMutex.Enter();
        for(id = newComponentConnections.begin();
            id != newComponentConnections.end();
            id++)
        {
            if(handler->mConnectionEventCallback)
            {
                handler->mConnectionEventCallback(ComponentConnect, *id, handler->mpConnectionEventCallbackArgs);
            }
            // Update Log
            handler->mLogger.WriteToLog(*id, ComponentConnect);
        }

        for(id = lostComponentConnections.begin();
            id != lostComponentConnections.end();
            id++)
        {
            if(handler->mConnectionEventCallback)
            {
                handler->mConnectionEventCallback(ComponentDisconnect, *id, handler->mpConnectionEventCallbackArgs);
            }
            // Notify SC management of disconnect.
            if(handler->mpServiceManager)
            {
                handler->mpServiceManager->ProcessConnectionEvent(*id, NodeConnectionHandler::ComponentDisconnect);
            }
            // Update Log
            handler->mLogger.WriteToLog(*id, ComponentDisconnect);
        }

        for(id = newNodeConnections.begin();
            id != newNodeConnections.end();
            id++)
        {
            if(handler->mConnectionEventCallback)
            {
                handler->mConnectionEventCallback(NodeConnect, *id, handler->mpConnectionEventCallbackArgs);
            }
            // Update Log
            handler->mLogger.WriteToLog(*id, NodeConnect);
        }

        for(id = lostNodeConnections.begin();
            id != lostNodeConnections.end();
            id++)
        {
            if(handler->mConnectionEventCallback)
            {
                handler->mConnectionEventCallback(NodeDisconnect, *id, handler->mpConnectionEventCallbackArgs);
            }
            // Notify SC management of disconnect.
            if(handler->mpServiceManager)
            {
                handler->mpServiceManager->ProcessConnectionEvent(*id, NodeConnectionHandler::NodeDisconnect);
            }
            // Update Log
            handler->mLogger.WriteToLog(*id, NodeDisconnect);
        }

        for(id = newConnections.begin();
            id != newConnections.end();
            id++)
        {
            if(handler->mConnectionEventCallback)
            {
                if(id->mComponent == 1 && id->mInstance == 1)
                {
                    handler->mConnectionEventCallback(NodeConnect, *id,handler->mpConnectionEventCallbackArgs);
                    handler->mLogger.WriteToLog(*id, NodeConnect);
                }
                else
                {
                    handler->mConnectionEventCallback(ComponentConnect, *id, handler->mpConnectionEventCallbackArgs);
                    handler->mLogger.WriteToLog(*id, ComponentConnect);
                }
            }
        }

        // If this is the first time we detected that
        // a callback exists, then notifiy the callback
        // of all current connections.
        if(haveCallback == false &&
            handler->mConnectionEventCallback)
        {
            Address::Set connections = handler->GetNodeConnections();
            for(id = connections.begin();
                id != connections.end();
                id++)
            {
                handler->mConnectionEventCallback(NodeConnect, *id,handler->mpConnectionEventCallbackArgs);
            }
            connections = handler->GetComponentConnections();
            for(id = connections.begin();
                id != connections.end();
                id++)
            {
                handler->mConnectionEventCallback(ComponentConnect, *id,handler->mpConnectionEventCallbackArgs);
            }
            haveCallback = true;
        }
        // Reset flag if callback is cleared.
        if(handler->mConnectionEventCallback == NULL)
        {
            haveCallback = false;
        }

        handler->mConnectionEventMutex.Leave();

        newComponentConnections.clear();
        lostComponentConnections.clear();
        newNodeConnections.clear();
        lostNodeConnections.clear();
        newConnections.clear();



        CxUtils::SleepMs(1);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks for new component connections, and removes lost ones.
///
///   \param newConnections New connections made.
///   \param lostConnections Connections lost.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::CheckComponents(Address::Set& newConnections, Address::Set& lostConnections)
{
    Address::List registry;
    Address::List::iterator id;
    Address::Set::iterator lostID;
    ComponentsMap::iterator component;

    newConnections.clear(); // Clear new connections.
    lostConnections.clear(); // Clear lost connections.

    // Look for new connections to make.
    if( mComponentRegistry.GetRegistry(registry) )
    {
        for(id = registry.begin();
            id != registry.end();
            id++)
        {
            mComponentsMutex.Enter();
            component = mComponents.find(*id);
            if(component == mComponents.end())
            {
                JSharedMemory* connection = new JSharedMemory();
                if(connection->OpenInbox(*id) && connection->IsActive())
                {
                    newConnections.insert(*id);
                    mComponents[*id] = connection;
                }
                else
                {
                    delete connection;
                    // Remove from registry if we cannot connect.
                    mComponentRegistry.UnRegister(*id);
                }
            }
            mComponentsMutex.Leave();
        }
    }

    // Now remove connections to nodes that have
    // not updated in a while.
    mComponentsMutex.Enter();
    for(component = mComponents.begin();
        component != mComponents.end();
        component++)
    {
        if(component->second->IsActive() == false)
        {
            lostConnections.insert(component->first);
        }
    }
    mComponentsMutex.Leave();

    // Close connections that are lost.
    for(lostID = lostConnections.begin();
        lostID != lostConnections.end();
        lostID++)
    {
        CloseConnection(*lostID);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks for new node connections, and removes lost ones.
///
///   \param newConnections New connections made.
///   \param lostConnections Connections lost.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnectionHandler::CheckNodes(Address::Set& newConnections, Address::Set& lostConnections)
{
    Address::List registry;
    Address::List::iterator id;
    Address::Set::iterator lostID;
    NodesMap::iterator node;

    newConnections.clear(); // Clear new connections.
    lostConnections.clear(); // Clear lost connections.

    // Look for new connections to make.
    if( mNodeRegistry.GetRegistry(registry) )
    {
        for(id = registry.begin();
            id != registry.end();
            id++)
        {
            if(*id == mComponentID) { continue; }

            mNodesMutex.Enter();
            node = mNodes.find(*id);
            if(node == mNodes.end())
            {
                NodeConnection* connection = new NodeConnection();
                if(connection->CreateConnection(*id, this, NULL))
                {
                    newConnections.insert(*id);
                    mNodes[*id] = connection;
                    connection->mDiscoveryFlag = true;
                    connection->mRecvTimeMs = Time::GetUtcTimeMs();
                }
                else
                {
                    delete connection;
                    // Remove from registry if we cannot connect.
                    mNodeRegistry.UnRegister(*id);
                }
            }
            mNodesMutex.Leave();
        }
    }

    // Now remove connections to nodes that have
    // not updated in a while.
    mNodesMutex.Enter();
    unsigned int timeStampUtcMs = Time::GetUtcTimeMs();
    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        unsigned int recvTimeMs;
        recvTimeMs = node->second->mRecvTimeMs;
        // If we discovered this node dynamically, and
        // we have not received any data from it in a while
        // then remove the connection.  But only do this if
        // not found through shared memory because we check those
        // connections differently.
        if(node->second->mDiscoveryFlag &&
            node->second->mConnectionType != StreamCallback::SharedMemory &&
            timeStampUtcMs > node->second->mRecvTimeMs &&
            timeStampUtcMs - node->second->mRecvTimeMs > 5000)
        {
            lostConnections.insert(node->first);
        }
        // If the connection is through shared memory, and is
        // not updating, remove it also.
        if(node->second->mDiscoveryFlag &&
            node->second->mConnectionType == StreamCallback::SharedMemory &&
            node->second->mpSharedMemory->IsActive() == false)
        {
            lostConnections.insert(node->first);
        }
    }
    mNodesMutex.Leave();

    // Close connections that are lost.
    for(lostID = lostConnections.begin();
        lostID != lostConnections.end();
        lostID++)
    {
        CloseConnection(*lostID);
    }
}

/*  End of File */
