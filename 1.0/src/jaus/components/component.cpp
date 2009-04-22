////////////////////////////////////////////////////////////////////////////////////
///
///  \file component.cpp
///  \brief This file contains software for deriving/creating components
///  and services that support JAUS.
///
///  The base class here (Component) contains all capability needed to
///  communicate with the JAUS Node Manager (NodeManager), and supports all of
///  the core messages defined in JAUS.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 9 February 2007
///  <br>Last Modified: 5 February 2009
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
#include "jaus/components/component.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/query/querymessages.h"
#include "jaus/messages/inform/informmessages.h"
#include "jaus/messages/experimental/experimentalmessages.h"
#include "jaus/messages/inform/acknackmessage.h"
#include <iostream>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Component::ComponentConnectionHandler::ComponentConnectionHandler()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, automatically shutsdown handler.
///
////////////////////////////////////////////////////////////////////////////////////
Component::ComponentConnectionHandler::~ComponentConnectionHandler()
{
    Shutdown();
    // Clear pending receipts (will stop any blocking sends).
    ClearPendingReceipts();
    // Close inbox.
    mInbox.Close();
    // Close outbox.
    mOutbox.Close();
    // Unregister the component ID.
    mRegistry.UnRegister(GetID());
    // Close registry
    mRegistry.CloseRegistry();
    // Clear ID.
    mComponentID(0, 0, 0, 0);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initialize the connection handler.
///
///   This method opens up a block of shared memory for the component to
///   receive messages.  It then hooks to the shared memory object with a
///   callback so it can pass incomming messages to a message handler.
///
///   \param id The ID of the JAUS component.
///   \param mbsize Size of shared memory to use in bytes.  This size should
///          be set based on the number and size of messages you expect to
///          receive.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ComponentConnectionHandler::Initialize(const Address& id, const unsigned int mbsize)
{
    int result = JAUS_FAILURE;
    // Shutdown any active connections.
    Shutdown();
    if(id.IsValid() && !id.IsBroadcast())
    {
        // Try and create a message inbox, if we cannot
        // do this, then someone else already has one running
        // and there is an ID conflict.
        if(mInbox.CreateInbox(id, NULL, mbsize) || mInbox.OpenInbox(id))
        {
            // Wait 250 ms, then see if anyone else is using
            // this shared memory location, and only initialize if
            // it is unused.
            CxUtils::SleepMs(250);
            if(!mInbox.IsActive(200))
            {
                mInbox.RegisterCallback(this);
                SetComponentID(id);
                // Now try and open a view to the registry
                if(mRegistry.OpenComponentRegistry(id.mSubsystem, id.mNode))
                {
                    mRegistry.Register(id);
                    // Now that we have a view of the registry, lets
                    // try open a view to a node managers shared memory inbox.
                    // However, it is not a no-go on success if we can't open it yet.
                    mOutbox.OpenInbox(Address(id.mSubsystem, id.mNode, 1, 1));
                    result = JAUS_OK;
                }
            }

        }
        else
        {
            SetJausError(ErrorCodes::AddressConflict);
        }
    }
    else
    {
        SetJausError(ErrorCodes::InvalidAddress);
    }
    if(result == JAUS_FAILURE)
    {
        Shutdown();
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the handler, closing any open connections.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ComponentConnectionHandler::Shutdown()
{
    ConnectionHandler::Shutdown();
    // Clear pending receipts (will stop any blocking sends).
    ClearPendingReceipts();
    // Close inbox.
    mInbox.Close();
    // Close outbox.
    mOutbox.Close();
    // Unregister the component ID.
    mRegistry.UnRegister(GetID());
    // Close registry
    mRegistry.CloseRegistry();
    // Clear ID.
    mComponentID(0, 0, 0, 0);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a message to the node manager for routing using
///          shared memory.
///
///   \param msg The serialized JAUS message to send.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ComponentConnectionHandler::SendStream(const Stream& msg)
{
    if(mOutbox.IsOpen() == false || mOutbox.IsActive() == false)
    {
        if(mComponentID.IsValid())
        {
            if(mOutbox.OpenInbox(Address(mComponentID.mSubsystem,
                                            mComponentID.mNode,
                                            1,
                                            1)) == JAUS_FAILURE)
            {
                return JAUS_FAILURE;
            }
        }
        else
        {
            return JAUS_FAILURE;
        }
    }

    if(mOutbox.EnqueueMessage(msg) > 0)
    {
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Component::Component()
{
    mIsControllableFlag = false;
    mPrimaryStatus = Status::Shutdown;
    mSecondaryStatus = 0;
    mAuthorityCode = 0;
    mpMessageHandler = new MessageHandler();
    mpMessageHandler->ToggleDeserialization();
    mpConnectionHandler = new ComponentConnectionHandler();
    mpConnectionHandler->SetMessageHandler(mpMessageHandler);

    Service srvc;
    srvc.LoadCoreMessageSupport();
    srvc.AddInputMessage(JAUS_QUERY_SERVICES, 0);
    srvc.AddInputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    srvc.AddOutputMessage(JAUS_REPORT_SERVICES, 0);
    srvc.AddOutputMessage(JAUS_REPORT_IDENTIFICATION, 0);
    AddService(srvc);

    SetupCoreService();
    SetupService();
    // Additional cleanup of static memory.
    MessageCreator::CleanupMessageCreator();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Component::~Component()
{
    // Call shutdown, depending on whether or not
    // it has been overloaded, it may close active
    // subscriptions, etc. or send out messages
    // to signal Shutdown to controllers, etc.
    Shutdown();

    // Just in case someone overloaded shutdown and
    // forgot to call the Component version, repeat the work.
    mpConnectionHandler->Shutdown();
    mpMessageHandler->ClearMessageQueue();
    mpMessageHandler->ClearMessageCallback();
    mpMessageHandler->ClearStreamCallback();
    mComponentID(0, 0, 0, 0);
    mPrimaryStatus = Status::Shutdown;
    mSecondaryStatus = 0;
    mControllerID(0, 0, 0, 0);
    mControllerAuthorityCode = 0;
    mComponentName.clear();

    mpConnectionHandler->ClearMessageHandler();

    // Delete allocated memory.
    // Delete connection handler first, so that
    // it stops putting data into the message handler.
    if(mpConnectionHandler)
    {
        mpConnectionHandler->Shutdown();
        delete mpConnectionHandler;
        mpConnectionHandler = NULL;
    }
    // Delete the message handler last.
    if(mpMessageHandler)
    {
        delete mpMessageHandler;
        mpMessageHandler = NULL;
    }
    mMessageCallbacks.clear();

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the component.  Initialization allows for connections
///          to the Node Manager to take place, and thus any communication.
///
///   Initialization creates two threads that are managed by the Component
///   class.  One is a message handler thread, within which all message
///   processing is done.  Whenever a message is given to this thread it
///   will either perform a function callback (if any are registered) or
///   call one of the various ProcessMessage virtual methods depending on
///   the type of message.  The second thread is the connection handler which
///   maintains communication with any Node Manager through shared memory.  It
///   sends all messages, and processes incomming messages by either sending them
///   to the message handler or to a pending Receipt (on blocking send).
///
///   The initialization of a component does not depend on a Node Manager
///   running on the computer at the time of initialization.  Once a Node Manager
///   is initialized on the computer with a matching subsystem and node ID as
///   your component, a connection will be made automatically.  Use the
///   IsConnected method to check for an active connection.
///
///   No communication can take place with your Component until it is initialized
///   and connected to a Node Manager.
///
///   \param name The name of your component.
///   \param id The ID of your component.
///   \param mbsize The size of the components shared memory buffer.  By default
///                 this buffer is large enough to store 10 JAUS_MAX_PACKET_SIZE
///                 message streams.  If the component will be receiving large
///                 amounts of data (like video) you may want to increase this
///                 size.
///
///   \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Initialize(const std::string& name, const Address& id, const unsigned int mbsize)
{
    int result = JAUS_FAILURE;
    // Shutdown everything.
    Shutdown();

    if(id.IsValid() && !id.IsBroadcast())
    {
        if(mpConnectionHandler->Initialize(id, mbsize))
        {
            SetupCoreService();
            SetupService();
            mComponentID = id;
            mComponentName = name;
            mPrimaryStatus = Status::Standby;
            // Register callbacks to receive data through callback in another thread.
            mpMessageHandler->RegisterCallback(&Component::StreamCallbackFunction, this);
            // Make sure the two handlers can talk to each other.
            mpConnectionHandler->SetMessageHandler(mpMessageHandler);
            result = JAUS_OK;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the component.  Use this method to unitialize the componet.
///   This method is also called when a Shutdown message is received.
///
///   Shutdown stops both the connection handler and message handler threads. It
///   puts the component in Shutdown status, then clears the component name
///   and ID, and any pending messages not yet processed.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Shutdown()
{
    mpConnectionHandler->Shutdown();
    CxUtils::SleepMs(250);
    mpMessageHandler->Shutdown();
    mComponentID(0, 0, 0, 0);
    mPrimaryStatus = Status::Shutdown;
    mSecondaryStatus = 0;
    mControllerID(0, 0, 0, 0);
    mControllerAuthorityCode = 0;
    mComponentName.clear();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the message.
///
///   \param msg The message stream to send.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE and error code set.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Send(const Stream& msg) const
{
    return ((ComponentConnectionHandler*)(mpConnectionHandler))->SendStream(msg);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Serializes a message and then sends it.
///
///   \param msg The message to serialize and then send.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE and error code set.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Send(const Message* msg)  const
{
    return ((ComponentConnectionHandler*)(mpConnectionHandler))->Send(msg);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a serialized message, and waits for a desired response message.
///
///   This method is useful when you need your program to wait until a response
///   to your query/command is recieved before continuing with other processing.
///   Just pass it the message you want to send and a receipt structure to
///   store the response.
///
///   Although setting the expected response message code is a parameter to
///   this method, it is optional.  If the value is set to 0, then this
///   method will try to automatically lookup what the response should be.
///   However, if the message is not part of this library (a custom message
///   you made without updating MessageCreator) this will not work and you
///   will need to manually set the response type.
///
///   If a message does not have a response message associated with (such as
///   Set Component Authority), then this method will fail, unless you set
///   the AckNack flags to Request AckNack.  If this is the case, then the
///   method will block until an Acknowledge or Negative Acknowledge message
///   is received.
///
///   This method does not support messages to a broadcast destination since
///   it is impossible to know exactly how many responses to expect.
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
int Component::Send(const Stream& msg,
                    Receipt& receipt,
                    const UShort responseCode,
                    const UShort tms,
                    const UShort tries) const
{
    return ((ComponentConnectionHandler*)(mpConnectionHandler))->Send(msg, receipt, responseCode, tms, tries);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a message, and waits for a desired response message.
///
///   This method is useful when you need your program to wait until a response
///   to your query/command is recieved before continuing with other processing.
///   Just pass it the message you want to send and a receipt structure to
///   store the response.
///
///   Although setting the expected response message code is a parameter to
///   this method, it is optional.  If the value is set to 0, then this
///   method will try to automatically lookup what the response should be.
///   However, if the message is not part of this library (a custom message
///   you made without updating MessageCreator) this will not work and you
///   will need to manually set the response type.
///
///   If a message does not have a response message associated with (such as
///   Set Component Authority), then this method will fail, unless you set
///   the AckNack flags to Request AckNack.  If this is the case, then the
///   method will block until an Acknowledge or Negative Acknowledge message
///   is received.
///
///   This method does not support messages to a broadcast destination since
///   it is impossible to know exactly how many responses to expect.
///
///   \param msg The message data to send.
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
int Component::Send(const Message* msg,
                    Receipt& receipt,
                    const UShort responseCode,
                    const UShort tms,
                    const UShort tries) const
{
    return ((ComponentConnectionHandler*)(mpConnectionHandler))->Send(msg, receipt, responseCode, tms, tries);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a flag to indicate that the component can be controlled.
///
///   If this value is set to true (default is false), then the component will
///   allow other components to request and transition control of this
///   component.  If a component is under control, then command messages from
///   non-controlling components will be ignored.
///
///   \param on If true, control capabilities are enabled.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
//////////////////////////////////////////////////////////////////////////////////
void Component::SetControllable(const bool on)
{
    mIsControllableFlag = on;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends an acknowledge response to the input message.
///
///   This method does not need to be used at all because ACK/NACK responses
///   are generated automatically.
///
///   \param msg The input message to send an acknowledge response to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
//////////////////////////////////////////////////////////////////////////////////
int Component::SendAcknowledgeResponse(const Message* msg)
{
    AckNackMessage response(msg->GetCommandCode());
    // Copy header data to copy priority, etc.
    response.CopyHeaderData(msg);
    response.SetDestinationID(msg->GetSourceID());
    response.SetSourceID(mComponentID);
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
///   This method does not need to be used at all because ACK/NACK responses
///   are generated automatically.
///
///   \param msg The input message to send a negative acknowledge response to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SendNegativeAcknowledgeResponse(const Message* msg)
{
    AckNackMessage response(msg->GetCommandCode());
    // Copy header data to copy priority, etc.
    response.CopyHeaderData(msg);
    response.SetDestinationID(msg->GetSourceID());
    response.SetSourceID(mComponentID);
    // Make sure SC flag is not set.
    response.SetServiceConnectionFlag();
    response.SetAckNack(Header::AckNack::Nack);
    // Send it.
    return Send(&response);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receives a specific message when it arrives.
///
///   If the specific message specified by mcode is received, then this callback
///   is performed, overriding any other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param mcode The message code to register callback to.
///   \param cb Pointer to callback object.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::RegisterCallback(const UShort mcode, MessageCallback* cb)
{
    int result = JAUS_FAILURE;
    MessageCallbackMap::iterator cbiter;

    mComponentMutex.Enter();
    cbiter = mMessageCallbacks.find(mcode);
    if(cbiter == mMessageCallbacks.end())
    {
        mMessageCallbacks[mcode] = MessageCallback::Data(cb);
        result = JAUS_OK;
    }
    mComponentMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receives a specific message when it arrives.
///
///   If the specific message specified by mcode is received, then this callback
///   is performed, overriding any other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param mcode The message code the callback to register callback for.
///   \param cb Pointer to function callback.
///   \param args Additional arguments for function callback.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::RegisterCallback(const UShort mcode, MessageFunctionCallback cb, void* args)
{
    int result = JAUS_FAILURE;
    MessageCallbackMap::iterator cbiter;

    mComponentMutex.Enter();
    cbiter = mMessageCallbacks.find(mcode);
    if(cbiter == mMessageCallbacks.end())
    {
        mMessageCallbacks[mcode] = MessageCallback::Data(cb, args);
        result = JAUS_OK;
    }
    mComponentMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Remove a callback registered for a specific type of message.
///
///   \param mcode Message code to remove callback for.
///
////////////////////////////////////////////////////////////////////////////////////
void Component::RemoveCallback(const UShort mcode)
{
    MessageCallbackMap::iterator cbiter;

    mComponentMutex.Enter();
    cbiter = mMessageCallbacks.find(mcode);
    if(cbiter != mMessageCallbacks.end())
    {
        mMessageCallbacks.erase(cbiter);
    }
    mComponentMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enables/Disables the blocking of messages that have the Ack/Nack
///   field of the message header not equal to None.
///
///   By default this is turned off, but to enable blocking, call this function
///   with a value of true.
///
///   \param enable If true, blocking is enabled.
///
////////////////////////////////////////////////////////////////////////////////////
void Component::BlockAckNackMessages(const bool enable)
{
    if(mpConnectionHandler)
    {
        mpConnectionHandler->BlockAckNackMessages(enable);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If you desire your component to ignore/block messages with specific
///   levels of priority, then use this method to set the levels that you
///   want to accept.  By default, all levels of priority are accepted.
///
///   \param priorities Set of priority levels to accept.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetAcceptedPriorityValues(const std::set<UShort> priorities)
{
    int result = JAUS_FAILURE;
    // Validate values passed.
    std::set<UShort>::const_iterator p;
    for(p = priorities.begin();
        p != priorities.end();
        p++)
    {
        if((*p) > 15)
        {
            SetJausError(ErrorCodes::InvalidValue);
            return result;
        }
    }

    if(priorities.size() > 0)
    {
        result = JAUS_OK;
        mComponentMutex.Enter();
        mpConnectionHandler->SetAcceptedPriorityValues(priorities);
        mComponentMutex.Leave();
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a Reset message is received.  Overload
///          this function to add specific reset behaviors for your service.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Reset()
{
    mComponentMutex.Enter();
    if(mpConnectionHandler)
    {
        mpConnectionHandler->mInbox.Empty();
    }
    if(mpMessageHandler)
    {
        mpMessageHandler->ClearMessageQueue();
    }
    mComponentMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a Standby message is received.  Overload
///          this function to add specific reset behaviors for your service.
///
///   This method is also called if the component is controllable and a controlling
///   component releases control and no other component is controlling this
///   component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Standby()
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a Resume message is received.  Overload
///          this function to add specific reset behaviors for your service.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::Resume()
{
	SetPrimaryStatus(Component::Status::Ready);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a component is created.  It sets up
///   all core services supported by the component.
///
///   If a component ID is specified in the RA, it may report only one
///   service in beyond the core message support, and this service must be
///   equal to the component ID.  If a component ID is not listed in the
///   RA, it may report any number of services.  For example, an component
///   with ID 33 must provide only serive 33.  The exception to this rule
///   is component ID 1 (the Node Mnager) which may provide any number of
///   services in addition to core message support.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetupCoreService()
{
    Service srvc;
    srvc.LoadCoreMessageSupport();
    srvc.AddInputMessage(JAUS_QUERY_SERVICES, 0);
    srvc.AddInputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    srvc.AddOutputMessage(JAUS_REPORT_SERVICES, 0);
    srvc.AddOutputMessage(JAUS_REPORT_IDENTIFICATION, 0);
    AddService(srvc);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a component is created.  It sets up
///   the single stand alone service (besides the core service) that the
///   component supports.
///
///   If a component ID is specified in the RA, it may report only one
///   service in beyond the core message support, and this service must be
///   equal to the component ID.  If a component ID is not listed in the
///   RA, it may report any number of services.  For example, an component
///   with ID 33 must provide only serive 33.  The exception to this rule
///   is component ID 1 (the Node Mnager) which may provide any number of
///   services in addition to core message support.
///
///   The core message support service is already added to the list
///   of component services by default (using SetupCoreService).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetupService()
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a service that is supported by your component to the
///          service set.
///
///  Services add through this method are used to support the Report Services
///  message.
///
///   \param service The service to add support for.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::AddService(const Service& service)
{
    mComponentMutex.Enter();
    mComponentServices[service.GetType()] = service;
    mComponentMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes support for the service from your component.
///
///   \param type The service type to remove support for.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::RemoveService(const UShort type)
{
    int result = JAUS_FAILURE;
    Service::Map::iterator srvc;

    mComponentMutex.Enter();
    srvc = mComponentServices.find( type );
    if(srvc != mComponentServices.end())
    {
        mComponentServices.erase(srvc);
        result = JAUS_OK;
    }
    mComponentMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the service (if supplied by component).
///
///   \param type The service type to get.
///   \param service Copy of the service data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::GetService(const UShort type, Service& service)
{
    int result = JAUS_FAILURE;
    Service::Map::iterator srvc;

    mComponentMutex.Enter();
    srvc = mComponentServices.find( type );
    if(srvc != mComponentServices.end())
    {
        service = srvc->second;
        result = JAUS_OK;
    }
    mComponentMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes a serialized message and attempts to process it.
///
///   This method is called only when a message is received that is formatted
///   correctly, but the type is not recognized and the data cannot be
///   de-serialized.  If you are receiving messages that are not part of the
///   standard library, overload this method to receive the data.
///
///   \param msg Serialized JAUS message not supported by standard library.
///              Includes message header and data block.
///   \param header Header information associated with the message that has
///                 already been read.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessMessage(const Stream& msg, const Header& header)
{
    Jaus::Message* deserializedMessage = NULL;
    // Check for an Ack/Nack message and route appropriately.
    if(header.mDataSize == 0 &&
       (header.mAckNack == Header::AckNack::Ack || header.mAckNack == Header::AckNack::Nack))
    {
        AckNackMessage acknack(header);
        return ProcessAckNackMessage(&acknack);
    }
    // Try to de-serialize the message data.
    deserializedMessage = MessageCreator::CreateMessage(msg, &header);
    if(deserializedMessage)
    {
        int result = JAUS_FAILURE;
        result = ProcessMessage(deserializedMessage);
        delete deserializedMessage;
        return result;
    }
    return JAUS_FAILURE;
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
///
///   \return JAUS_OK on processing success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessMessage(const Message* msg)
{
    int result = JAUS_FAILURE;
    UShort code = msg->GetCommandCode();
    if(Address::DestinationMatch(msg->GetDestinationID(),mComponentID))
    {
        mComponentMutex.Enter();
        MessageCallbackMap::iterator cb;
        cb = mMessageCallbacks.find(msg->GetCommandCode());
        if(cb != mMessageCallbacks.end())
        {
            cb->second.RunCallback(msg);
            result = JAUS_OK;
        }
        mComponentMutex.Leave();
        if(result == JAUS_FAILURE)
        {
            // Check for an Ack/Nack message.
            if(msg->GetAckNack() == Header::AckNack::Ack || msg->GetAckNack() == Header::AckNack::Nack)
            {
                result = ProcessAckNackMessage(dynamic_cast<const AckNackMessage*>(msg));
            }
            else
            {
                // Based on the value of the message code, pass
                // the message to the right category (command, query, inform, ...).
                if(code <= 0x1FFF)
                {
                    result = ProcessCommandMessage(msg);
                }
                else if(code >= 0x2000 && code <= 0x3FFF)
                {
                    result = ProcessQueryMessage(msg);
                }
                else if(code >= 0x4000 && code <= 0x5FFF)
                {
                    result = ProcessInformMessage(msg);
                }
                else if(code >= 0xD000)


                {
                    result = ProcessExperimentalMessage(msg);
                }
                else
                {
                    result = JAUS_FAILURE;
                    SetJausError(ErrorCodes::UnknownMessageType);
                }
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing command messages.  This method is called
///   whenever a command message is received.  Overload this method to
///   add support for other input message types.
///
///   This specific implementation queries the authority level the
///   component sending the command if the source component does not have
///   direct control of this component.  For example, component A sends a
///   Request Component Control message to this component, B, and B accepts.
///   Then component A sends a command, it is accepted since A has exclusive
///   control.  Next component C sends a command message, B will then Query
///   the authority of component C before processing.
///
///   \param msg Command message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessCommandMessage(const Message* msg)
{
    // If from component controlling exclusively, accept the message right away.
    if(mControllerID.IsValid() && msg->GetSourceID() == mControllerID)
    {
        return ProcessCommandMessage(msg, mControllerAuthorityCode);
    }
    // Query authority, then process.
    else
    {
        Byte commandAuthority = GetComponentAuthority();
        // If the component authority level is anything
        // but zero, then we must determine if the sending
        // component has authority to send commands to us.
        if(commandAuthority > 0)
        {
            // These core messages require a level of authority
            // before changes can be made, so lets find out what the
            // commanding authority is before going further.
            if(QueryComponentAuthorityLevel(msg->GetSourceID(), commandAuthority) == JAUS_FAILURE)
            {
                return JAUS_FAILURE;
            }
        }
        return ProcessCommandMessage(msg, commandAuthority);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing command messages.  This method is called
///   whenever a command message is received.  Overload this method to
///   add support for other input message types.  This method
///   uses the authority level of the source component to determine if it
///   should implement the command or not.
///
///   \param msg Command message to process.
///   \param commandAuthority The authority level of the component that
///                           sent the command.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessCommandMessage(const Message* msg, const Byte commandAuthority)
{
    int processed = JAUS_OK;

    // If component does not have minimum authority to
    // use this component, ignore command.  Also ignore
    // if under exclusive control by another component.
    if(commandAuthority < mAuthorityCode ||
        (mControllerID.IsValid() && mControllerID != msg->GetSourceID()))
    {
        if(msg->GetCommandCode() != JAUS_REQUEST_COMPONENT_CONTROL)
        {
            return JAUS_FAILURE;
        }
    }

    switch(msg->GetCommandCode())
    {
    case JAUS_SET_COMPONENT_AUTHORITY:
        {
            const Jaus::SetComponentAuthority* command = dynamic_cast<const Jaus::SetComponentAuthority*>(msg);
            mAuthorityCode = command->GetAuthorityCode();
        }
        break;
    case JAUS_SHUTDOWN:
        {
            if(dynamic_cast<const Jaus::Shutdown*>(msg))
            {
                SetPrimaryStatus(Component::Status::Shutdown);
                Shutdown();
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_STANDBY:
        {
            if(dynamic_cast<const Jaus::Standby*>(msg))
            {
                if(mPrimaryStatus == Component::Status::Ready)
                {
                    SetPrimaryStatus(Component::Status::Standby);
                    std::cout << GetID().ToString() << " Received Standby Command from " << msg->GetSourceID().ToString() << "\n";
                    Standby();                    
                }
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_RESUME:
        {
            if(dynamic_cast<const Jaus::Resume*>(msg))
            {
                if(mPrimaryStatus == Component::Status::Standby)
                {
                    SetPrimaryStatus(Component::Status::Ready);
                    std::cout << GetID().ToString() << " Received Resume Command from " << msg->GetSourceID().ToString() << "\n";
                    Resume();                    
                }
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_RESET:
        {
            if(dynamic_cast<const Jaus::Reset*>(msg))
            {
                SetPrimaryStatus(Component::Status::Initialized);
                Reset();
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_SET_EMERGENCY:
        {
            const Jaus::SetEmergency* command = dynamic_cast<const Jaus::SetEmergency*>(msg);
            if(command)
            {
                SetEmergency(command->GetEmergencyCode());
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_CLEAR_EMERGENCY:
        {
            const Jaus::ClearEmergency* command = dynamic_cast<const Jaus::ClearEmergency*>(msg);
            if(command)
            {
                ClearEmergency(command->GetEmergencyCode());
            }
            else { processed = JAUS_FAILURE; }
        }
        break;
    case JAUS_CREATE_SERVICE_CONNECTION:
        {
            ConfirmServiceConnection response;
            const CreateServiceConnection *command = dynamic_cast<const CreateServiceConnection*>(msg);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetMessageCode(command->GetMessageCode());
            response.SetResponseCode(ConfirmServiceConnection::ComponentDoesNotSupport);
            Send(&response);
        }
        break;
    case JAUS_SUSPEND_SERVICE_CONNECTION:
        {
            ConfirmServiceConnection response;
            const SuspendServiceConnection *command = dynamic_cast<const SuspendServiceConnection*>(msg);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetMessageCode(command->GetMessageCode());
            response.SetResponseCode(ConfirmServiceConnection::ComponentDoesNotSupport);
            Send(&response);
        }
        break;
    case JAUS_ACTIVATE_SERVICE_CONNECTION:
        {
            ConfirmServiceConnection response;
            const ActivateServiceConnection *command = dynamic_cast<const ActivateServiceConnection*>(msg);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetMessageCode(command->GetMessageCode());
            response.SetResponseCode(ConfirmServiceConnection::ComponentDoesNotSupport);
            Send(&response);
        }
        break;
    case JAUS_TERMINATE_SERVICE_CONNECTION:
        {
            ConfirmServiceConnection response;
            const TerminateServiceConnection *command = dynamic_cast<const TerminateServiceConnection*>(msg);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetMessageCode(command->GetMessageCode());
            response.SetResponseCode(ConfirmServiceConnection::ComponentDoesNotSupport);
            Send(&response);
        }
        break;
    case JAUS_REQUEST_COMPONENT_CONTROL:
        {
            const Jaus::RequestComponentControl* command = dynamic_cast<const Jaus::RequestComponentControl*>(msg);
            bool acceptControl = false;

            if(mIsControllableFlag && command->GetAuthorityCode() >= mAuthorityCode)
            {
                acceptControl = true;
                if(mControllerID != command->GetSourceID() && mControllerID.IsValid())
                {
                    // If the current controller has an authority equal to or
                    // higher, then reject the request for control.
                    if(mControllerAuthorityCode >= command->GetAuthorityCode())
                    {
                        acceptControl = false;
                    }
                    else
                    {
                        // Notify the current controller of lost control.
                        RejectComponentControl response;
                        Receipt receipt;
                        response.SetSourceID(mComponentID);
                        response.SetDestinationID(mControllerID);
                        response.SetAckNack(Header::AckNack::Request);
                        // Perform a blocking send with receipt, which will make
                        // all the necessary attempts to ensure receiving of rejection
                        // by previous controller.
                        Send(&response, receipt, JAUS_REJECT_COMPONENT_CONTROL);
                    }
                }
            }

            ConfirmComponentControl response;
            response.SetSourceID(mComponentID);
            response.SetDestinationID(command->GetSourceID());

            if(acceptControl)
            {
                // Update values.
                mControllerID = command->GetSourceID();
                mControllerAuthorityCode = command->GetAuthorityCode();
                response.SetResponseCode(ConfirmComponentControl::Granted);
            }
            else
            {
                // If not a controllable component, let
                // them know.
                if(mIsControllableFlag == false)
                {
                    response.SetResponseCode(ConfirmComponentControl::Unsupported);
                }
                else
                {
                    response.SetResponseCode(ConfirmComponentControl::Rejected);
                }
            }
            // Send the response message which either
            // accepts, rejects, or notifies control is not supported.
            Send(&response);
        }
        break;
    case JAUS_RELEASE_COMPONENT_CONTROL:
        {
            if(msg->GetSourceID() == mControllerID && mControllerID.IsValid())
            {
                // Clear controller information.
                mControllerID(0, 0, 0, 0);
                mControllerAuthorityCode = 0;
            }
        }
        break;
    case JAUS_SET_TIME:
        {
            const ReportTime* report = dynamic_cast<const ReportTime*>(msg);
            if(report)
            {
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
int Component::ProcessQueryMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_COMPONENT_AUTHORITY:
        {
            ReportComponentAuthority response;
            response.SetAuthorityCode(mAuthorityCode);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_QUERY_COMPONENT_STATUS:
        {
            ReportComponentStatus response;
            response.SetPrimaryStatusCode(mPrimaryStatus);
            response.SetSecondaryStatusCode(mSecondaryStatus);
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            Send(&response);
        }
        break;
    case JAUS_QUERY_TIME:
        {
            ReportTime response;
            const QueryTime* query = dynamic_cast<const QueryTime*>(msg);
            response.SetSourceID(mComponentID);
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
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            response.SetControllerID(mControllerID);
            response.SetControllerAuthorityCode(mControllerAuthorityCode);
            Send(&response);
        }
        break;
    case JAUS_QUERY_SERVICES:
        {
            ReportServices response;
            response.SetSourceID(mComponentID);
            response.SetDestinationID(msg->GetSourceID());
            Service::Map::const_iterator service;
            Service::Set serviceSet;
            for(service = mComponentServices.begin();
                service  != mComponentServices.end();
                service++)
            {
                serviceSet.insert(service->second);
            }
            response.SetServices(serviceSet);
            Send(&response);
        }
        break;
    case JAUS_QUERY_IDENTIFICATION:
        {
            const QueryIdentification* query = dynamic_cast<const QueryIdentification*>(msg);
            if(query && query->GetQueryType() == QueryIdentification::Component)
            {
                ReportIdentification response;
                Identification ident;
                response.SetDestinationID(msg->GetSourceID());
                response.SetSourceID(GetID());
                response.SetQueryType(ReportIdentification::Component);
                ident.SetType(mComponentID.mComponent);
                ident.SetIdentification(mComponentName);
                ident.SetAuthority(mAuthorityCode);
                response.SetIdentification(ident);
                Send(&response);
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
int Component::ProcessInformMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_EVENT:
        {
            const EventMessage* report = dynamic_cast<const EventMessage*>(msg);
            if(report)
            {
                if(report->GetEventMessage())
                {
                    ProcessMessage(report->GetEventMessage());
                }
                else if(report->GetEventMessageBody())
                {
                    Header header;
                    if(report->GetEventMessageBody()->Read(header, 0))
                    {
                        ProcessMessage(*report->GetEventMessageBody(), header);
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
///   \brief Method for processing experimental messages.  This method is called
///   whenever a experimental message is received.  Overload this method to
///   add support for other input message types.
///
///   \param msg Experimental message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessExperimentalMessage(const Message* msg)
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for handling received Ack/Nack messages.
///
///   \param msg Message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ProcessAckNackMessage(const AckNackMessage* msg)
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the primary status of the component.  This value is used
///          for the Report Component Status message.
///
///   \param status Primary status of the component.
///
///   \return JAUS_OK on success, otherwise JAUS-FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetPrimaryStatus(const Component::Status::Primary status )
{
    mComponentMutex.Enter();
    mPrimaryStatus = (Byte)(status);
    mComponentMutex.Leave();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the secondary status of the component.  This value is used
///          for the Report Component Status message.
///
///   \param status Secondary status of the component.
///
///   \return JAUS_OK on success, otherwise JAUS-FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetSecondaryStatus(const UInt status )
{
    mSecondaryStatus = status;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the emergency code/state for the component.  This method
///          is called when a Set Emergency message is received.
///
///   \param code Emergency code, currently only value is 0 which tells the
///               component to go into an emergency state.
///
///   \return JAUS_OK on success, otherwise JAUS-FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::SetEmergency(const UShort code)
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the emergency state of the component.  This method is called
///          whenever the Clear Emergency message is received.
///
///   \param code Emergency code to clear.
///
///   \return JAUS_OK on success, otherwise JAUS-FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::ClearEmergency(const UShort code)
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a message to query the authority of another component.
///
///   \param dest The destination ID of component to get authority level for.
///   \param auth The retrieved authority level value.
///   \param tms How long to wait in milliseconds for response.
///   \param tries How many re-try attempts to make to get value.
///
///   \return JAUS_OK on success, otherwise JAUS-FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::QueryComponentAuthorityLevel(const Address& dest,
                                            Byte& auth,
                                            const UShort tms,
                                            const UShort tries)
{
    Receipt receipt;
    Jaus::QueryComponentAuthority query;

    query.SetSourceID(mComponentID);
    query.SetDestinationID(dest);
    if(Send(&query, receipt, JAUS_REPORT_COMPONENT_AUTHORITY, tms, tries) == JAUS_OK)
    {
        auth = dynamic_cast<const ReportComponentAuthority*>(receipt.GetResponseMessage())->GetAuthorityCode();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is used to ensure certain types of data are not
///   buffered internally.  You can add a message type, and even tie the data
///   to a specific source of the information.
///
///   This method is extremely useful if you only want to the most recent of
///   a specific message received.  For example, if you are getting GPS data
///   and only care about the latest reading, you can make sure that only the
///   last Global Pose message received is in the message queue when you are
///   ready to process it.  Another example would be if you are getting the
///   video feed from a sensor and it takes a while to process the image.  This
///   method will make sure that you are not creating a bottleneck where you are
///   still processing a frame 30 seconds in the past!
///
///   \param[in] messageCode The type of message to not buffer.
///   \param[in] source The source of the message to not buffer.  Set to 
///                     [0, 0, 0, 0] if you don't care who the source of the
///                     message is.
///
///   \return OK on success, FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::AddToDoNotBufferList(const UShort messageCode, const Address& source)
{
    return mpMessageHandler->AddToDoNotBufferList(messageCode, source);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if connected to node manager, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsConnected() const
{
    if(mpConnectionHandler &&
        mpConnectionHandler->mOutbox.IsOpen() &&
        mpConnectionHandler->mOutbox.IsActive() &&
        mpConnectionHandler->mRegistry.IsRegistered(mComponentID))
    {
        return true;
    }

    // If outbox is not open, and we are initalized,
    // then re-open it and possible re-register
    // the component ID just in case.
    if(mComponentID.IsValid())
    {
        JSharedMemory* outbox = &mpConnectionHandler->mOutbox;
        JSharedMemory::Registry* registry = &mpConnectionHandler->mRegistry;
        if( outbox->IsOpen() == false )
        {
            outbox->OpenInbox(Address(mComponentID.mSubsystem, mComponentID.mNode, 1, 1));
        }
        if(registry->IsOpen() == false)
        {
            registry->OpenComponentRegistry(mComponentID.mSubsystem, mComponentID.mNode);
        }
        if(registry->IsOpen())
        {
            registry->Register(mComponentID);
        }
    }

    if(mpConnectionHandler &&
        mpConnectionHandler->mOutbox.IsOpen() &&
        mpConnectionHandler->mOutbox.IsActive() &&
        mpConnectionHandler->mRegistry.IsRegistered(mComponentID))
    {
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the component has been initialized, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsInitialized() const
{
    if(mComponentID.IsValid())
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the component can be controlled by other components.  If true
///           then this class will automatically handle control requests and
///           transitions.  False if component cannot be controlled (default value
///           for components).
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsControllable() const
{
    return mIsControllableFlag;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the component is being controlled exclusively by another
///           commanding component.  If not false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsControlled() const
{
    bool result = false;

    mComponentMutex.Enter();
    if(mControllerID.IsValid())
    {
        result = true;
    }
    mComponentMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks the services of the component to see if the input message
///          and presence vector are supported.
///
///   This method checks the internal list of component services setup through
///   the SetupService function or AddService function.  If you have not
///   added any messages/services to your component, then this function will fail.
///
///   By default, all core messages are supported.
///
///   \param messageCode The message type to check for within services.
///   \param presenceVector The presence vector associated with the message
///                         to check for support.
///
///   \return True if input message and presence vector supported, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsInputMessageSupported(const UShort messageCode, const UInt presenceVector) const
{
    bool result = false;
    Service::Map::const_iterator srvc;
    mComponentMutex.Enter();
    for(srvc = mComponentServices.begin();
        srvc != mComponentServices.end();
        srvc++)
    {
        if(srvc->second.IsInputSupported(Service::Message(messageCode, presenceVector)))
        {
            result = true;
            break;
        }
    }
    mComponentMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks the services of the component to see if the output message
///          and presence vector are supported.
///
///   This method checks the internal list of component services setup through
///   the SetupService function or AddService function.  If you have not
///   added any messages/services to your component, then this function will fail.
///
///   By default, all core messages are supported.
///
///   \param messageCode The message type to check for within services.
///   \param presenceVector The presence vector associated with the message
///                         to check for support.
///
///   \return True if output message and presence vector supported, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsOutputMessageSupported(const UShort messageCode, const UInt presenceVector) const
{
    bool result = false;
    Service::Map::const_iterator srvc;
    mComponentMutex.Enter();
    for(srvc = mComponentServices.begin();
        srvc != mComponentServices.end();
        srvc++)
    {
        if(srvc->second.IsOutputSupported(Service::Message(messageCode, presenceVector)))
        {
            result = true;
            break;
        }
    }
    mComponentMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the authority level needed to gain control/access to the
///          component.
///
///   \param auth The minimum level of authority needed to take control of the
///               component.
///
////////////////////////////////////////////////////////////////////////////////////
void Component::SetComponentAuthority(const Byte auth)
{
    mAuthorityCode = auth;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If a controller has release control, or you need to change control
///           (or remove being controlled), call this function to reset
///           the ID of the controlling component to (0, 0, 0, 0).
///
////////////////////////////////////////////////////////////////////////////////////
void Component::ClearControllerID()
{
    mComponentMutex.Enter();
    mControllerID(0, 0, 0, 0);
    mControllerAuthorityCode = 0;
    mComponentMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief  Prints information about the component to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void Component::PrintStatus() const
{
    static CxUtils::Mutex sPrintMutex;

    sPrintMutex.Lock();

    std::cout << GetName() << " - " << GetID().ToString() << " - ";
    switch(GetPrimaryStatus())
    {
    case Status::Ready:
        std::cout << "Ready" << std::endl;
        break;
    case Status::Standby:
        std::cout << "Standby" << std::endl;
        break;
    case Status::Shutdown:
        std::cout << "Shutdown" << std::endl;
        break;
    case Status::Failure:
        std::cout << "Failure" << std::endl;
        break;
    default:
        std::cout << "Initialized" << std::endl;
        break;
    }
    if(IsControlled())
    {
        std::cout << "Controlled by: " << GetControllerID().ToString() << std::endl;
    }

    sPrintMutex.Unlock();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Primary status of the component.
///
////////////////////////////////////////////////////////////////////////////////////
Component::Status::Primary Component::GetPrimaryStatus() const
{
    Component::Status::Primary stat;
    mComponentMutex.Enter();
    stat = (Component::Status::Primary)mPrimaryStatus;
    mComponentMutex.Leave();
    return stat;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list of all Node Managers running on the host machine.
///
///   If you are simulating multiple JAUS components on a single computer then
///   this method can be used to lookup all of the nodes running.
///
///   \param id List of Node Manager ID's on host machine.
///
///   \return Number of nodes found.
///
////////////////////////////////////////////////////////////////////////////////////
int Component::GetNodeManagerList(Address::List& id)
{
    JSharedMemory::Registry nodeRegistry;
    if(nodeRegistry.OpenNodeRegistry(0, 0))
    {
        Address::List registeredNodes;
        if(nodeRegistry.GetRegistry(registeredNodes))
        {
            if(registeredNodes.size() > 0)
            {
                id = registeredNodes;
            }
        }
    }

    return (int)(id.size());
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks if a node manager is running on the host machine.
///
///   \param id Optional value, if not NULL the value of any ID found for a node
///             manager is copied.
///
///   \return True if a node manager is running on the host machine, otherwise
///           false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Component::IsNodeManagerPresent(Address* id)
{
    JSharedMemory::Registry nodeRegistry;
    if(nodeRegistry.OpenNodeRegistry(0, 0))
    {
        Address::List registeredNodes;
        if(nodeRegistry.GetRegistry(registeredNodes))
        {
            if(registeredNodes.size() > 0)
            {
                if(id)
                {
                    *id = *registeredNodes.begin();
                }
                return true;
            }
        }
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Callback function for incomming data streams.
///
////////////////////////////////////////////////////////////////////////////////////
void Component::StreamCallbackFunction(const Stream& msg, const Header* header, const Byte transport, void *args)
{
    Component* cmp = (Component*)args;
    Header readHeader;

    if(header)
    {
        readHeader = *header;
    }
    else
    {
        if(msg.Read(readHeader, 0) == JAUS_FAILURE)
        {
            cmp->SetJausError(ErrorCodes::BadPacket);
            return;
        }
    }

    cmp->ProcessMessage(msg, readHeader);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of all services provided by component.
///
///   \return Copy of all services.
///
////////////////////////////////////////////////////////////////////////////////////
Service::Map Component::GetServices() const
{
    Service::Map copy;
    mComponentMutex.Enter();
    copy = mComponentServices;
    mComponentMutex.Leave();
    return copy;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to the Connection Handler being used by the
///          component for sending and receiving of messages.
///
///   \return Pointer to Connection Handler.
///
////////////////////////////////////////////////////////////////////////////////////
ConnectionHandler* Component::GetConnectionHandler() const
{
    return mpConnectionHandler;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to the Message Handler being used by the
///          component for storing of and prioritizing messages as they
///          are received by the Connection Handler.
///
///   \return Pointer to Message Handler.
///
////////////////////////////////////////////////////////////////////////////////////
MessageHandler* Component::GetMessageHandler() const
{
    return mpMessageHandler;
}


/*  End of File */
