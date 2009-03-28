////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_posesensor.cpp
///  \brief Examples demonstrating/testing the GlobalPoseSensor class.  The
///  GlobalPoseSensor class automatically handles service connection and
///  some types of event requests.  To see an example of a component accessing
///  the services provided by this component see example_eventsubscriber and
///  example_scsubscriber.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 March 2008
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
#include <algorithm>
#ifdef WIN32
#include <vld.h>
#endif
#include "jaus/services/srvclib.h"

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;


int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    GlobalPose globalPose;
    GlobalPoseSensor sensor;
    
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

    // Intialize global pose data.
    globalPose.SetPosition(34.3543, -115.817389, 0.0);
    globalPose.SetAttitude(0.0, 0.0, 0.0);
    globalPose.SetTimeStamp(Time::GetUtcTime());

    // At this point, we must add support for the
    // service of Global Pose Sensor.  This MUST be done
    // so that the component knows what Global Pose data/messages
    // are supported.

    // This example manually adds the Global Pose Sensor service
    // to the GlobalPoseSensor component.  An alternative method of
    // doing this is to inherit from the GlobalPoseSensor class and
    // override the SetupService virtual function.  See example_primitivedriver.cpp
    // or example_velocitysensor.cpp for this behavior.

    UInt globalPosePresenceVector = 0;
    Service myService;

    
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Latitude);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Longitude);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Elevation);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Roll);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Pitch);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::Yaw);
    BitVector::SetBit(globalPosePresenceVector, QueryGlobalPose::VectorBit::TimeStamp);

    myService.SetType(Service::GlobalPoseSensor);
    myService.AddInputMessage(JAUS_QUERY_GLOBAL_POSE, globalPosePresenceVector);
    myService.AddOutputMessage(JAUS_REPORT_GLOBAL_POSE, globalPosePresenceVector);

    sensor.AddService(myService);

    // Set the maximum update rate of the sensor.
    sensor.SetSensorUpdateRate(100);
    //sensor.BlockAckNackMessages(true);
    cout << "Initializing Global Pose Sensor...";

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

        globalPose.SetLongitude(globalPose.GetLongitude() - .0005);
        if(globalPose.GetLongitude() < -116.244797)  //  Loop back to beginning.
            globalPose.SetLongitude(-115.817389);
        
        if(globalPose.GetYaw() + JAUS_DEG2RAD(.2) >= GlobalPose::Limits::MaxAttitude)
            globalPose.SetYaw(JAUS_DEG2RAD(-180.0)); //  Wrap around again.
        else
            globalPose.SetYaw(globalPose.GetYaw() + JAUS_DEG2RAD(.2));
        
        globalPose.SetTimeStamp(Time::GetUtcTime());

        //  Set the new global pose.  This will
        //  save the data internally and generate 
        //  events and update service connections subscribing
        //  to this data.
        sensor.SetGlobalPose(globalPose);

        // Display values to console.
        globalPose.PrintGlobalPose();

        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(200);
    }

    return 0;
}

/*  End of File */
