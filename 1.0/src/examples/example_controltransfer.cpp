////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_controltransfer.cpp
///  \brief Example program showing how to use the Command Component class to
///  take control and transfer control of other components.
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
#include "jaus/components/cmplib.h"
#include <iostream>

using namespace std;
using namespace Jaus;

bool gExitFlag = false;


int main(int argc, char* argv[])
{
    Address nodeID;     // ID of the node manager.
    CommandComponent controlledComponent;
    CommandComponent commander1;
    CommandComponent commander2;

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
        if(controlledComponent.Initialize("Controlled Component",
                                          Address(nodeID.mSubsystem,
                                                  nodeID.mNode,
                                                  10,
                                                  i)))
        {
            break;
        }
    }

    for(Byte i = 1; i < 255; i++)
    {
        if(commander1.Initialize("Commander 1",
                                 Address(nodeID.mSubsystem,
                                 nodeID.mNode,
                                 12,
                                 i)))
        {
            break;
        }
    }

    for(Byte i = 1; i < 255; i++)
    {
        if(commander2.Initialize("Commander 2",
                                 Address(nodeID.mSubsystem,
                                 nodeID.mNode,
                                 12,
                                 i)))
        {
            break;
        }
    }
    if(!controlledComponent.IsInitialized() ||
        !commander1.IsInitialized() ||
        !commander2.IsInitialized())
    {
        cout << "Failed to initialize components for testing.\n";
        return 0;
    }

    CxUtils::SleepMs(500);

    controlledComponent.SetControllable(true);
    commander1.SetPrimaryStatus(Component::Status::Ready);
    commander2.SetPrimaryStatus(Component::Status::Ready);

    commander1.SetComponentAuthority(10);
    commander2.SetComponentAuthority(10);

    // Take control of component
    cout << "Commander 1 taking control of component...";
    if(commander1.RequestComponentControl(controlledComponent.GetID()))
    {
        cout << "Success!\n";
        cout << "Controlled Component Status: ";
        switch(controlledComponent.GetPrimaryStatus())
        {
        case Component::Status::Ready:
            cout << "Ready\n";
            break;
        case Component::Status::Standby:
            cout << "Standby\n";
            break;
        default:
            cout << "ERROR!\n";
            break;
        };
    }
    else
    {
        cout << "Failure!\n";
    }

    CxUtils::SleepMs(1000);

    // Release control of component
    cout << "Commander 1 releasing control of component...";
    if(commander1.ReleaseComponentControl(controlledComponent.GetID()))
    {
        cout << "Success!\n";
        cout << "Controlled Component Status: ";
        switch(controlledComponent.GetPrimaryStatus())
        {
        case Component::Status::Ready:
            cout << "Ready\n";
            break;
        case Component::Status::Standby:
            cout << "Standby\n";
            break;
        default:
            cout << "ERROR!\n";
            break;
        };
    }
    else
    {
        cout << "Failure!\n";
    }

    CxUtils::SleepMs(1000);

    // Take control of component
    cout << "Commander 2 taking control of component...";
    if(commander2.RequestComponentControl(controlledComponent.GetID()))
    {
        cout << "Success!\n";
        cout << "Controlled Component Status: ";
        switch(controlledComponent.GetPrimaryStatus())
        {
        case Component::Status::Ready:
            cout << "Ready\n";
            break;
        case Component::Status::Standby:
            cout << "Standby\n";
            break;
        default:
            cout << "ERROR!\n";
            break;
        };
    }
    else
    {
        cout << "Failure!\n";
        return 0;
    }

    CxUtils::SleepMs(1000);

    // Take control of component
    cout << "Commander 1 taking control of component...";
    if(commander1.RequestComponentControl(controlledComponent.GetID()))
    {
        cout << "Failure\n";
        cout << "Commander 1 has equal authority to Commander 2 so control should be rejected.\n";
        return 0;
    }
    else
    {
        cout << "Success!\n";
        cout << "Commander 1 was rejected, this is the correct behavior!\n";
    }

    CxUtils::SleepMs(1000);

    cout << "Increasing Commander 1 authority and trying again\n";
    commander1.SetComponentAuthority(100);
    cout << "Commander 1 taking control of component...";
    if(commander1.RequestComponentControl(controlledComponent.GetID()))
    {
        cout << "Success!\n";
        cout << "Controlled Component Status: ";
        switch(controlledComponent.GetPrimaryStatus())
        {
        case Component::Status::Ready:
            cout << "Ready\n";
            break;
        case Component::Status::Standby:
            cout << "Standby\n";
            break;
        default:
            cout << "ERROR!\n";
            break;
        };

        // Check that commander 2 knows it lost control.
        cout << "Checking if Commander 2 was notified of control loss.\n";
        if(commander2.HaveComponentControl(controlledComponent.GetID()))
        {
            cout << "ERROR!\n";
            cout << "Commander 2 was not notified!\n";
            return 0;
        }
        else
        {
            cout << "Success!\n";
            cout << "Commander 2 was notified!\n";
        }
    }
    else
    {
        cout << "Failure!\n";
    }

    CxUtils::SleepMs(1000);

    return 0;
}

/*  End of File */
