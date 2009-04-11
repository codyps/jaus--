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
#ifndef __JAUS_COMMAND_COMPONENT__H
#define __JAUS_COMMAND_COMPONENT__H

#include "jaus/components/informcomponent.h"


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class CommandComponent
    ///   \brief Interface class for creating a command components in JAUS.
    ///
    ///   This class builds upon InformComponent to add functions specifically
    ///   related to control of other components, and for being controlled.  
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL CommandComponent : public InformComponent
    {
    public:
        ///////////////////////////////////////////////////////////////////////
        ///
        ///  \brief Enumerates the different type of control events
        ///  that can happen.
        ///
        ///////////////////////////////////////////////////////////////////////
        enum ControlEvents
        {
            ControlLost = 0,     // Lost control of a component
            ComponentDisconnect, // Component no longer exists (control lost).
            ControlReleased,     // Control of this component has been released
        };
        // Constructor.
        CommandComponent();
        // Destructor.
        virtual ~CommandComponent();
        // Shutdown control requests, subscriptions, and communication.
        virtual int Shutdown();
        // Resets component to initialized state.  Overload for other behaviors.
        virtual int Reset();
        // Sets up core services used by the command component.
        virtual int SetupCoreService();
        // Method to process command message
        virtual int ProcessCommandMessage(const Message* msg, const Byte commandAuthority);
        // Request uninterruptible control of a component.
        int RequestComponentControl(const Address& id,
                                    const UShort timeoutMs = JAUS_RESPONSE_TIMEOUT_TIME);
        // Release uninterruptible control of a component.
        int ReleaseComponentControl(const Address& id,
                                    const UShort timeoutMs = JAUS_RESPONSE_TIMEOUT_TIME);
        // Send a Resume Command to a component.
        int SendResumeCommand(const Address& id,
                              const bool queryStatus = false);
        // Sends a Standby Command message to a component.
        int SendStandbyCommand(const Address& id,
                               const bool queryStatus = false);
        // Release uninterruptible control of all components controlled.
        int ReleaseControlOfAllComponents();
        // If discovery is enabled, this function is called and checks for lost controlled components.
        virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                          const SubscriberComponent::DiscoveryEvents eventType = SubsystemUpdate);
        // Check if the component has control of a component
        bool HaveComponentControl(const Address& id) const;
        // Get updated configuration data for a subsystem.
        bool QueryConfiguration(const Byte subsystemID);
        // Get a copy of all components being controlled.
        Address::Set GetControlledComponents() const;
        // Function called anytime a control event happens.
        virtual int ProcessControlEvent(const ControlEvents type, 
                                        const Address& component,
                                        const Message* message = NULL);
    protected:
        CxUtils::Mutex mControlledComponentsMutex;  ///< Mutex to protect controlled component data.
        Address::Set mControlledComponents;         ///< Components we have control of.
    };
}


#endif
/*  End of File */
