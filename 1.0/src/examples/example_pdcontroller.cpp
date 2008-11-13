////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_pdcontroller.cpp
///  \brief Simple example program demonstrating how to take control of and send
///         wrench effort messages to a Primitive Driver component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 16 June 2008
///  <br>Last Modified: 16 June 2008
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
#include "jaus/components/cmplib.h"
#include <iostream>

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;


int main(int argc, char* argv[])
{
    Address nodeID;            // ID of the node manager.
    Address primitiveDriverID; // ID of primitive driver.
    CommandComponent controller;

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

    // Initialize all three components (get any vailable ID).
    for(Byte i = 1; i < 255; i++)
    {
        if(controller.Initialize("Primitive Driver Controller",
                                  Address(nodeID.mSubsystem,
                                          nodeID.mNode,
                                          10,
                                          i)))
        {
            break;
        }
    }
    
    // Wait to connect to Node Manager.
    CxUtils::SleepMs(500);

    controller.SetPrimaryStatus(Component::Status::Ready);

    CxUtils::SleepMs(1000);

    cout << "Finding a Primitive Driver to Control.\n";
    QueryConfiguration queryConfiguration;
    Receipt receipt;

    queryConfiguration.SetDestinationID(nodeID);
    queryConfiguration.SetSourceID(controller.GetID());
    queryConfiguration.SetQueryField(QueryConfiguration::Subsystem);
    
    if(controller.Send(&queryConfiguration, receipt) == JAUS_OK)
    {
        const ReportConfiguration* report = dynamic_cast<const ReportConfiguration*>(receipt.GetResponseMessage());
        Address::List drivers;
        drivers = report->GetConfiguration()->GetComponentsOfType(Service::PrimitiveDriver);
        if(drivers.size() == 0)
        {
            cout << "No Primitive Drivers Available for Control.\n";
        }
        primitiveDriverID = *drivers.begin();
    }
    else
    {
        cout << "Failed to get Subsytem Configuration.\n";
        return 0;
    }
  
    cout << "Taking control of Primitive Driver...";
    if(controller.RequestComponentControl(primitiveDriverID) == JAUS_OK)
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failed.\n";
        return 0;
    }

    // Resume the primitive driver and start sending commands.
    
    Resume resumeCommand;
    Standby standbyCommand;
    SetWrenchEffort setWrenchEffortCommand;

    resumeCommand.SetDestinationID(primitiveDriverID);
    resumeCommand.SetSourceID(controller.GetID());
    standbyCommand.SetDestinationID(primitiveDriverID);
    standbyCommand.SetSourceID(controller.GetID());
    setWrenchEffortCommand.SetDestinationID(primitiveDriverID);
    setWrenchEffortCommand.SetSourceID(controller.GetID());

    controller.Send(&resumeCommand);

    double speed = 0;
    bool forward = true;

    while(!gExitFlag)
    {
        if(forward)
        {
            speed += 0.5;
        }
        else
        {
            speed -= 0.5;
        }

        if(speed > 100.0)
        {
            speed = 100.0;
            forward = false;
        }
        else if(speed < -100.0)
        {
            speed = -100.0;
            forward = true;
        }
        setWrenchEffortCommand.SetPropulsiveLinearEffortX(speed);
        setWrenchEffortCommand.SetPropulsiveRotationalEffortZ(0);
        if(controller.Send(&setWrenchEffortCommand))
        {
            setWrenchEffortCommand.PrintWrenchEffort();
        }
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(100);
    }

    controller.Send(&standbyCommand);
    controller.ReleaseComponentControl(primitiveDriverID);

    return 0;
}

/*  End of File */
