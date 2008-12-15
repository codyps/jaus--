////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_nodemanager.cpp
///  \brief Test program to validate the Node Manager class works correctly, and
///         to show others how to use it.  For a full Node Manager application,
///         see the wxNodeManager which is the Node Manager with a GUI front end
///         written in wxWidgets.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 11 March 2008
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

#include <vld.h>
#include "jaus/components/cmplib.h"
#include <cxutils/thread.h>
#include <cxutils/timer.h>

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;

void NodeManagerTest();

int main(int argc, char *argv[])
{
    string input;
    NodeManager node;

    if(argc <= 1)
    {
        // Try load using the example XML settings
        // file.
        if(node.Initialize("settings/nodesettings.xml"))
        {
            cout << "JAUS Node Manager initialized." << endl;
        }
        // Initialize the Node Manager with a given
        // subsystem and node number.  The third parameter is
        // set to false because this example uses UDP for inter-node
        // communication.  The fourth parameter is the size of
        // the nodes Shared Memory buffer.  This buffer is used
        // for receiving messages from components that need to
        // be routed.  The default size is 4MB. Make this value
        // larger depending the volume of traffice you expect.
        else if(node.Initialize(2, 1, false, JAUS_NODE_SM_BUFFER_DEFAULT_SIZE))
        {
            cout << "JAUS Node Manager initialized." << endl;
        }
        else
        {
            cout << "Failed to initialize Node Manager." << endl;
            return 0;
        }
    }
    else if(argc == 2)
    {
        // Try load using the example XML settings
        // file.
        if(node.Initialize(argv[1]))
        {
            cout << "JAUS Node Manager initialized." << endl;
        }
        else
        {
            cout << "Failed to initialize Node Manager." << endl;
            return 0;
        }
    }

    // At this point the Node Manager is fully initialized
    // and running.  It will handle all message routing
    // etc.

    //  While node manager does its' thing, display the
    //  subsystem configuration at 1Hz.
    while(!gExitFlag)
    {
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        // Display node configuration data and current time.
        Time::GetUtcTime().PrintTime();
        node.GetSubsystemConfiguration().Print();
        CxUtils::SleepMs(1000);
    }

    return 0;
}

// Test adding and removing components.  This
// is a test program, not a very good example.
void NodeManagerTest()
{
    NodeManager node;

    Component* component = NULL;
    std::vector<Component*> componentList;
    unsigned int updateTimeMs = 0, eventTimeMs = 0;
    if(node.Initialize(80, 2))
    {
        node.EnableSubsystemConfigDiscovery(true);
        // Keep creating and deleting components
        // at random.  Create another node manager
        // with the same subsystem information, and
        // verify that the subsystem configuation data
        // matches. (Try using wxNodeManager).
        while(!gExitFlag)
        {
            if(eventTimeMs != node.GetConnectionEventTimeMs())
            {
                eventTimeMs = node.GetConnectionEventTimeMs();
                node.GetSubsystemConfiguration().Print();
            }

            if( Time::GetUtcTimeMs() - updateTimeMs > 5000)
            {
                if(componentList.size() == 0)
                {
                    for(int i = 1; i <= 5; i++)
                    {
                        component = new Component();
                        if(component->Initialize("Test Component",
                                                 Address(node.GetNodeID().mSubsystem,
                                                         node.GetNodeID().mNode,
                                                         5,
                                                         (Byte)(i))))
                        {
                            CxUtils::SleepMs(150);
                            if(eventTimeMs != node.GetConnectionEventTimeMs())
                            {
                                eventTimeMs = node.GetConnectionEventTimeMs();
                                node.GetSubsystemConfiguration().Print();
                            }
                            componentList.push_back(component);
                            component = NULL;
                        }
                        else
                        {
                            delete component;
                            component = NULL;
                        }
                    }
                }
                else
                {
                    for(int i = 0; i < (int)componentList.size(); i++)
                    {
                        componentList[i]->Shutdown();
                        delete componentList[i];
                        CxUtils::SleepMs(150);
                        if(eventTimeMs != node.GetConnectionEventTimeMs())
                        {
                            eventTimeMs = node.GetConnectionEventTimeMs();
                            node.GetSubsystemConfiguration().Print();
                        }
                    }
                    componentList.clear();
                }
                updateTimeMs = Time::GetUtcTimeMs();
            }
            if(CxUtils::GetChar() == 27)
            {
                gExitFlag = true;
            }
            CxUtils::SleepMs(1);
        }
    }

    for(int i = 0; i < (int)componentList.size(); i++)
    {
        componentList[i]->Shutdown();
        delete componentList[i];
    }
    if(component)
    {
        delete component;
    }
}


/*  End of File */
