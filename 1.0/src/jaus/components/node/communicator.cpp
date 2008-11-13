////////////////////////////////////////////////////////////////////////////////////
///
///  \file communicator.cpp
///  \brief The Communicator component handles all communication to other
///  subsystems using custom data links.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 August 2008
///  <br>Last Modified: 13 August 2008
///  <br>Copyright (c) 2008
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
#include "jaus/components/node/communicator.h"
#include "jaus/messages/common/configuration/service.h"
#include "jaus/messages/command/communications/setdatalinkselect.h"
#include "jaus/messages/command/communications/setdatalinkstate.h"
#include "jaus/messages/command/communications/setselecteddatalinkstate.h"
#include "jaus/messages/query/communications/querydatalinkstatus.h"
#include "jaus/messages/query/communications/queryheartbeatpulse.h"
#include "jaus/messages/query/communications/queryselecteddatalinkstatus.h"
#include "jaus/messages/query/configuration/queryidentification.h"
#include "jaus/messages/query/configuration/queryconfiguration.h"
#include "jaus/messages/inform/communications/reportdatalinkstatus.h"
#include "jaus/messages/inform/communications/reportheartbeatpulse.h"
#include "jaus/messages/inform/communications/reportselecteddatalinkstatus.h"
#include "jaus/messages/inform/configuration/reportconfiguration.h"
#include "jaus/messages/inform/configuration/reportidentification.h"
#include "jaus/components/transport/net.h"


//#define _JAUS_DEBUG

