////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_subsystem.cpp
///  \brief Example program how to put together a subsystem from JAUS components.
///  This example can be thought of us a simulated unmanned system.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 14 October 2008
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
#include <iostream>
#include <iomanip>
#include "jaus/services/srvclib.h"
#include "jaus/video/videolib.h"
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace CxUtils;

bool gExitFlag = false;

/// Simulated Primitive Driver
class SimPrimitiveDriver : public Jaus::PrimitiveDriver
{
public:
    SimPrimitiveDriver()
    {
        // Component is controllable.
        SetControllable(true);
        mThrust = mSteering = 0;
    }
    ~SimPrimitiveDriver()
    {
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Anytime a Standy by command is received, this function is called.
    ///
    ///  For this driver, when told to Standby, thrust and steering are set to 0.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int Standby()
    {
        mThrust = mSteering = 0.0;
        SetPrimaryStatus(Component::Status::Standby);
        // Adjust motors/actuators if needed.
        return Jaus::OK;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Set up type of services/messages supported by component.
    ///  If this is not done, then other component won't be able
    ///  to detect all capabilities of component
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetupService()
    {
        Jaus::Service service;
        service.SetType(Jaus::Service::PrimitiveDriver);
        // This primitive driver uses the
        // Wrench Effort Messages, so add those
        // to the service.
        // Specifically, this driver use only 
        // thrust on the X axis and steering on the Z
        // axis.
        Jaus::UInt presenceVector = 0;
        Jaus::BitVector::SetBit(presenceVector, Jaus::SetWrenchEffort::VectorBit::PropulsiveLinearEffortX);
        Jaus::BitVector::SetBit(presenceVector, Jaus::SetWrenchEffort::VectorBit::PropulsiveRotationalEffortZ);
        service.AddInputMessage(Jaus::JAUS_SET_WRENCH_EFFORT, presenceVector);
        service.AddInputMessage(Jaus::JAUS_QUERY_WRENCH_EFFORT, presenceVector);
        service.AddOutputMessage(Jaus::JAUS_REPORT_WRENCH_EFFORT, presenceVector);

        // Now add the service.
        AddService(service);
        return Jaus::OK;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Set Discrete Devices Messages are used to change gears, activate
    ///         horns or lights, etc.
    ///
    ///   \param command Set Discrete Deivces command to implement.
    ///
    ///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetDiscreteDevices(const Jaus::SetDiscreteDevices* command)
    {
        if(Jaus::BitVector::IsBitSet(command->GetPresenceVector(), Jaus::SetDiscreteDevices::VectorBit::ParkingBrakeAndHorn))
        {
            if(Jaus::BitVector::IsBitSet(command->GetParkingBrakeAndHornField(), Jaus::SetDiscreteDevices::ParkingBrakeAndHorn::HornOnOff))
            {
                cout << "Light On!\n";
            }
            else
            {
                cout << "Light Off!\n";
            }
        }
        return Jaus::OK;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Any time a command to set the Wrench is received, 
    ///         this function is called.
    ///   
    ///   Verification of message source is done before this function is called
    ///   to make sure they have authority or control of driver.
    ///
    ///   Use this function to send motor/actuator commands, etc.
    ///
    ///   \param command Set Wrench Effort command to implement.
    ///
    ///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetWrenchEffort(const Jaus::SetWrenchEffort* command)
    {
        if(GetPrimaryStatus() == Jaus::Component::Status::Ready)
        {
            if(Jaus::BitVector::IsBitSet(command->GetPresenceVector(), Jaus::SetWrenchEffort::VectorBit::PropulsiveLinearEffortX))
            {
                mThrust = command->GetPropulsiveLinearEffortX();
            }
            if(Jaus::BitVector::IsBitSet(command->GetPresenceVector(), Jaus::SetWrenchEffort::VectorBit::PropulsiveRotationalEffortZ))
            {
                mSteering = command->GetPropulsiveRotationalEffortZ();
            }

            // Now use value to adjust motors, actuators, etc.
            return Jaus::OK;
        }

        return Jaus::FAILURE;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Any time a query is sent to get the current wrench being applied
    ///  this function is called to generate a Report Wrench Effort message.
    ///   
    ///   \param query The query message to respond to.
    ///
    ///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int ReportWrenchEffort(const Jaus::QueryWrenchEffort* query)
    {
        int result = Jaus::FAILURE;
        if(IsInputMessageSupported(query->GetCommandCode(), query->GetPresenceVector()))
        {
            result = Jaus::OK;
            Jaus::ReportWrenchEffort report;
            report.SetSourceID(GetID());
            report.SetDestinationID(query->GetSourceID());
            if(Jaus::BitVector::IsBitSet(query->GetPresenceVector(), Jaus::QueryWrenchEffort::VectorBit::PropulsiveLinearEffortX))
            {
                report.SetPropulsiveLinearEffortX(mThrust);
            }
            if(Jaus::BitVector::IsBitSet(query->GetPresenceVector(), Jaus::QueryWrenchEffort::VectorBit::PropulsiveRotationalEffortZ))
            {
                report.SetPropulsiveRotationalEffortZ(mSteering);
            }
            Send(&report);
        }
        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Any time a control event happens such as control lost or 
    ///         control of this component has been released, then this
    ///         function is called.
    ///
    ///   This overload of this control event function resets the thrust and steering
    ///   values to 0 if control of the primitive driver has been released.
    ///   
    ///   \param type Type of control event.
    ///   \param component The ID of the component generating the event.
    ///   \param message Pointer to any message data associated with the event.
    ///
    ///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int ProcessControlEvent(const Jaus::CommandComponent::ControlEvents type, 
                                    const Jaus::Address& component,
                                    const Jaus::Message* message = NULL)
    {
        if(type == Jaus::CommandComponent::ControlReleased)
        {
            mThrust = mSteering = 0.0;
        }
        return Jaus::OK;
    }
    virtual int ReportPlatformSpecifications(const Jaus::QueryPlatformSpecifications* query) { return Jaus::FAILURE; }
    volatile double mThrust;        ///< Linear thrust on X axis.
    volatile double mSteering;      ///< Rotational thrust on Z axis.
};

/// Simulated Global Pose Sensor
class SimGlobalPoseSensor : public Jaus::GlobalPoseSensor
{
public:
    SimGlobalPoseSensor()
    {
    }
    ~SimGlobalPoseSensor()
    {
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Set up type of services/messages supported by component.
    ///  If this is not done, then other component won't be able
    ///  to detect all capabilities of component
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetupService()
    {
        Jaus::Service service;
        Jaus::UInt presenceVector = 0;
        service.SetType(Jaus::Service::GlobalPoseSensor);

        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Latitude);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Longitude);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Elevation);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Roll);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Pitch);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::Yaw);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::TimeStamp);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::PositionRms);
        Jaus::BitVector::SetBit(presenceVector, Jaus::QueryGlobalPose::VectorBit::AttitudeRms);

        service.AddInputMessage(Jaus::JAUS_QUERY_GLOBAL_POSE, presenceVector);
        service.AddOutputMessage(Jaus::JAUS_REPORT_GLOBAL_POSE, presenceVector);
        
        return AddService(service);        
    }
};

