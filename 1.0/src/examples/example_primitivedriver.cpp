////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_primitivedriver.cpp
///  \brief Example program showing how to use the PrimitiveDriver class
///  to create a primitive driver service component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 April 2008
///  <br>Last Modified: 25 April 2008
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
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace CxUtils;
using namespace Jaus;

bool gExitFlag = false;

class MyPrimitiveDriver : public PrimitiveDriver
{
public:
    MyPrimitiveDriver()
    {
        // Component is controllable.
        SetControllable(true);
        mThrust = mSteering = 0;
    }
    ~MyPrimitiveDriver()
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
        return JAUS_OK;
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
        Service service;
        service.SetType(Service::PrimitiveDriver);
        // This primitive driver uses the
        // Wrench Effort Messages, so add those
        // to the service.
        // Specifically, this driver use only 
        // thrust on the X axis and steering on the Z
        // axis.
        UInt presenceVector = 0;
        BitVector::SetBit(presenceVector, SetWrenchEffort::VectorBit::PropulsiveLinearEffortX);
        BitVector::SetBit(presenceVector, SetWrenchEffort::VectorBit::PropulsiveRotationalEffortZ);
        service.AddInputMessage(JAUS_SET_WRENCH_EFFORT, presenceVector);
        service.AddInputMessage(JAUS_QUERY_WRENCH_EFFORT, presenceVector);
        service.AddOutputMessage(JAUS_REPORT_WRENCH_EFFORT, presenceVector);

        // Now add the service.
        AddService(service);
        return JAUS_OK;
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
        return JAUS_OK;
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
        if(GetPrimaryStatus() == Component::Status::Ready)
        {
            if(BitVector::IsBitSet(command->GetPresenceVector(), Jaus::SetWrenchEffort::VectorBit::PropulsiveLinearEffortX))
            {
                mThrust = command->GetPropulsiveLinearEffortX();
            }
            if(BitVector::IsBitSet(command->GetPresenceVector(), Jaus::SetWrenchEffort::VectorBit::PropulsiveRotationalEffortZ))
            {
                mSteering = command->GetPropulsiveRotationalEffortZ();
            }

            // Now use value to adjust motors, actuators, etc.
            return JAUS_OK;
        }

        return JAUS_FAILURE;
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
    virtual int ReportWrenchEffort(const QueryWrenchEffort* query)
    {
        int result = JAUS_FAILURE;
        if(IsInputMessageSupported(query->GetCommandCode(), query->GetPresenceVector()))
        {
            result = JAUS_OK;
            Jaus::ReportWrenchEffort report;
            report.SetSourceID(GetID());
            report.SetDestinationID(query->GetSourceID());
            if(BitVector::IsBitSet(query->GetPresenceVector(), QueryWrenchEffort::VectorBit::PropulsiveLinearEffortX))
            {
                report.SetPropulsiveLinearEffortX(mThrust);
            }
            if(BitVector::IsBitSet(query->GetPresenceVector(), QueryWrenchEffort::VectorBit::PropulsiveRotationalEffortZ))
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
    virtual int ProcessControlEvent(const ControlEvents type, 
                                    const Address& component,
                                    const Message* message = NULL)
    {
        if(type == Jaus::CommandComponent::ControlReleased)
        {
            mThrust = mSteering = 0.0;
        }
        return Jaus::JAUS_OK;
    }
    virtual int ReportPlatformSpecifications(const QueryPlatformSpecifications* query) { return JAUS_FAILURE; }
    volatile double mThrust;        ///< Linear thrust on X axis.
    volatile double mSteering;      ///< Rotational thrust on Z axis.
};

int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    MyPrimitiveDriver driver;

    cout << "Looking for node manager...";
    while(gExitFlag == false)
    {
        if(Component::IsNodeManagerPresent(&nodeID))
        {
            cout << "Success!\n";
            cout << "Node Manager ID is: ";
            nodeID.PrintID();
            break;
        }
        Sleep(100);
    }
    if(nodeID.IsValid() == false)
    {
        cout << "Failure.\n";
        cout << "Exiting...";
        return 0;
    }

    cout << "Initializing Component for Discovery...";

    // Initialize the component with any instance ID.
    driver.Initialize(nodeID.mSubsystem, nodeID.mNode, 0);
    if(driver.IsInitialized())
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        return 0;
    }

    Sleep(50);

    while(!gExitFlag)
    {
        cout << "Status: ";
        switch(driver.GetPrimaryStatus())
        {
        case Component::Status::Ready:
            cout << "Ready\n";
            break;
        default:
            cout << "Standby\n";
            break;
        }
        // If component is being controlled, indicate this.
        if(driver.GetControllerID().IsValid())
        {
            cout << "Controlled By: " << driver.GetControllerID().ToString() << endl;
        }
        // Display current wrench effort being used.
        cout << "Thrust: " << driver.mThrust << " Steering: " << driver.mSteering << endl;
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(200);
    }

    driver.Shutdown();

    return 0;
}


/*  End of File */
