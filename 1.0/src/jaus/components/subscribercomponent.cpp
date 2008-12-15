////////////////////////////////////////////////////////////////////////////////////
///
///  \file subscribercomponent.cpp
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
#include "jaus/components/subscribercomponent.h"
#include "jaus/messages/msglib.h"
#include <iostream>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SubscriberComponent::SubscriberComponent() : mSystemDiscoveryFlag(false),
                                             mSubsystemsToDiscover(NULL)
{
    Service core;
    core.LoadCoreMessageSupport();

    // Dynamic configuration messages.
    core.AddInputMessage(JAUS_QUERY_SERVICES, 0);
    core.AddInputMessage(JAUS_QUERY_IDENTIFICATION, 0);
    core.AddInputMessage(JAUS_REPORT_CONFIGURATION, 0);
    core.AddInputMessage(JAUS_REPORT_SUBSYSTEM_LIST, 0);
    core.AddOutputMessage(JAUS_REPORT_SERVICES, 0);
    core.AddOutputMessage(JAUS_REPORT_IDENTIFICATION, 0);
    core.AddOutputMessage(JAUS_QUERY_SUBSYSTEM_LIST, 0);
    core.AddOutputMessage(JAUS_QUERY_CONFIGURATION, 0);

    // Add messages for events.
    core.AddInputMessage(JAUS_CREATE_EVENT, 0xFF);
    core.AddInputMessage(JAUS_UPDATE_EVENT, 0xFF);
    core.AddInputMessage(JAUS_CANCEL_EVENT, 0x03);
    core.AddInputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
    core.AddInputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
    core.AddOutputMessage(JAUS_CREATE_EVENT, 0xFF);
    core.AddOutputMessage(JAUS_UPDATE_EVENT, 0xFF);
    core.AddOutputMessage(JAUS_CANCEL_EVENT, 0x03);
    core.AddOutputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
    core.AddOutputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
    core.AddOutputMessage(JAUS_EVENT, 0);

    AddService(core);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SubscriberComponent::~SubscriberComponent()
{
    Shutdown();
    mPeriodicEventThread.StopThread(500);
    TerminateServiceConnections();
    CancelRequestedEvents();
    // Make sure all is deleted.
    mEventManager.Lock();
    mEventManager.DeleteAllEvents();
    mEventManager.Unlock();
    Component::Shutdown();
    if(mSubsystemsToDiscover)
    {
        delete mSubsystemsToDiscover;
        mSubsystemsToDiscover = NULL;
    }
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
int SubscriberComponent::Initialize(const std::string& name, const Address& id, const unsigned int mbsize)
{
    if(Component::Initialize(name, id, mbsize))
    {
        mEventManager.Lock();
        mEventManager.SetComponentID(id);
        mEventManager.Unlock();
        // Override the callback for incomming messages so that we
        // can gaurantee that we update statistics for incoming service connections
        // and events.
        GetMessageHandler()->RegisterCallback(&SubscriberComponent::StreamCallbackFunction, this);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Performs additional Shutdown procedures before calling
///   the parent class Shutdown method.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::Shutdown()
{
    mPeriodicEventThread.StopThread(500);
    CancelRequestedEvents();
    // Make sure all is deleted.
    mEventManager.Lock();
    mEventManager.DeleteAllEvents();
    mEventManager.Unlock();
    TerminateServiceConnections();  

    if(mSubsystemsToDiscover)
    {
        delete mSubsystemsToDiscover;
        mSubsystemsToDiscover = NULL;
    }
    CxUtils::SleepMs(500);
    return Component::Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Resets the component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::Reset()
{
    TerminateServiceConnections();
    CancelRequestedEvents();
    mSubsystemsMutex.Enter();
    mSubsystems.clear();
    mSubsystemList.clear();
    mSubsystemsMutex.Leave();
    return Component::Reset();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called when a component is created.  It sets up
///   all core services supported by the component.
///
///   If a component ID is specified in the RA, it may report only one
///   service in beyond the core message support, and this ervice must be
///   equal to the component ID.  If a component ID is not listed in the
///   RA, it may report any number of services.  For example, an component
///   with ID 33 must provide only serive 33.  The exception to this rule
///   is component ID 1 (the Node Mnager) which may provide any number of
///   services in addition to core message support.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::SetupCoreService()
{
    Service::Map services;
    Service::Map::iterator coreServices;

    Component::SetupCoreService();

    services = GetServices();

    coreServices = services.find(Service::CoreMessageSupport);
    // Add additional message to core set.
    if(coreServices != services.end())
    {
        // Dynamic configuration messages.
        coreServices->second.AddInputMessage(JAUS_REPORT_CONFIGURATION, 0);
        coreServices->second.AddInputMessage(JAUS_REPORT_SUBSYSTEM_LIST, 0);
        coreServices->second.AddInputMessage(JAUS_REPORT_SERVICES, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_SUBSYSTEM_LIST, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_CONFIGURATION, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_SERVICES, 0);

        // Add messages for events.
        coreServices->second.AddInputMessage(JAUS_CREATE_EVENT, 0xFF);
        coreServices->second.AddInputMessage(JAUS_UPDATE_EVENT, 0xFF);
        coreServices->second.AddInputMessage(JAUS_CANCEL_EVENT, 0x03);
        coreServices->second.AddInputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
        coreServices->second.AddInputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_CREATE_EVENT, 0xFF);
        coreServices->second.AddOutputMessage(JAUS_UPDATE_EVENT, 0xFF);
        coreServices->second.AddOutputMessage(JAUS_CANCEL_EVENT, 0x03);
        coreServices->second.AddOutputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
        coreServices->second.AddOutputMessage(JAUS_EVENT, 0);

        // Add the updated version of the core services to component
        AddService(coreServices->second);
    }

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enable dynamic discovery of subsystems and there configurations.
///
///   As a subscriber, it is important to know the configuration of nodes and
///   subsystems, otherwise it is impossible to verify that events requested
///   are still valid, etc.  If this feature is enabled (by default it is false)
///   then configurations and identifications of subsystems are discovered
///   and any connection events are sent to the ProcessDiscoveryEvent function.
///   Events and Service Connections requested are also validated against the
///   system configuration data too.
///
///   \param on If true, discovery is enabled, otherwise false.
///   \param subsystems Optional parameter.  If a pointer to a set of
///          subsystem numbers is passed, then the class will only discover
///          configuration data about those subsystems (ignoring all others).
///          If this value is NULL (default) then information about all 
///          subsystems is found.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::EnableSubsystemDiscovery(const bool on, 
                                                  const std::set<Byte>* subsystems)
{
    int result = JAUS_OK;

    if(on)
    {
        mSubsystemsMutex.Enter();
        if(subsystems)
        {
            if(mSubsystemsToDiscover == NULL)
            {
                mSubsystemsToDiscover = new std::set<Byte>;
            }
            *mSubsystemsToDiscover = *subsystems;
            mSubsystemsToDiscover->insert(GetID().mSubsystem);
        }
        else if(mSubsystemsToDiscover)
        {
            delete mSubsystemsToDiscover;
            mSubsystemsToDiscover = NULL;
        }
        mSubsystemsMutex.Leave();

        if(mPeriodicEventThread.IsThreadActive())
        {
            result = JAUS_OK;
            mSystemDiscoveryFlag = true;
        }
        else
        {
            mSystemDiscoveryFlag = true;
            if(mPeriodicEventThread.CreateThread(&SubscriberComponent::PeriodicEventThreadFunction, this) == 0)
            {
                mSystemDiscoveryFlag = false;
                result = JAUS_FAILURE;
            }
        }
    }
    else
    {
        mSubsystemsMutex.Enter();
        mSubsystems.clear();
        mSubsystemList.clear();
        if(mSubsystemsToDiscover)
        {
            delete mSubsystemsToDiscover;
            mSubsystemsToDiscover = NULL;
        }
        mSubsystemsMutex.Leave();
        mSystemDiscoveryFlag = false;
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enable dynamic discovery of a specific system.
///
///   Enables discovery of a specific subsystems data including configuration,
///   identification, global pose, velocity.  Event types include connect,
///   update, and disconnect.
///
///   \param subsystemID ID of subsystem to discover information about.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::AddSubsystemToDiscover(const Byte subsystemID)
{
    int result = JAUS_OK;

    mSubsystemsMutex.Enter();

    if(mSubsystemsToDiscover == NULL)
    {
        mSubsystemsToDiscover = new std::set<Byte>();
    }
    mSubsystemsToDiscover->insert(subsystemID);

    mSubsystemsMutex.Leave();

    if(mPeriodicEventThread.IsThreadActive())
    {
        result = JAUS_OK;
        mSystemDiscoveryFlag = true;
    }
    else
    {
        mSystemDiscoveryFlag = true;
        if(mPeriodicEventThread.CreateThread(&SubscriberComponent::PeriodicEventThreadFunction, this) == 0)
        {
            mSystemDiscoveryFlag = false;
            result = JAUS_FAILURE;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a Query Services messages to all components on the subsystem.
///   All responses generated will be added to platform configurations stored
///   internally (and built using subsystem discovery).
///
///   \param subsystemID The subsystem to send the query to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::QuerySubsystemServices(const Byte subsystemID)
{
    QueryServices query;
    query.SetSourceID(GetID());
    query.SetDestinationID(Address(subsystemID, 255, 255, 255));
    return Send(&query);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for processing inform messages.  This method is called
///   whenever a inform message is received.  Overload this method to
///   add support for other input message types.
///
///   In addition to messages in the Core Component Service set, this function
///   processes Report Configuration, Report Identification messages, and
///   Report Subsystem List messages.
///
///   \param msg Inform message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::ProcessInformMessage(const Message* msg)
{
    int processed = JAUS_OK;

    switch(msg->GetCommandCode())
    {
    case JAUS_REPORT_SUBSYSTEM_LIST:
        {
            const ReportSubsystemList* report = dynamic_cast<const ReportSubsystemList*>(msg);
            if(report)
            {
                UpdateSubsystemList(report);
            }
        }
        break;
    case JAUS_REPORT_IDENTIFICATION:
        {
            const ReportIdentification* report = dynamic_cast<const ReportIdentification*>(msg);
            if(report && report->GetQueryType() == ReportIdentification::Subsystem)
            {                
                // Add the subsystem identification info.
                mSubsystemsMutex.Enter();
                mSubsystems[report->GetSourceID().mSubsystem].GetConfiguration()->AddComponent(report->GetSourceID());
                mSubsystems[report->GetSourceID().mSubsystem].SetIdentification(*report->GetIdentification());
                
                if(mSystemDiscoveryFlag)
                {
                    Platform platformCopy;
                    platformCopy = mSubsystems[report->GetSourceID().mSubsystem];
                    mSubsystemsMutex.Leave();
                    // Run callback outside of mutex.
                    ProcessDiscoveryEvent(platformCopy, SubsystemUpdate);
                }
                else
                {
                    mSubsystemsMutex.Leave();
                }
            }
        }
        break;
    case JAUS_REPORT_SERVICES:
        {
            const ReportServices* report = dynamic_cast<const ReportServices*>(msg);
            if(report)
            {                
                Configuration::Component* cmp;
                // Add the subsystem identification info.
                mSubsystemsMutex.Enter();
                mSubsystems[report->GetSourceID().mSubsystem].GetConfiguration()->AddComponent(report->GetSourceID());
                cmp = mSubsystems[report->GetSourceID().mSubsystem].GetConfiguration()->GetComponent(report->GetSourceID());
                if(cmp)
                {
                    if(cmp->mpServices)
                    {
                        *cmp->mpServices = *report->GetServices();
                    }
                    else
                    {
                        cmp->mpServices = new Service::Set(*report->GetServices());
                    }
                }
                
                if(mSystemDiscoveryFlag)
                {
                    Platform platformCopy;
                    platformCopy = mSubsystems[report->GetSourceID().mSubsystem];
                    mSubsystemsMutex.Leave();
                    // Run callback outside of mutex.
                    ProcessDiscoveryEvent(platformCopy, SubsystemUpdate);
                }
                else
                {
                    mSubsystemsMutex.Leave();
                }
            }
        }
        break;
    case JAUS_REPORT_CONFIGURATION:
        {
            const ReportConfiguration* report = dynamic_cast<const ReportConfiguration*>(msg);
            if(report)
            {
                Configuration::Subsystem* config;
                mSubsystemsMutex.Enter();
                mSubsystems[report->GetSourceID().mSubsystem].GetConfiguration()->AddComponent(report->GetSourceID());
                config = mSubsystems[report->GetSourceID().mSubsystem].GetConfiguration();
                if(config->mSubsystemID == 0)
                {
                    config->mSubsystemID = report->GetSourceID().mSubsystem;
                }
                // If more than one node, then this is a subsystem
                // configuration report, so overwrite existing config.
                if(report->GetConfiguration()->mNodes.size() > 1)
                {
                    *config = *report->GetConfiguration();
                }
                // If this is just a single node, then add it to
                // the platform configuration.
                else
                {
                    // If this message is from a Report Configuration event provider
                    // in our subsystem list, then take the whole thing, otherwise
                    // merge the configuration data.
                    if(report->GetSourceID().mSubsystem != GetID().mSubsystem &&
                        mSubsystemList.find(report->GetSourceID()) != mSubsystemList.end())
                    {
                        *config = *report->GetConfiguration();
                    }
                    else
                    {
                        Configuration::Node::Map::const_iterator node;
                        for(node = report->GetConfiguration()->mNodes.begin();
                            node != report->GetConfiguration()->mNodes.end();
                            node++)
                        {
                            config->mNodes[node->first] = node->second;
                        }
                    }
                }
                config->mSubsystemID = report->GetSourceID().mSubsystem;
                if(mSystemDiscoveryFlag)
                {
                    Platform platformCopy;
                    platformCopy = mSubsystems[report->GetSourceID().mSubsystem];
                    mSubsystemsMutex.Leave();
                    // Run callback outside of mutex.
                    ProcessDiscoveryEvent(platformCopy, SubsystemUpdate);
                }
                else
                {
                    mSubsystemsMutex.Leave();
                }
            }
        }
        break;
    case JAUS_REPORT_GLOBAL_POSE:
        {
            const ReportGlobalPose* report = dynamic_cast<const ReportGlobalPose*>(msg);
            if(report)
            {
                Platform::Map::iterator platform;
                mSubsystemsMutex.Enter();
                platform = mSubsystems.find(report->GetSourceID().mSubsystem);
                if(platform != mSubsystems.end())
                {
                    platform->second.AddGlobalPoseData(*(GlobalPose*)(report));
                    if(mSystemDiscoveryFlag)
                    {
                        Platform platformCopy;
                        platformCopy = platform->second;
                        mSubsystemsMutex.Leave();
                        // Run callback outside of mutex.
                        ProcessDiscoveryEvent(platformCopy, SubsystemUpdate);
                    }
                    else
                    {
                        mSubsystemsMutex.Leave();
                    }
                }
                else
                {
                    mSubsystemsMutex.Leave();
                }
            }   
        }
        break;
    default:
        processed = Component::ProcessInformMessage(msg);
        break;
    }

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the Create Event Request message, and verifies the
///   response received.  If the event was created successfully, it is
///   added to the Event Manager and kept track of.
///
///   Although it is not required that you use this method to create event
///   subscriptions, you should because it will automatically check for
///   confirmation and add the event to the Event Manager.  This is good because
///   the manager keeps track of all events and will automatically cancel them
///   on Shutdown/Reset.
///
///   \param request The event being requested.
///   \param eventInfo If not NULL, a copy of the created event is made.  This
///                    is helpful if you need the Event ID or Confirmed 
///                    Periodic Rate.
///   \param timeoutMs How long to wait for response before time out.
///   \param tries Number of send attempts within timeoutMs period.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::RequestEvent(const CreateEventRequest& request,
                                      Event* eventInfo,
                                      const UShort timeoutMs,
                                      const UShort tries)
{
    int result = JAUS_FAILURE;
    Receipt receipt;
    
    if(Send(&request, receipt, JAUS_CONFIRM_EVENT_REQUEST, timeoutMs, tries) == JAUS_OK)
    {
        const ConfirmEventRequest* confirm = dynamic_cast<const ConfirmEventRequest*>(receipt.GetResponseMessage());
        if(confirm && 
            confirm->GetRequestID() == request.GetRequestID())
        {
            result = JAUS_OK;
            Event newEvent(request);
            newEvent.SetEventProvider(confirm->GetSourceID());
            newEvent.GetEventSubscribers()->insert(GetID());
            newEvent.SetEventID(confirm->GetEventID());
            if(BitVector::IsBitSet(confirm->GetPresenceVector(), ConfirmEventRequest::VectorBit::ConfirmedPeriodicUpdateRate))
            {
                newEvent.SetPeriodicRate(confirm->GetConfirmedPeriodicUpdateRate());
            }
            if(eventInfo)
            {
                *eventInfo = newEvent;
            }
            // Add to event manager.
            mEventManager.Lock();
            if(mEventManager.AddEvent(newEvent) == JAUS_FAILURE)
            {
                // If event already exists, update timestamp.
                Event* evInfo = mEventManager.GetEvent(newEvent.GetKey());
                if(evInfo != NULL)
                {
                    evInfo->SetTimeStampMs(Time::GetUtcTimeMs());
                }
            }

            mEventManager.Unlock();

            if(result == JAUS_OK)
            {
                // If discovery is on, add the component
                // to the platforms configuration.
                if(mSystemDiscoveryFlag)
                {
                    Platform::Map::iterator platform;
                    mSubsystemsMutex.Enter();
                    mSubsystems[confirm->GetSourceID().mSubsystem].GetConfiguration()->AddComponent(confirm->GetSourceID());
                    mSubsystemsMutex.Leave();
                }
                if(mPeriodicEventThread.IsThreadActive() == false)
                {
                    mPeriodicEventThread.CreateThread(SubscriberComponent::PeriodicEventThreadFunction, this);
                    mPeriodicEventThread.SetThreadName("Periodic Events");
                }
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the Update Event Request message, and verifies the
///   response received.  If the event was updated successfully, it is
///   updated within the Event Manager.
///
///   Although it is not required that you use this method to update event
///   subscriptions, you should because this method, when used with the
///   Request Event function, will first check that the event exists, and then
///   update it within the Event Manager if the update is accepted by the
///   event provider.
///
///   \param request The event and update properties.
///   \param timeoutMs How long to wait for response before time out.
///   \param tries Number of send attempts within timeoutMs period.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::RequestEventUpdate(const UpdateEvent& request,
                                            const UShort timeoutMs,
                                            const UShort tries)
{
    int result = JAUS_FAILURE;

    Event* eventInfo;
    Receipt receipt;

    mEventManager.Lock();
    
    eventInfo = mEventManager.GetEvent(request);

    if(eventInfo && Send(&request, receipt, JAUS_CONFIRM_EVENT_REQUEST, timeoutMs, tries) == JAUS_OK)
    {
        const ConfirmEventRequest* confirm = dynamic_cast<const ConfirmEventRequest*>(receipt.GetResponseMessage());
        if(confirm && 
            confirm->GetRequestID() == request.GetRequestID())
        {
            result = JAUS_OK;

            eventInfo->CopyFrom(request);
            eventInfo->GetEventSubscribers()->insert(GetID());
            eventInfo->SetEventProvider(confirm->GetSourceID());

            if(BitVector::IsBitSet(confirm->GetPresenceVector(), ConfirmEventRequest::VectorBit::ConfirmedPeriodicUpdateRate))
            {
                eventInfo->SetPeriodicRate(confirm->GetConfirmedPeriodicUpdateRate());
            }
        }
    }

    mEventManager.Unlock();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates an inform type service connection with a service provider.
///
///   This function asks the service provider to create the SC specified.  It 
///   then verifies that the periodic rates are within acceptable parameters, and
///   if not sends a terminate message.  The SC is then added to internal
///   data members and maintained automatically.
///
///   If an SC created using this method stops updating, this class will
///   automatically try to re-create it.
///
///   \param provider The service provider.
///   \param messageCode The service desired.
///   \param presenceVector Presence vector associated with message code.
///   \param desiredPeriodicRate The desired periodic rate for the SC.  Set
///          to <= 0 if you don't care about the minimum rate.
///   \param minPeriodicRate The minimum acceptable periodic rate.
///   \param timeoutMs How long to wait for response before time out.
///   \param tries Number of send attempts within timeoutMs period.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::CreateInformServiceConnection(const Address& provider,
                                                       const UShort messageCode,
                                                       const UInt presenceVector,
                                                       const double desiredPeriodicRate,
                                                       const double minPeriodicRate,
                                                       const UShort timeoutMs,
                                                       const UShort tries)
{
    int result = JAUS_FAILURE;

    Jaus::CreateServiceConnection command;
    Receipt receipt;

    command.SetDestinationID(provider);
    command.SetSourceID(GetID());
    command.SetMessageCode(messageCode);
    command.SetPresenceVector(presenceVector);
    command.SetPeriodicUpdateRate(desiredPeriodicRate);

    if(Send(&command, receipt, JAUS_CONFIRM_SERVICE_CONNECTION, timeoutMs, tries) == JAUS_OK)
    {
        const Jaus::ConfirmServiceConnection* confirm = dynamic_cast<const Jaus::ConfirmServiceConnection*>(receipt.GetResponseMessage());
        if(confirm && confirm->GetResponseCode() == Jaus::ConfirmServiceConnection::CreatedSuccessfully)
        {
            if(confirm->GetConfirmedPeriodicUpdateRate() >= desiredPeriodicRate ||
                confirm->GetConfirmedPeriodicUpdateRate() >= minPeriodicRate ||
                minPeriodicRate <= 0)
            {
                // SUCCESS!
                result = JAUS_OK;
                bool existingMatch = false;                
                ServiceConnectionMap::iterator sc;
                mServiceConnectionsMutex.Enter();
                // If we already have this SC created, just update it
                // don't delete it.
                for(sc = mServiceConnections.begin();
                    sc != mServiceConnections.end();
                    sc++)
                {
                    if(sc->second.mProviderID == provider &&
                        sc->second.mInstanceID == confirm->GetInstanceID() &&
                        sc->second.mMessageCode == confirm->GetMessageCode() &&
                        sc->second.mPresenceVector == command.GetPresenceVector())
                    {
                        existingMatch = true;
                        sc->second.mUpdateTimeMs = Time::GetUtcTimeMs();
                        break;
                    }
                }
                // Create new SC.
                if(existingMatch == false)
                {
                    ServiceConnection newSC;
                    newSC.mPresenceVector = presenceVector;
                    newSC.mMessageCode = messageCode;
                    newSC.mProviderID = provider;
                    newSC.mPeriodicRate = confirm->GetConfirmedPeriodicUpdateRate();
                    newSC.mRequestorID = GetID();
                    newSC.mInstanceID = confirm->GetInstanceID();
                    mServiceConnections[newSC.GetKey()] = newSC;
                }
                mServiceConnectionsMutex.Leave();
            }
            else
            {
                Jaus::TerminateServiceConnection terminate;
                terminate.SetSourceID(GetID());
                terminate.SetDestinationID(provider);
                terminate.SetInstanceID(confirm->GetInstanceID());
                terminate.SetMessageCode(confirm->GetMessageCode());
                Send(&terminate);
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Any time a discovery event happens, this method is called.  Overload
///   it to add additional functionality.
///
///   \param platform Platform information to process.
///   \param eventType The type of discovery event.  SubsystemConnected, 
///                    SubsystemDisconnect, or SubsystemUpdate.
///
///   \return JAUS_OK.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::ProcessDiscoveryEvent(const Platform& platform,
                                               const DiscoveryEvents eventType)
{
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Cancels all events requested by this component that are stored 
///   within the Event Manager.
///
///   To cancel individual events, use the GetEvents function to get a pointer
///   to the event manager, and use it to lookup or stop individual events.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::CancelRequestedEvents()
{
    int result = JAUS_FAILURE;
    mEventManager.Lock();
    result = mEventManager.CancelEventSubscriptions(this->GetConnectionHandler());
    mEventManager.Unlock();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Cancels all events requested from a specific service.
///
///   \param id The ID of the component to cancel all event subscriptions from.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE if no events to cancel
///           or could not cancel.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::CancelEvents(const Address& id)
{
    int result = JAUS_FAILURE;
    Jaus::CancelEvent command;
    Receipt receipt;
    Event::Map* subscriptions = NULL;
    Event::Map::iterator sevent;
    mEventManager.Lock();
    subscriptions = mEventManager.GetEventSubscriptions();
    sevent = subscriptions->begin();
    while(sevent != subscriptions->end())
    {
        if(sevent->first.mSourceID == id)
        {                  
            command.SetSourceID(GetID());
            command.SetDestinationID(sevent->first.mSourceID);
            command.SetEventID(sevent->first.mID);
            command.SetRequestID(EventManager::GenerateRequestID());
            command.SetMessageCode(sevent->first.mMessageCode);
            if(Send(&command, receipt, JAUS_CONFIRM_EVENT_REQUEST) == JAUS_OK)
            {
                const Jaus::ConfirmEventRequest* confirm = 
                    dynamic_cast<const Jaus::ConfirmEventRequest*>(receipt.GetResponseMessage());
                if(confirm && confirm->GetRequestID() == command.GetRequestID())
                {
                    result = JAUS_OK;
#ifdef WIN32
                    sevent = subscriptions->erase(sevent);
#else 
                    subscriptions->erase(sevent);
#endif
                }
                else
                {
                    // Failed, don't remove from list, because
                    // it may still get removed during Shutdown.
                    sevent++;
                }
            }
            else
            {
                // Failed, don't remove from list, because
                // it may still get removed during Shutdown.
                sevent++;
            }
        }
        else
        {
            sevent++;
        }
    }
    mEventManager.Unlock();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Activates a specific service we have requested.
///
///   This method only works with services we have requested (services this
///   component is not providing).
///
///   \param sc Ther service connection to activate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::ActivateServiceConnection(const ServiceConnection& sc)
{
    int result = JAUS_FAILURE;
    ServiceConnectionMap::iterator scIter;    

    // We cannot cancel services we are providing, only
    // the service requestor or node manager can.
    if(sc.mProviderID == GetID())
    {
        return JAUS_FAILURE;
    }

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();
    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mInstanceID == sc.mInstanceID &&
            scIter->second.mMessageCode == sc.mMessageCode &&
            scIter->second.mProviderID == sc.mProviderID)
        {
            Jaus::ActivateServiceConnection command;
            Jaus::Receipt receipt;

            command.SetSourceID(GetID());
            command.SetDestinationID(sc.mProviderID);
            command.SetInstanceID(sc.mInstanceID);
            command.SetMessageCode(sc.mMessageCode);
            command.SetAckNack(Header::AckNack::Request);
            
            if(Send(&command, receipt, JAUS_ACTIVATE_SERVICE_CONNECTION) == JAUS_OK)
            {
                // If it is an inform message, then I can say it
                // is not suspended for me.  However, if the SC was a
                // command SC, I may not be suspended on my side, but
                // it is up to the Node Manager to determine if I am
                // the active commander or not.
                if(IsInformMessage(sc.mMessageCode))
                {
                    scIter->second.mPrimaryStatus = ServiceConnection::Active;
                }
                
                result = JAUS_OK;
            }

            break;
        }
        else
        {
            scIter++;
        }
    }

    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Suspends a specific service we have requested.
///
///   This method only works with services we have requested (services this
///   component is not providing).
///
///   \param sc Ther service connection to suspend.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::SuspendServiceConnection(const ServiceConnection& sc)
{
    int result = JAUS_FAILURE;
    ServiceConnectionMap::iterator scIter;    

    // We cannot cancel services we are providing, only
    // the service requestor or node manager can.
    if(sc.mProviderID == GetID())
    {
        return JAUS_FAILURE;
    }

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();

    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mInstanceID == sc.mInstanceID &&
            scIter->second.mMessageCode == sc.mMessageCode &&
            scIter->second.mProviderID == sc.mProviderID)
        {
            Jaus::SuspendServiceConnection command;
            Jaus::Receipt receipt;

            command.SetSourceID(GetID());
            command.SetDestinationID(sc.mProviderID);
            command.SetInstanceID(sc.mInstanceID);
            command.SetMessageCode(sc.mMessageCode);
            command.SetAckNack(Header::AckNack::Request);
            
            if(Send(&command, receipt, JAUS_SUSPEND_SERVICE_CONNECTION) == JAUS_OK)
            {
                scIter->second.mPrimaryStatus = ServiceConnection::Suspended;
                result = JAUS_OK;
            }
            break;
        }
        else
        {
            scIter++;
        }
    }

    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Terminates a specific service we have requested.
///
///   <b>This function does not cancel service connections being provided by
///   this component</b>.  Service connections can only be terminated by
///   the Node Manager or the requesting component.  To stop services being
///   provided, then Shutdown or Restart the component.
///
///   \param sc Ther service connection to cancel.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::TerminateServiceConnection(const ServiceConnection& sc)
{
    int result = JAUS_FAILURE;
    ServiceConnectionMap::iterator scIter;    

    // We cannot cancel services we are providing, only
    // the service requestor or node manager can.
    if(sc.mProviderID == GetID())
    {
        return JAUS_FAILURE;
    }

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();
    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mInstanceID == sc.mInstanceID &&
            scIter->second.mMessageCode == sc.mMessageCode &&
            scIter->second.mProviderID == sc.mProviderID)
        {
            Jaus::TerminateServiceConnection terminate;
            Jaus::Receipt receipt;

            terminate.SetSourceID(GetID());
            terminate.SetDestinationID(sc.mProviderID);
            terminate.SetInstanceID(sc.mInstanceID);
            terminate.SetMessageCode(sc.mMessageCode);
            terminate.SetAckNack(Header::AckNack::Request);
            
            if(Send(&terminate, receipt, JAUS_TERMINATE_SERVICE_CONNECTION) == JAUS_OK)
            {
#ifdef WIN32
                scIter = mServiceConnections.erase(scIter);                
#else
                mServiceConnections.erase(scIter);
                scIter = mServiceConnections.begin();
#endif
                result = JAUS_OK;
            }
            break;
        }
        else
        {
            scIter++;
        }
    }

    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Cancels all service connections that the component has requested by
///   notifying the service provider to terminate (Terminate Service Connection
///   message).
///
///   <b>This function does not cancel service connections being provided by
///   this component</b>.  Service connections can only be terminated by
///   the Node Manager or the requesting component.  To stop services being
///   provided, then Shutdown or Restart the component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::TerminateServiceConnections()
{
    Jaus::TerminateServiceConnection terminate;
    Jaus::Receipt receipt;
    int total = 0, expected = 0;
    int result = JAUS_FAILURE;
    ServiceConnectionMap::iterator sc;
    terminate.SetSourceID(GetID());

    mServiceConnectionsMutex.Enter();
    sc = mServiceConnections.begin();
    while(sc != mServiceConnections.end())
    {
        // If this component is the service provider, then
        // we want to tell the requestor of termination.
        if(sc->second.mProviderID == GetID())
        {
            // Only the Node Manager, or the requesting
            // component can terminate a service connection, 
            // that this component is providing.
            sc++;
            continue;
        }
        else
        {
            terminate.SetDestinationID(sc->second.mProviderID);
            terminate.SetMessageCode(sc->second.mMessageCode);
            terminate.SetInstanceID(sc->second.mInstanceID);
            expected++;
            total += Send(&terminate, receipt) != JAUS_FAILURE ? 1 : 0;
            // Remove the SC from map.
#ifdef WIN32
            sc = mServiceConnections.erase(sc);
#else
            mServiceConnections.erase(sc);
            sc = mServiceConnections.begin();
#endif
        }
        
    }
    if(total == expected) { result = JAUS_OK; }
    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Cancels all service connections that the component has requested by
///   notifying the service provider to terminate (Terminate Service Connection
///   message) from a specific provider.
///
///   <b>This function does not cancel service connections being provided by
///   this component</b>.  Service connections can only be terminated by
///   the Node Manager or the requesting component.  To stop services being
///   provided, then Shutdown or Restart the component.
///
///   \param provider The SC provider to terminate SC's from.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::TerminateServiceConnections(const Address& provider)
{
    Jaus::TerminateServiceConnection terminate;
    Jaus::Receipt receipt;
    int total = 0, expected = 0;
    int result = JAUS_FAILURE;
    ServiceConnectionMap::iterator sc;
    terminate.SetSourceID(GetID());

    mServiceConnectionsMutex.Enter();
    sc = mServiceConnections.begin();
    while(sc != mServiceConnections.end())
    {
        // If this component is the service provider, then
        // we want to tell the requestor of termination.
        if(sc->second.mProviderID == GetID() ||
            sc->second.mProviderID != provider)
        {
            // Only the Node Manager, or the requesting
            // component can terminate a service connection, 
            // that this component is providing.
            sc++;
            continue;
        }
        else if(sc->second.mProviderID == provider)
        {
            terminate.SetDestinationID(sc->second.mProviderID);
            terminate.SetMessageCode(sc->second.mMessageCode);
            terminate.SetInstanceID(sc->second.mInstanceID);
            expected++;
            total += Send(&terminate, receipt) != JAUS_FAILURE ? 1 : 0;
            // Remove the SC from map.
#ifdef WIN32
            sc = mServiceConnections.erase(sc);
#else
            mServiceConnections.erase(sc);
            sc = mServiceConnections.begin();
#endif
        }   
        else
        {
            sc++;
        }
    }
    if(total == expected) { result = JAUS_OK; }
    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks the Event Manager to see if there are any event subscriptions
///   of a specific type of message.
///
///   \param messageCode The type of message to check for events of.
///
///   \return True if an event has been requested of the type asked.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::HaveEventSubscriptionsOfType(const UShort messageCode) const
{
    bool result = false;
    mEventManager.Lock();
    result = mEventManager.GetEventSubscriptionsOfType(messageCode).size() > 0 ? true : false;
    mEventManager.Unlock();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Check if an event subscription from a service provder exists.
///
///   \param source The source/provider of the event.
///   \param messageCode The type of message to check for events of.
///
///   \return True if the event exists, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::HaveEventSubscription(const Address& source,
                                                const UShort messageCode) const
{
    bool result = false;
    mEventManager.Lock();
    result = mEventManager.GetEvent(source, messageCode) != NULL ? true : false;
    mEventManager.Unlock();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Use this method to check if an SC is present.
///
///   \param provider The ID of the service provider.
///   \param messageCode The type of service connection.
///
///   \return True if SC exists, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::HaveServiceConnection(const Address& provider,
                                                const UShort messageCode) const
{
    bool result = false;
    ServiceConnectionMap::const_iterator scIter;    

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();

    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mProviderID == provider &&
            scIter->second.mMessageCode == messageCode)
        {
            result = true;
            break;
        }
        else
        {
            scIter++;
        }
    }

    mServiceConnectionsMutex.Leave();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Use this method to check if an SC is present.
///
///   \param provider The ID of the service provider.
///   \param messageCode The type of SC.
///   \param scInfo Copy of the service connection information found.
///
///   \return True if SC exists, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::HaveServiceConnection(const Address& provider,
                                                const UShort messageCode,
                                                ServiceConnection& scInfo) const
{
    bool result = false;
    ServiceConnectionMap::const_iterator scIter;    

    mServiceConnectionsMutex.Enter();
    scIter = mServiceConnections.begin();

    while(scIter != mServiceConnections.end())
    {
        if(scIter->second.mProviderID == provider &&
            scIter->second.mMessageCode == messageCode)
        {
            result = true;
            scInfo = scIter->second;
            break;
        }
        else
        {
            scIter++;
        }
    }

    mServiceConnectionsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called whenever the SubscriberComponent class
///          detects that a Service Connection being requested (Inform type)
///          is no longer updating.  This function returns true to indicate
///          that the service connection should be re-created, and false to
///          remove the service connection subscription.
///
///   If an inform type service connection has been created to a providing
///   component, the data arrives at a fixed rate.  If the data stops
///   arriving at this rate, then this class detects that it is no longer
///   updating and calls this method.  If the method returns true, the 
///   SubscriberComponent class will try to re-create the service connection.  If
///   this function returns false, the service connection is terminated.
///
///   Overload this method to change the default behavior for specific
///   SC's.  By default, this method returns true, meaning this class attempts
///   to re-establish all service connections.
///
///   \param sc The service connection that is no longer updating.
///
///   \return True if you want to re-establish the SC, otherwise false to
///           terminate.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::ReEstablishServiceConnection(const ServiceConnection& sc) const
{
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the class detects that an event has been lost because 
///   the component producing it has disconnected/shutdown or stopped updating
///   (for periodic events) this method is called.  
///
///   Based on the return value of function, the class will try to
///   re-create the event the next time communication with the 
///   providing component can be re-established.
///
///   \return True if you want to re-establish the event, otherwise false to
///           terminate.
///
////////////////////////////////////////////////////////////////////////////////////
bool SubscriberComponent::ReEstablishEvent(const Event* eventInfo) const
{
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Number of service connections being generated and suscribed to.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int SubscriberComponent::GetNumServiceConnections() const
{
    unsigned int count = 0;
    mServiceConnectionsMutex.Enter();
    count = (unsigned int)mServiceConnections.size();
    mServiceConnectionsMutex.Leave();
    return count;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Copy of all service connection data.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnectionMap SubscriberComponent::GetServiceConnections() const
{
    ServiceConnectionMap scMap;

    mServiceConnectionsMutex.Enter();
    scMap = mServiceConnections;
    mServiceConnectionsMutex.Leave();

    return scMap;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks to see if any service connection subscription has stopped
///          updating, and if so, attempts to re-create the service connection.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::CheckServiceConnections()
{
    double delay = 0;
    double scTime = 0.0;
    double curTime = Time::GetUtcTimeMs();
    ServiceConnectionMap::iterator sc;

    mServiceConnectionsMutex.Enter();

    sc = mServiceConnections.begin();
    while(sc != mServiceConnections.end())
    {
        if(IsInformMessage(sc->second.mMessageCode) &&
            sc->second.mProviderID != GetID() &&
            sc->second.mPrimaryStatus != ServiceConnection::Suspended)
        {
            scTime = sc->second.mUpdateTimeMs;
            delay = 500 + 1000.0/(sc->second.mPeriodicRate + JAUS_EPSILON);
            if(sc->second.mSequenceNumber > 1 && (curTime - scTime) > delay)
            {
                if(ReEstablishServiceConnection(sc->second) == true)
                {
                    Jaus::CreateServiceConnection command;
                    Jaus::Receipt receipt;

                    command.SetSourceID(GetID());
                    command.SetDestinationID(sc->second.mProviderID);
                    command.SetMessageCode(sc->second.mMessageCode);
                    command.SetPeriodicUpdateRate(sc->second.mPeriodicRate);
                    command.SetPresenceVector(sc->second.mPresenceVector);

                    if(Send(&command, receipt, JAUS_CONFIRM_SERVICE_CONNECTION, 250, 1) == JAUS_OK)
                    {
                        const Jaus::ConfirmServiceConnection* confirm = dynamic_cast<const Jaus::ConfirmServiceConnection*>(receipt.GetResponseMessage());
                        if(confirm->GetResponseCode() == Jaus::ConfirmServiceConnection::CreatedSuccessfully)
                        {
                            // Update instance ID in case it changed.
                            sc->second.mInstanceID = confirm->GetInstanceID();
                            sc->second.mSecondaryStatus = ServiceConnection::Active;
                        }
                        else
                        {
                            sc->second.mSecondaryStatus = ServiceConnection::NotUpdating;
                        }
                    }
                }
                else
                {
                    // Send a terminate message just in case.
                    Jaus::TerminateServiceConnection terminate;
                    terminate.SetSourceID(GetID());
                    terminate.SetDestinationID(sc->second.mProviderID);
                    terminate.SetInstanceID(sc->second.mInstanceID);
                    terminate.SetMessageCode(sc->second.mMessageCode);
                    Send(&terminate);
#ifdef WIN32
                    sc = mServiceConnections.erase(sc);
#else
                    mServiceConnections.erase(sc);
                    sc = mServiceConnections.begin();
#endif
                    continue;
                }
            }
        }
        sc++;
    }

    mServiceConnectionsMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Verifies existing events against system configuration, and if needed
///          removes them.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::CheckEvents()
{
    double delay = 0;
    double curTime = Time::GetUtcTimeMs();
    Event::Map::iterator eventItr;

    mEventManager.Lock();

    eventItr = mEventManager.GetEventSubscriptions()->begin();
    while(eventItr != mEventManager.GetEventSubscriptions()->end())
    {
        // If discovery is enabled, or the component
        // is part of our subsystem, then we should
        // have knowledge of whether or not the component
        // is still exists on the subsystem or not.
        if(mSystemDiscoveryFlag || eventItr->second->GetEventProvider().mSubsystem == GetID().mSubsystem)
        {
            // Check to see if the producing component
            // still exits on the network.
            bool lost = false;
            Platform::Map::iterator platform;
            mSubsystemsMutex.Enter();
            platform = mSubsystems.find(eventItr->second->GetEventProvider().mSubsystem);
            if(platform == mSubsystems.end())
            {
                lost = true;
            }
            else
            {
                if(platform->second.GetConfiguration()->HaveComponent(eventItr->second->GetEventProvider()) == false)
                {
                    lost = true;
                }
            }                
            mSubsystemsMutex.Leave();
            if(lost)
            {
                // If we want to re-establish this event, then
                // we'll add it to our list of lost events.
                if(ReEstablishEvent(eventItr->second))
                {
                    mLostEvents.push_back(*eventItr->second);
                }
#ifdef WIN32
                eventItr = mEventManager.GetEventSubscriptions()->erase(eventItr);
#else
                mEventManager.GetEventSubscriptions()->erase(eventItr);
#endif
                continue;
            }                
        }
        // If we don't have system discovery on, we 
        // can detect loss of periodic events because they'll
        // stop updating at the rate desired.
        else if(eventItr->second->IsPeriodic())
        {
            delay = 500 + 1000.0/(eventItr->second->GetPeriodicRate() + JAUS_EPSILON);
            if(eventItr->second->GetSequenceNumber() > 1 && 
                (curTime - eventItr->second->GetTimeStampMs()) > delay)
            {
                if(ReEstablishEvent(eventItr->second) == true)
                {
                    CreateEventRequest command;
                    Receipt receipt;

                    command.SetSourceID(GetID());
                    command.SetDestinationID(eventItr->second->GetEventProvider());
                    eventItr->second->CopyTo(command);
                    
                    if(Send(&command, receipt, JAUS_CONFIRM_SERVICE_CONNECTION, 250, 1) == JAUS_OK)
                    {
                        eventItr->second->SetTimeStampMs(Time::GetUtcTimeMs());
                    }
                }
                else
                {
                    CancelEvent command;
                    command.SetSourceID(GetID());
                    command.SetDestinationID(eventItr->second->GetEventProvider());
                    command.SetMessageCode(eventItr->second->GetMessageCode());
                    command.SetEventID(eventItr->second->GetEventID());
                    command.SetRequestID(EventManager::GenerateRequestID());
                    Send(&command);
#ifdef WIN32
                    eventItr = mEventManager.GetEventSubscriptions()->erase(eventItr);
#else
                    mEventManager.GetEventSubscriptions()->erase(eventItr);
#endif
                    continue;
                }
            }
        }

        eventItr++;
    }

    mEventManager.Unlock();

    // Go through the list of lost events, and see
    // if we can re-establish them.
    Event::List::iterator lost;
    lost = mLostEvents.begin();
    while(lost != mLostEvents.end())
    {
        bool retry = false;
        Platform::Map::iterator platform;
        mSubsystemsMutex.Enter();
        platform = mSubsystems.find(lost->GetEventProvider().mSubsystem);
        if(platform != mSubsystems.end())
        {
            if(platform->second.GetConfiguration()->HaveComponent(lost->GetEventProvider()) == true)
            {
                retry = true;
            }
        }
        mSubsystemsMutex.Leave();

        if(retry)
        {
            // Try re-create the event since the component
            // is available again.
            CreateEventRequest command;
            command.SetSourceID(GetID());
            command.SetDestinationID(lost->GetEventProvider());
            lost->CopyTo(command);
            if(RequestEvent(command, 0, 250, 1) == JAUS_OK)
            {
                lost = mLostEvents.erase(lost);
            }
            else
            {
                lost++;
            }
        }
        else
        {
            lost++;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If Subsystem discovery is enabled, this function is used
///   used to check events for the subsystem configuration and list.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::CheckDiscoveryEvents()
{
    if(IsConnected() == false)
    {
        // If not connected, all events
        // are invalid.
        mEventManager.Lock();
        mEventManager.CancelEventSubscriptions(GetConnectionHandler());
        mEventManager.CancelProducedEvents(GetConnectionHandler());
        mEventManager.Unlock();
    }
    else
    {
        Event* eventInfo = NULL;
        QueryEvents query;
        Receipt receipt;            
        bool haveSubsystemListEvent = false;
        bool haveConfigurationEvent = false;

        query.SetSourceID(GetID());
        query.SetDestinationID(Address(GetID().mSubsystem, GetID().mNode, 1, 1));
        query.SetEventType(QueryEvents::EveryChange);

        // Only request subsystem list if performing discovery.
        if(mSystemDiscoveryFlag)
        {
            query.SetMessageCode(JAUS_REPORT_SUBSYSTEM_LIST);

            
            mEventManager.Lock();
            // Check and see if we have an event with
            // the node manager for the subsystem list.
            if((eventInfo = mEventManager.GetEvent(Address(GetID().mSubsystem, 
                                                   GetID().mNode, 
                                                   1,
                                                   1),
                                                   JAUS_REPORT_SUBSYSTEM_LIST)) != NULL)
            {         
                query.SetEventID(eventInfo->GetEventID());
                haveSubsystemListEvent = true;
            }
            mEventManager.Unlock();

            // Double check that we have the event by sending a query.
            if(Send(&query, receipt, JAUS_REPORT_EVENTS, 100, 1) == JAUS_OK)
            {
                const ReportEvents* report = dynamic_cast<const ReportEvents*>(receipt.GetResponseMessage());
                if(report && report->GetEvents()->size() == 0)
                {
                    haveSubsystemListEvent = false;
                }
            }

            if(!haveSubsystemListEvent)
            {
                // Create an event for the subsystem list.
                CreateEventRequest command;
                command.SetDestinationID(Address(GetID().mSubsystem,
                    GetID().mNode, 1, 1));
                command.SetSourceID(GetID());
                command.SetMessageCode(JAUS_REPORT_SUBSYSTEM_LIST);
                command.SetEventType(CreateEventRequest::EveryChange);
                command.SetRequestID(EventManager::GenerateRequestID());
                RequestEvent(command, NULL, 50, 1);
            }
        }

        // Now check for report configuration event for this subsystem.
        query.SetMessageCode(JAUS_REPORT_CONFIGURATION);

        // See if we have event, and get the event ID
        mEventManager.Lock();
        // Check and see if we have an event with
        // the node manager for the subsystem configuration.
        if((eventInfo = mEventManager.GetEvent(Address(GetID().mSubsystem, 
                                               GetID().mNode, 
                                               1,
                                               1),
                                               JAUS_REPORT_CONFIGURATION)) != NULL)
        {         
            query.SetEventID(eventInfo->GetEventID());
            haveConfigurationEvent = true;
        }
        mEventManager.Unlock();

        // Double check that we have the event by sending a query.
        if(Send(&query, receipt, JAUS_REPORT_EVENTS, 100, 1) == JAUS_OK)
        {
            const ReportEvents* report = dynamic_cast<const ReportEvents*>(receipt.GetResponseMessage());
            if(report && report->GetEvents()->size() == 0)
            {
                haveConfigurationEvent = false;
            }
            else
            {
                bool match = false;
                Event::List::const_iterator elist;
                for(elist = report->GetEvents()->begin();
                    elist != report->GetEvents()->end();
                    elist++)
                {
                    if(elist->GetMessageCode() == JAUS_REPORT_CONFIGURATION &&
                        elist->GetQueryMessage())
                    {
                        const QueryConfiguration* query = dynamic_cast<const QueryConfiguration*>(elist->GetQueryMessage());
                        if(query && query->GetQueryField() == QueryConfiguration::Subsystem)
                        {
                            match = true;
                            break;
                        }
                    }
                }
                if(match == false)
                {
                    haveConfigurationEvent = false;
                }
            }
        }            

        // Check and see if we have an event with
        // the node manager for the subsystem configuration.
        if(!haveConfigurationEvent)
        {                    
            // Create an event for the subsystem list.
            CreateEventRequest command;
            QueryConfiguration query;
            command.SetDestinationID(Address(GetID().mSubsystem,
                GetID().mNode, 1, 1));
            command.SetSourceID(GetID());
            command.SetMessageCode(JAUS_REPORT_CONFIGURATION);
            command.SetEventType(CreateEventRequest::EveryChange);
            command.SetRequestID(EventManager::GenerateRequestID());
            query.SetQueryField(QueryConfiguration::Subsystem);
            command.SetQueryMessage(&query);
            RequestEvent(command, NULL, 50, 1);
        }

        // Go through the subsystem list and verify we have
        // all the data we need.
        Platform::Map::iterator platform;
        
        mSubsystemsMutex.Enter();
        for(platform = mSubsystems.begin();
            platform != mSubsystems.end();
            platform++)
        {
            // If we don't have identification data yet, or we don't
            // have complete subsystem identification send a query.
            if(platform != mSubsystems.end() &&
                platform->second.HaveIdentification() == false ||
                (platform->second.HaveIdentification() &&
                    platform->second.GetIdentification()->GetIdentification().empty() ||
                    platform->second.GetIdentification()->GetType() != ReportIdentification::Subsystem))
            {
                Address::Set::iterator id;
                for(id = mSubsystemList.begin();
                    id != mSubsystemList.end();
                    id++)
                {
                    if(id->mSubsystem == platform->second.GetSubsystemID())
                    {
                        QueryIdentification queryIdent;
                        queryIdent.SetSourceID(GetID());
                        queryIdent.SetDestinationID(*id);
                        queryIdent.SetQueryType(QueryIdentification::Subsystem);                    
                        Send(&queryIdent);
                    }
                }
            }
        }
        mSubsystemsMutex.Leave();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Updates system configuration and generates discovery events
///          based on the Report Subsystem List message.
///
///   \param report Report Subsystem List message.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::UpdateSubsystemList(const ReportSubsystemList* report)
{
    Address::Set newSubsystems;
    Address::Set lostComponents;
    Platform::Map::iterator platform;
    Platform::Map lostSubsystems;
    Address::Set::const_iterator id;
    Address::Set::const_iterator id2;
    mSubsystemsMutex.Enter();   

    // Check for lost subsystem
    for(id = mSubsystemList.begin();
        id != mSubsystemList.end();
        id++)
    {
        // Check to see if the entire subsystem
        // is gone so we can delete all events for it.
        Address::Set::const_iterator id2;
        bool match = false;
        for(id2 = report->GetSubsystemList()->begin();
            id2 != report->GetSubsystemList()->end();
            id2++)
        {
            if(id->mSubsystem == id2->mSubsystem)
            {
                match = true;
                break;
            }
        }
        if(match == false)
        {
            // Remove the subsystem from our map, and add to
            // lost platforms.
            platform = mSubsystems.find(id->mSubsystem);
            if(platform != mSubsystems.end())
            {
                lostSubsystems[platform->first] = platform->second;
                mSubsystems.erase(platform);
            }
        }
        // Always add to list of lost components if not found.
        if(report->GetSubsystemList()->find(*id) == report->GetSubsystemList()->end())
        {
            lostComponents.insert(*id);
        }
    }

    // Check for new subsystems
    for(id = report->GetSubsystemList()->begin();
        id != report->GetSubsystemList()->end();
        id++)
    {
        if(mSubsystemList.find(*id) == mSubsystemList.end())
        {            
            // Check for duplicates.
            bool duplicate = false;
            for(id2 = report->GetSubsystemList()->begin();
                id2 != report->GetSubsystemList()->end();
                id2++)
            {
                if(id2 != id && id2->mSubsystem == id->mSubsystem)
                {
                    duplicate = true;
                    break;
                }
            }
            if(duplicate) { continue; }

            if(mSubsystemsToDiscover == NULL ||
                mSubsystemsToDiscover->find(id->mSubsystem) != mSubsystemsToDiscover->end())
            {
                newSubsystems.insert(*id);
                // Add to the subsystem configuration.
                mSubsystems[id->mSubsystem].GetConfiguration()->AddComponent(*id);
            }
        }
    }    

    mSubsystemList = *report->GetSubsystemList();

    mSubsystemsMutex.Leave();

    std::set<Byte>::iterator ss;
    mEventManager.Lock();
    
    // Now cancel  events from the lost subsystems.
    for(platform = lostSubsystems.begin();
        platform != lostSubsystems.end();
        platform++)
    {
        // Delete all requested events from the subsystem.
        mEventManager.DeleteComponentsFromSubsystem(platform->first);
    }

    // Now cancel events from the lost components.
    for(id = lostComponents.begin();
        id != lostComponents.end();
        id++)
    {
        // If node manager is gone, then
        // delete everything from that node.
        if(id->mComponent == 1)
        {
            mEventManager.DeleteComponentsFromNode(id->mSubsystem, id->mNode);
        }
        else
        {
            mEventManager.DeleteComponent(*id);
        }
        
    }
    mEventManager.Unlock();
    
    CreateEventRequest request;
    QueryConfiguration queryConfig;
    QueryIdentification queryIdent;

    request.SetSourceID(GetID());
    queryConfig.SetSourceID(GetID());
    queryIdent.SetSourceID(GetID());
    queryIdent.SetQueryType(QueryIdentification::Subsystem);

    request.SetEventType(CreateEventRequest::EveryChange);
    request.SetMessageCode(JAUS_REPORT_CONFIGURATION);
    queryConfig.SetQueryField(QueryConfiguration::Subsystem);
    request.SetQueryMessage(&queryConfig);
    request.SetRequestID(GetID().mInstance + GetID().mComponent);

    // Now create new event requests
    for(id = newSubsystems.begin();
        id != newSubsystems.end();
        id++)
    {
        // Query for the subsystem configuration.
        queryConfig.SetDestinationID(*id);
        Send(&queryConfig);
        // Request an event in case the subsystem
        // configuration every changes.
        request.SetDestinationID(*id);
        RequestEvent(request);
        // Query the subsystem identification.
        queryIdent.SetDestinationID(*id);
        Send(&queryIdent);
    }

    // Now go through the subsystem list, and
    // verify that we have up to date information for
    // subsystems we already know about.
    for(id = report->GetSubsystemList()->begin();
        id != report->GetSubsystemList()->end();
        id++)
    {
        // Only create events for those in our to discover list.
        mSubsystemsMutex.Enter();       
        if(mSubsystemsToDiscover &&
            mSubsystemsToDiscover->find(id->mSubsystem) == mSubsystemsToDiscover->end())
        {
            mSubsystemsMutex.Leave();
            continue;
        }
        else
        {
            mSubsystemsMutex.Leave();
        }
        // If not a new subsystem
        if(newSubsystems.find(*id) == newSubsystems.end())
        {
            bool haveEvent;
            mEventManager.Lock();
            haveEvent = mEventManager.GetEvent(*id, JAUS_REPORT_CONFIGURATION) != NULL ? true : false;
            mEventManager.Unlock();
            
            if(!haveEvent)
            {
                request.SetDestinationID(*id);
                if(RequestEvent(request) == JAUS_FAILURE)
                {
                    // If we can't create an event, query
                    // the configuration
                    queryConfig.SetDestinationID(*id);
                    Send(&queryConfig);
                }
            }

            // Query the subsystem identification if we
            // do not have it.
            mSubsystemsMutex.Enter();
            platform = mSubsystems.find(id->mSubsystem);
            // If we don't have identification data yet, or we don't
            // have complete subsystem identification send a query.
            if(platform != mSubsystems.end() &&
                platform->second.HaveIdentification() == false ||
                (platform->second.HaveIdentification() &&
                    platform->second.GetIdentification()->GetIdentification().empty() ||
                    platform->second.GetIdentification()->GetType() != ReportIdentification::Subsystem))
            {
                queryIdent.SetDestinationID(*id);
                Send(&queryIdent);
            }

            mSubsystemsMutex.Leave();            
        }
    }

    // Now generate connection events!
    for(platform = lostSubsystems.begin();
        platform != lostSubsystems.end();
        platform++)
    {
        ProcessDiscoveryEvent(platform->second, SubsystemDisconnect);
    }

    for(id = newSubsystems.begin();
        id != newSubsystems.end();
        id++)
    {
        // Get copies of data so that we don't
        // run the callback inside of a mutex.
        Platform platformCopy;
        mSubsystemsMutex.Enter();
        platformCopy = mSubsystems[id->mSubsystem];
        mSubsystemsMutex.Leave();
        ProcessDiscoveryEvent(platformCopy, SubsystemConnected);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets of a platforms configuration info.  This data is valid for other
///   subsystems only if discovery is enabled.
///
///   \param platform Where to copy the platform data to.
///   \param subsystemID The ID of the subsystem/platform to get information for.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILLURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SubscriberComponent::GetPlatformInfo(Platform& platform, const Byte subsystemID) const
{
    int result = JAUS_FAILURE;
    Platform::Map::const_iterator itr;

    mSubsystemsMutex.Enter();

    if(subsystemID == 0 && IsConnected())
    {
        itr = mSubsystems.find(GetID().mSubsystem);
        if(itr != mSubsystems.end())
        {
            platform = itr->second;
            result = JAUS_OK;
        }
    }
    else
    {
        itr = mSubsystems.find(subsystemID);
        if(itr != mSubsystems.end())
        {
            platform = itr->second;
            result = JAUS_OK;
        }
    }
    mSubsystemsMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a copy of the known system configuration.  The data returned
///   is only valid if Subsystem Discovery has been enabled.  Subsystems within
///   provided within the configuration will only be for those that are part
///   of the Subsystem List the Node Manager has.
///
///   \return Copy of the system configuration, only valid if Subsystem Discovery
///           has been turned on.
///
////////////////////////////////////////////////////////////////////////////////////
Platform::Map SubscriberComponent::GetSystemConfiguration() const
{
    Platform::Map config;
    mSubsystemsMutex.Enter();
    config = mSubsystems;
    mSubsystemsMutex.Leave();
    return config;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets of a platforms configuration info.  This data is valid for other
///   subsystems only if discovery is enabled.
///
///   \param subsystemID The ID of the subsystem/platform to get information for.
///
///   \return Copy of platform information.
///
////////////////////////////////////////////////////////////////////////////////////
Platform SubscriberComponent::GetPlatformInfo(const Byte subsystemID) const
{
    Platform platform;
    GetPlatformInfo(platform, subsystemID);
    return platform;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This static function replaces the callback used by the
///   parent class (Component) used for all incomming messages that need
///   to be processed.  
///
///   This function does 2 pre-processing steps before allowing the
///   class to actually process the data.  First, it checks to see if the
///   message is part of a service connection, and if so it matches the
///   message to a SC the component has requested and updates the sequence number
///   and time stamps.  Second, if the message is an event, a match is found within
///   the Event Manager, and the sequence number and time stamp is updated. 
///   After the pre-processing is done, the message is passed on for processing
///   using the ProcessMessage(const Stream& msg, const Header& header) 
///   function of the Component class.
///
///   \param msg Serialized JAUS message sent to component for processing.
///   \param info If already read, this is a pointer to header information from
///               the serialized message.
///   \param transport The transport medium the message came in on (should always
///                    be Shared Memory).
///   \param cmp Pointer to the component that will process the message.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::StreamCallbackFunction(const Stream& msg, 
                                                 const Header* info, 
                                                 const Byte transport, 
                                                 void* cmp)
{
    SubscriberComponent* subscriber = (SubscriberComponent*)(cmp);
    Header header;
    bool scMatch = false;

    // Get a copy of the header information.
    if(info)
    {
        header = *info;
    }
    else
    {
        if(msg.Read(header, 0) == JAUS_FAILURE)
        {
            subscriber->SetJausError(ErrorCodes::BadPacket);
            return;
        }
    }     

    // If SC bit is set, update the stats of our service connections.
    // This will let us know that an SC is updating regularly.
    if(header.mServiceConnectionFlag == JAUS_SERVICE_CONNECTION)
    {
        // Find the matching SC, and update it's stats.
        ServiceConnectionMap::iterator sc;
        UInt pv = 0;
        MessageCreator::GetPresenceVector(msg, pv);
        subscriber->mServiceConnectionsMutex.Enter();
        for(sc = subscriber->mServiceConnections.begin();
            sc != subscriber->mServiceConnections.end();
            sc++)
        {
            if(sc->second.mMessageCode == header.mCommandCode &&
                sc->second.mPresenceVector == pv &&
                sc->second.mProviderID == header.mSourceID)
            {
                sc->second.mSecondaryStatus = ServiceConnection::Active;
                sc->second.mUpdateTimeMs = Time::GetUtcTimeMs();
                sc->second.mSequenceNumber = header.mSequenceNumber;
                scMatch = true;
                break;
            }
        }
        // Make sure our background thread is running if we have SC's.
        if(subscriber->mServiceConnections.size() > 0)
        {
            if(subscriber->mPeriodicEventThread.IsThreadActive() == false)
            {
                subscriber->mPeriodicEventThread.CreateThread(SubscriberComponent::PeriodicEventThreadFunction, subscriber);
                subscriber->mPeriodicEventThread.SetThreadName("Periodic Events");
            }
        }
        subscriber->mServiceConnectionsMutex.Leave();
    }

    // If this is an event message, de-serialize the data
    // and update the statistics for the event within
    // the event manager.  This will help us detect if
    // and event is no longer updated, etc.
    if(header.mCommandCode == JAUS_EVENT)
    {
        Jaus::Event* e = NULL;
        EventMessage emessage;
        if(emessage.Read(msg))
        {
            subscriber->mEventManager.Lock();
            if( (e = subscriber->mEventManager.GetEvent(emessage.GetSourceID(), 
                                                    emessage.GetEventMessageCode(), 
                                                    emessage.GetEventID())) != NULL)
            {
                e->SetTimeStampMs(Time::GetUtcTimeMs());
                e->SetSequenceNumber(emessage.GetEventSequenceNumber());
            }
            subscriber->mEventManager.Unlock();

            // Process the event message.
            subscriber->ProcessMessage(&emessage);
            return;
        }
        else
        {
            return;
        }
    }

    subscriber->Component::ProcessMessage(msg, header);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function that is run in a thread which is used to
///   generate periodic and service connection events, and checks to see if
///   service connections and periodic events requested have stopped updating, and
///   attempts to re-establish the connections.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::PeriodicEventThreadFunction(void *args)
{
    SubscriberComponent* subscriber = (SubscriberComponent*)args;
    subscriber->SubscriptionsLoop();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function that is run in a thread which is used to
///   generate periodic and service connection events, and checks to see if
///   service connections and periodic events requested have stopped updating, and
///   attempts to re-establish the connections.
///
////////////////////////////////////////////////////////////////////////////////////
void SubscriberComponent::SubscriptionsLoop()
{
    
    unsigned int loopCounter = 0;
    unsigned int loopCountLimit = 1;
    unsigned int lastCheckTimeMs = 0;   // Last time that we checked for lost services/events.
    unsigned int numServiceConnections = 0, numEvents = 0;

    while(mPeriodicEventThread.QuitThreadFlag() == false)
    {
        // Only check once a second for lost/non-updating connections
        if(Time::GetUtcTimeMs() - lastCheckTimeMs > 2500)
        {
            CheckServiceConnections();
            if(mPeriodicEventThread.QuitThreadFlag() == false)
            {
                CheckEvents();
            }
            if(mPeriodicEventThread.QuitThreadFlag() == false)
            {
                CheckDiscoveryEvents();  
            }
            lastCheckTimeMs = Time::GetUtcTimeMs();
        }
        
        mServiceConnectionsMutex.Enter();
        numServiceConnections = (unsigned int)mServiceConnections.size();
        mServiceConnectionsMutex.Leave();
        if(mPeriodicEventThread.QuitThreadFlag() == false)
        {
            mEventManager.Lock();
            numEvents = (unsigned int)mEventManager.GetEventSubscriptions()->size();
            mEventManager.Unlock();
        }
        // If we have no service connections or
        // periodic events, exit thread.
        if(numServiceConnections == 0 && 
            numEvents == 0 && 
            mLostEvents.size() == 0 &&
            mSystemDiscoveryFlag == false)
        {
            break;
        }
        
        // Try not to max out the CPU
        if(++loopCounter == loopCountLimit)
        {
            loopCounter = 0;
            CxUtils::SleepMs(1);
        }
    }
}


/*  End of File */
