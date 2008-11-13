////////////////////////////////////////////////////////////////////////////////////
///
///  \file primitivedriver.cpp
///  \brief This file contains software for creating a JAUS Primitive Driver
///  component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 April 2008
///  <br>Created: 1 May 2008
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
#include "jaus/services/primitivedriver.h"

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
PrimitiveDriver::PrimitiveDriver()
{
    SetControllable(true);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
PrimitiveDriver::~PrimitiveDriver()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the Primitive component.
///
///   This method overrides the parent class Initialize function, filling in
///   the component name and component type number.  
///
///   \param subsystem Subsystem ID number.
///   \param node Node ID number.
///   \param instance Instance ID to use.  Set to 0 to find first available
///             instance.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int PrimitiveDriver::Initialize(const Byte subsystem, const Byte node, const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(CommandComponent::Initialize("Primitive Driver", 
                                            Address(subsystem, 
                                            node, 
                                            (Byte)(Service::PrimitiveDriver), 
                                            i)))
            {
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        return CommandComponent::Initialize("Primitive Driver", 
                                            Address(subsystem, 
                                            node, 
                                            (Byte)(Service::PrimitiveDriver), 
                                            instance));
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
int PrimitiveDriver::ProcessCommandMessage(const Message* msg,
                                           const Byte commandAuthority)
{
    int result = JAUS_FAILURE;
    // If under uninterrupted control, only take drive commands
    // from the controller.
    if(GetControllerID().IsValid() && msg->GetSourceID() != GetControllerID())
    {
        return CommandComponent::ProcessCommandMessage(msg, commandAuthority);
    }
    
    // If under uninterrupted control, only take drive commands
    // from the controller.  Also verify authority level.
    if(commandAuthority >= GetComponentAuthority() &&
        (GetControllerID().IsValid() == false || GetControllerID() == msg->GetSourceID()))
    {
        switch(msg->GetCommandCode())
        {
        case JAUS_SET_WRENCH_EFFORT:
            {
                const Jaus::SetWrenchEffort* command = dynamic_cast<const Jaus::SetWrenchEffort*>(msg);
                if(command && GetPrimaryStatus() == Component::Status::Ready)
                {
                    result = this->SetWrenchEffort(command);
                }
            }
            break;
        case JAUS_SET_DISCRETE_DEVICES:
            {
                const Jaus::SetDiscreteDevices* command = dynamic_cast<const Jaus::SetDiscreteDevices*>(msg);
                result = this->SetDiscreteDevices(command);
            }
            break;
        default:
            result = CommandComponent::ProcessCommandMessage(msg, commandAuthority);
            break;
        }
    }
    else
    {
        result = CommandComponent::ProcessCommandMessage(msg, commandAuthority);
    }

    return result;
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
int PrimitiveDriver::ProcessQueryMessage(const Message* msg)
{
    int result = JAUS_FAILURE;
    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_WRENCH_EFFORT:
        {
            const QueryWrenchEffort* query = dynamic_cast<const QueryWrenchEffort*>(msg);
            if(query)
            {
                result = ReportWrenchEffort(query);
            }
        }
        break;
    default:
        result = CommandComponent::ProcessQueryMessage(msg);
        break;
    }

    return result;
}


/*  End of File */
