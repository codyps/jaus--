////////////////////////////////////////////////////////////////////////////////////
///
///  \file commandcomponent.h
///  \brief This file contains a more advanced version of the JAUS++ Component
///  class, specifically for command type components.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 April 2008
///  <br>Last Modified: 30 April 2008
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
#include "jaus/components/commandcomponent.h"
#include "jaus/messages/command/core/resume.h"
#include "jaus/messages/command/core/standby.h"
#include "jaus/messages/command/core/requestcomponentcontrol.h"
#include "jaus/messages/command/core/confirmcomponentcontrol.h"
#include "jaus/messages/command/core/rejectcomponentcontrol.h"
#include "jaus/messages/command/core/releasecomponentcontrol.h"
#include "jaus/messages/query/core/querycomponentcontrol.h"
#include "jaus/messages/query/core/querycomponentstatus.h"
#include "jaus/messages/query/configuration/querysubsystemlist.h"
#include "jaus/messages/query/configuration/queryconfiguration.h"
#include "jaus/messages/inform/configuration/reportsubsystemlist.h"
#include "jaus/messages/inform/core/reportcomponentcontrol.h"
#include "jaus/messages/inform/core/reportcomponentstatus.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CommandComponent::CommandComponent()
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
    core.AddInputMessage(JAUS_QUERY_EVENTS, 0);
    core.AddOutputMessage(JAUS_CREATE_EVENT, 0xFF);
    core.AddOutputMessage(JAUS_UPDATE_EVENT, 0xFF);
    core.AddOutputMessage(JAUS_CANCEL_EVENT, 0x03);
    core.AddOutputMessage(JAUS_REJECT_EVENT_REQUEST, 0x01);
    core.AddOutputMessage(JAUS_CONFIRM_EVENT_REQUEST, 0x01);
    core.AddOutputMessage(JAUS_EVENT, 0);
    core.AddOutputMessage(JAUS_REPORT_EVENTS, 0);
    // Control messages
    core.AddOutputMessage(JAUS_REQUEST_COMPONENT_CONTROL, 0);
    core.AddOutputMessage(JAUS_RELEASE_COMPONENT_CONTROL, 0);
    AddService(core);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CommandComponent::~CommandComponent()
{
    ReleaseControlOfAllComponents();
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Release control of all components, then calls the 
///   InformComponent::Shutdown method (which will shutdown  all subscriptions
///   followed by communication).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::Shutdown() 
{
    ReleaseControlOfAllComponents();
    return InformComponent::Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Release control of all components, then calls the 
///   InformComponent::Shutdown method (which will shutdown  all subscriptions).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::Reset() 
{
    ReleaseControlOfAllComponents();
    return InformComponent::Reset();
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
int CommandComponent::SetupCoreService()
{
    Service::Map services;
    Service::Map::iterator coreServices;

    // Initialize core services from parent.
    InformComponent::SetupCoreService();

    services = GetServices();

    coreServices = services.find(Service::CoreMessageSupport);
    // Add additional message to core set.
    if(coreServices != services.end())
    {
        // Add additional messages for control.
        coreServices->second.AddOutputMessage(JAUS_REQUEST_COMPONENT_CONTROL, 0);
        coreServices->second.AddOutputMessage(JAUS_QUERY_COMPONENT_CONTROL, 0);
        coreServices->second.AddOutputMessage(JAUS_RELEASE_COMPONENT_CONTROL, 0);

        coreServices->second.AddInputMessage(JAUS_REPORT_COMPONENT_CONTROL, 0);

        // Add the updated version of the core services to component
        AddService(coreServices->second);
    }
    return JAUS_OK;
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
int CommandComponent::ProcessCommandMessage(const Message* msg, const Byte commandAuthority)
{
    int result = JAUS_FAILURE;
    switch(msg->GetCommandCode())
    {
    case JAUS_RELEASE_COMPONENT_CONTROL:
        {
            if(msg->GetSourceID() == GetControllerID())
            {
                // Generate event for release of control.
                ProcessControlEvent(ControlReleased, msg->GetSourceID(), msg);
            }
            // Let parent class do additional processing.
            result = InformComponent::ProcessCommandMessage(msg, commandAuthority);
        }
        break;
    case JAUS_REJECT_COMPONENT_CONTROL:
        {
            result = JAUS_OK;

            const RejectComponentControl* command = 
                dynamic_cast<const RejectComponentControl*>(msg);
            if(command)
            {
                Address::Set::iterator id;
                bool controlEvent = false;
                mControlledComponentsMutex.Lock();
                id = mControlledComponents.find(command->GetSourceID());
                if(id != mControlledComponents.end())
                {
                    controlEvent = true;
                    mControlledComponents.erase(id);
                }
                mControlledComponentsMutex.Unlock();
                if(controlEvent)
                {
                    ProcessControlEvent(ControlLost, command->GetSourceID(), command);
                }
            }
        }
        break;
    default:
        result = InformComponent::ProcessCommandMessage(msg, commandAuthority);
        break;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Request control of the component.
///
///   \param id The ID of the component to control.
///   \param timeoutMs How long to wait for a response.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::RequestComponentControl(const Address& id,
                                              const UShort timeoutMs)
{
    int result = JAUS_FAILURE;
    Jaus::RequestComponentControl command;
    Receipt receipt;
    command.SetSourceID(GetID());
    command.SetDestinationID(id);
    command.SetAuthorityCode(GetComponentAuthority());
    if(Send(&command, receipt, JAUS_CONFIRM_COMPONENT_CONTROL, timeoutMs))
    {
        const ConfirmComponentControl* response = 
            dynamic_cast<const ConfirmComponentControl*>(receipt.GetResponseMessage());
        if(response && response->GetResponseCode() == ConfirmComponentControl::Granted)
        {
            result = JAUS_OK;
            mControlledComponentsMutex.Lock();
            mControlledComponents.insert(id);
            mControlledComponentsMutex.Unlock();
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Release control of a component.
///
///   \param id The ID of the component to release control of.
///   \param timeoutMs How long to wait for a response.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::ReleaseComponentControl(const Address& id,
                                              const UShort timeoutMs)
{
    int result = JAUS_FAILURE;
    Jaus::ReleaseComponentControl command;
    Receipt receipt;
    command.SetSourceID(GetID());
    command.SetDestinationID(id);
    command.SetAckNack(Header::AckNack::Request);

    Send(&command, receipt, JAUS_RELEASE_COMPONENT_CONTROL, timeoutMs);

    result = JAUS_OK;
    Address::Set::iterator component;

    mControlledComponentsMutex.Lock();
    component = mControlledComponents.find(id);
    if(component != mControlledComponents.end())
    {
        mControlledComponents.erase(component);
    }

    mControlledComponentsMutex.Unlock();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a Resume command to tell a component to transition out
///          of Standby mode.
///
///   \param id The ID of the component to send command to.
///   \param queryStatus If true, the status of the component is queried to
///                      verify state transition.
///
///   \return JAUS_OK if sent message or status confirmed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::SendResumeCommand(const Address& id, const bool queryStatus)
{
    Jaus::Resume command;
    int result = Jaus::FAILURE;
    bool sendReleaseFlag = false;
    if(!HaveComponentControl(id) )
    {
        if(this->RequestComponentControl(id, 250))
        {
            sendReleaseFlag = true;
        }
    }
    command.SetSourceID(GetID());
    command.SetDestinationID(id);
    if(Send(&command))
    {
        if(queryStatus)
        {
            Jaus::QueryComponentStatus query;
            Receipt receipt;
            query.SetSourceID(GetID());
            query.SetDestinationID(id);
            if(Send(&query, receipt, 0, 250, 1) == JAUS_OK)
            {
                const Jaus::ReportComponentStatus* report = 
                    dynamic_cast<const Jaus::ReportComponentStatus*>(receipt.GetResponseMessage());
                if(report &&
                    report->GetPrimaryStatusCode() == Jaus::ReportComponentStatus::Status::Ready)
                {
                    result = JAUS_OK;
                }
            }
        }
        else
        {
            result = JAUS_OK;
        }
    }

    if(sendReleaseFlag)
    {
        this->ReleaseComponentControl(id);
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a Standby command to tell a component to transition out
///          of Ready mode.
///
///   \param id The ID of the component to send command to.
///   \param queryStatus If true, the status of the component is queried to
///                      verify state transition.
///
///   \return JAUS_OK if sent message or status confirmed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::SendStandbyCommand(const Address& id, const bool queryStatus)
{
    Jaus::Standby command;

    int result = Jaus::FAILURE;
    bool sendReleaseFlag = false;
    if(!HaveComponentControl(id) )
    {
        if(this->RequestComponentControl(id, 250))
        {
            sendReleaseFlag = true;
        }
    }

    command.SetSourceID(GetID());
    command.SetDestinationID(id);

    if(Send(&command))
    {
        if(queryStatus)
        {
            Jaus::QueryComponentStatus query;
            Receipt receipt;
            query.SetSourceID(GetID());
            query.SetDestinationID(id);
            if(Send(&query, receipt, 0, 250, 1) == JAUS_OK)
            {
                const Jaus::ReportComponentStatus* report = 
                    dynamic_cast<const Jaus::ReportComponentStatus*>(receipt.GetResponseMessage());
                if(report &&
                    report->GetPrimaryStatusCode() == Jaus::ReportComponentStatus::Status::Standby)
                {
                    result = JAUS_OK;
                }
            }
        }
        else
        {
            result = JAUS_OK;
        }
    }

    if(sendReleaseFlag)
    {
        this->ReleaseComponentControl(id);
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Release control of all components being controlled.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::ReleaseControlOfAllComponents()
{
    Address::Set::iterator id;
    Jaus::ReleaseComponentControl command;
    Receipt receipt;
    command.SetSourceID(GetID());
    command.SetAckNack(Header::AckNack::Request);
    mControlledComponentsMutex.Lock();
    for(id = mControlledComponents.begin();
        id != mControlledComponents.end(); 
        id++)
    {
        command.SetDestinationID(*id);
        Send(&command, receipt, JAUS_RELEASE_COMPONENT_CONTROL, 250, 3);
    }
    mControlledComponents.clear();
    mControlledComponentsMutex.Unlock();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This function is called anytime a discovery event happens.  See
///   SubscriberComponent for details about this function.
///
///   This overload the ProcessDiscoveryEvent message checks to see if any
///   components being controlled have disconnected.
///
///   \param subsystem Platform/subsystem that has updated.
///   \param eventType the type of discovery event.
///
///   \return JAUS_OK.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::ProcessDiscoveryEvent(const Platform& subsystem,
                                            const SubscriberComponent::DiscoveryEvents eventType)
{
    Address::Set::iterator component;
    Address::Set lost;

    // Go through and remove any components that have
    // been lost.
    mControlledComponentsMutex.Lock();
    component = mControlledComponents.begin();
    while(component != mControlledComponents.end())
    {
        if(component->mSubsystem == subsystem.GetSubsystemID())
        {
            if(subsystem.GetConfiguration()->HaveComponent(*component) == false)
            {
                lost.insert(*component);
                mControlledComponents.erase(component);
                component = mControlledComponents.begin();
                /*
#ifdef WIN32                
                component = mControlledComponents.erase(component);
#else
                mControlledComponents.erase(component);
#endif
                */
                continue;
            }
        }
        component++;
    }
    mControlledComponentsMutex.Unlock();

    for(component = lost.begin();
        component != lost.end();
        component++)
    {
        ProcessControlEvent(ComponentDisconnect, *component, NULL);                            
    }

    // See if the component that was controlling us
    // disconnected.
	/*
    if(GetControllerID().IsValid() && GetControllerID().mSubsystem == subsystem.GetSubsystemID())
    {
        if(subsystem.GetConfiguration()->HaveComponent(GetControllerID()) == false)
        {
            Address controllerID = GetControllerID();
            ClearControllerID();
            if(IsControllable())
            {
                Standby();
                SetPrimaryStatus(Component::Status::Standby);
            }
            ProcessControlEvent(ControlReleased, controllerID, NULL);
        }
    }
	*/
    return InformComponent::ProcessDiscoveryEvent(subsystem, eventType);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks to see if control of a component has been made.  If needed
///   a Query Component Control message is sent to the component to verify.
///
///   This function first checks the internal list of controlled components
///   and the known subsystem configuration to see if the component is
///   being controlled.  If these steps fail, a Query Component Control
///   message is sent to check for control, and if the component is controlled
///   it is added to the list of controlled components.
///
///   \param id The ID of the component to check for control of.
///   \param forceCheck If true, then a QueryComponentControl message is sent
///                     to verify control.
///
///   \return True if component is under this components control, otherwise
///           false.
///
////////////////////////////////////////////////////////////////////////////////////
bool CommandComponent::HaveComponentControl(const Address& id, const bool forceCheck) const
{
    bool result = false;
    Address::Set::const_iterator itr;

    mControlledComponentsMutex.Lock();

    itr = mControlledComponents.find(id);
    if(itr != mControlledComponents.end())
    {
        result = true;
    }
	if(result == false || forceCheck)
	{
		QueryComponentControl query;
        Receipt receipt;
        query.SetSourceID(GetID());
        query.SetDestinationID(id);
        if(Send(&query, receipt, JAUS_REPORT_COMPONENT_CONTROL) == JAUS_OK)
        {
            const Jaus::ReportComponentControl* report = 
                        dynamic_cast<const Jaus::ReportComponentControl*>(receipt.GetResponseMessage());
            if(report && report->GetControllerID() == GetID())
            {
                result = true;
				// Add to set of controlled components for future use.
				((Address::Set* )(&mControlledComponents))->insert(id);
            }
        }
	}

    if(result == false)
    {
        Address::Set::iterator rm;
        Address::Set* controlled = (Address::Set*)(&mControlledComponents);
        rm = controlled->find(id);
        if(rm != controlled->end())
        {
            controlled->erase(rm);
        }
        
    }

	mControlledComponentsMutex.Unlock();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a Query Configuration message to the subsystem.
///
///   \param subsystemID ID of the subsystem to query for configuration data.
///
///   \return True on success, false on failure.
///
////////////////////////////////////////////////////////////////////////////////////
bool CommandComponent::QueryConfiguration(const Byte subsystemID)
{
    Jaus::QuerySubsystemList querySubsystem;
    Receipt receipt;
    querySubsystem.SetSourceID(GetID());
    querySubsystem.SetDestinationID(Address(GetID().mSubsystem, GetID().mNode, 1, 1));
    if(Send(&querySubsystem, receipt, 0, 100, 1))
    {
        ReportSubsystemList *reportSubsystemList = (ReportSubsystemList *)receipt.GetResponseMessage();
        Jaus::QueryConfiguration queryConfig;
        queryConfig.SetSourceID(GetID());
        queryConfig.SetDestinationID(Address(subsystemID, 255, 1, 1));

        Address::Set::const_iterator id;
        for(id = reportSubsystemList->GetSubsystemList()->begin(); 
            id != reportSubsystemList->GetSubsystemList()->end(); 
            id++)
        {
            if(id->mSubsystem == subsystemID)
            {
                queryConfig.SetDestinationID(*id);
            }
        }
        return Send(&queryConfig) == JAUS_OK ? true : false;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return List of all components under control by this component.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set CommandComponent::GetControlledComponents() const
{
    Address::Set list;
    mControlledComponentsMutex.Lock();
    list = mControlledComponents;
    mControlledComponentsMutex.Unlock();
    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Any time a control event happens (lost control, component disconnect,
///   or release, etc.) this function is called.  Overload it to add
///   custom behaviors to your command component.
///
///   \param type The type of event.
///   \param component The component the event is for.
///   \param message Optional parameter (default value is NULL), and is any
///                  messages recevied that may have generated the event.
///
///   \return JAUS_OK if event processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CommandComponent::ProcessControlEvent(const ControlEvents type, 
                                          const Address& component, 
                                          const Message* message)
{
    return JAUS_OK;
}

/*  End of File */
