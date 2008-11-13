////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_joystick.cpp
///  \brief Example program showing how to use the JoystickDriver class
///  to control a Primitive Driver component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 April 2008
///  <br>Last Modified: 6 May 2008
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


int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    JoystickDriver joystick;
    
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

    cout << "Initializing Component for Discovery... ";
     
    // Initialize the component with any instance ID.
    for(Byte i = 1; i < 255; i++)
    {
        if(joystick.Initialize("JoystickDriver", 
                                Address(nodeID.mSubsystem, nodeID.mNode, 5, i)) == JAUS_OK)
        {
            joystick.AddSubsystemToDiscover(nodeID.mSubsystem);
            break;
        }
    }
  
    if(joystick.IsInitialized())
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        return 0;
    }

    Sleep(50);

    cout << "Initializing Connection to Joystick...";
    
    if(argc > 1)
    {
        if(!joystick.InitializeJoystick(argv[1]))
        {
            if(!joystick.InitializeJoystick())
            {
                cout << "Failure!\n";
                return 0;
            }
        }
    }
    else if(!joystick.InitializeJoystick())
    {
        cout << "Failure!\n";
        return 0;
    }
    cout << "Success!\n";
   
    joystick.SetSubsystemToControl(nodeID.mSubsystem);

    // Transition the component from the standy by state, 
    // which is default after initialization to 
    // a ready state.
    joystick.SetPrimaryStatus(Component::Status::Ready);   

    while(!gExitFlag)
    {
        cout << "=====================================================\n";
        joystick.PrintWrenchEffort();
        joystick.PrintCameraWrench();
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }

        Sleep(100);
    }

    joystick.Shutdown();

    return 0;
}


/*  End of File */
