////////////////////////////////////////////////////////////////////////////////////
///
///  \file service.h
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
#ifndef __JAUS_SERVICE_INFO_H
#define __JAUS_SERVICE_INFO_H

#include "jaus/messages/types.h"

#ifdef __cplusplus

#include <set>
#include <map>
#include <vector>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Service
    ///   \brief Data structure containing information about a service supported
    ///   by a component.
    ///
    ///   Service information includes the type of service a component provides.
    ///   Service types match up with the component ID types for JAUS defined
    ///   component types.  In addition to the service type are the input and
    ///   output messages supported by the service type.  For example, a 
    ///   component that supports only the core set of JAUS messages will have
    ///   all the input and output messages from the core set.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Service
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Message
        ///   \brief Message struture for storing information about a supported message
        ///          and its presence vector in the Service.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class Message
        {
        public:
            typedef std::set<Service::Message> Set;
            typedef std::map<UShort, Service::Message> Map;
            Message() : mMessageCode(0), mPresenceVector(0) {}
            Message(const UShort msg, const UInt pv) 
                : mMessageCode(msg), mPresenceVector(pv) {}
            Message(const Service::Message& msg)
            {
                mMessageCode = msg.mMessageCode;
                mPresenceVector = msg.mPresenceVector;
            }
            bool operator<(const Message& msg) const
            {
                return mMessageCode < msg.mMessageCode;
            }
            Service::Message& operator=(const Message& msg)
            {
                mMessageCode = msg.mMessageCode;
                mPresenceVector = msg.mPresenceVector;
                return *this;
            }
            UShort mMessageCode;
            UInt mPresenceVector;
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Type
        ///   \brief Types of services.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Type
        {
            CoreMessageSupport = 0,
            SubsystemCommander = 32,
            PrimitiveDriver,
            GlobalVectorDriver,
            Communicator,
            VisualSensor = 37,
            GlobalPoseSensor,
            SystemCommander = 40,
            LocalPoseSensor,
            VelocityStateSensor,
            ReflexiveDriver,
            LocalVectorDriver,
            GlobalWaypointDriver,
            LocalWaypointDriver,
            GlobalPathSegmentDriver,
            LocalPathSegmentDriver,
            PrimitiveManipulator,
            RangeSensor,
            ManipulatorJointPositionSensor,
            ManipulatorJointVelocitySensor,
            ManipulatorJointForceTorqueSensor,
            ManipulatorJointPositionsDriver,
            ManipulatorEndEffectorPoseDriver,
            ManipulatorJointVelocitiesDriver,
            ManipulatorEndEffectorVelocityStateDriver,
            ManipulatorJointMoveDriver,
            ManipulatorEndEffectorDiscretePoseDriver
        };
        typedef std::vector<Service> List;       ///<  STL vector of service configurations.
        typedef std::set<Service> Set;           ///<  STL set of Service configurations.
        typedef std::map<UShort, Service> Map;   ///<  STL map of Service configurations.
        Service();
        Service(const Service& info);
        ~Service();
        
        int SetType(const UShort type);
        int AddInputMessage(const Service::Message& msg);
        int AddInputMessage(const UShort type, const UInt pv);
        int AddOutputMessage(const Service::Message& msg);
        int AddOutputMessage(const UShort type, const UInt pv);

        int RemoveInputMessage(const UShort type);
        int RemoveOutputMessage(const UShort type);

        void LoadCoreMessageSupport();
        void PrintService() const;

        bool IsInputMessage(const UShort type) const;
        bool IsInputSupported(const Service::Message& msg) const;
        bool IsInputSupported(const UShort type, const UInt pv) const { return IsInputSupported(Message(type, pv)); }
        bool IsOutputMessage(const UShort type) const;
        bool IsOutputSupported(const Service::Message& msg) const;
        bool IsOutputSupported(const UShort type, const UInt pv) const { return IsOutputSupported(Message(type, pv)); }

        Byte GetNumInputMessages() const { return (Byte)(mInputMessages.size()); }
        Byte GetNumOutputMessages() const { return (Byte)(mOutputMessages.size()); }
        UShort GetType() const { return mServiceType; }
        const Service::Message::Map* GetInputMessages() const;
        const Service::Message::Map* GetOutputMessages() const;
        void ClearService();
        bool operator<(const Service& s) const { return mServiceType < s.mServiceType ? true : false; }
        Service& operator=(const Service& info);
    protected:
        UShort mServiceType;        ///<  The service type.
        Service::Message::Map mInputMessages;  ///<  Input messages associated with the service.
        Service::Message::Map mOutputMessages; ///<  Output messages associated with service.
    };    
    Service::List operator+(const Service::List& a, const Service::List& b);
}


#endif // __cplusplus

#endif // __JAUS_SERVICE_INFO_H

/*  End of File */
