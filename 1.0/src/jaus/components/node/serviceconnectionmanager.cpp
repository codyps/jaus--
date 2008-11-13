////////////////////////////////////////////////////////////////////////////////////
///
///  \file serviceconnectionmanager.cpp
///  \brief The service connection manager is used to route and manage all
///  service connection messages for the Node Manager.
///
///  Author(s): Daniel Barber
///  Created: 20 November 2007
///  Last Modified: 9 March 2008
///  Copyright (c) 2007
///  Applied Cognition and Training in Immersive Virtual Environments
///  (ACTIVE) Laboratory
///  Institute for Simulation and Training (IST)
///  University of Central Florida (UCF)
///  Email: dbarber@ist.ucf.edu
///  Web:  http://active.ist.ucf.edu
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
#include "jaus/components/node/serviceconnectionmanager.h"
#include "jaus/components/node/nodeconnectionhandler.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/query/querymessages.h"
#include "jaus/messages/inform/informmessages.h"
#include "jaus/messages/experimental/experimentalmessages.h"
#include "jaus/messages/inform/acknackmessage.h"
#include "jaus/messages/messagecreator.h"

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnectionManager::StreamPipeline::StreamPipeline()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, deletes any messages in pipeline.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnectionManager::StreamPipeline::~StreamPipeline()
{
    Destroy();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any messages in pipeline.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::StreamPipeline::Destroy()
{
    ServiceConnectionManager::StreamQueue::iterator str;
    ServiceConnectionManager::HeaderQueue::iterator hdr;

    for(str = mStreams.begin();
        str != mStreams.end();
        str++)
    {
        delete (*str);
    }
    for(hdr = mHeaders.begin();
        hdr != mHeaders.end();
        hdr++)
    {
        delete (*hdr);
    }

    mStreams.clear();
    mHeaders.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnectionManager::ServiceConnectionManager() : mInitializedFlag(false),
                                                       mpCurrentMessage(0),
                                                       mpCurrentMessageHeader(0)

{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnectionManager::~ServiceConnectionManager()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the SC management capabilities.
///
///   \param sid The subsystem ID of node.
///   \param nid The node ID number.
///   \param handler Pointer to connection handler for sending messages.
///
///   \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnectionManager::Initialize(const Byte sid,
                                         const Byte nid,
                                         NodeConnectionHandler* handler)
{
    Shutdown();
    mNodeID(sid, nid, 1, 1);
    if(mNodeID.IsValid() && handler)
    {
        mServiceConnectionsMutex.Enter();
        mpConnectionHandler = handler;
        mServiceConnectionsMutex.Leave();
        mInitializedFlag = true;
        return JAUS_OK;
    }
    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutdown SC management.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnectionManager::Shutdown()
{
    mInitializedFlag = false;
    mServiceConnectionManagerThread.StopThread();

    StopServiceConnections();

    mStreamPipeline.Destroy();
    if(mpCurrentMessage)
    {
        delete mpCurrentMessage;
        mpCurrentMessage = NULL;
    }
    if(mpCurrentMessageHeader)
    {
        delete mpCurrentMessageHeader;
        mpCurrentMessageHeader = NULL;
    }

    mInformSC.clear();
    mCommandSC.clear();
    mNodeID(0, 0, 0, 0);
    mServiceConnectionsMutex.Enter();
    mpConnectionHandler = NULL;
    mServiceConnectionsMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Places an SC flaged or SC setup message into queue for additional
///   processing.
///
///   \param msg The SC message to route.
///   \param header The header for the message.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnectionManager::RouteServiceConnectionMessage(const Stream& msg, const Header& header)
{
    if(mInitializedFlag)
    {
        // Only create thread if needed to save system resources.
        if(!mServiceConnectionManagerThread.IsThreadActive())
        {
            if(mServiceConnectionManagerThread.CreateThread(ServiceConnectionManager::ServiceConnectionManagerThread, this) == 0)
            {
                return JAUS_FAILURE;
            }
            mServiceConnectionManagerThread.SetThreadPriority(50);
        }

        // Add message and header to queue.
        mPipelineMutex.Enter();
        mStreamPipeline.mHeaders.push_back(new Header(header));
        mStreamPipeline.mStreams.push_back(new Stream(msg));
        mPipelineMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Notifies the SC manager of component and node disconnects.
///
///   This method checks if a service provider has disconnected and automatically
///   sends Terminate Service Connection messaes to subscribers or commanders.
///   If the ID passed is for a node or component that is a subscriber or
///   commander, they are removed from SC management, and if necessary the
///   service provider is notified with a Terminate message also.
///
///   \param id The component or node that disconnected.
///   \param type Type of connection event
///               (see NodeConnectionHandler::ConnectionEvents).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnectionManager::ProcessConnectionEvent(const Address& id, const Byte type)
{
    if(type == NodeConnectionHandler::NodeDisconnect ||
       type == NodeConnectionHandler::ComponentDisconnect &&
       mInitializedFlag && mpConnectionHandler)
    {

        Jaus::TerminateServiceConnection terminate;
        terminate.SetSourceID(mNodeID);

        // See if we are managing any service connections for this ID or node, and
        // if so send any Terminate messages as needed.
        mServiceConnectionsMutex.Enter();

        ServiceConnection::InformMap::iterator inform;

        inform = mInformSC.begin();
        while(inform != mInformSC.end())
        {
            terminate.SetMessageCode(inform->first.mMessageCode);
            terminate.SetInstanceID(inform->second.mInstanceID);

            // If the disconnecting component is a provider of
            // the service, or the providing component was on a node
            // we lost connection with, tell all subscribers to
            // terminate.
            if(id == inform->second.mProviderID ||
                (id.mComponent == 1 &&
                id.mSubsystem == inform->second.mProviderID.mSubsystem &&
                id.mNode == inform->second.mProviderID.mNode))
            {
                Address::Set::iterator subscriber;

                for(subscriber = inform->second.mSubscribers.begin();
                    subscriber != inform->second.mSubscribers.end();
                    subscriber++)
                {
                    terminate.SetDestinationID(*subscriber);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }

                for(subscriber = inform->second.mSuspended.begin();
                    subscriber != inform->second.mSuspended.end();
                    subscriber++)
                {
                    terminate.SetDestinationID(*subscriber);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }

                for(subscriber = inform->second.mPending.begin();
                    subscriber != inform->second.mPending.end();
                    subscriber++)
                {
                    terminate.SetDestinationID(*subscriber);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }
                // Delete SC.
                mInformSC.erase(inform);
                inform = mInformSC.begin();
                /*
#ifdef WIN32
                inform = mInformSC.erase(inform);
#else
                mInformSC.erase(inform);
#endif
                */
                continue;
            }

            // If the id is a subscriber, then we need to
            // remove them.
            if(inform->second.RemoveSubscriber(id) ||
                (id.mComponent == 1 && inform->second.RemoveSubscribersFromNode(id)))
            {
                // If there are no more subscribers, remove
                // the service connection from our list of
                // informs, and tell provider to terminate.
                if(inform->second.GetNumSubscribers() == 0)
                {
                    terminate.SetDestinationID(inform->first.mProviderID);
                    mpConnectionHandler->Send(&terminate);
                    mInformSC.erase(inform);
                    inform = mInformSC.begin();
                    /*
#ifdef WIN32
                    inform = mInformSC.erase(inform);
#else
                    mInformSC.erase(inform);
#endif
                    */
                    continue;
                }
            }

            inform++;
        }

        ServiceConnection::CommandMap::iterator command;

        command = mCommandSC.begin();
        while(command != mCommandSC.end())
        {
            terminate.SetMessageCode(command->first.mMessageCode);
            terminate.SetInstanceID(command->second.mInstanceID);

            // If the commanded component has disconnected, then
            // tell all commanders to terminate their service connections.
            if(command->second.mProviderID == id)
            {
                ServiceConnection::Command::CommanderSet::iterator commander;

                for(commander = command->second.mSuspended.begin();
                    commander != command->second.mSuspended.end();
                    commander++)
                {
                    terminate.SetDestinationID(commander->mID);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }

                for(commander = command->second.mPending.begin();
                    commander != command->second.mPending.end();
                    commander++)
                {
                    terminate.SetDestinationID(commander->mID);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }

                if(command->second.mCommander.mID.IsValid())
                {
                    terminate.SetDestinationID(command->second.mCommander.mID);
                    // Now send the message.
                    mpConnectionHandler->Send(&terminate);
                }

                // Delete the SC
                mCommandSC.erase(command);
                command = mCommandSC.begin();
                /*
#ifdef WIN32
                command = mCommandSC.erase(command);
#else
                mCommandSC.erase(command);
#endif
                */
                continue;
            }
            else if(command->second.RemoveCommander(id) ||
                     (id.mComponent == 1 && command->second.RemoveCommandersFromNode(id)))
            {
                if(command->second.GetNumCommanders() == 0)
                {
                    // If no command components let, tell the provider
                    // of the service that this SC is done.
                    terminate.SetDestinationID(command->first.mProviderID);
                    mpConnectionHandler->Send(&terminate);

                    mCommandSC.erase(command);
                    command = mCommandSC.begin();
                    /*
#ifdef WIN32
                    command = mCommandSC.erase(command);
#else
                    mCommandSC.erase(command);
#endif
                    */
                    continue;
                }
            }
            command++;
        }

        mServiceConnectionsMutex.Leave();
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if initialized, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnectionManager::IsInitialized() const { return mInitializedFlag; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the authority level of a component.
///
///   \param id The ID of the component to query for authority.
///   \param authority Authority level found.
///
///   \return True if authority level retrieved, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnectionManager::QueryComponentAuthority(const Address& id, Byte& authority)
{
    if(mpConnectionHandler)
    {
        Jaus::QueryComponentAuthority query;
        Receipt rcpt;
        query.SetSourceID(mNodeID);
        query.SetDestinationID(id);
        if(mpConnectionHandler->Send(&query, rcpt, JAUS_REPORT_COMPONENT_AUTHORITY) == JAUS_OK)
        {
            authority = ((ReportComponentAuthority*)(rcpt.GetResponseMessage()))->GetAuthorityCode();
            return true;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for creating a service connection.
///
///   \param msg The Create Service Connection message to process.
///   \param header Header data for message.
///   \param command The Create Service Connection message to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::CreateInformServiceConnection(const Stream& msg,
                                                             const Header& header,
                                                             Jaus::CreateServiceConnection& command)
{
    ServiceConnection::InformMap::iterator informSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();
    key.mPresenceVector = command.GetPresenceVector();
    informSC = mInformSC.find(key);
    if( informSC == mInformSC.end() )
    {
        mpConnectionHandler->Send(msg);
        // Creates new Inform SC data, and adds the
        // creator of the SC to list of those pending
        // confirmation for the SC.
        mInformSC[key] = ServiceConnection::Inform(key.mMessageCode,
                                                   key.mPresenceVector,
                                                   command.GetDestinationID(),
                                                   command.GetSourceID());
        mInformSC[key].AddPendingSubscriber(command.GetSourceID());
    }
    else
    {
        // Adding a subscriber pending confirmation.
        informSC->second.AddPendingSubscriber(command.GetSourceID());

        if( command.GetPeriodicUpdateRate() > informSC->second.mPeriodicRate )
        {
            mpConnectionHandler->Send(msg);
        }
        else
        {
            // If the periodic rate is less than or equal to the existing
            // periodic rate, send confirmation automatically.
            Jaus::ConfirmServiceConnection confirm;
            confirm.SetSourceID(header.mDestinationID);
            confirm.SetDestinationID(header.mSourceID);
            confirm.SetInstanceID(informSC->second.mInstanceID);
            confirm.SetResponseCode(Jaus::ConfirmServiceConnection::CreatedSuccessfully);
            confirm.SetConfirmedPeriodicUpdateRate(informSC->second.mPeriodicRate);
            mpConnectionHandler->Send(&confirm);
            // Activate the subscriber
            informSC->second.ActivateSubscriber(header.mSourceID);
        }// If request is within existing periodic rates.
    } // If need to create SC or have existing one.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for creating a service connection.
///
///   \param msg The Create Service Connection message to process.
///   \param header Header data for message.
///   \param command The Create Service Connection message to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::CreateCommandServiceConnection(const Stream& msg,
                                                              const Header& header,
                                                              Jaus::CreateServiceConnection& command)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();
    key.mPresenceVector = command.GetPresenceVector();

    Byte commanderAuthorityLevel = 0;
    Byte componentAuthorityLevel = 0;
    Receipt receipt;

    // If I got the response I needed, add to pending list
    if(QueryComponentAuthority(command.GetSourceID(), commanderAuthorityLevel) &&
        QueryComponentAuthority(command.GetDestinationID(), componentAuthorityLevel))
    {
        commandSC = mCommandSC.find(key);
        if(commandSC == mCommandSC.end())
        {
            // Create a new Command SC data
            mCommandSC[key] = ServiceConnection::Command(key.mMessageCode,
                                                         key.mPresenceVector,
                                                         key.mProviderID);
            mCommandSC[key].AddPendingCommander(ServiceConnection::Command::Commander(header.mSourceID, commanderAuthorityLevel));
            // Update the component authority level.
            mCommandSC[key].mProviderAuthorityCode = componentAuthorityLevel;
            mpConnectionHandler->Send(msg);
        } // If new SC is needed
        else
        {
            // Update component authority level.
            commandSC->second.mProviderAuthorityCode = componentAuthorityLevel;

            if(commanderAuthorityLevel >= componentAuthorityLevel)
            {
                // Add to pending confirmation.
                commandSC->second.AddPendingCommander(ServiceConnection::Command::Commander(header.mSourceID, commanderAuthorityLevel));
                // If a higher periodic rate is desired, we have to let the component decide
                // whether or not to confirm it.
                if(command.GetPeriodicUpdateRate() > commandSC->second.mPeriodicRate)
                {
                    // Send create message to service provider.
                    mpConnectionHandler->Send(msg);
                }
                else
                {
                    // If the periodic rate is less than or equal to the existing
                    // periodic rate, send confirmation automatically.
                    Jaus::ConfirmServiceConnection confirm;
                    confirm.SetSourceID(header.mDestinationID);
                    confirm.SetDestinationID(header.mSourceID);
                    confirm.SetInstanceID(commandSC->second.mInstanceID);
                    confirm.SetResponseCode(Jaus::ConfirmServiceConnection::CreatedSuccessfully);
                    confirm.SetConfirmedPeriodicUpdateRate(commandSC->second.mPeriodicRate);
                    mpConnectionHandler->Send(&confirm);
                    // Activate the subscriber, to remove them from pending.
                    commandSC->second.ActivateCommander(header.mSourceID);

                    // Now that we have activated a new commander, we may have to
                    // suspend the previous one or activate a new one.
                    // Get the current commander.
                    ServiceConnection::Command::Commander prevCommander = commandSC->second.mCommander;
                    // Figure out who the new commander is.
                    commandSC->second.mCommander = commandSC->second.GetNextCommander();
                    // If the active commander has changed, activate the new commander.
                    if( prevCommander.mID != commandSC->second.mCommander.mID &&
                        commandSC->second.mCommander.mID.IsValid() )
                    {
                        // Suspend the previously active commander.
                        if(prevCommander.mID.IsValid())
                        {
                            Jaus::SuspendServiceConnection suspend;
                            suspend.SetSourceID(mNodeID);
                            suspend.SetDestinationID(prevCommander.mID);
                            suspend.SetInstanceID(commandSC->second.mInstanceID);
                            mpConnectionHandler->Send(&suspend);
                        }
                        // Activate the new commander
                        Jaus::ActivateServiceConnection activate;
                        activate.SetSourceID(mNodeID);
                        activate.SetDestinationID(commandSC->second.mCommander.mID);
                        activate.SetInstanceID(commandSC->second.mInstanceID);
                        mpConnectionHandler->Send(&activate);
                    } // If the lead commander has changed
                } // If we can automatically confirm for the component, do so.
            }
            else
            {
                ConfirmServiceConnection reject;
                reject.SetSourceID(command.GetDestinationID());
                reject.SetDestinationID(command.GetSourceID());
                reject.SetResponseCode(ConfirmServiceConnection::Refused);
                mpConnectionHandler->Send(&reject);
            }
        }// If an exisiting SC was found
    } // If we were able to query the authority level of the requesting component.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Confirm Service Connection.
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Confirm Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::ConfirmInformServiceConnection(const Stream& msg,
                                                              const Header& header,
                                                              Jaus::ConfirmServiceConnection& command)
{
    ServiceConnection::InformMap::iterator informSC;
    ServiceConnection::Key key;

    // Send the message.
    mpConnectionHandler->Send(msg);

    // Find the SC
    for(informSC = mInformSC.begin();
        informSC != mInformSC.end();
        informSC++)
    {
        // If the provider matches, and the type of message
        // included in the SC matches, and the destination ID of the
        // subscriber is pending on confirmation for this SC, then this
        // is the correct service connection.
        if( informSC->first.mProviderID == header.mSourceID &&
            informSC->first.mMessageCode == command.GetMessageCode() &&
            informSC->second.IsPending( command.GetDestinationID() ) )
        {
            informSC->second.mInstanceID = command.GetInstanceID();

            if( command.GetResponseCode() == Jaus::ConfirmServiceConnection::CreatedSuccessfully )
            {
                informSC->second.mInstanceID = command.GetInstanceID();
                if(command.GetConfirmedPeriodicUpdateRate() > informSC->second.mPeriodicRate)
                {
                    informSC->second.mPeriodicRate = command.GetConfirmedPeriodicUpdateRate();
                }
                informSC->second.mInstanceID = command.GetInstanceID();
                bool wasSuspended = informSC->second.IsServiceSuspended();
                // Add to active subscribers.
                informSC->second.mSubscribers.insert(command.GetDestinationID());
                // Remove from pending
                informSC->second.RemovePendingSubscriber(command.GetDestinationID());
                // If the SC was suspended, but no longer is because a new
                // connection was made, then activate it
                if(wasSuspended && informSC->second.IsServiceSuspended() == false)
                {
                    Jaus::ActivateServiceConnection activate;
                    activate.SetSourceID(mNodeID);
                    activate.SetDestinationID(command.GetSourceID());
                    activate.SetInstanceID(informSC->second.mInstanceID);
                    activate.SetMessageCode(informSC->second.mMessageCode);
                    mpConnectionHandler->Send(&activate);
                }
            }
            else
            {
                // Failed to create, remove.
                informSC->second.RemoveSubscriber( command.GetDestinationID() );
                // If there are no subscribers, and nobody is pending, then
                // remove this from SC management.
                if( informSC->second.GetNumSubscribers() == 0 &&
                    !informSC->second.IsPending() )
                {
                    mInformSC.erase(informSC);
                    informSC = mInformSC.begin();
                    /*
#ifdef WIN32
                    informSC = mInformSC.erase(informSC);
#else
                    mInformSC.erase(informSC);
#endif
                    */
                }
            }
            break;
        }
    } // Find an SC being monitored
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Confirm Service Connection.
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Confirm Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::ConfirmCommandServiceConnection(const Stream& msg,
                                                               const Header& header,
                                                               Jaus::ConfirmServiceConnection& command)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    // Send the message.
    mpConnectionHandler->Send(msg);

    // Find the SC
    for(commandSC = mCommandSC.begin();
        commandSC != mCommandSC.end();
        commandSC++)
    {
        if( commandSC->first.mProviderID == header.mSourceID &&
            commandSC->first.mMessageCode == command.GetMessageCode() &&
            commandSC->second.IsPending( command.GetDestinationID() ) )
        {
            // On successful creation, suspend/activate as needed and update
            // SC management data
            if(command.GetResponseCode() == Jaus::ConfirmServiceConnection::CreatedSuccessfully)
            {
                // Commander was confirmed.
                ServiceConnection::Command::Commander prevCommander;

                commandSC->second.mInstanceID = command.GetInstanceID();
                if(command.GetConfirmedPeriodicUpdateRate() > commandSC->second.mPeriodicRate)
                {
                    commandSC->second.mPeriodicRate = command.GetConfirmedPeriodicUpdateRate();
                }

                prevCommander = commandSC->second.mCommander;
                // Add to active commanders list
                commandSC->second.ActivateCommander(command.GetDestinationID());
                commandSC->second.mCommander = commandSC->second.GetNextCommander();
                // If the commander has changed, we must suspend the
                // previous commander, and activate the new one.
                if(commandSC->second.mCommander.mID != prevCommander.mID)
                {
                    Jaus::SuspendServiceConnection suspend;
                    Jaus::ActivateServiceConnection activate;
                    Receipt receipt;
                    suspend.SetSourceID(mNodeID);
                    activate.SetSourceID(mNodeID);

                    // If the ID is not valid, then there was no previous commander
                    // activated for this SC.
                    if(prevCommander.mID.IsValid())
                    {
                        suspend.SetDestinationID(prevCommander.mID);
                        suspend.SetInstanceID(commandSC->second.mInstanceID);
                        suspend.SetAckNack(Header::AckNack::Request);
                        mpConnectionHandler->Send(&suspend, receipt, JAUS_SUSPEND_SERVICE_CONNECTION);
                    }
                    // Now activate the new commander
                    activate.SetDestinationID(commandSC->second.mCommander.mID);
                    activate.SetInstanceID(commandSC->second.mInstanceID);
                    activate.SetAckNack(Header::AckNack::Request);
                    mpConnectionHandler->Send(&activate, receipt, JAUS_ACTIVATE_SERVICE_CONNECTION);
                }
            }
            else
            {
                // Remove the commander if rejected
                commandSC->second.RemovePendingCommander(command.GetDestinationID());
                if(commandSC->second.GetNumCommanders() == 0 )
                {
                    mCommandSC.erase(commandSC);
                    commandSC = mCommandSC.begin();
                    /*
#ifdef WIN32
                    commandSC = mCommandSC.erase(commandSC);
#else
                    mCommandSC.erase(commandSC);
#endif
                    */
                }
            }
            break;
        }
    } // Find an SC being monitored.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Terminate Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Terminate Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::TerminateInformServiceConnection(const Stream& msg,
                                                                const Header& header,
                                                                Jaus::TerminateServiceConnection& command)
{
    ServiceConnection::InformMap::iterator informSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for( informSC = mInformSC.begin();
         informSC != mInformSC.end();
         informSC++ )
    {
        // Find the SC.
        if( informSC->first.mProviderID == key.mProviderID &&
            informSC->first.mMessageCode == key.mMessageCode &&
            informSC->second.mInstanceID == command.GetInstanceID() &&
            informSC->second.IsSubscriber( command.GetSourceID() ) )
        {
            informSC->second.RemoveSubscriber( command.GetSourceID() );
            if( informSC->second.GetNumSubscribers() == 0 )
            {
                // Tell component to terminate.
                command.SetSourceID( mNodeID );
                mpConnectionHandler->Send(&command);
                // Remove from managed SC's.
                mInformSC.erase(informSC);
                informSC = mInformSC.begin();
                /*
#ifdef WIN32
                informSC = mInformSC.erase(informSC);
#else
                mInformSC.erase(informSC);
#endif
                */
            }
            return;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Terminate Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Terminate Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::TerminateCommandServiceConnection(const Stream& msg,
                                                                 const Header& header,
                                                                 Jaus::TerminateServiceConnection& command)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for(commandSC = mCommandSC.begin();
        commandSC != mCommandSC.end();
        commandSC++)
    {
        // Find the matching SC.
        if( commandSC->first.mProviderID == key.mProviderID &&
            commandSC->first.mMessageCode == key.mMessageCode &&
            commandSC->second.mInstanceID == command.GetInstanceID() &&
            commandSC->second.IsCommander(command.GetSourceID()) )
        {
            // Erase the commander
            ServiceConnection::Command::Commander prevCommander = commandSC->second.mCommander;

            commandSC->second.RemoveCommander(command.GetSourceID());
            // Get the new commander.
            commandSC->second.mCommander = commandSC->second.GetNextCommander();

            // If the active commander has changed, activate the new commander.
            if( prevCommander.mID != commandSC->second.mCommander.mID &&
                commandSC->second.mCommander.mID.IsValid() )
            {
                Jaus::ActivateServiceConnection activate;
                activate.SetSourceID(mNodeID);
                activate.SetDestinationID(commandSC->second.mCommander.mID);
                activate.SetInstanceID(commandSC->second.mInstanceID);
                mpConnectionHandler->Send(&activate);
            }

            if( commandSC->second.GetNumCommanders() == 0 )
            {
                // Notifiy provider of termination
                command.SetSourceID(mNodeID);
                mpConnectionHandler->Send(&command);
                mCommandSC.erase(commandSC);
            }

            return;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Suspend Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Suspend Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::SuspendInformServiceConnection(const Stream& msg,
                                                              const Header& header,
                                                              Jaus::SuspendServiceConnection& command)
{
    ServiceConnection::InformMap::iterator informSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for( informSC = mInformSC.begin();
         informSC != mInformSC.end();
         informSC++ )
    {
        // Find the SC.
        if( informSC->first.mProviderID == key.mProviderID &&
            informSC->first.mMessageCode == key.mMessageCode &&
            informSC->second.mInstanceID == command.GetInstanceID() &&
            informSC->second.IsSubscriber( command.GetSourceID() ) )
        {
            informSC->second.SuspendSubscriber( command.GetSourceID() );
            if( informSC->second.IsServiceSuspended() )
            {
                // Tell component to suspend.
                command.SetSourceID( mNodeID );
                mpConnectionHandler->Send(&command);
            }
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Suspend Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Suspend Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::SuspendCommandServiceConnection(const Stream& msg,
                                                               const Header& header,
                                                               Jaus::SuspendServiceConnection& command)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for(commandSC = mCommandSC.begin();
        commandSC != mCommandSC.end();
        commandSC++)
    {
        // Find the matching SC.
        if( commandSC->first.mProviderID == key.mProviderID &&
            commandSC->first.mMessageCode == key.mMessageCode &&
            commandSC->second.mInstanceID == command.GetInstanceID() &&
            commandSC->second.IsCommander(command.GetSourceID()) )
        {
            // Get the current commander.
            ServiceConnection::Command::Commander prevCommander = commandSC->second.mCommander;
            // Suspend the commander.
            commandSC->second.SuspendCommander(command.GetSourceID());
            // Figure out who the new commander is.
            commandSC->second.mCommander = commandSC->second.GetNextCommander();
            // If the active commander has changed, activate the new commander.
            if( prevCommander.mID != commandSC->second.mCommander.mID &&
                commandSC->second.mCommander.mID.IsValid() )
            {
                Jaus::ActivateServiceConnection activate;
                activate.SetSourceID(mNodeID);
                activate.SetDestinationID(commandSC->second.mCommander.mID);
                activate.SetInstanceID(commandSC->second.mInstanceID);
                mpConnectionHandler->Send(&activate);
            }

            if( commandSC->second.mCommander.mID.IsValid() == false)
            {
                // Notify the service provider that all are suspended.
                command.SetSourceID(mNodeID);
                mpConnectionHandler->Send(&command);
            }

            break;
        } // If matching SC found.
    } // Search each active SC.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Activate Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Activate Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::ActivateInformServiceConnection(const Stream& msg,
                                                               const Header& header,
                                                               Jaus::ActivateServiceConnection& command)
{
    ServiceConnection::InformMap::iterator informSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for( informSC = mInformSC.begin();
         informSC != mInformSC.end();
         informSC++ )
    {
        // Find the SC.
        if( informSC->first.mProviderID == key.mProviderID &&
            informSC->first.mMessageCode == key.mMessageCode &&
            informSC->second.mInstanceID == command.GetInstanceID()  &&
            informSC->second.IsSubscriber( command.GetSourceID() ) )
        {
            bool wasSuspended = informSC->second.IsServiceSuspended();
            informSC->second.ActivateSubscriber( command.GetSourceID() );
            if( wasSuspended == true && informSC->second.IsServiceSuspended() == false)
            {
                // Tell component to activate.
                command.SetSourceID( mNodeID );
                mpConnectionHandler->Send(&command);
            }
            break;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes the message for Activate Service Connection
///
///   \param msg The message to process.
///   \param header Header data for message.
///   \param command The Activate Service Connection message read to process.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::ActivateCommandServiceConnection(const Stream& msg,
                                                                const Header& header,
                                                                Jaus::ActivateServiceConnection& command)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    key.mProviderID = header.mDestinationID;
    key.mMessageCode = command.GetMessageCode();

    for(commandSC = mCommandSC.begin();
        commandSC != mCommandSC.end();
        commandSC++)
    {
        // Find the matching SC.
        if( commandSC->first.mProviderID == key.mProviderID &&
            commandSC->first.mMessageCode == key.mMessageCode &&
            commandSC->second.mInstanceID == command.GetInstanceID() &&
            commandSC->second.IsCommander(command.GetSourceID()) )
        {
            // Get the current commander.
            ServiceConnection::Command::Commander prevCommander = commandSC->second.mCommander;
            // Activate the commander.
            commandSC->second.ActivateCommander(command.GetSourceID());
            // Figure out who the new commander is.
            commandSC->second.mCommander = commandSC->second.GetNextCommander();
            // If the active commander has changed, activate the new commander.
            if( prevCommander.mID != commandSC->second.mCommander.mID &&
                commandSC->second.mCommander.mID.IsValid() )
            {
                // Suspend the previously active commander.
                if(prevCommander.mID.IsValid())
                {
                    Jaus::SuspendServiceConnection suspend;
                    suspend.SetSourceID(mNodeID);
                    suspend.SetDestinationID(prevCommander.mID);
                    suspend.SetInstanceID(commandSC->second.mInstanceID);
                    mpConnectionHandler->Send(&suspend);
                }
                // Activate the new commander
                Jaus::ActivateServiceConnection activate;
                activate.SetSourceID(mNodeID);
                activate.SetDestinationID(commandSC->second.mCommander.mID);
                activate.SetInstanceID(commandSC->second.mInstanceID);
                mpConnectionHandler->Send(&activate);
            }

            // If not previously active, then we need to
            // notify the provider that the SC is activated again.
            if( prevCommander.mID.IsValid() == false)
            {
                // Let the service provider know the SC is active again.
                command.SetSourceID(mNodeID);
                mpConnectionHandler->Send(&command);
            }
            break;
        } // If matching SC found
    } // Search all active SC's
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the SC message to all subscribers of the SC.
///
///   \param msg The message to process.
///   \param header Header data for message.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::RouteInformServiceConnectionMessage(Stream& msg,
                                                                   Header& header)
{
    ServiceConnection::InformMap::iterator inform;
    ServiceConnection::Key key;

    //  Generate key so we can look up the SC.
    key.mProviderID = header.mSourceID;
    key.mMessageCode = header.mCommandCode;
    MessageCreator::GetPresenceVector(msg, key.mPresenceVector);
    // Find the service connection if it exists.
    inform = mInformSC.find( key );
    if( inform != mInformSC.end() )
    {
        // Send the message to each subscriber.
        Address::Set::const_iterator dest;
        Address::Set failure;

        for(dest = inform->second.mSubscribers.begin();
            dest != inform->second.mSubscribers.end();
            dest++ )
        {
            // Re-write the message header so it goes
            // to the subscriber
            header.mDestinationID = (*dest);
            msg.Write(header, 0);
            if( mpConnectionHandler->Send(msg) == JAUS_FAILURE )
            {
                failure.insert(*dest);
            }
        }

        // Remove destinations we failed to send to.
        for(dest = failure.begin(); dest != failure.end(); dest++)
        {
            inform->second.RemoveSubscriber( *dest );
        }

        //  If there are no subscribers, terminate the service connection.
        if(inform->second.GetNumSubscribers() == 0 )
        {
            // Tell provider to stop service connection.
            Jaus::TerminateServiceConnection kill;
            kill.SetSourceID( mNodeID );
            kill.SetDestinationID( key.mProviderID );
            kill.SetMessageCode( key.mMessageCode );
            kill.SetInstanceID( inform->second.mInstanceID );

            // Send
            mpConnectionHandler->Send(&kill);

            // Remove from SC list.
            mInformSC.erase(inform);
        }
    }
    else
    {
        // If no service connection present, do nothing with the
        // message (block it).
        //mpConnectionHandler->Send(msg);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Routes a command service connection type message.  This
///   method will either block or let through the command message.
///
///   \param msg The message to process.
///   \param header Header data for message.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::RouteCommandServiceConnectionMessage(Stream& msg, Header& header)
{
    ServiceConnection::CommandMap::iterator commandSC;
    ServiceConnection::Key key;

    //  Generate key so we can look up the SC.
    key.mProviderID = header.mDestinationID;
    key.mMessageCode = header.mCommandCode;
    MessageCreator::GetPresenceVector(msg, key.mPresenceVector);
    // Find the service connection if it exists.
    commandSC = mCommandSC.find( key );
    if( commandSC != mCommandSC.end() )
    {
        // Only send messages from the active commander to provider.
        if( header.mSourceID == commandSC->second.mCommander.mID )
        {
            mpConnectionHandler->Send(msg);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Terminates all service connections forcefully.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::StopServiceConnections()
{
    ServiceConnection::InformMap::iterator inform;
    ServiceConnection::CommandMap::iterator command;
    Jaus::TerminateServiceConnection terminate;

    terminate.SetSourceID(mNodeID);

    mServiceConnectionsMutex.Enter();
    for(inform = mInformSC.begin();
        inform != mInformSC.end();
        inform++)
    {
        terminate.SetInstanceID(inform->second.mInstanceID);
        terminate.SetMessageCode(inform->second.mMessageCode);
        terminate.SetDestinationID(inform->second.mProviderID);
        mpConnectionHandler->Send(&terminate);
    }

    for(command = mCommandSC.begin();
        command != mCommandSC.end();
        command++)
    {
        terminate.SetInstanceID(command->second.mInstanceID);
        terminate.SetMessageCode(command->second.mMessageCode);
        ServiceConnection::Command::CommanderSet::iterator cmdr;

        for(cmdr = command->second.mActive.begin();
            cmdr != command->second.mActive.end();
            cmdr++)
        {
            terminate.SetDestinationID(cmdr->mID);
            mpConnectionHandler->Send(&terminate);
        }
        for(cmdr = command->second.mPending.begin();
            cmdr != command->second.mPending.end();
            cmdr++)
        {
            terminate.SetDestinationID(cmdr->mID);
            mpConnectionHandler->Send(&terminate);
        }
        for(cmdr = command->second.mSuspended.begin();
            cmdr != command->second.mSuspended.end();
            cmdr++)
        {
            terminate.SetDestinationID(cmdr->mID);
            mpConnectionHandler->Send(&terminate);
        }

        if(command->second.mCommander.mID.IsValid())
        {
            terminate.SetDestinationID(command->second.mCommander.mID);
            mpConnectionHandler->Send(&terminate);
        }

        terminate.SetDestinationID(command->second.mProviderID);
        mpConnectionHandler->Send(&terminate);
    }
    mInformSC.clear();
    mCommandSC.clear();
    mServiceConnectionsMutex.Leave();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method performed in thread for SC manager which pulls
///   SC related messages from queue and routes them as needed or creates
///   SC management related information.
///
////////////////////////////////////////////////////////////////////////////////////
void ServiceConnectionManager::ServiceConnectionManagerThread(void *args)
{
    ServiceConnectionManager* manager = (ServiceConnectionManager*)args;
    unsigned int count = 0;
    Header header;
    bool haveMessage = false;

    while( manager &&
           !manager->mServiceConnectionManagerThread.QuitThreadFlag() &&
           manager->mpConnectionHandler)
    {
        // Clear previous message data.
        haveMessage = false;
        if(manager->mpCurrentMessage)
        {
            delete manager->mpCurrentMessage;
            manager->mpCurrentMessage = NULL;
        }
        if(manager->mpCurrentMessageHeader)
        {
            delete manager->mpCurrentMessageHeader;
            manager->mpCurrentMessageHeader = NULL;
        }

        manager->mPipelineMutex.Enter();
        if(manager->mStreamPipeline.mStreams.size() > 0)
        {
            manager->mpCurrentMessage = manager->mStreamPipeline.mStreams.front();
            manager->mpCurrentMessageHeader = manager->mStreamPipeline.mHeaders.front();
            manager->mStreamPipeline.mStreams.pop_front();
            manager->mStreamPipeline.mHeaders.pop_front();
            haveMessage = true;
            header = *manager->mpCurrentMessageHeader;
        }
        manager->mPipelineMutex.Leave();

        manager->mServiceConnectionsMutex.Enter();

        if( haveMessage && manager->mpConnectionHandler)
        {
            switch(header.mCommandCode)
            {
                case JAUS_CREATE_SERVICE_CONNECTION:
                    {
                        // If someone wants to create an SC to a provider on this node, then
                        // we must enable SC management for it.
                        if(header.mDestinationID.mSubsystem == manager->mNodeID.mSubsystem &&
                            header.mDestinationID.mNode == manager->mNodeID.mNode)
                        {
                            Jaus::CreateServiceConnection command;
                            if(command.Read(*manager->mpCurrentMessage))
                            {
                                if(IsInformMessage(command.GetMessageCode()))
                                {
                                    manager->CreateInformServiceConnection(*manager->mpCurrentMessage,
                                                                           *manager->mpCurrentMessageHeader,
                                                                           command);
                                }
                                else
                                {
                                    manager->CreateCommandServiceConnection(*manager->mpCurrentMessage,
                                                                            *manager->mpCurrentMessageHeader,
                                                                            command);
                                }
                            }
                        }
                        // Just send the message along, it's not our problem.
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
                case JAUS_CONFIRM_SERVICE_CONNECTION:
                    {
                        // If the confirmation came from a provider on this node, then
                        // we must follow up with the SC capabilities started with the
                        // Create Service Connection message.
                        if(header.mSourceID.mSubsystem == manager->mNodeID.mSubsystem &&
                            header.mSourceID.mNode == manager->mNodeID.mNode)
                        {
                            Jaus::ConfirmServiceConnection command;
                            if(command.Read(*manager->mpCurrentMessage))
                            {
                                if(IsInformMessage(command.GetMessageCode()))
                                {
                                    manager->ConfirmInformServiceConnection(*manager->mpCurrentMessage,
                                                                            *manager->mpCurrentMessageHeader,
                                                                            command);
                                }
                                else
                                {
                                    manager->ConfirmCommandServiceConnection(*manager->mpCurrentMessage,
                                                                             *manager->mpCurrentMessageHeader,
                                                                             command);
                                }
                            }
                        }
                        // Just send the message along, it's not our problem.
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
                case JAUS_TERMINATE_SERVICE_CONNECTION:
                    {
                        // If the terminate message is for a provider on this node
                        // then we do additional handling, otherwise just pass
                        // the message onward.
                        if(header.mDestinationID.mSubsystem == manager->mNodeID.mSubsystem &&
                            header.mDestinationID.mNode == manager->mNodeID.mNode)
                        {
                            Jaus::TerminateServiceConnection command;
                            if(command.Read(*manager->mpCurrentMessage))
                            {
                                if(IsInformMessage(command.GetMessageCode()))
                                {
                                    manager->TerminateInformServiceConnection(*manager->mpCurrentMessage,
                                                                              *manager->mpCurrentMessageHeader,
                                                                              command);
                                }
                                else
                                {
                                    manager->TerminateCommandServiceConnection(*manager->mpCurrentMessage,
                                                                               *manager->mpCurrentMessageHeader,
                                                                               command);
                                }
                            }
                        }
                        // Just send the message along, it's not our problem.
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
                case JAUS_SUSPEND_SERVICE_CONNECTION:
                    {
                        // If the suspend message is for a provider on this node
                        // then we do additional handling, otherwise just pass
                        // the message onward.
                        if(header.mDestinationID.mSubsystem == manager->mNodeID.mSubsystem &&
                            header.mDestinationID.mNode == manager->mNodeID.mNode)
                        {
                            Jaus::SuspendServiceConnection command;
                            if(command.Read(*manager->mpCurrentMessage))
                            {
                                if(IsInformMessage(command.GetMessageCode()))
                                {
                                    manager->SuspendInformServiceConnection(*manager->mpCurrentMessage,
                                                                            *manager->mpCurrentMessageHeader,
                                                                            command);
                                }
                                else
                                {
                                    manager->SuspendCommandServiceConnection(*manager->mpCurrentMessage,
                                                                             *manager->mpCurrentMessageHeader,
                                                                             command);
                                }
                            }
                        }
                        // Just send the message along, it's not our problem.
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
                case JAUS_ACTIVATE_SERVICE_CONNECTION:
                    {
                        // If the activate message is for a provider on this node
                        // then we do additional handling, otherwise just pass
                        // the message onward.
                        if(header.mDestinationID.mSubsystem == manager->mNodeID.mSubsystem &&
                            header.mDestinationID.mNode == manager->mNodeID.mNode)
                        {
                            Jaus::ActivateServiceConnection command;
                            if(command.Read(*manager->mpCurrentMessage))
                            {
                                if(IsInformMessage(command.GetMessageCode()))
                                {
                                    manager->ActivateInformServiceConnection(*manager->mpCurrentMessage,
                                                                             *manager->mpCurrentMessageHeader,
                                                                             command);
                                }
                                else
                                {
                                    manager->ActivateCommandServiceConnection(*manager->mpCurrentMessage,
                                                                              *manager->mpCurrentMessageHeader,
                                                                              command);
                                }
                            }
                        }
                        // Just send the message along, it's not our problem.
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
                default:
                    {
                        if(header.mServiceConnectionFlag == JAUS_SERVICE_CONNECTION )
                        {
                            if( IsInformMessage(header.mCommandCode) &&
                                header.mSourceID.mSubsystem == manager->mNodeID.mSubsystem &&
                                header.mSourceID.mNode == manager->mNodeID.mNode )
                            {
                                manager->RouteInformServiceConnectionMessage(*manager->mpCurrentMessage, header);
                            }
                            else if (IsCommandMessage(header.mCommandCode) &&
                                     header.mDestinationID.mSubsystem == manager->mNodeID.mSubsystem &&
                                     header.mDestinationID.mNode == manager->mNodeID.mNode )
                            {
                                manager->RouteCommandServiceConnectionMessage(*manager->mpCurrentMessage, header);
                            }
                            else
                            {
                                // Just send normally (this nodes SC management doesn't need to handle)
                                manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                            }
                        }
                        else
                        {
                            manager->mpConnectionHandler->Send(*manager->mpCurrentMessage);
                        }
                    }
                    break;
            }
        }

        manager->mServiceConnectionsMutex.Leave();

        // Every now and then let the thread processor
        // switch from this thread, "FREE" of charge!
        if( ++count == 250 )
        {
            count = 0;
            CxUtils::SleepMs(1);
        }
    }
}


/*  End of File */