/// Simulated Velocity State Sensor
class SimVelocityStateSensor : public Jaus::VelocityStateSensor
{
public:
    SimVelocityStateSensor()
    {
    }
    ~SimVelocityStateSensor()
    {
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Set up type of services/messages supported by component.
    ///  If this is not done, then other component won't be able
    ///  to detect all capabilities of component
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetupService()
    {
        Jaus::Service service;
        Jaus::UInt presenceVector = 0;
        service.SetType(Jaus::Service::VelocityStateSensor);

        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::VelocityX);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::VelocityY);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::VelocityZ);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::VelocityRms);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::RollRate);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::PitchRate);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::YawRate);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::RateRms);
        Jaus::BitVector::SetBit(presenceVector, Jaus::ReportVelocityState::VectorBit::TimeStamp);

        service.AddInputMessage(Jaus::JAUS_QUERY_VELOCITY_STATE, presenceVector);
        service.AddOutputMessage(Jaus::JAUS_REPORT_VELOCITY_STATE, presenceVector);
        service.AddInputMessage(Jaus::JAUS_QUERY_TRAVEL_SPEED, 0);
        service.AddOutputMessage(Jaus::JAUS_REPORT_TRAVEL_SPEED, 0);
        
        return AddService(service);
    }
};

Jaus::Byte gSubsystem = 1;
Jaus::Byte gNode = 1;

