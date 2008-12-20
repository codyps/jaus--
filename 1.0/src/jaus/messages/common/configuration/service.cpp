////////////////////////////////////////////////////////////////////////////////////
///
///  \file service.cpp
///  \brief This file contains data structure for storing service information.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 July 2007
///  <br>Last Modified: 1 March 2008
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
#include "jaus/messages/common/configuration/service.h"
#include "jaus/messages/bitvector.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/query/querycodes.h"
#include "jaus/messages/inform/informcodes.h"
#include "jaus/messages/experimental/experimentalcodes.h"
#include "jaus/messages/messagecreator.h"

#include <iostream>
#include <iomanip>

///  Adds to Service vectors together.
Jaus::Service::List Jaus::operator +(const Jaus::Service::List& a, const Jaus::Service::List& b)
{
    Jaus::Service::List result;
    result = a;
    for(unsigned int i = 0; i < (unsigned int)b.size(); i++)
        result.push_back(b[i]);

    return result;
}

using namespace Jaus;
using namespace std;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Service::Service() : mServiceType(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Service::Service(const Service& info)
{
    *this = info;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Service::~Service()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set the service type.  See the JAUS RA for type service defintions
///  table, or the Service header file list.
///
///  \param type The service type.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::SetType(const UShort type)
{
    mServiceType = type;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add a supported input message to the service.  A service can have
///  at most 255 supported input messages.
///
///  \param msg Message code and presence vector information.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::AddInputMessage(const Service::Message& msg)
{
    if( mInputMessages.size() < 255 )
    {
        mInputMessages[msg.mMessageCode] = msg;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add a supported input message to the service.  A service can have
///  at most 255 supported input messages.
///
///  \param type The command/class code of the message.
///  \param pv The presence vector associated with the message.  For presence
///            vectors smaller than 32 bits, the representative data shall be
///            inserted with matching bit significance.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::AddInputMessage(const UShort type, const UInt pv)
{
    return AddInputMessage(Service::Message(type, pv));
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add a supported output message from the service.  A service can have
///  at most 255 supported output messages.
///
///  \param msg Message code and presence vector information.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::AddOutputMessage(const Service::Message& msg)
{
    if(mOutputMessages.size() < 255)
    {   
        mOutputMessages[msg.mMessageCode] = msg;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add a supported output message from the service.  A service can have
///  at most 255 supported output messages.
///
///  \param type The command/class code of the message.
///  \param pv The presence vector associated with the message.  For presence
///            vectors smaller than 32 bits, the representative data shall be
///            inserted with matching bit significance.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::AddOutputMessage(const UShort type, const UInt pv)
{
    return AddOutputMessage(Service::Message(type, pv));
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Remove a message from the list of outputs.
///
///  \param type Message command/class code.
///
///  \return JAUS_OK if removed, otherwise JAUS_FAILURE if nothing to remove.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::RemoveOutputMessage(const UShort type)
{
    Service::Message::Map::iterator itr;
    
    itr = mOutputMessages.find(type);
    if(itr != mOutputMessages.end())
    {
        //itr = mOutputMessages.erase(itr);
        mOutputMessages.erase(itr);
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Remove a message from the list of inputs.
///
///  \param type Message command/class code.
///
///  \return JAUS_OK if removed, otherwise JAUS_FAILURE if nothing to remove.
///
////////////////////////////////////////////////////////////////////////////////////
int Service::RemoveInputMessage(const UShort type)
{
    Service::Message::Map::iterator itr;
    
    itr = mInputMessages.find(type);
    if(itr != mInputMessages.end())
    {
        //itr = mInputMessages.erase(itr);
        mInputMessages.erase(itr);
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if input message is supported.
///
///  \param type Message command/class code.
///
///  \return True if message is an input type for the service otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Service::IsInputMessage(const UShort type) const
{
    Service::Message::Map::const_iterator itr;    
    itr = mInputMessages.find(type);
    if(itr != mInputMessages.end())
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Checks the service to see if a message and its associated 
///         presence vector are supported.
///
///  \param msg Message code and presence vector to check for support.
///
///  \return True if supported, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Service::IsInputSupported(const Service::Message& msg) const
{
    Service::Message::Map::const_iterator itr;
    
    itr = mInputMessages.find(msg.mMessageCode);
    if(itr != mInputMessages.end())
    {
        if(BitVector::Supports( itr->second.mPresenceVector, (MessageCreator::GetPresenceVectorMask(msg.mMessageCode) & msg.mPresenceVector)))
            return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if output message is supported.
///
///  \param type Message command/class code.
///
///  \return True if message is an output type for the service otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Service::IsOutputMessage(const UShort type) const
{
    Service::Message::Map::const_iterator itr;
    
    itr = mOutputMessages.find(type);
    if(itr != mOutputMessages.end())
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Checks the service to see if a message and its associated 
///         presence vector are supported.
///
///  \param msg Message code and presence vector to check for support.
///
///  \return True if supported, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Service::IsOutputSupported(const Service::Message& msg) const
{
    Service::Message::Map::const_iterator itr;
    
    itr = mOutputMessages.find(msg.mMessageCode);
    if(itr != mOutputMessages.end())
    {
        if(BitVector::Supports( itr->second.mPresenceVector, (MessageCreator::GetPresenceVectorMask(msg.mMessageCode) & msg.mPresenceVector)))
            return true;
    }
    return false;
}



const Service::Message::Map* Service::GetInputMessages() const
{ 
    return &mInputMessages; 
}

const Service::Message::Map* Service::GetOutputMessages() const 
{ 
    return &mOutputMessages; 
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all data.
///
////////////////////////////////////////////////////////////////////////////////////
void Service::ClearService()
{
    mServiceType = 0;
    mInputMessages.clear();
    mOutputMessages.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Converts service type information to a string (e.g. 38 = "Global Pose
///  Sensor").
///
///  \param[in] serviceType The service type to get string of.
///
///  \return String representing what the service type is.
///
////////////////////////////////////////////////////////////////////////////////////
std::string Service::ToString(const Type serviceType)
{
    std::string str;
    switch(serviceType)
    {
    case CoreMessageSupport:
        str = "Core";
        break;
    case NodeManager:
        str = "Node Manager";
        break;
    case SubsystemCommander:
        str = "Subsystem Commander";
        break;
    case PrimitiveDriver:
        str = "Primitive Driver";
        break;
    case GlobalVectorDriver:
        str = "Global Vector Driver";
        break;
    case Communicator:
        str = "Communicator";
        break;
    case VisualSensor:
        str = "Visual Sensor";
        break;
    case GlobalPoseSensor:
        str = "Global Pose Sensor";
        break;
    case SystemCommander:
        str = "System Commander";
        break;
    case LocalPoseSensor:
        str = "Local Pose Sensor";
        break;
    case VelocityStateSensor:
        str = "Velocity State Sensor";
        break;
    case ReflexiveDriver:
        str = "Reflexive Driver";
        break;
    case LocalVectorDriver:
        str = "Local Vector Driver";
        break;
    case GlobalWaypointDriver:
        str = "Global Vector Driver";
        break;
    case LocalWaypointDriver:
        str = "Local Waypoint Driver";
        break;
    case GlobalPathSegmentDriver:
        str = "Global Path Segment Driver";
        break;
    case LocalPathSegmentDriver:
        str = "Local Path Segment Driver";
        break;
    case PrimitiveManipulator:
        str = "Primitive Manipulator";
        break;
    case RangeSensor:
        str = "Range Sensor";
        break;
    case ManipulatorJointPositionSensor:
        str = "Manipulator Joint Position Sensor";
        break;
    case ManipulatorJointVelocitySensor:
        str = "Manipulator Joint Velocity Sensor";
        break;
    case ManipulatorJointForceTorqueSensor:
        str = "Manipulator Joint Force Torque Sensor";
        break;
    case ManipulatorJointPositionsDriver:
        str = "Manipulator Joint Positions Driver";
        break;
    case ManipulatorEndEffectorPoseDriver:
        str = "Manipulator End Effector Pose Driver";
        break;
    case ManipulatorJointVelocitiesDriver:
        str = "Manipulator Joint Velocities Driver";
        break;
    case ManipulatorEndEffectorVelocityStateDriver:
        str = "Manipulator End Effector Velocity State Driver";
        break;
    case ManipulatorJointMoveDriver:
        str = "Manipulator Joint Move Driver";
        break;
    case ManipulatorEndEffectorDiscretePoseDriver:
        str = "Manipulator End Effector Discrete Pose Driver";
        break;
    default:
        if((Byte)serviceType >= 2 && (Byte)serviceType <= 31)
        {
            str = "User Defined";
        }
        else
        {
            str = "Core";
        }
        break;
    }
    return str;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets all input messages and output messages to the core set of
///  JAUS messages, and sets the service type to JAUS_SERVICE_CORE_MESSAGE_SUPPORT.
///
////////////////////////////////////////////////////////////////////////////////////
void Service::LoadCoreMessageSupport()
{
    ClearService();
    mServiceType = CoreMessageSupport;

    //  Core Commands
    AddInputMessage(JAUS_SET_COMPONENT_AUTHORITY, 0);
    AddInputMessage(JAUS_SHUTDOWN, 0);
    AddInputMessage(JAUS_STANDBY, 0);
    AddInputMessage(JAUS_RESUME, 0);
    AddInputMessage(JAUS_RESET, 0);
    AddInputMessage(JAUS_SET_EMERGENCY, 0);
    AddInputMessage(JAUS_CLEAR_EMERGENCY, 0);
    AddInputMessage(JAUS_CREATE_SERVICE_CONNECTION, 0);
    AddInputMessage(JAUS_ACTIVATE_SERVICE_CONNECTION, 0);
    AddInputMessage(JAUS_SUSPEND_SERVICE_CONNECTION, 0);
    AddInputMessage(JAUS_TERMINATE_SERVICE_CONNECTION, 0);
    AddInputMessage(JAUS_REQUEST_COMPONENT_CONTROL, 0);
    AddInputMessage(JAUS_RELEASE_COMPONENT_CONTROL, 0);
    AddInputMessage(JAUS_SET_TIME, 0x02);
    //  Core Query
    AddInputMessage(JAUS_QUERY_COMPONENT_AUTHORITY, 0);
    AddInputMessage(JAUS_QUERY_COMPONENT_STATUS, 0);
    AddInputMessage(JAUS_QUERY_TIME, 0);
    AddInputMessage(JAUS_QUERY_COMPONENT_CONTROL, 0);
    //  Core Responses
    AddOutputMessage(JAUS_CONFIRM_COMPONENT_CONTROL, 0);
    AddOutputMessage(JAUS_CONFIRM_SERVICE_CONNECTION, 0);
    AddOutputMessage(JAUS_REJECT_COMPONENT_CONTROL, 0);
    AddOutputMessage(JAUS_REPORT_COMPONENT_AUTHORITY, 0);
    AddOutputMessage(JAUS_REPORT_COMPONENT_STATUS, 0);
    AddOutputMessage(JAUS_REPORT_TIME, 0x02);
    AddOutputMessage(JAUS_REPORT_COMPONENT_CONTROL, 0);
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Prints values to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void Service::PrintService() const
{
    cout << "Service Type: " << mServiceType << endl;
    Service::Message::Map::const_iterator itr;
    cout << "Num Input Messages: " << (int)mInputMessages.size() << endl;
    for(itr = mInputMessages.begin();
        itr != mInputMessages.end();
        itr++) {
        std::cout << "Code: " 
              << std::setw(4) << std::setfill('0') << std::setbase (16) 
              << itr->second.mMessageCode << ", PV: "
              << std::setw(4) << std::setfill('0') << std::setbase (16)
              << itr->second.mPresenceVector << std::setbase(10) 
              << ", Name: " << MessageCreator::GetMessageString( itr->second.mMessageCode) << std::endl;
    }

    cout << "Num Output Messages: " << std::setbase(10) << (int)mOutputMessages.size() << endl;
    for(itr = mOutputMessages.begin();
        itr != mOutputMessages.end();
        itr++) {
        std::cout << "Code: " 
              << std::setw(4) << std::setfill('0') << std::setbase (16) 
              << itr->second.mMessageCode << ", PV: "
              << std::setw(4) << std::setfill('0') << std::setbase (16)
              << itr->second.mPresenceVector << std::setbase(10)
              << ", Name: " << MessageCreator::GetMessageString( itr->second.mMessageCode) << std::endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Service& Service::operator =(const Service& info)
{
    if(this != &info) {
        mServiceType = info.mServiceType;
        mInputMessages = info.mInputMessages;
        mOutputMessages = info.mOutputMessages;
    }
    return *this;
}
/*  End of File */

