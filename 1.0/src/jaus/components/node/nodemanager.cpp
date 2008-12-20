////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodemanager.cpp
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
#include "jaus/components/node/nodemanager.h"
#include "jaus/components/node/nodeconnectionhandler.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/query/querymessages.h"
#include "jaus/messages/inform/informmessages.h"
#include "jaus/messages/experimental/experimentalmessages.h"
#include "jaus/components/transport/net.h"
#include <cxutils/fileio.h>
#include <iostream>
#include <tinyxml.h>

using namespace Jaus;

#define JAUS_SUBSYSTEM_LIST_CHANGE_EVENT    1
#define JAUS_SUBSYSTEM_CONFIG_CHANGE_EVENT  2
#define JAUS_NODE_CONFIG_CHANGE_EVENT       3


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.  Sets default values.
///
////////////////////////////////////////////////////////////////////////////////////
NodeManager::NodeManager() : mpConnectionHandler(NULL),
                             mpMessageHandler(NULL),
                             mSubsystemDiscoveryFlag(true),
                             mConnectionEventTimeMs(0)

{
    mpConnectionHandler = new NodeConnectionHandler(this);
    mpConnectionHandler->SetConnectionEventCallback(NodeManager::ProcessConnectionEvent, this);
    mpMessageHandler = new MessageHandler();
    mpMessageHandler->RegisterCallback(NodeManager::MessageCallbackFunction, this);

    mServices.LoadCoreMessageSupport();
    // Add supported input messages
    mServices.AddInputMessage(JAUS_EVENT, 0);
    mServices.AddInputMessage(JAUS_CREATE_EVENT, CreateEventRequest::VectorMask::QueryMessageBody);
    mServices.AddInputMessage(JAUS_CANCEL_EVENT, CancelEvent::VectorMask::EventID |
                                                 CancelEvent::VectorMask::MessageCode);
    mServices.AddInputMessage(JAUS_QUERY_SERVICES, 0);
    mServices.AddInputMessage(JAUS_QUERY_SUBSYSTEM_LIST, 0);
    mServices.AddInputMessage(JAUS_QUERY_CONFIGURATION, 0);
    mServices.AddInputMessage(JAUS_REPORT_CONFIGURATION, 0);
    mServices.AddInputMessage(JAUS_QUERY_HEARTBEAT_PULSE, 0);
    mServices.AddInputMessage(JAUS_REPORT_HEARTBEAT_PULSE, 0);
    mServices.AddInputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    mServices.AddInputMessage(JAUS_REPORT_IDENTIFICATION, 0);
    mServices.AddInputMessage(JAUS_QUERY_EVENTS, QueryEvents::VectorMask::MessageCode |
                                                 QueryEvents::VectorMask::EventType |
                                                 QueryEvents::VectorMask::EventID);

    // Add supported output messages
    mServices.AddOutputMessage(JAUS_CREATE_EVENT, CreateEventRequest::VectorMask::QueryMessageBody);
    mServices.AddOutputMessage(JAUS_CANCEL_EVENT, CancelEvent::VectorMask::EventID |
                                                  CancelEvent::VectorMask::MessageCode);
    mServices.AddOutputMessage(JAUS_REPORT_SUBSYSTEM_LIST, 0);
    mServices.AddOutputMessage(JAUS_REPORT_SERVICES, 0);
    mServices.AddOutputMessage(JAUS_REPORT_CONFIGURATION, 0);
    mServices.AddOutputMessage(JAUS_QUERY_HEARTBEAT_PULSE, 0);
    mServices.AddOutputMessage(JAUS_REPORT_HEARTBEAT_PULSE, 0);
    mServices.AddOutputMessage(JAUS_EVENT, 0);
    mServices.AddOutputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0);
    mServices.AddOutputMessage(JAUS_REJECT_EVENT_REQUEST, 0);
    mServices.AddOutputMessage(JAUS_REPORT_EVENTS, 0);
    mServices.AddOutputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    mServices.AddOutputMessage(JAUS_REPORT_IDENTIFICATION, 0);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeManager::~NodeManager()
{
    Shutdown();
    MessageCreator::CleanupMessageCreator();
    if(mpConnectionHandler) { delete mpConnectionHandler; }
    if(mpMessageHandler) { delete mpMessageHandler; }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initialize the message handler.
///
///   \param sid Subsystem ID.
///   \param nid Node ID.
///   \param tcp If true TCP services are used over UDP services.  If false, then
///              UDP is used for inter-node connections (default).
///   \param smSize How large of an internal buffer should be used for storing
///                 messages from components on node for routing.  This number can
///                 be made larger if a lot of components with large volumes of
///                 data transfer will be running on the node.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::Initialize(const Byte sid,
                            const Byte nid,
                            const bool tcp,
                            const UInt smSize)
{
    int result = JAUS_FAILURE;

    Shutdown();

    if(mpConnectionHandler->Initialize(sid, nid, mpMessageHandler, tcp, smSize))
    {
        //std::cout << "Initialized Connection Handler.\n";
        mNodeID(sid, nid, 1, 1);
        mEvents.SetComponentID(mNodeID);

        mpConnectionHandler->SetConnectionEventCallback(NodeManager::ProcessConnectionEvent, this);
        mpMessageHandler->RegisterCallback(NodeManager::MessageCallbackFunction, this);
        mpConnectionHandler->CreateConnection(GetCommunicator()->GetID(), true, NULL);
        mNodeMutex.Enter();
        mSystemConfiguration.AddComponent(mNodeID);
        mSystemConfiguration.AddComponent(GetCommunicator()->GetID());
        mSubsystemConfigFromXML.AddComponent(mNodeID);
        mSubsystemConfigFromXML.AddComponent(GetCommunicator()->GetID());
        mConnectionEventTimeMs = Time::GetUtcTimeMs();
        mNodeMutex.Leave();


        mInitializedFlag = true;
        result = JAUS_OK;
    }
    else
    {
        SetJausError(mpConnectionHandler->GetLastJausError());
    }
    if(result == JAUS_FAILURE)
    {
        Shutdown();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Loads a settings file containing information about the node
///   manager, and other nodes on the network.
///
///   Since settings files change internal parameters to the Node Manager,
///   it will also be shutdown if already initialized.
///
///   \param settingsFileXML The settings file for the manager to load.
///
///   \return JAUS_OK if complete, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::Initialize(const std::string& settingsFileXML)
{
    int result = JAUS_FAILURE;

    //  Close any current operations.
    Shutdown();
    
    mSystemConfiguration.Clear();

    std::string tempString;
    TiXmlDocument xml;
#ifdef JAUS_DEBUG
    std::cout << "Loading node settings file " << settingsFileXML << std::endl;
#endif
    if(!xml.LoadFile(settingsFileXML.c_str()))
    {
        std::cout << "NodeManager: Unable to parse XML file\n";
        SetJausError(ErrorCodes::InvalidValue);
        return result;
    }
    if(xml.FirstChildElement("Jaus") == NULL)
    {
        std::cout << "NodeManager: XML file does not contain JAUS data.\n";
        SetJausError(ErrorCodes::InvalidValue);
        return result;
    }

    std::string temp;
    Address id;
    unsigned int mbsize = JAUS_NODE_SM_DEFAULT_SIZE;

    TiXmlHandle docHandle(&xml);
    int s, n;
    TiXmlNode *node;
    TiXmlElement* nodeElement;
    
    // Get configuration data
    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("SubsystemID").ToNode();
    nodeElement = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("SubsystemConfiguration").FirstChild("Node").ToElement();
    while(node && nodeElement)
    {
        Byte subsystemID = (Byte)atoi(node->FirstChild()->Value());
        Byte nodeID = (Byte)atoi(docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NodeID").ToNode()->FirstChild()->Value());
        
        // Add ourselves to the configuration.
        mSystemConfiguration.AddComponent(Address(subsystemID, nodeID, 1, 1));
        
        TiXmlElement* componentElement = NULL;
        if(nodeElement->Attribute("id"))
        {
            Byte nodeID = (Byte)atoi(nodeElement->Attribute("id"));
            mSystemConfiguration.AddComponent(Address(subsystemID, nodeID, 1, 1));
            componentElement = nodeElement->FirstChildElement("Component");
            while(componentElement)
            {
                if(componentElement->Attribute("id") && componentElement->Attribute("instance"))
                {
                    mSystemConfiguration.AddComponent(Address(subsystemID,
                                                              nodeID,
                                                              (Byte)atoi(componentElement->Attribute("id")),
                                                              (Byte)atoi(componentElement->Attribute("instance"))));
                }
                componentElement = componentElement->NextSiblingElement("Component");
            }
        }
        mSubsystemConfigFromXML = *mSystemConfiguration.GetSubsystem(subsystemID);
        nodeElement = nodeElement->NextSiblingElement("Node");
    }
    
    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NetHeader").ToNode();
    if(node == NULL)
    {
        // Use defaults.
    }
    else
    {
        tempString = node->FirstChild()->Value();
        gNetworkHeader = tempString;
    }
    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NetPort").ToNode();
    if(node == NULL)
    {
        // Use defaults.
    }
    else
    {
        gNetworkPort = (unsigned short)atoi(node->FirstChild()->Value());
    }
    
    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("SubsystemID").ToNode();
    if(!node)
    {
        std::cout << "NodeManager: Subsystem ID Number not present in XML data.\n";
        return result;
    }
    s = atoi( node->FirstChild()->Value() );

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NodeID").ToNode();
    if(!node) 
    {
        std::cout << "NodeManager: Node ID Number not present in XML data.\n";
        return result;
    }
    n = atoi( node->FirstChild()->Value() );

    if( s <= 0 || s >= 255 || n <= 0 || n >= 255)
    {
        std::cout << "NodeManager:: Invalid Subsystem and Node ID values. Value ranges [1,254].\n";
        return result;
    }

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("MessageBoxSize").ToNode();
    if (node )
    {
        mbsize = atoi( node->FirstChild()->Value() );
        #ifdef JAUS_DEBUG
        std::cout << "Message Box Size: " << mbsize << " bytes.\n";
        #endif
    }

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("MulticastAddress").ToNode();
    if(node)
    {
        unsigned char ttl = 1;
        TiXmlElement* elem = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("MulticastAddress").ToElement();
        if(elem && elem->Attribute("ttl"))
        {
            ttl = (unsigned char)(atoi(elem->Attribute("ttl")));
        }
    
        if(SetMulticastAddress(node->FirstChild()->Value(), ttl) == JAUS_FAILURE)
        {
            std::cout << "Failed to use Multicast Address " << node->FirstChild()->Value() << std::endl;
        }
        else
        {
            #ifdef JAUS_DEBUG
            std::cout << "Using Multicast Address " << node->FirstChild()->Value() << std::endl;
            #endif
        }
    }

    bool useTCP = false;

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("Transport").ToNode();
    if (node )
    {
        std::string val = node->FirstChild()->Value();
        if(val == "UDP" || val == "udp")
        {
            useTCP = false;
            #ifdef JAUS_DEBUG
            std::cout << "Using UDP communication transport\n";
            #endif
        }
        else
        {
            useTCP = true;
            #ifdef JAUS_DEBUG
            std::cout << "Using TCP communication transport.\n";
            #endif
        }
    }

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NodeManagerNetAddress").ToNode();
    if(node && node->FirstChild())
    {
        if(CxUtils::Socket::IsIP4(node->FirstChild()->Value()) || CxUtils::Socket::IsIP6(node->FirstChild()->Value()))
        {
            mpConnectionHandler->SetNetworkInterface(node->FirstChild()->Value());
        }
        else
        {
            mpConnectionHandler->SetNetworkInterface(atoi(node->FirstChild()->Value()));
        }
    }

   
    if(!Initialize((Byte)s, (Byte)n, useTCP, mbsize) )
    {
        Shutdown();
        std::cout << "NodeManager: Failed to initialize with Node ID [" << (int)s << "." << (int)n << ".1.1] provided in XML data.\n";
        return result;
    }

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("CommunicatorNetAddress").ToNode();
    if(node && node->FirstChild())
    {
        // Get the default data link for communicator.
        mpConnectionHandler->GetCommunicator()->LockDataLinks();
        Communicator::DataLink::Map* links = mpConnectionHandler->GetCommunicator()->GetDataLinks();
        Communicator::DataLink::Map::iterator l;
        Communicator::DefaultDataLink* defaultLink = NULL;
        
        for(l = links->begin(); l != links->end(); l++)
        {
            defaultLink = dynamic_cast<Communicator::DefaultDataLink*>(l->second);
            if(defaultLink)
            {
                if(CxUtils::Socket::IsIP4(node->FirstChild()->Value()) || CxUtils::Socket::IsIP6(node->FirstChild()->Value()))
                {
                    defaultLink->SetNetworkInterface(node->FirstChild()->Value());
                }
                else
                {
                    defaultLink->SetNetworkInterface(atoi(node->FirstChild()->Value()));
                }
                break;
            }
        }
        mpConnectionHandler->GetCommunicator()->UnlockDataLinks();
    }

    nodeElement = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").ToElement();
    if(nodeElement->Attribute("broadcast"))
    {
        std::string broadcastValue = nodeElement->Attribute("broadcast");
        if(broadcastValue == "true" || broadcastValue == "TRUE" ||
            broadcastValue == "yes" || broadcastValue == "YES" ||
            broadcastValue == "y" || broadcastValue == "Y")
        {
            EnableSubsystemConfigDiscovery(true);
        }
        else
        {
            EnableSubsystemConfigDiscovery(false);
        }
    }


    std::cout << "Node ID: ";
    mNodeID.PrintID();
    nodeElement = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("SubsystemIdentification").ToElement();
    if(nodeElement)
    {
        mNodeMutex.Enter();
        if(nodeElement->Attribute("authority"))
        {
            mSubsystemIdentification.SetAuthority((Byte)atoi(nodeElement->Attribute("authority")));
        }
        if(nodeElement->Attribute("type"))
        {
            mSubsystemIdentification.SetType((UShort)atoi(nodeElement->Attribute("type")));
        }
        if(nodeElement->Attribute("identification"))
        {
            mSubsystemIdentification.SetIdentification(nodeElement->Attribute("identification"));
        }
        mSubsystemIdentification.PrintIdentification();
        GetCommunicator()->SetSubsystemIdentification(mSubsystemIdentification);
        mNodeMutex.Leave();
    }

    node = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NodeConnections").ToNode();
    if(node && node->FirstChild("Connection"))
    {
        TiXmlElement* connection = node->FirstChild("Connection")->ToElement();
        while(connection)
        {
            if(connection->Attribute("subsystem") &&
                connection->Attribute("node") &&
                (connection->Attribute("host") || connection->Attribute("port")))
            {
                Address nodeID;
                nodeID.SetAddress((Byte)(atoi(connection->Attribute("subsystem"))),
                                  (Byte)(atoi(connection->Attribute("node"))), 1, 1);
                if(nodeID != mNodeID)
                {
                    if(connection->Attribute("host"))
                    {
                        std::string host = connection->Attribute("host");
                        #ifdef JAUS_DEBUG
                        std::cout << "Connecting to " << connection->Attribute("host") << "...";
                        #endif
                        if(mpConnectionHandler->CreateConnection(nodeID, false, &host))
                        {
                            #ifdef JAUS_DEBUG
                            std::cout << "Success!\n";
                            #endif
                        }
                        else
                        {
                            #ifdef JAUS_DEBUG
                            std::cout << "Failure!\n";
                            #endif
                        }
                    }
                    else
                    {
                        unsigned int baud, bits, parity, stop;
                        baud = 9600;
                        bits = 8;
                        parity = 0;
                        stop = 1;
                        if(connection->Attribute("baud"))
                        {
                            baud = atoi(connection->Attribute("baud"));
                        }
                        if(connection->Attribute("bits"))
                        {
                            bits = atoi(connection->Attribute("bits"));
                        }
                        if(connection->Attribute("parity"))
                        {
                            parity = atoi(connection->Attribute("parity"));
                        }
                        if(connection->Attribute("stop"))
                        {
                            stop = atoi(connection->Attribute("stop"));
                        }
                        #ifdef JAUS_DEBUG
                        std::cout << "Connecting to port " << connection->Attribute("port") << "...";
                        #endif
                        if(mpConnectionHandler->CreateSerialConnection(nodeID,
                                                                       connection->Attribute("port"),
                                                                       baud, bits, parity, stop))
                        {
                            #ifdef JAUS_DEBUG
                            std::cout << "Success!\n";
                            #endif
                        }
                        else
                        {
                            #ifdef JAUS_DEBUG
                            std::cout << "Failure!\n";
                            #endif
                        }
                    }
                }
            }
            connection = connection->NextSiblingElement();
        }
    }

    /*  At this point we have the minimum data to run
        the Node Manager, so now lets try get optional data. */
    result = JAUS_OK;

    TiXmlElement *element = docHandle.FirstChild("Jaus").FirstChild("NodeManagerComponent").FirstChild("NodeLogger").ToElement();
    if(element) {
        temp = element->Attribute("logdata");
        if(temp == "yes" || temp == "y" || temp == "Y" || temp == "true" )
        {
            if(!CreateLogFile())
            {
                #ifdef JAUS_DEBUG
                std::cout << "NodeManager: Unable to create logfile.\n";
                #endif
            }
            else
            {
                #ifdef JAUS_DEBUG
                std::cout << "Created log file " << mpConnectionHandler->GetNodeLogger()->GetLogFileName() << std::endl;
                #endif
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the node manager.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::Shutdown()
{
    // Terminate any events we are subscribed to or generating.
    mEvents.CancelEvents(mpConnectionHandler);
    // Now close down connections, etc.
    mpConnectionHandler->Shutdown();
    mpMessageHandler->ClearMessageQueue();
    mNodeID(0, 0, 0, 0);
    mNodeMutex.Enter();
    mSubsystemList.clear();
    mConnectionEventTimeMs = 0;
    mSystemConfiguration.Clear();
    mSubsystemConfigFromXML.Clear();
    mSubsystemIdentification.ClearIdentification();
    mNodeMutex.Leave();

    mInitializedFlag = false;

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Resets the node manager.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::Restart()
{
    mEvents.CancelEvents(mpConnectionHandler);
    mpConnectionHandler->Restart();
    mpMessageHandler->ClearMessageQueue();

    mNodeMutex.Enter();

    // Delete discovered configuration data.
    Configuration::Subsystem::Map::iterator config;
    config = mSystemConfiguration.mSubsystems.begin();
    while(config != mSystemConfiguration.mSubsystems.end())
    {
        if(config->first != mNodeID.mSubsystem)
        {
#ifdef WIN32
            config = mSystemConfiguration.mSubsystems.erase(config);
#else
            mSystemConfiguration.mSubsystems.erase(config);
            config = mSystemConfiguration.mSubsystems.begin();
#endif
        }
        else
        {
            config++;
        }
    }
    // Reset to any configuration setup from XML.
    *mSystemConfiguration.GetSubsystem(mNodeID.mSubsystem) = mSubsystemConfigFromXML;

    Configuration::IdentificationMap::iterator identity;
    identity = mSystemConfiguration.mIdentifications.begin();
    while(identity != mSystemConfiguration.mIdentifications.end())
    {
        if(identity->first != mNodeID.mSubsystem)
        {
#ifdef WIN32
            identity = mSystemConfiguration.mIdentifications.erase(identity);
#else
            mSystemConfiguration.mIdentifications.erase(identity);
            identity = mSystemConfiguration.mIdentifications.begin();
#endif
        }
        else
        {
            identity++;
        }
    }

    mSubsystemList.clear();
    
    mConnectionEventTimeMs = Time::GetUtcTimeMs();
    if(mSubsystemDiscoveryFlag && mNodeID.IsValid())
    {
        mSubsystemList.insert(mNodeID);
    }
    if(GetCommunicator()->IsInitialized())
    {
        mSystemConfiguration.AddComponent(GetCommunicator()->GetID());
    }
    mNodeMutex.Leave();

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
int NodeManager::Send(const Stream& msg)
{
    if(mpConnectionHandler)
    {
        return mpConnectionHandler->Send(msg);
    }
    return JAUS_FAILURE;
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
int NodeManager::Send(const Message* msg)
{
    if(mpConnectionHandler)
    {
        return mpConnectionHandler->Send(msg);
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
int NodeManager::Send(const Stream& msg,
                      Receipt& receipt,
                      const UShort responseCode,
                      const UShort tms,
                      const UShort tries)
{
    if(mpConnectionHandler)
    {
        return mpConnectionHandler->Send(msg, receipt, responseCode, tms, tries);
    }
    return JAUS_FAILURE;
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
int NodeManager::Send(const Message* msg,
                      Receipt& receipt,
                      const UShort responseCode,
                      const UShort tms,
                      const UShort tries)
{
    if(mpConnectionHandler)
    {
        return mpConnectionHandler->Send(msg, receipt, responseCode, tms, tries);
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends an acknowledge response to the input message.
///
///   \param msg The input message to send an acknowledge response to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
//////////////////////////////////////////////////////////////////////////////////
int NodeManager::SendAcknowledgeResponse(const Message* msg)
{
    AckNackMessage response(msg->GetCommandCode());
    // Copy header data to copy priority, etc.
    response.CopyHeaderData(msg);
    response.SwapSourceAndDestination();
    // Make sure SC flag is not set.
    response.SetServiceConnectionFlag();
    response.SetAckNack(Header::AckNack::Ack);
    // Send it.
    return Send(&response);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a negative acknowledge response to the input message.
///
///   \param msg The input message to send a negative acknowledge response to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::SendNegativeAcknowledgeResponse(const Message* msg)
{
    AckNackMessage response(msg->GetCommandCode());
    // Copy header data to copy priority, etc.
    response.CopyHeaderData(msg);
    response.SwapSourceAndDestination();
    // Make sure SC flag is not set.
    response.SetServiceConnectionFlag();
    response.SetAckNack(Header::AckNack::Nack);
    // Send it.
    return Send(&response);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing command messages.  This method is called
///   whenever a command message is received.  Overload this method to
///   add support for other input message types.
///
///   \param msg Command message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::ProcessCommandMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_SET_COMPONENT_AUTHORITY:
        break;
    case JAUS_SHUTDOWN:
        {
            if(dynamic_cast<const Jaus::Shutdown*>(msg))
            {
                Shutdown();
            }
        }
        break;
    case JAUS_STANDBY:
        break;
    case JAUS_RESUME:
        break;
    case JAUS_RESET:
        {
            if(dynamic_cast<const Jaus::Reset*>(msg))
            {
                Reset();
            }
        }
        break;
    case JAUS_SET_EMERGENCY:
        break;
    case JAUS_CLEAR_EMERGENCY:
        break;
    case JAUS_CREATE_SERVICE_CONNECTION:
        {
            ConfirmServiceConnection response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetResponseCode(ConfirmServiceConnection::ComponentDoesNotSupport);
            Send(&response);
        }
        break;
    case JAUS_REQUEST_COMPONENT_CONTROL:
        {
            RejectComponentControl response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_RELEASE_COMPONENT_CONTROL:
        break;
    case JAUS_CREATE_EVENT:
        {
            const CreateEventRequest* command = dynamic_cast<const CreateEventRequest*>(msg);
            if(command)
            {
                bool confirm = false;
                bool newEvent = false;
                bool subsystemConfigurationEvent = false;
                Event* configEvent = NULL;
                RejectEventRequest::ResponseCode rejectType = RejectEventRequest::MessageNotSupported;

                mEvents.Lock();

                if(command->GetEventType() == Event::EveryChange ||
                    command->GetEventType() == Event::OneTime)
                {
                    if(command->GetMessageCode() == JAUS_REPORT_CONFIGURATION)
                    {
                        // See if we have an existing event
                        configEvent = mEvents.GetEvent(*command);
                        if(configEvent == NULL)
                        {
                            configEvent = new Event();
                            newEvent = true;
                            configEvent->SetEventProvider(mNodeID);
                            configEvent->SetMessageCode(JAUS_REPORT_CONFIGURATION);
                            configEvent->SetEventID(JAUS_NODE_CONFIG_CHANGE_EVENT);
                            configEvent->SetEventType(Event::EveryChange);
                            // Verify that the configuration type being requested
                            // is for subsystem or node configuration.
                            if(command->GetQueryMessage())
                            {
                                const QueryConfiguration* query = dynamic_cast<const QueryConfiguration*>(command->GetQueryMessage());
                                if(query &&
                                    (query->GetQueryField() != QueryConfiguration::Subsystem &&
                                     query->GetQueryField() != QueryConfiguration::Node))
                                {
                                    confirm = false;
                                    delete configEvent;
                                    configEvent = NULL;
                                }
                                else
                                {
                                    if(query->GetQueryField() == QueryConfiguration::Subsystem)
                                    {
                                        configEvent->SetEventID(JAUS_SUBSYSTEM_CONFIG_CHANGE_EVENT);
                                        subsystemConfigurationEvent = true;
                                    }
                                    configEvent->SetQueryMessage(query, true);
                                }
                            }

                            if(configEvent)
                            {
                                // Signal to send confirmation message.
                                confirm = true;
                                // Add the subscriber to the newly created event.
                                configEvent->GetEventSubscribers()->insert(command->GetSourceID());
                            }
                        }
                        else
                        {
                            // Add subscriber to event.
                            confirm = true;
                            configEvent->GetEventSubscribers()->insert(command->GetSourceID());
                            if(configEvent->GetQueryMessage())
                            {
                                if( ((QueryConfiguration *)(configEvent->GetQueryMessage()))->GetQueryField() ==
                                    QueryConfiguration::Subsystem)
                                {
                                    subsystemConfigurationEvent = true;
                                }
                            }
                        }
                    }
                    else if(command->GetMessageCode() == JAUS_REPORT_SUBSYSTEM_LIST)
                    {
                        // See if we have an existing event
                        configEvent = mEvents.GetEvent(*command);
                        if(configEvent == NULL)
                        {
                            configEvent = new Event();
                            configEvent->SetEventProvider(mNodeID);
                            configEvent->SetMessageCode(JAUS_REPORT_SUBSYSTEM_LIST);
                            configEvent->SetEventID(JAUS_SUBSYSTEM_LIST_CHANGE_EVENT);
                            configEvent->SetEventType(Event::EveryChange);
                            mEvents.AddEvent(configEvent);
                        }
                        // Add the subscriber.
                        configEvent->GetEventSubscribers()->insert(command->GetSourceID());
                        confirm = true; // Signal confirmation of event creation
                    }
                }
                else
                {
                    if(command->GetEventType() == Event::Periodic ||
                        command->GetEventType() == Event::PeriodicWithoutReplacement)
                    {
                        rejectType = RejectEventRequest::PeriodicEventsNotSupported;
                    }
                    else
                    {
                        rejectType = RejectEventRequest::ConnectionRefused;
                    }
                }
                if(confirm && configEvent)
                {
                    ConfirmEventRequest confirm;
                    confirm.SetSourceID(mNodeID);
                    confirm.SetDestinationID(command->GetSourceID());
                    confirm.SetMessageCode(command->GetMessageCode());
                    confirm.SetRequestID(command->GetRequestID());
                    confirm.SetEventID(configEvent->GetEventID());
                    Send(&confirm);

                    // Send the event data at least one time.
                    if(command->GetMessageCode() == JAUS_REPORT_CONFIGURATION)
                    {
                        if(subsystemConfigurationEvent)
                        {
                            this->ReportSubsystemConfiguration(configEvent);
                        }
                        else
                        {
                            this->ReportNodeConfiguration(configEvent);
                        }
                    }
                    else if(command->GetMessageCode() == JAUS_REPORT_SUBSYSTEM_LIST)
                    {
                        this->ReportSubsystemList(configEvent);
                    }
                    // If this is only a one time thing, then delete the event
                    // otherwise keep track of the event in the manager.
                    if(configEvent->GetEventType() == Event::OneTime)
                    {
                        delete configEvent;
                    }
                    else if(newEvent)
                    {
                        mEvents.AddEvent(configEvent);
                    }
                }
                else
                {
                    RejectEventRequest reject;
                    reject.SetSourceID(mNodeID);
                    reject.SetDestinationID(command->GetSourceID());
                    reject.SetRequestID(command->GetRequestID());
                    reject.SetResponseCode(rejectType);
                    Send(&reject);
                }
                mEvents.Unlock();
            }
        }
        break;
    case JAUS_CANCEL_EVENT:
        {
            const CancelEvent* command = dynamic_cast<const CancelEvent*>(msg);
            if(command)
            {
                mEvents.Lock();
                mEvents.CancelProducedEvent(*command, mpConnectionHandler);
                mEvents.Unlock();
            }
        }
        break;
    default:
        processed = JAUS_FAILURE;
        break;
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
int NodeManager::ProcessQueryMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_HEARTBEAT_PULSE:
        {
            ReportHeartbeatPulse response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_QUERY_COMPONENT_AUTHORITY:
        {
            ReportComponentAuthority response;
            response.SetAuthorityCode(0);
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_QUERY_COMPONENT_STATUS:
        {
            ReportComponentStatus response;
            response.SetPrimaryStatusCode(ReportComponentStatus::Status::Ready);
            response.SetSecondaryStatusCode(0);
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_QUERY_TIME:
        {
            ReportTime response;
            const QueryTime* query = dynamic_cast<const QueryTime*>(msg);
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            if(BitVector::IsBitSet(query->GetPresenceVector(), QueryTime::VectorBit::Time))
            {
                Time tstamp;
                tstamp.SetCurrentTime();
                response.SetTimeStamp(tstamp);
            }
            if(BitVector::IsBitSet(query->GetPresenceVector(), QueryTime::VectorBit::Date))
            {
                Date dstamp;
                dstamp.SetCurrentDate();
                response.SetDateStamp(dstamp);
            }

            Send(&response);
        }
        break;
    case JAUS_QUERY_COMPONENT_CONTROL:
        {
            ReportComponentControl response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetControllerID(Address());
            response.SetControllerAuthorityCode(0);
            Send(&response);
        }
        break;
    case JAUS_QUERY_SERVICES:
        {
            ReportServices response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            Service::Set serviceSet;
            mNodeMutex.Enter();
            serviceSet.insert(mServices);
            mNodeMutex.Leave();
            response.SetServices(serviceSet);
            Send(&response);
        }
        break;
    case JAUS_QUERY_SUBSYSTEM_LIST:
        {
            Jaus::ReportSubsystemList response;
            response.SetSourceID(mNodeID);
            response.SetDestinationID(msg->GetSourceID());
            mNodeMutex.Enter();
            if(mSubsystemDiscoveryFlag)
            {
                mSubsystemList.insert(mNodeID);
            }
            response.SetSubsystemList(mSubsystemList);
            mNodeMutex.Leave();
            Send(&response);
        }
        break;
    case JAUS_QUERY_EVENTS:
        {
            const QueryEvents* query = dynamic_cast<const QueryEvents*>(msg);
            if(query)
            {
                Jaus::ReportEvents response;
                response.SetSourceID(mNodeID);
                response.SetDestinationID(msg->GetSourceID());
                mEvents.Lock();
                mEvents.PopulateReportEventsMessage(*query, response);
                mEvents.Unlock();
                Send(&response);
            }
        }
        break;
    case JAUS_QUERY_CONFIGURATION:
        {
            const QueryConfiguration* query = dynamic_cast<const QueryConfiguration*>(msg);
            if(query->GetQueryField() == QueryConfiguration::Subsystem)
            {
                ReportSubsystemConfiguration(query->GetSourceID(), true);
            }
            else
            {
                ReportNodeConfiguration(query->GetSourceID(), true);
            }
        }
        break;
    case JAUS_QUERY_IDENTIFICATION:
        {
            const QueryIdentification* query = dynamic_cast<const QueryIdentification*>(msg);
            if(query)
            {
                ReportIdentification report;
                report.SetSourceID(mNodeID);
                report.SetDestinationID(query->GetSourceID());
                report.SetQueryType(query->GetQueryType());
                if(query->GetQueryType() == QueryIdentification::Node)
                {
                    report.GetIdentification()->SetType(40001);
                    report.GetIdentification()->SetIdentification("JAUS++ Node Manager");
                    report.GetIdentification()->SetAuthority(0);
                    Send(&report);
                }
                else if(query->GetQueryType() == QueryIdentification::Subsystem)
                {
                    // Only report if we have the data.
                    if(mSubsystemIdentification.GetIdentification().empty() == false)
                    {
                        report.SetIdentification(mSubsystemIdentification);
                        Send(&report);
                    }
                    else
                    {
                        processed = JAUS_FAILURE;
                    }
                }
            }
        }
        break;
    default:
        processed = JAUS_FAILURE;
        break;
    }

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing inform messages.  This method is called
///   whenever a inform message is received.  Overload this method to
///   add support for other input message types.
///
///   \param msg Inform message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::ProcessInformMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_EVENT:
        {
            const EventMessage* report = dynamic_cast<const EventMessage*>(msg);

            if(report)
            {
                // Find the event in the manager, and update it's
                // values, then process the data.
                Event* myEvent;
                Event::Key key;
                key.mSourceID == report->GetSourceID();
                key.mID = report->GetEventID();
                key.mMessageCode = report->GetEventMessageCode();
                mEvents.Lock();
                myEvent = mEvents.GetEvent(key);
                if(myEvent)
                {
                    myEvent->SetSequenceNumber(report->GetEventSequenceNumber());
                    myEvent->SetTimeStampMs(Time::GetUtcTimeMs());
                }
                mEvents.Unlock();

                mConnectionEventTimeMs = Time::GetUtcTimeMs();

                // Get the message data and process it
                if(report->GetEventMessage())
                {
                    MessageCallbackFunction(report->GetEventMessage(), this);
                }
            }
        }
        break;
    case JAUS_REPORT_HEARTBEAT_PULSE:
        {
            // If subsystem hearbeat pulse...
            if( msg->GetSourceID().IsValid() && 
                msg->GetDestinationID() == Address( 255, 255, 1, 1 ) )
            {
                bool subsystemListEvent = false;
                mNodeMutex.Enter();
                // If we are broadcasting to 255.255.1.1 then
                // add ourselves to list always.
                if(mSubsystemDiscoveryFlag && mNodeID.IsValid())
                {
                    mSubsystemList.insert(mNodeID);
                }

                Address::Set::iterator id;
                // Only add one entry for a subsystem to the list.
                bool inList = false;
                for(id = mSubsystemList.begin();
                    id != mSubsystemList.end();
                    id++)
                {
                    if(id->mSubsystem == msg->GetSourceID().mSubsystem)
                    {
                        inList = true;
                        mSubsystemHeartbeatTimes[msg->GetSourceID().mSubsystem] = Time::GetUtcTimeMs();
                    }
                }
                // Add to our subsystem if not already in it
                if(!inList)
                {
                    subsystemListEvent = true;
                    mSubsystemList.insert( msg->GetSourceID() );
                    mConnectionEventTimeMs = Time::GetUtcTimeMs();
                    mSubsystemHeartbeatTimes[msg->GetSourceID().mSubsystem] = Time::GetUtcTimeMs();
                }

                mNodeMutex.Leave();

                if(subsystemListEvent)
                {
                    ReportSubsystemListEvent(true, true);
                }

            }

            // If node heartbeat pulse...
            if( msg->GetSourceID().mSubsystem == mNodeID.mSubsystem &&
                msg->GetDestinationID().mNode == 255 &&
                msg->GetDestinationID().mComponent == 1 &&
                msg->GetDestinationID().mInstance == 1 &&
                msg->GetSourceID() != mNodeID)
            {
                //  See if we have an event subscription for
                //  this nodes configuration, and if not, create one.
                //  but first, query its configuration.
                Event::Set eventSet;
                bool haveEvent = false;

                mEvents.Lock();
                eventSet = mEvents.GetEventSubscriptionsOfType(JAUS_REPORT_CONFIGURATION, msg->GetSourceID());
                if(eventSet.size() > 0)
                {
                    haveEvent = true;
                }
                mEvents.Unlock();

                // We don't have an event subscription
                // from this node, lets try and create one!
                if(haveEvent == false)
                {
                    // Check to see if the component is in our list of those to ignore.
                    bool ignoreID = false;
                    unsigned int requestTimeMs = 0;
                    std::map<Byte, unsigned int>::iterator nodeRequestTime;
                    mNodeMutex.Enter();
                    // Look up the last time we requested events/configuration
                    // data from this node.
                    nodeRequestTime = mNodeRequestTimeMs.find(msg->GetSourceID().mNode);
                    if(nodeRequestTime != mNodeRequestTimeMs.end())
                    {
                        requestTimeMs = nodeRequestTime->second;
                    }
                    else
                    {
                        mNodeRequestTimeMs[msg->GetSourceID().mNode] = 0;
                    }
                    if(mNoEventSupport.find(msg->GetSourceID()) != mNoEventSupport.end())
                    {
                        ignoreID = true;
                    }
                    mNodeMutex.Leave();
                    // If we are not ignoring, and it has been more than a period of time.
                    if(!ignoreID && Time::GetUtcTimeMs() - requestTimeMs > 5000)
                    {
                        // Try create an event.
                        Receipt receipt;
                        QueryConfiguration query;
                        query.SetSourceID(mNodeID);
                        query.SetDestinationID(msg->GetSourceID());
                        query.SetQueryField(QueryConfiguration::Node);

                        CreateEventRequest create;
                        create.SetSourceID(mNodeID);
                        create.SetDestinationID(msg->GetSourceID());
                        create.SetRequestID(JAUS_NODE_CONFIG_CHANGE_EVENT);
                        create.SetMessageCode(JAUS_REPORT_CONFIGURATION);
                        create.SetEventType(CreateEventRequest::EveryChange);
                        create.SetQueryMessage(&query);

                        if(Send(&create, receipt) == JAUS_OK &&
                            receipt.GetResponseMessageCode() == JAUS_CONFIRM_EVENT_REQUEST &&
                            receipt.GetResponseMessage())
                        {
                            const ConfirmEventRequest* confirm = dynamic_cast<const ConfirmEventRequest*>(receipt.GetResponseMessage());
                            if(confirm)
                            {
                                Event *newEvent = new Event();
                                newEvent->SetEventID(confirm->GetEventID());
                                newEvent->SetQueryMessage(&query, true);
                                newEvent->SetEventProvider(confirm->GetSourceID());
                                newEvent->GetEventSubscribers()->insert(mNodeID);
                                mEvents.Lock();
                                mEvents.AddEvent(newEvent);
                                mEvents.Unlock();
                            } // Confirmed!
                        } // Try create event subscription.
                        // If events not supported, add to list of supported events.
                        if(receipt.GetStatus() == Receipt::Success &&
                            receipt.GetResponseMessageCode() == JAUS_REJECT_EVENT_REQUEST)
                        {
                            mNoEventSupport.insert(msg->GetSourceID());
                        }

                        if(receipt.GetStatus() == Receipt::Timeout)
                        {
                            QueryConfiguration query;
                            query.SetDestinationID(msg->GetSourceID());
                            query.SetSourceID(mNodeID);
                            query.SetQueryField(QueryConfiguration::Node);
                            Send(&query);
                        }
                        mNodeMutex.Enter();
                        mNodeRequestTimeMs[msg->GetSourceID().mNode] = Time::GetUtcTimeMs();
                        mNodeMutex.Leave();
                    } // If not in our list of nodes to ignore.
                } // Don't have an event subscription
            }
        }
        break;
    case JAUS_REPORT_CONFIGURATION:
        {
            const Jaus::ReportConfiguration* report = dynamic_cast<const Jaus::ReportConfiguration*>(msg);
            if(report)
            {
                mNodeMutex.Enter();
                report->AddToConfiguration(mSystemConfiguration);
                if(report->GetSourceID().mSubsystem == mNodeID.mSubsystem)
                {
                    mConnectionEventTimeMs = Time::GetUtcTimeMs();
                }
                // If the configuration data was from this subsystem
                // we may need to generate a subsystem event, so do so.
                if(report->GetSourceID().mSubsystem == mNodeID.mSubsystem)
                {
                    mNodeMutex.Leave();
                    ReportSubsystemConfigurationEvent(true, true);
                }
                else
                {
                    mNodeMutex.Leave();
                }

            }
        }
        break;
    case JAUS_REPORT_IDENTIFICATION:
        {
            if(msg->GetSourceID().mSubsystem == mNodeID.mSubsystem)
            {
                const ReportIdentification* report = dynamic_cast<const ReportIdentification*>(msg);
                if(report && report->GetQueryType() == ReportIdentification::Subsystem &&
                    report->GetIdentification()->GetIdentification().empty() == false)
                {
                    mNodeMutex.Enter();
                    mSubsystemIdentification = *report->GetIdentification();
                    mNodeMutex.Leave();
                }
            }
        }
        break;
    default:
        processed = JAUS_FAILURE;
        break;
    }

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing experimental messages.  This method is called
///   whenever a experimental message is received.  Overload this method to
///   add support for other input message types.
///
///   \param msg Experimental message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::ProcessExperimentalMessage(const Message* msg)
{
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Starts a message log file using a unique name.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::CreateLogFile()
{
    char buff[256];
    unsigned short day, hour, minute, second, msecond;
    CxUtils::GetSystemTime(day, hour, minute, second, msecond);
    CxUtils::FileIO::CreateDir("logs");
    sprintf(buff,
            "logs/node_log_%.3d.%.3d_%d.%d.%d.%d.%d.xml",
            mNodeID.mSubsystem, mNodeID.mNode, day, hour, minute, second, msecond);
    return CreateLogFile(buff);
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
///   \param ttl Time To Live for Multicast Transmissions.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.                               
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::SetMulticastAddress(const std::string& multicast, const unsigned char ttl)
{
    int result = JAUS_FAILURE;
    mNodeMutex.Enter();
    result = mpConnectionHandler->SetMulticastAddress(multicast, ttl);
    mNodeMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the network interface to use for receiving UDP message traffic.
///
///   \param num The network interface number to use for receiving UDP traffic.
///              A value of -1 means any interface, 0 the first, 1 the second, etc.
///
///   \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManager::SetNetworkInterface(const int num)
{
    int result = JAUS_FAILURE;
    mNodeMutex.Enter();
    result = mpConnectionHandler->SetNetworkInterface(num);
    mNodeMutex.Leave();
    return result == JAUS_OK ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the network interface to use for receiving UDP message traffic.
///
///   This method looks up the network interface number to use, then sets it.
///
///   \param address The IP address of the network interface to use for receving
///                  network traffic. 
///
///   \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManager::SetNetworkInterface(const std::string& address)
{
    int result = JAUS_FAILURE;
    mNodeMutex.Enter();
    result = mpConnectionHandler->SetNetworkInterface(address);
    mNodeMutex.Leave();
    return result == JAUS_OK ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a message log.
///
///   \param fileXML The name of the log file.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::CreateLogFile(const std::string& fileXML)
{
    if( mNodeID.IsValid() )
    {
        return mpConnectionHandler->GetNodeLogger()->CreateLogFile(mNodeID, fileXML );
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Closes any open log file.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::CloseLogFile()
{
    return mpConnectionHandler->GetNodeLogger()->CloseLogFile();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the identification of the subsystem so the node can
///          share it.
///
///   \param ident Subsystem identification.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::SetSubsystemIdentification(const Identification& ident)
{
    mNodeMutex.Enter();
    mSubsystemIdentification = ident;
    mNodeMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enables broadcasting heartbeat messages to
///   the 255.255.1.1 address, which will cause others to query the Node Manager
///   for subsystem configuration and identification data.
///
///   \param enable If true, the node will broadcast to the 255.255.1.1 ID
///          signaling that it supports sharing of subsystem configuration and
///          identification data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::EnableSubsystemConfigDiscovery(const bool enable)
{
    mSubsystemDiscoveryFlag = enable;
    mNodeMutex.Enter();
    if(mpConnectionHandler)
    {
        mpConnectionHandler->ToggleSubsystemHeartbeatBroadcast(enable);
    }
    if(enable)
    {
        if(mNodeID.IsValid())
        {
            mSubsystemList.insert(mNodeID);
        }
    }
    else
    {
        // Remove ourselves from subsystem list if enable is false.
        if(mNodeID.IsValid())
        {
            if(mSubsystemList.find(mNodeID) != mSubsystemList.end())
            {
                mSubsystemList.erase(mSubsystemList.find(mNodeID));
            }
        }
    }
    mNodeMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the configuration of the system and subystem, etc.
///
///   This data can be set in an XML and loaded through Initialize also.
///
///   \param config Configuration of the system or the node and subsystem, etc.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::SetSystemConfiguration(const Configuration& config)
{
    mNodeMutex.Enter();
    mSystemConfiguration = config;
    mNodeMutex.Leave();
    return OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Closes any open log file.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManager::IsLogOpen() const
{
    return mpConnectionHandler->GetNodeLogger()->IsLogOpen();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Use this method to check if the Node Manager is initialized.
///
///   \return True if initialized, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManager::IsInitialized() const
{
    return mInitializedFlag;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the subsystem identification (if it has been set).
///
///   \return Copy of subsystem identification.
///
////////////////////////////////////////////////////////////////////////////////////
Identification NodeManager::GetSubsystemIdentification() const
{
    Identification ident;
    mNodeMutex.Enter();
    ident = mSubsystemIdentification;
    mNodeMutex.Leave();
    return ident;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of all known system configuration information.  This
///   information will include identified subsystems, nodes, and as much
///   information about the nodes subsystem that can be gathered.
///
///   \return Copy of system configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration NodeManager::GetSystemConfiguration() const
{
    Configuration config;
    mNodeMutex.Enter();
    config = mSystemConfiguration;
    mNodeMutex.Leave();
    return config;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the Subsystem Configuration.
///
///   \return Copy of subsystem configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem NodeManager::GetSubsystemConfiguration() const
{
    Configuration::Subsystem config;
    mNodeMutex.Enter();
    Configuration::Subsystem* ptr = mSystemConfiguration.GetSubsystem(mNodeID.mSubsystem);
    if(ptr)
    {
        config = *ptr;
    }
    mNodeMutex.Leave();
    return config;
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
void NodeManager::ToggleBroadcastUDP(const bool on)
{
    mNodeMutex.Enter();
    if(mpConnectionHandler)
    {
        mpConnectionHandler->ToggleBroadcastUDP(on);
    }
    mNodeMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list of all components broadcasting to an address of
///   255.255.1.1.  Components on this list must be able
///   to provide configuration information.
///
///   \return Subsystem list.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set NodeManager::GetSubsystemList() const
{
    Address::Set list;
    mNodeMutex.Enter();
    list = mSubsystemList;
    mNodeMutex.Leave();
    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the nodes connection handler.
///
////////////////////////////////////////////////////////////////////////////////////
NodeConnectionHandler* NodeManager::GetConnectionHandler() const
{
    return mpConnectionHandler;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the Communicator component used by the Node Manager.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator* NodeManager::GetCommunicator() const
{
    if(mpConnectionHandler)
    {
        return mpConnectionHandler->GetCommunicator();
    }
    else
    {
        return NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Determines if there is a Node Manager running, and gets can
///   get a copy of it's JAUS ID.
///
///   This function only finds the first instance of a Node Manager
///   running on a machine.
///
///   \param id Pointer to structure to populate with JAUS ID of Node Manager
///             if one is present.
///
///   \return True if Node Manager present and data copied to structure,
///           otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManager::IsNodeManagerPresent(Address *id)
{
    bool result = false;
    char name[100];
    for(Byte i = 1; i < 255 && !result; i++)
    {
        for(Byte j = 1; j < 255 && !result; j++)
        {
            sprintf(name, "%03d.%03d.001.001_NodeID", i, j);
            //  Check that no other node is running with this ID.
            if(CxUtils::MappedMemory::IsMemOpen(name))
            {
                result = true;
                if(id)
                {
                    id->mSubsystem = i;
                    id->mNode = j;
                    id->mComponent = 1;
                    id->mInstance = 1;
                }
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Determines if there is a Node Manager running on a single
///   compuater, and gets a copy of there JAUS ID.
///
///   \param id Vector of all found JAUS Node Managers running on a system.
///
///   \return Number of active Node Manager ID's found.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeManager::GetNodeManagerAddresses(Address::List &id)
{
    char name[100];
    id.clear();
    for(Byte i = 1; i < 255; i++)
    {
        for(Byte j = 1; j < 255; j++)
        {
            sprintf(name, "%03d.%03d.001.001_NodeID", i, j);
            //  Check that no other node is running with this ID.
            if(CxUtils::MappedMemory::IsMemOpen(name))
            {
                id.push_back(Address(i, j, 1, 1));
            }
        }
    }

    return (int)(id.size());
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the nodes configuration data to the address provide.
///
///   \param dest Destination to send to.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportNodeConfiguration(const Address& dest, const bool lockNode)
{
    ReportConfiguration report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(dest);

    if(lockNode) { mNodeMutex.Enter(); }

    report.SetConfiguration(mNodeID.mSubsystem,
                            *mSystemConfiguration.GetNode(mNodeID.mSubsystem, mNodeID.mNode));

    if(lockNode) { mNodeMutex.Leave(); }

    Send(&report);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the nodes configuration data to the address provide.
///
///   \param eventInfo The event information to use for the report message.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportNodeConfiguration(const Event* eventInfo, const bool lockNode)
{
    ReportConfiguration report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(eventInfo->GetEventProvider());

    if(lockNode) { mNodeMutex.Enter(); }
    report.SetConfiguration(mNodeID.mSubsystem,
                            *mSystemConfiguration.GetNode(mNodeID.mSubsystem, mNodeID.mNode));
    if(lockNode) { mNodeMutex.Leave(); }
    EventManager::GenerateEvent(eventInfo, &report, mpConnectionHandler);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the subsystem configuration data to the address provide.
///
///   \param dest The destination ID for sending the report to.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemConfiguration(const Address& dest, const bool lockNode)
{
    ReportConfiguration report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(dest);
    if(lockNode) { mNodeMutex.Enter(); }
    report.SetConfiguration(*mSystemConfiguration.GetSubsystem(mNodeID.mSubsystem));
    if(lockNode) { mNodeMutex.Leave(); }
    Send(&report);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the subsystem configuration data to the address provide using
///          and event.
///
///   \param eventInfo event information to use for generation.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemConfiguration(const Event* eventInfo, const bool lockNode)
{
    ReportConfiguration report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(eventInfo->GetEventProvider());
    if(lockNode) { mNodeMutex.Enter(); }
    report.SetConfiguration(*mSystemConfiguration.GetSubsystem(mNodeID.mSubsystem));
    if(lockNode) { mNodeMutex.Leave(); }
    EventManager::GenerateEvent(eventInfo, &report, mpConnectionHandler);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the subsystem list to the destination provided.
///
///   \param dest Destination to send to.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemList(const Address& dest, const bool lockNode)
{
    Jaus::ReportSubsystemList report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(dest);

    if(lockNode) { mNodeMutex.Enter(); }
    report.SetSubsystemList(mSubsystemList);
    if(lockNode) { mNodeMutex.Leave(); }

    Send(&report);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reports the subsystem list to the destination provided using an
///          event.
///
///   \param eventInfo event information to use for generation.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemList(const Event* eventInfo, const bool lockNode)
{
    Jaus::ReportSubsystemList report;

    report.SetSourceID(mNodeID);
    report.SetDestinationID(eventInfo->GetEventProvider());

    if(lockNode) { mNodeMutex.Enter(); }
    report.SetSubsystemList(mSubsystemList);
    if(lockNode) { mNodeMutex.Leave(); }

    EventManager::GenerateEvent(eventInfo, &report, mpConnectionHandler);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief When the configuration of the node changes, use this method
///   to check if we have event subscribers, and send the new configuration
///   event.
///
///   \param lockEvents Use the Event Manager mutex if true.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportNodeConfigurationEvent(const bool lockNode, const bool lockEvents)
{
    Event::Set eventSet;
    Event::Set::iterator ev;
    ReportConfiguration reportConfiguration;

    if(lockNode) { mNodeMutex.Enter(); }
    reportConfiguration.SetConfiguration(mNodeID.mSubsystem, *mSystemConfiguration.GetNode(mNodeID.mSubsystem, mNodeID.mNode));
    if(lockNode) { mNodeMutex.Leave(); }

    if(lockEvents) { mEvents.Lock(); }
    eventSet = mEvents.GetEventsOfType(JAUS_REPORT_CONFIGURATION);
    if(eventSet.size() > 0)
    {
        // Get node configuration data
        // Generate events for this message type.
        for(ev = eventSet.begin(); ev != eventSet.end(); ev++)
        {
            EventManager::GenerateEvent(*ev, &reportConfiguration, mpConnectionHandler);
        }
    }
    if(lockEvents) { mEvents.Unlock(); }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief When the nodes subsystem configuration changes, use this method
///   to check if we have event subscribers, and send the new configuration
///   event.
///
///   \param lockEvents Use the Event Manager mutex if true.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemConfigurationEvent(const bool lockNode, const bool lockEvents)
{
    Event::Set eventSet;
    Event::Set::iterator ev;
    ReportConfiguration reportConfiguration;
    QueryConfiguration* queryConfiguration;

    if(lockNode) { mNodeMutex.Enter(); }
    reportConfiguration.SetConfiguration(*mSystemConfiguration.GetSubsystem(mNodeID.mSubsystem));
    if(lockNode) { mNodeMutex.Leave(); }

    if(lockEvents) { mEvents.Lock(); }

    eventSet = mEvents.GetEventsOfType(JAUS_REPORT_CONFIGURATION);

    if(eventSet.size() > 0)
    {
        // Generate events for this message type.
        for(ev = eventSet.begin(); ev != eventSet.end(); ev++)
        {
            if((*ev)->GetQueryMessage())
            {
                queryConfiguration = dynamic_cast<QueryConfiguration*>((*ev)->GetQueryMessage());
                if(queryConfiguration &&
                    queryConfiguration->GetQueryField() == QueryConfiguration::Subsystem)
                {
                    EventManager::GenerateEvent(*ev, &reportConfiguration, mpConnectionHandler);
                }
            }
        }
    }

    if(lockEvents) { mEvents.Unlock(); }

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Generates a Report Subsystem List event message to all subscribers.
///
///   \param lockEvents use the event Manager mutex if true.
///   \param lockNode If true mutex is locked, otherwise not locked.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ReportSubsystemListEvent(const bool lockNode, const bool lockEvents)
{
    Event::Set eventSet;
    Event::Set::iterator ev;
    Jaus::ReportSubsystemList reportSubsystemList;

    //std::cout << "Subsystem List Event!\n";

    if(lockNode) { mNodeMutex.Enter(); }

    if(mSubsystemDiscoveryFlag && mNodeID.IsValid())
    {
        mSubsystemList.insert(mNodeID);
    }
    reportSubsystemList.SetSubsystemList(mSubsystemList);

    if(lockNode) { mNodeMutex.Leave(); }

    if(lockEvents) { mEvents.Lock(); }

    eventSet = mEvents.GetEventsOfType(JAUS_REPORT_SUBSYSTEM_LIST);
    if(eventSet.size() > 0)
    {
        // Generate events for this message type.
        for(ev = eventSet.begin(); ev != eventSet.end(); ev++)
        {
            EventManager::GenerateEvent(*ev, &reportSubsystemList, mpConnectionHandler);
        }
    }
    if(lockEvents) { mEvents.Unlock(); }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method updates known configurations and generates events
///          by processing changes in subsystem and node configurations.
///
///   \param type Type of event (component connect/disconnect, node connect or
///               disconnect, etc.)
///   \param id ID of the component who had a connection event.
///   \param args Pointer to the NodeManager so it can process the data.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::ProcessConnectionEvent(const Byte type,
                                         const Address& id,
                                         void *args)
{
    NodeManager* node = (NodeManager*)args;

    switch(type)
    {
    case NodeConnectionHandler::ComponentConnect:
        {
            // First update our configuration.
            node->mNodeMutex.Enter();
            node->mSystemConfiguration.AddComponent(id);
            node->mNodeMutex.Leave();

            // Generate event when new component connects.
            node->ReportNodeConfigurationEvent(true, true);
            node->ReportSubsystemConfigurationEvent(true, true);

            QueryIdentification query;
            // See if the component has subsystem identification.
            query.SetSourceID(node->mNodeID);
            query.SetDestinationID(id);
            query.SetQueryType(QueryIdentification::Subsystem);
            node->Send(&query);
        }
        break;
    case NodeConnectionHandler::NodeConnect:
        {
            // First update our configuration.
            node->mNodeMutex.Enter();
            node->mSystemConfiguration.AddComponent(id);
            node->mNodeMutex.Leave();
        }
        break;
    case NodeConnectionHandler::ComponentDisconnect:
        {
            // Delete any event subscriptions from the component.
            node->mEvents.Lock();
            node->mEvents.DeleteComponent(id);
            node->mEvents.Unlock();

            // Now update our configuration.
            node->mNodeMutex.Enter();
            node->mSystemConfiguration.RemoveComponent(id);
            node->mNodeMutex.Leave();

            // Generate event when components disconnect.
            node->ReportNodeConfigurationEvent(true, true);
            node->ReportSubsystemConfigurationEvent(true, true);

            node->mNodeMutex.Enter();
            // See if we need to remove component from subsystem list.
            if(node->mSubsystemList.find(id) != node->mSubsystemList.end())
            {
                node->mSubsystemList.erase(node->mSubsystemList.find(id));
                node->mNodeMutex.Leave();
                // Generate event if component is removed from list.
                node->ReportSubsystemListEvent(true, true);
            }
            else
            {
                node->mNodeMutex.Leave();
            }
        }
        break;
    case NodeConnectionHandler::NodeDisconnect:
        {
            // Delete any event subscriptions from the node.
            node->mEvents.Lock();
            node->mEvents.DeleteComponentsFromNode(id.mSubsystem, id.mNode);
            node->mEvents.Unlock();

            // First update our configuration.
            node->mNodeMutex.Enter();
            
            // If the node was part of our susbystem, we must
            // generate a subsystem event to event subscribers.
            if(id.mSubsystem == node->mNodeID.mSubsystem)
            {
                std::map<Byte, unsigned int>::iterator requestTime;
                requestTime = node->mNodeRequestTimeMs.find(id.mNode);
                if(requestTime != node->mNodeRequestTimeMs.end())
                {
                    node->mNodeRequestTimeMs.erase(requestTime);
                }
                node->mNodeMutex.Leave();
                node->ReportSubsystemConfigurationEvent(true, true);
                node->mNodeMutex.Enter();
            }

            node->mSystemConfiguration.RemoveComponent(id);
            // Remove any components that were attached to that node
            // from the subsystem list.
            bool subsystemListChange = false;
            Address::Set::iterator sid;
            for(sid = node->mSubsystemList.begin();
                sid != node->mSubsystemList.end();)
            {
                if(sid->mSubsystem == id.mSubsystem && sid->mNode == id.mNode)
                {
                    node->mSubsystemList.erase(sid);
                    sid = node->mSubsystemList.begin();
                    /*
#ifdef WIN32
                    sid = node->mSubsystemList.erase(sid);
#else
                    node->mSubsystemList.erase(sid);
#endif
                    */
                    subsystemListChange = true;
                }
                else
                {
                    sid++;
                }
            }

            // If the subsystem list changed, then
            // generate event.
            if(subsystemListChange)
            {
                node->mNodeMutex.Leave();
                node->ReportSubsystemListEvent(true, true);
                node->mNodeMutex.Enter();
            }

            // Remove from set of nodes to ignore for event creation.
            if(node->mNoEventSupport.find(id) != node->mNoEventSupport.end())
            {
                node->mNoEventSupport.erase(node->mNoEventSupport.find(id));
            }
            // If the node disconnecting was part of this subsystem
            // generate an event.
            if(id.mSubsystem == node->mNodeID.mSubsystem)
            {
                node->mNodeMutex.Leave();
                node->ReportSubsystemConfigurationEvent(true, true);
            }
            else
            {
                node->mNodeMutex.Leave();
            }
        }
        break;
    default:
        return;
        break;
    }

    node->mConnectionEventTimeMs = Time::GetUtcTimeMs();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called whenever a JAUS message has been received and
///          needs to be processed.
///
///   This function looks at the type of message, and then calls either
///   ProcessCommandMessage, ProcessQueryMessage, ProcessInformMessage or
///   ProcessExperimentalMessage depending on the message type.
///
///   \param msg Received message for processing.
///   \param args Pointer to NodeManager to use for message processing.
///
///   \return JAUS_OK on processing success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::MessageCallbackFunction(const Message* msg, void* args)
{
    NodeManager *node = (NodeManager*)args;

    UShort code = msg->GetCommandCode();

    if(Address::DestinationMatch(msg->GetDestinationID(), node->mNodeID))
    {
        // Signal that the message was received.
        if(msg->GetAckNack() == Header::AckNack::Request)
        {
            node->SendAcknowledgeResponse(msg);
        }

        // Check for an Ack/Nack message.
        if(msg->GetAckNack() == Header::AckNack::Ack || msg->GetAckNack() == Header::AckNack::Nack)
        {
            return;
        }
        else
        {
            // Based on the value of the message code, pass
            // the message to the right category.
            if(code <= 0x1FFF)
            {
                node->ProcessCommandMessage(msg);
            }
            else if(code >= 0x2000 && code <= 0x3FFF)
            {
                node->ProcessQueryMessage(msg);
            }
            else if(code >= 0x4000 && code <= 0x5FFF)
            {
                node->ProcessInformMessage(msg);
            }
            else if(code >= 0xD000)
            {
                node->ProcessExperimentalMessage(msg);
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method goes through the subsystem list and makes sure it is up
///          to date.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManager::CheckSubsystemList()
{
    bool subsystemListEvent = false;

    mNodeMutex.Enter();

    // If we are broadcasting to 255.255.1.1 then
    // add ourselves to list always.
    if(mSubsystemDiscoveryFlag && mNodeID.IsValid())
    {
        mSubsystemList.insert(mNodeID);
    }

    Address::Set::iterator id;
    // Delete any OLD heartbeat messages.
    std::map<Byte, unsigned int>::iterator updateTimeMs;
    updateTimeMs = mSubsystemHeartbeatTimes.begin();
    while(updateTimeMs != mSubsystemHeartbeatTimes.end())
    {
        if(Time::GetUtcTimeMs() - updateTimeMs->second > 3000)
        {
            // Delete ID.
            for(id = mSubsystemList.begin();
                id != mSubsystemList.end();
                id++)
            {
                if(id->mSubsystem == updateTimeMs->first)
                {
                    mSubsystemList.erase(id);
                    mConnectionEventTimeMs  = Time::GetUtcTimeMs();
                    break;
                }
            }
            mSubsystemHeartbeatTimes.erase(updateTimeMs);
            updateTimeMs = mSubsystemHeartbeatTimes.begin();
        }
        else
        {
            updateTimeMs++;
        }
    }

    mNodeMutex.Leave();

    if(subsystemListEvent)
    {
        ReportSubsystemListEvent(true, true);
    }

}

/*  End of File */