int main(int argc, char *argv[])
{
    Jaus::NodeManager nodeManager;
    SimPrimitiveDriver primitiveDriver;
    SimGlobalPoseSensor globalPoseSensor;
    SimVelocityStateSensor velocityStateSensor;
    Jaus::VisualSensor visualSensor;
    Jaus::GlobalPose globalPose;
    Jaus::ReportVelocityState velocityState;

    // Initialize components.
    globalPoseSensor.Initialize(gSubsystem, gNode);
    velocityStateSensor.Initialize(gSubsystem, gNode);
    primitiveDriver.Initialize(gSubsystem, gNode);
    visualSensor.Initialize(gSubsystem, gNode, 1);

    Jaus::Image imageData;

    if(imageData.LoadFrame("images/calculon640.jpg") == Jaus::FAILURE)
    {
        imageData.Create(640, 480, 3, NULL);
    }

    CxUtils::SleepMs(100);

    // Initialize node manager
    Jaus::Identification identification;
    identification.SetAuthority(0);
    identification.SetType(10001);
    identification.SetIdentification("Megatron");
    
    if(nodeManager.Initialize("settings/example_subsystem_nodesettings.xml") == false)
    {
        Jaus::Configuration config;
        config.AddComponent(Jaus::Address(gSubsystem, gNode, 1, 1));
        config.AddComponent(Jaus::Address(gSubsystem, gNode, (Jaus::Byte)Jaus::Service::Communicator, 1));
        config.AddComponent(Jaus::Address(gSubsystem, gNode, (Jaus::Byte)Jaus::Service::GlobalPoseSensor, 1));
        config.AddComponent(Jaus::Address(gSubsystem, gNode, (Jaus::Byte)Jaus::Service::VelocityStateSensor, 1));
        config.AddComponent(Jaus::Address(gSubsystem, gNode, (Jaus::Byte)Jaus::Service::PrimitiveDriver, 1));
        config.AddComponent(Jaus::Address(gSubsystem, gNode, (Jaus::Byte)Jaus::Service::VisualSensor, 1));
        nodeManager.SetSubsystemIdentification(identification);
        nodeManager.Initialize(gSubsystem, gNode);
        nodeManager.SetSystemConfiguration(config);
        
    }
    else
    {
        gSubsystem = nodeManager.GetNodeID().mSubsystem;
        gNode = nodeManager.GetNodeID().mNode;        
    }
    nodeManager.EnableSubsystemConfigDiscovery(true);


    globalPoseSensor.SetPrimaryStatus(Jaus::Component::Status::Ready);
    velocityStateSensor.SetPrimaryStatus(Jaus::Component::Status::Ready);
    primitiveDriver.SetPrimaryStatus(Jaus::Component::Status::Standby);

    // Near SPAWAR
    globalPose.SetLatitude(32.703356);
    globalPose.SetLongitude(-117.253919);
    globalPose.SetElevation(300);
    globalPose.SetAttitude(0, 0, 0);
    globalPose.SetTimeStamp(Jaus::Time::GetUtcTime());
    globalPose.SetPositionRms(0);
    globalPose.SetAttitudeRms(0);

    globalPoseSensor.SetGlobalPose(globalPose);

    velocityState.SetVelocityX(0);
    velocityState.SetVelocityY(0);
    velocityState.SetVelocityZ(0);
    velocityState.SetVelocityRms(0);
    velocityState.SetRollRate(0);
    velocityState.SetPitchRate(0);
    velocityState.SetYawRate(0);
    velocityState.SetRateRms(0);
    velocityState.SetTimeStamp(Jaus::Time::GetUtcTime());
    
    velocityStateSensor.SetVelocityState(velocityState);
    velocityStateSensor.SetTravelSpeed(0);

    Sleep(50);

    while(!gExitFlag)
    {
        velocityState.SetTimeStamp(Jaus::Time::GetUtcTime());
        globalPose.SetTimeStamp(Jaus::Time::GetUtcTime());
        
        velocityStateSensor.SetVelocityState(velocityState);
        globalPoseSensor.SetGlobalPose(globalPose);

        cout << "Status: ";
        switch(primitiveDriver.GetPrimaryStatus())
        {
        case Jaus::Component::Status::Ready:
            cout << "Ready\n";
            break;
        default:
            cout << "Standby\n";
            break;
        }
        // If component is being controlled, indicate this.
        if(primitiveDriver.GetControllerID().IsValid())
        {
            cout << "Controlled By: " << primitiveDriver.GetControllerID().ToString() << endl;
        }
        // Display current wrench effort being used.
        cout << "Thrust: " << primitiveDriver.mThrust << " Steering: " << primitiveDriver.mSteering << endl;
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        visualSensor.SetCurrentFrame(imageData);
        Sleep(200);
    }

    primitiveDriver.Shutdown();

    return 0;
}


/*  End of File */