#ifdef _JAUS_DEBUG
#include <iostream>
using namespace std;
#endif

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::DataLink::DataLink()
            : mID(0),
              mpCommunicator(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::DataLink::~DataLink()
{
    mCommunicatorMutex.Enter();
    mpCommunicator = NULL;
    mID = 0;
    mCommunicatorMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID number for the data link.
///
///   \param id The ID number to set for the data link.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::DataLink::SetID(const Byte id)
{
    mCommunicatorMutex.Enter();
    mID = id;
    mCommunicatorMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method should be called whenever data has been received from
///   the data link.  The Stream data passed must contain only JAUS formatted
///   data.  Any additional encryption or transport header information that
///   is added for the data link <b>MUST</b> be stripped.
///
///   \param data Serialized JAUS message data received over the data link.
///
///   \return True if the message was processed/consumed by the communicator
///   and false if not.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::DataLink::ProcessReceivedMessage(const Stream &data)
{
    bool result = false;

    mCommunicatorMutex.Enter();

    if(mpCommunicator)
    {
        // Sends to node manager for routing.
        result = mpCommunicator->ProcessDataLinkMessage(data);
    }
    mCommunicatorMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The ID of the data link.
///
////////////////////////////////////////////////////////////////////////////////////
Byte Communicator::DataLink::GetID() const
{
    return mID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The ID of the data link.
///
////////////////////////////////////////////////////////////////////////////////////
Byte Communicator::DataLink::GetSubsystemID() const 
{ 
    Byte val = 0;
    mCommunicatorMutex.Enter();
    if(mpCommunicator)
    {  
        val = mpCommunicator->GetID().mSubsystem;  
    }
    mCommunicatorMutex.Leave();
    return val; 
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Default constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::DefaultDataLink::DefaultDataLink()
            : mBroadcastFlag(false), mMulticastTTL(1)
{
    mLinkState = Off;
    mMulticastAddress = "224.1.0.1";
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::DefaultDataLink::~DefaultDataLink()
{
    SetState(Off);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the desired state of the data link.
///
///   Depending on the state value passed, the data link is shutdown, put into
///   standby mode, or initialized/resumed.
///
///   \param state The desired state of the data link.
///
///   \return True on success, false on failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool Jaus::Communicator::DefaultDataLink::SetState(const State state)
{
    mConnectionsMutex.Enter();

    if(state == Off)
    {
        mBroadcastClient.Shutdown();
        mMulticastClient.Shutdown();
        mServer.Shutdown();
        mRecvThread.StopThread();
        mSubsystems.clear();
        mSubsystemHeartbeatTimesMs.clear();
    }
    else if(state == On)
    {
        // If the link was off, then we need to
        // initialize things, if we were in standby, then we don't need
        // to re-init.
        if(mLinkState == Off)
        {
            mMulticastClient.InitializeMulticastSocket(mMulticastAddress.c_str(), gNetworkPort, mMulticastTTL);
            mBroadcastClient.InitializeSocket("225.225.225.225", gNetworkPort);
            mServer.InitializeMulticastSocket(gNetworkPort, mMulticastAddress.c_str());
            mRecvThread.CreateThread(DefaultDataLink::RecvThread, this);
        }
    }
    else if(state == Standby)
    {
        // Nothing to do.
    }
    mLinkState = state;

    mConnectionsMutex.Leave();

    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Transmits the message over the data link.
///
///   For this data link, if a unicast connection is available to the destination
///   then it is used, otherwise Broadcast or Multicast sending is used to
///   deliver to other subsystems.
///
///   \param data Serialized JAUS message to transmit.
///
///   \return True on success, otherwise false on failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::DefaultDataLink::Transmit(const Stream& data)
{
    Header header;
    bool result = false;
    // Only transmit to other subsystems.
    if(data.Read(header, 0) && header.mDestinationID.mSubsystem != GetSubsystemID())
    {
        mTransportStream.Clear();
        if(gNetworkHeader.size() > 0)
        {
            mTransportStream.Write((const unsigned char *)gNetworkHeader.c_str(), (unsigned int)gNetworkHeader.size());
            mTransportStream.Write(data);
            std::map<Byte, CxUtils::UdpClient>::iterator destination;
            // If broadcast send to all subsystems.
            if(header.mDestinationID.IsBroadcast())
            {
                // If using a hearbeat pulse message, treat differently by using
                // UDP Multicast or Broadcast.
                if(header.mCommandCode == JAUS_REPORT_HEARTBEAT_PULSE)
                {
                    if(mBroadcastFlag)
                    {
                        if(mBroadcastClient.Send(mTransportStream)) { result = true; };
                    }
                    else
                    {
                        if(mMulticastClient.Send(mTransportStream)) { result = true; };
                    }
                }
                else
                {
                    for(destination = mSubsystems.begin();
                        destination != mSubsystems.end();
                        destination++)
                    {
                        if(destination->first != GetSubsystemID())
                        {
                            if(destination->second.Send(mTransportStream)) { result = true; }
                        }
                    }
                }
            }
            else
            {
                destination = mSubsystems.find(header.mDestinationID.mSubsystem);
                if(destination != mSubsystems.end())
                {
                    if(destination->second.Send(mTransportStream)) { result = true; }
                }
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enables use of Broadast over Multicast.
///
///   \param on If true, broadcasting is used instead of multicast.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::DefaultDataLink::ToggleBroadcast(const bool on)
{
    mBroadcastFlag = on;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets Multicast Group to join and send to and the Time To Live for
///   Multicast data.
///
///   \param multicast Multicast group to use "224.1.0.2" is the default. Valid
///                    values are: "224.0.0.0" - "239.255.255.255".
///   \param ttl   Time To Live for Multicast transmission.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::DefaultDataLink::SetMulticastAddress(const std::string& multicast,
                                                        const unsigned char ttl)
{
    State prevState = GetState();

    SetState(Off);

     mMulticastAddress = multicast;
     mMulticastTTL = ttl;

    if(prevState == On || prevState == Standby)
    {
        SetState(On);
    }
    if(prevState == Standby)
    {
        SetState(Standby);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a unicast connection to a subsystem that is not subject to
///   dynamic discovery operations (can't be dynamically removed).
///
///   If an existing Unicast UDP connection exists, this method will fail.
///
///   \param subsystemID The ID of the subsystem to create connection to.
///   \param host The IP address/hostname of the subsystems data link.
///
///   \return True on success, false on failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::DefaultDataLink::AddSubsystem(const Byte subsystemID, const std::string& host)
{
    bool result = false;
    std::map<Byte, CxUtils::UdpClient>::iterator unicast;
    mConnectionsMutex.Enter();
    unicast = mSubsystems.find(subsystemID);
    // If we don't already have an exisiting connection.
    if(unicast == mSubsystems.end())
    {
        if(mSubsystems[subsystemID].InitializeSocket(host.c_str(), gNetworkPort) > 0)
        {
            result = true;
            mFixedConnections.insert(subsystemID);
        }
        else
        {
            unicast = mSubsystems.find(subsystemID);
            mSubsystems.erase(unicast);
        }
    }
    mConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief A thread which continuously receives UDP data over a socket.
///
///   Depending on the state of the data link (Off, On, Standby) UDP messages
///   are received and passed to the ProcessReceivedMessage method.  Additionally,
///   if new subsystems are discovered Unicast UDP clients are created so that
///   future UDP transmissions can be sent using Unicast over Multicast/Broadcast
///   to known subsystems.
///
///   \param arg Pointer to the data link to use within the method.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::DefaultDataLink::RecvThread(void *arg)
{
    Communicator::DefaultDataLink *link = (Communicator::DefaultDataLink*)(arg);
    unsigned int count = 0;
    std::string source;
    std::map<Byte, CxUtils::UdpClient>::iterator connection;
    std::map<Byte, unsigned int>::iterator timestampMs;

    while(link->mRecvThread.QuitThreadFlag() == false)
    {
        // Only receive when On
        if(link->mLinkState == On)
        {
            link->mRecvStream.Clear();
            // Receive UDP data
            if(link->mServer.Recv(link->mRecvStream, 2*(gNetworkHeader.size() + JAUS_MAX_PACKET_SIZE), 100, &source))
            {
                // Check for valid transport header and then strip it.  By default, UDP
                // traffic adds a "JAUS0.01" string header to the front of all JAUS messages.
                if( link->mRecvStream.Length() >= gNetworkHeader.size() &&
                    strncmp( (const char *)link->mRecvStream.Ptr(),
                             gNetworkHeader.c_str(),
                             gNetworkHeader.size()) == 0)
                {
                    // Delete the header
                    link->mRecvStream.Delete(gNetworkHeader.size(), 0);
                    link->mRecvStream.SetReadPos(0);

                    Header header;
                    // Read the header data to check for validity, and
                    // to detect/create connections to newly discovered subsystems.
                    if(link->mRecvStream.Read(header))
                    {
#ifdef _JAUS_DEBUG
                        cout << "Received Message From: " << header.mSourceID.ToString() << endl;
#endif
                        link->mConnectionsMutex.Enter();
                        connection = link->mSubsystems.find(header.mSourceID.mSubsystem);
                        if(connection == link->mSubsystems.end())
                        {
                            // Create a new connection.
                            link->mSubsystems[header.mSourceID.mSubsystem].InitializeSocket(source.c_str(), gNetworkPort);
#ifdef _JAUS_DEBUG
                            cout << "Created Connection To " << header.mSourceID.ToString() << ", at: " << source << endl;
#endif
                        }
                        // If this is a hearbeat message, use it to update timestamps so we
                        // can maintain UDP unicast client connections (to reduce bandwidth).
                        if(header.mCommandCode == JAUS_REPORT_HEARTBEAT_PULSE &&
                            header.mDestinationID.mSubsystem == 255 && header.mDestinationID.mNode == 255)
                        {
                            link->mSubsystemHeartbeatTimesMs[header.mSourceID.mSubsystem] = Time::GetUtcTimeMs();
                        }
                        link->mConnectionsMutex.Leave();

                        link->mRecvStream.SetReadPos(0);
                        // Process the data.
                        link->ProcessReceivedMessage(link->mRecvStream);
                    }
                }
            }
        }

        if(++count == 50)
        {
            // Delete old client connections.
            link->mConnectionsMutex.Enter();
            timestampMs = link->mSubsystemHeartbeatTimesMs.begin();
            while(timestampMs != link->mSubsystemHeartbeatTimesMs.end())
            {
                if(Time::GetUtcTimeMs() - timestampMs->second > 2200)
                {
                    connection = link->mSubsystems.find(timestampMs->first);
                    if(connection != link->mSubsystems.end())
                    {
                        link->mSubsystems.erase(connection);
#ifdef _JAUS_DEBUG
                        cout << "Removed Connection To Subsystem: " << (int)timestampMs->first << endl;
#endif
                    }
#ifdef WIN32
                    timestampMs = link->mSubsystemHeartbeatTimesMs.erase(timestampMs);
#else
                    timestampMs = link->mSubsystemHeartbeatTimesMs.begin();
#endif
                    continue;
                }
                timestampMs++;
            }
            link->mConnectionsMutex.Leave();

            CxUtils::SleepMs(1);
            count = 0;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::Communicator() : mSelectedDataLink(0), mpNodeConnectionHandler(0)
{
    //mDataLinks[0] = new DefaultDataLink();
    //mDataLinks[0]->mpCommunicator = this;
    //mDataLinks[0]->SetState(DataLink::On);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::~Communicator()
{
    Shutdown();
    mNodeConnectionHandlerMutex.Enter();
    mpNodeConnectionHandler = NULL;
    mNodeConnectionHandlerMutex.Leave();
    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();
    for(dl = mDataLinks.begin(); dl != mDataLinks.end(); dl++)
    {
        if(dl->second)
        {
            dl->second->SetState(DataLink::Off);
            delete dl->second;
        }
    }
    mDataLinks.clear();
    mDataLinksMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the Communicator Service to the list of services
///   for this component.  This will tell other components what messages are
///   supported by the Communicator component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetupService()
{
    Service communicator;
    // Service type provided by component.
    communicator.SetType((Byte)Service::Communicator);
    // Input messages
    communicator.AddInputMessage(JAUS_SET_DATA_LINK_SELECT, 0);
    communicator.AddInputMessage(JAUS_SET_DATA_LINK_STATE, 0);
    communicator.AddInputMessage(JAUS_SET_SELECTED_DATA_LINK_STATE, 0);
    communicator.AddInputMessage(JAUS_QUERY_DATA_LINK_STATUS, 0);
    communicator.AddInputMessage(JAUS_QUERY_HEARTBEAT_PULSE, 0);
    communicator.AddInputMessage(JAUS_QUERY_SELECTED_DATA_LINK_STATUS, 0);
    communicator.AddInputMessage(JAUS_QUERY_SUBSYSTEM_LIST, 0);
    communicator.AddInputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    communicator.AddInputMessage(JAUS_QUERY_CONFIGURATION, 0);
    // Output messages
    communicator.AddOutputMessage(JAUS_REPORT_DATA_LINK_STATUS, 0);
    communicator.AddOutputMessage(JAUS_REPORT_SELECTED_DATA_LINK_STATUS, 0);
    communicator.AddOutputMessage(JAUS_REPORT_HEARTBEAT_PULSE, 0);
    communicator.AddOutputMessage(JAUS_REPORT_SUBSYSTEM_LIST, 0);
    communicator.AddOutputMessage(JAUS_REPORT_IDENTIFICATION, 0);
    communicator.AddOutputMessage(JAUS_REPORT_CONFIGURATION, 0);

    return AddService(communicator);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a data link to the Communicator for communication with other
///   Subsystems.
///
///   The Communicator will take ownership of the pointer and delete the link
///   when the communicator is destroyed or by using the DeleteDataLink method.
///
///   If a link already exists with the same ID as newLink then this method will
///   fail.
///
///   This method does not make this link the current selected data link if
///   another already exists (only if this is the first link added).
///
///   \param newLink Pointer to data link for communicator to use.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::AddDataLink(DataLink* newLink)
{
    int result = JAUS_FAILURE;
    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();
    // Check for conflicting data link ID
    dl = mDataLinks.find(newLink->GetID());
    if(dl == mDataLinks.end())
    {
        mDataLinks[newLink->GetID()] = newLink;
        newLink->mpCommunicator = this;
        if(mDataLinks.size() == 1)
        {
            mSelectedDataLink = newLink->GetID();
        }
        result = JAUS_OK;
    }
    else
    {
        SetJausError(ErrorCodes::AddressConflict);
    }
    mDataLinksMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes the data link.  This method will not set a new selected
///   data link if the one being deleted was the selected link.
///
///   \param dataLinkID ID of the data link to delete.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::DeleteDataLink(const Byte dataLinkID)
{
    int result = JAUS_FAILURE;

    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();

    dl = mDataLinks.find(dataLinkID);
    if(dl != mDataLinks.end())
    {
        delete dl->second;
        mDataLinks.erase(dl);
        result = JAUS_OK;
    }
    else
    {
        SetJausError(ErrorCodes::InvalidValue);
    }
    mDataLinksMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Selects the desired data link to an alternate communication device
///   if more than one exists.  This data link shall be used until another
///   one is selected.
///
///   \param dataLinkID ID of the data link to select.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetDataLinkSelect(const Byte dataLinkID)
{
    int result = JAUS_FAILURE;

    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();

    dl = mDataLinks.find(dataLinkID);
    if(dl != mDataLinks.end())
    {
        mSelectedDataLink = dataLinkID;
        result = JAUS_OK;
    }
    else
    {
        SetJausError(ErrorCodes::InvalidValue);
    }
    mDataLinksMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Selects the state of the current data link.
///
///   \param state Desired state of the data link.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetDataLinkState(const DataLink::State state)
{
    int result = JAUS_FAILURE;

    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();

    dl = mDataLinks.find(mSelectedDataLink);
    if(dl != mDataLinks.end())
    {
        dl->second->SetState(state);
        result = JAUS_OK;
    }
    else
    {
        SetJausError(ErrorCodes::InvalidValue);
    }
    mDataLinksMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Selects the state of the selected data link.
///
///   \param dataLinkID ID of the data link to modify state of.
///   \param state Desired state of the data link.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetSelectedDataLinkState(const Byte dataLinkID, const DataLink::State state)
{
    int result = JAUS_FAILURE;

    DataLink::Map::iterator dl;
    mDataLinksMutex.Enter();

    dl = mDataLinks.find(dataLinkID);
    if(dl != mDataLinks.end())
    {
        dl->second->SetState(state);
        result = JAUS_OK;
    }
    else
    {
        SetJausError(ErrorCodes::InvalidValue);
    }
    mDataLinksMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Transmits a message using the selected data link.
///
///   The transmit method will only send messages that originate from within
///   the same subsystem as the Communicator over the data link.
///   It will also only tranmist messages that are going to subsystems that are
///   different than the Communicators subsystem.  This prevents
///   reflecting back messages received which results in an infinite loop.
///
///   \param msg The serialized JAUS message to send on current data link.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::Transmit(const Stream& msg) const
{
    int result = JAUS_FAILURE;
    Header header;

    if(msg.Read(header, 0))
    {
        // Only transmits messages that
        // originate from the same subsystem as the Communicator.
        if(header.mSourceID.mSubsystem != GetID().mSubsystem)
        {
            return result;
        }
        // Only send on data link if destination not on our subsystem.
        if(header.mDestinationID.mSubsystem == 255 || header.mDestinationID.mSubsystem != GetID().mSubsystem)
        {
            DataLink::Map::const_iterator dl;
            mDataLinksMutex.Enter();
            dl = mDataLinks.find(mSelectedDataLink);
            if(dl != mDataLinks.end() &&
                dl->second->GetState() == DataLink::On)
            {
                if( ((DataLink*)(dl->second))->Transmit(msg))
                {
                    result = JAUS_OK;
                }
                else
                {
                    // Try find another link that is not
                    // in a failed state.
                    for(dl = mDataLinks.begin();
                        dl != mDataLinks.end() && result == JAUS_FAILURE;
                        dl++)
                    {
                        if(dl->first != mSelectedDataLink)
                        {
                            if(dl->second->GetState() == DataLink::On)
                            {
                                if( ((DataLink*)(dl->second))->Transmit(msg))
                                {
                                    result = JAUS_OK;
                                }
                            }
                        }
                    }
                }
            }
            mDataLinksMutex.Leave();
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is used to check and see if the current selected
///   data link is of type Communicator::DefaultDataLink, and if the
///   multicast address it is using is equal to the argument passed.
///
///   This method is used by the Node Manager (NM)  during dynamic discover
///   procedures to prevent sending the same message over UDP multicast twice.
///   For example, for node discovery, the NM multicasts or broadcasts over
///   UDP, but if Subsystem Discover is enabled (sending heartbeat messages to
///   255.255.1.1) then the data must also be transmitted by the Communicator.
///   Therefore, if the Communicator is also using multicast/broadcast, then the
///   heartbeat message will get transmitted twice.  This method allows the
///   NM program to prevent this during discover procedures.
///
///   \param multicast If a DefaultDataLink is being used, this parameter checks
///                    what multicast address it is sending to.
///
///   \return True if a Communicator::DefaultDataLink is selected using the
///           multicast group address passed, false otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::IsDefaultDataLinkSelected(const std::string& multicast) const
{
    bool result = false;
    DataLink::Map::const_iterator dl;
    const DefaultDataLink* defaultDataLink;
    mDataLinksMutex.Enter();
    dl = mDataLinks.find(mSelectedDataLink);
    if(dl != mDataLinks.end())
    {
        defaultDataLink = dynamic_cast<const DefaultDataLink*>(dl->second);
        if(defaultDataLink && defaultDataLink->GetMulticastAddress() == multicast)
        {
            result = true;
        }
    }
    mDataLinksMutex.Leave();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes command messages received by the communicator.
///
///   \param msg The message to process.
///   \param commandAuthority The command authority level of the message sender.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::ProcessCommandMessage(const Message* msg, Byte commandAuthority)
{
    if(commandAuthority >= GetComponentAuthority())
    {
        int result = JAUS_OK;
        switch(msg->GetCommandCode())
        {
        case JAUS_SET_DATA_LINK_SELECT:
            {
                const Jaus::SetDataLinkSelect* command = dynamic_cast<const Jaus::SetDataLinkSelect*>(msg);
                if(command)
                {
                    this->SetDataLinkSelect(command->GetDataLinkID());
                }
            }
            break;
        case JAUS_SET_DATA_LINK_STATE:
            {
                const Jaus::SetDataLinkState* command =
                    dynamic_cast<const Jaus::SetDataLinkState*>(msg);
                if(command)
                {
                    this->SetDataLinkState((DataLink::State)command->GetState());
                }
            }
            break;
        case JAUS_SET_SELECTED_DATA_LINK_STATE:
            {
                const Jaus::SetSelectedDataLinkState* command =
                    dynamic_cast<const Jaus::SetSelectedDataLinkState*>(msg);
                if(command)
                {
                    this->SetSelectedDataLinkState(command->GetDataLinkID(), (DataLink::State)command->GetState());
                }
            }
            break;
        default:
            result = Component::ProcessCommandMessage(msg, commandAuthority);
            break;
        }
        return result;
    }
    else
    {
        return Component::ProcessCommandMessage(msg, commandAuthority);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes query messages received by the communicator.
///
///   \param msg The message to process.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::ProcessQueryMessage(const Message* msg)
{
    int result = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_IDENTIFICATION:
        {
            const Jaus::QueryIdentification* query = dynamic_cast<const Jaus::QueryIdentification*>(msg);
            if(query)
            {
                if(query->GetQueryType() == Jaus::QueryIdentification::Subsystem)
                {
                    Jaus::ReportIdentification report;
                    report.SetDestinationID(query->GetSourceID());
                    report.SetSourceID(GetID());
                    report.SetIdentification(GetSubsystemIdentification());
                    report.SetQueryType(Jaus::ReportIdentification::Subsystem);
                    Send(&report);
                }
                else
                {
                    return Component::ProcessQueryMessage(query);
                }
            }
        }
        break;
    case JAUS_QUERY_CONFIGURATION:
        {
            const Jaus::QueryConfiguration* query = dynamic_cast<const Jaus::QueryConfiguration*>(msg);
            if(query)
            {
                Jaus::ReportConfiguration report;
                report.SetDestinationID(query->GetSourceID());
                report.SetSourceID(GetID());
                mConfigurationMutex.Enter();
                if(query->GetQueryField() == Jaus::QueryConfiguration::Node)
                {
                    report.SetConfiguration(GetID().mSubsystem, mSubsystemConfiguration.mNodes[GetID().mNode]);
                }
                else if(query->GetQueryField() == Jaus::QueryConfiguration::Subsystem)
                {
                    report.SetConfiguration(mSubsystemConfiguration);
                }
                mConfigurationMutex.Leave();
                Send(&report);
            }
        }
        break;
    case JAUS_QUERY_DATA_LINK_STATUS:
        {
            const Jaus::QueryDataLinkStatus* query = dynamic_cast<const Jaus::QueryDataLinkStatus*>(msg);
            if(query)
            {
                Jaus::ReportDataLinkStatus report;

                report.SetDestinationID(query->GetSourceID());
                report.SetSourceID(GetID());

                DataLink::Map::iterator dl;
                mDataLinksMutex.Enter();
                dl = mDataLinks.find(query->GetDataLinkID());
                if(dl != mDataLinks.end())
                {
                    report.SetDataLinkID(dl->first);
                    report.SetDataLinkState((Byte)dl->second->GetState());
                    Send(&report);
                }
                mDataLinksMutex.Leave();
            }
        }
        break;
    case JAUS_QUERY_SELECTED_DATA_LINK_STATUS:
        {
            const Jaus::QuerySelectedDataLinkStatus* query = dynamic_cast<const Jaus::QuerySelectedDataLinkStatus*>(msg);
            if(query)
            {
                Jaus::ReportDataLinkStatus report;

                report.SetDestinationID(query->GetSourceID());
                report.SetSourceID(GetID());

                DataLink::Map::iterator dl;
                mDataLinksMutex.Enter();
                dl = mDataLinks.find(mSelectedDataLink);
                if(dl != mDataLinks.end())
                {
                    report.SetDataLinkID(dl->first);
                    report.SetDataLinkState((Byte)dl->second->GetState());
                    Send(&report);
                }
                mDataLinksMutex.Leave();
            }
        }
        break;
    case JAUS_QUERY_HEARTBEAT_PULSE:
        {
            ReportHeartbeatPulse report;
            report.SetSourceID(GetID());
            report.SetDestinationID(msg->GetSourceID());
            Send(&report);
        }
        break;
    default:
        result = Component::ProcessQueryMessage(msg);
        break;
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief  Method called by a DataLink to have the Communicator route
///   the message received on an active data link connection.  This method
///   will pass to the Node Manager for routing via the Node Connection Handler.
///
///   \param data Received JAUS message to process/route.
///
///   \return True on success, otherwise failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::ProcessDataLinkMessage(const Stream &data)
{
    data.SetReadPos(0);
    if(mpNodeConnectionHandler)
    {
        mNodeConnectionHandlerMutex.Enter();
        mpNodeConnectionHandler->ProcessStreamCallback(data, NULL, StreamCallback::Communicator);
        mNodeConnectionHandlerMutex.Leave();
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief  Sets the pointer to the Node Connection Handler to use for
///   routing messages received on the data link by the Communicator.
///
///   This is already called by the Node Manager when it initializes, and is
///   not necessary to use unless you are making a duplicate communicator on the
///   node.
///
///   \return True on success, otherwise failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool Communicator::SetNodeConnectionHandlerCallback(StreamCallback* handler)
{
    if(handler)
    {
        mNodeConnectionHandlerMutex.Enter();
        mpNodeConnectionHandler = handler;
        mNodeConnectionHandlerMutex.Leave();
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Locks the mutex protecting the data links used by the communicator.
///
///   This should only be used when accesing the data links directly with the
///   GetDataLinks() method.  Don't forget to Unlock when done.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::LockDataLinks() const
{
    mDataLinksMutex.Enter();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to the map of all data links used by the
///   Communicator.  This should ONLY be used in conjunction with the
///   LockDataLinks and UnlockDataLinks methods for thread safety.
///
///   \return Pointer to data links.
///
////////////////////////////////////////////////////////////////////////////////////
Communicator::DataLink::Map* Communicator::GetDataLinks()
{
    return &mDataLinks;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Unlocks the mutex protecting the data links used by the communicator.
///
///   This should only be used when accesing the data links directly with the
///   GetDataLinks() method.
///
////////////////////////////////////////////////////////////////////////////////////
void Communicator::UnlockDataLinks() const
{
    mDataLinksMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return a Vector containing the ID information of all available data links.
///
////////////////////////////////////////////////////////////////////////////////////
std::vector<Byte> Communicator::GetDataLinkList() const
{
    std::vector<Byte> ids;
    DataLink::Map::const_iterator dl;
    mDataLinksMutex.Enter();
    for(dl = mDataLinks.begin();
        dl != mDataLinks.end();
        dl++)
    {
        ids.push_back(dl->first);
    }
    mDataLinksMutex.Leave();
    return ids;

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets subsystem configuration data.
///
///   \return Returns known subsystem configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem Communicator::GetSubsystemConfiguration() const
{
    Configuration::Subsystem config;
    mConfigurationMutex.Enter();
    config = mSubsystemConfiguration;
    mConfigurationMutex.Leave();
    return config;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets subsystem configuration data.
///
///   If the communicator is initialized, the configuration data passed is not
///   for the communicators subsystem, this method will return failure.
///
///   \param config Configuration of the Communicators subsystem.
///
///   \return Returns JAUS_OK on succes, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetSubsystemConfiguration(const Configuration::Subsystem& config)
{
    if(IsInitialized() &&
        config.mSubsystemID != GetID().mSubsystem)
    {
        return JAUS_FAILURE;
    }

    mConfigurationMutex.Enter();
    mSubsystemConfiguration = config;
    if(IsInitialized())
    {
        mSubsystemConfiguration.AddComponent(GetID());
    }
    mConfigurationMutex.Leave();
    return  OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the subsystems identification.
///
///   \return Returns known subsystem identification data.
///
////////////////////////////////////////////////////////////////////////////////////
Identification Communicator::GetSubsystemIdentification() const
{
    Identification identity;
    mConfigurationMutex.Enter();
    identity = mSubsystemIdentification;
    mConfigurationMutex.Leave();
    return identity;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets subsystem identification data.
///
///   \param identity Subsystem identification.
///
///   \return Returns JAUS_OK on succes, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Communicator::SetSubsystemIdentification(const Identification& identity)
{
    mConfigurationMutex.Enter();
    mSubsystemIdentification = identity;
    mConfigurationMutex.Leave();
    return  OK;
}

/*  End of File */
