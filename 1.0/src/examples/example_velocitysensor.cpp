////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_velocitysensor.cpp
///  \brief Examples demonstrating/testing the VelocitySensorSensor class.  The
///  VelocitySensorSensor class automatically handles service connection and
///  some types of event requests.  To see an example of a component accessing
///  the services provided by this component see example_eventsubscriber and
///  example_scsubscriber.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 March 2008
///  <br>Last Modified: 18 May 2008
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
#include <algorithm>

#include <vld.h>
#include "jaus/services/srvclib.h"

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;

class FakeVelocityStateSensor : public VelocityStateSensor
{
public:
    FakeVelocityStateSensor()
    {
        SetControllable(false);
    }
    ~FakeVelocityStateSensor()
    {
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \brief Add the service for Velocity State Sensor.  This is not done
    ///  by the parent class because it doesn't know which parts of the
    ///  Report Velocity State message you will support.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual int SetupService()
    {
        Service service;
        service.SetType(Service::VelocityStateSensor);
        // This sensor contains velocity information around x and rotation 
        // around z with time stamp data.
        UInt presenceVector = 0;
    

        BitVector::SetBit(presenceVector, ReportVelocityState::VectorBit::VelocityX);
        BitVector::SetBit(presenceVector, ReportVelocityState::VectorBit::YawRate);
        BitVector::SetBit(presenceVector, ReportVelocityState::VectorBit::TimeStamp);
        service.AddInputMessage(JAUS_QUERY_VELOCITY_STATE, presenceVector);
        service.AddOutputMessage(JAUS_REPORT_VELOCITY_STATE, presenceVector);
        
        service.AddInputMessage(JAUS_QUERY_TRAVEL_SPEED, 0);
        service.AddOutputMessage(JAUS_REPORT_TRAVEL_SPEED, 0);

        // Now add the service.
        AddService(service);
        return JAUS_OK;
    }
};


int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    ReportVelocityState velocityState;
    FakeVelocityStateSensor sensor;
    
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

    // Intialize velocity state data.
    velocityState.SetVelocityX(0);
    velocityState.SetYawRate(0);
    velocityState.SetTimeStamp(Time::GetUtcTime());

    // Set the maximum update rate of the sensor.
    sensor.SetSensorUpdateRate(50);
    //sensor.BlockAckNackMessages(true);
    cout << "Initializing Velocity State Sensor...";

    // Instance is set to 0, so the Initialize function will
    // use the first available instance number.
    if(sensor.Initialize(nodeID.mSubsystem, nodeID.mNode, 0))
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        sensor.PrintJausError();
        return 0;
    }

    Sleep(50);

    // Transition the sensor from the standy by state, 
    // which is default after initialization to 
    // a ready state.
    sensor.SetPrimaryStatus(Component::Status::Ready);

    while(!gExitFlag)
    {
        // Simulate real pose sensors by modifying values.

        velocityState.SetVelocityX(velocityState.GetVelocityX() + .1);
        if(velocityState.GetVelocityX() > 10)  //  Loop back to beginning.
            velocityState.SetVelocityX(0);
        
        if(velocityState.GetYawRate() + 0.2 >= ReportVelocityState::Limits::MaxRate)
            velocityState.SetYawRate(ReportVelocityState::Limits::MinRate); //  Wrap around again.
        else
            velocityState.SetYawRate(velocityState.GetYawRate() + 0.2);
        
        velocityState.SetTimeStamp(Time::GetUtcTime());

        //  Set the new velocity state.  This will
        //  save the data internally and generate 
        //  events and update service connections subscribing
        //  to this data.
        sensor.SetVelocityState(velocityState);
        // Set travel speed.
        sensor.SetTravelSpeed(0.13/*velocityState.GetVelocityX()*/);

        // Display values to console.
        velocityState.PrintVelocityState();

        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(200);
    }

    return 0;
}

/*  End of File */
