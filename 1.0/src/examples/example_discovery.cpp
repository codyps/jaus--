////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_discovery.cpp
///  \brief Example program showing how to use the SubscriberComponent class
///  to discovery subsystem configurations and identifications.
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
#include "jaus/components/cmplib.h"
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace CxUtils;
using namespace Jaus;

bool gExitFlag = false;

class MyComponent : public SubscriberComponent
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief Any time a discovery event happens, this method is called.  Overload
    ///   it to add whatever capabilities you need.
    ///
    ///   Another way to access the discovered system configuration is use the
    ///   GetSystemConfiguration().  Using this virtual method is best because 
    ///   you will get the data as soon as it updates.
    ///
    ///   \param subsystem The subsystem platform information.
    ///   \param eventType The type of discovery event.  SubsystemConnected, 
    ///                    SubsystemDisconnect, or SubsystemUpdate.
    ///
    ///   \return JAUS_OK.
    ///
    //////////////////////////////////////////////////////////////////////////////////// 
    virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                      const DiscoveryEvents eventType = SubsystemUpdate)
    {
        if(eventType == SubscriberComponent::SubsystemConnected)
        {
            std::cout << "\n===========================================================\n";
            if(subsystem.HaveIdentification())
            {
                std::cout << "Subsystem [" 
                          << (int)subsystem.GetSubsystemID()
                          << " - " << subsystem.GetIdentification()->GetIdentification()
                          << "] Discovered!\n";
            }
            else
            {
                std::cout << "Subsystem [" << (int)subsystem.GetSubsystemID() << "] Discovered!\n";
            }
            std::cout << "===========================================================\n"; 
        }
        else if(eventType == SubscriberComponent::SubsystemDisconnect)
        {
            std::cout << "\n===========================================================\n";
            if(subsystem.HaveIdentification())
            {
                std::cout << "Subsystem [" 
                          << (int)subsystem.GetSubsystemID() 
                          << " - " << subsystem.GetIdentification()->GetIdentification() 
                          << "] Disconnected.\n";
            }
            else
            {
                std::cout << "Subsystem [" << (int)subsystem.GetSubsystemID() << "] Disconnected!\n";
            }
            std::cout << "===========================================================\n";
        }
        else if(eventType == SubscriberComponent::SubsystemUpdate)
        {
            // Print the configuration to the screen.
            std::cout << "Subsystem Update!\n";
            subsystem.Print();

            // See if there is a global pose sensor on the subsystem, and if so
            // try get a subscription.
            Address::List sensors = subsystem.GetConfiguration()->GetComponentsOfType(Service::GlobalPoseSensor);
            if(sensors.size() > 0)
            {               
                Address::List::iterator id;
                UShort pv = 0;
                BitVector::SetBit(pv, ReportGlobalPose::VectorBit::Latitude);
                BitVector::SetBit(pv, ReportGlobalPose::VectorBit::Longitude);
                for(id = sensors.begin();
                    id != sensors.end();
                    id++)
                {
                    if(HaveServiceConnection(*id, JAUS_REPORT_GLOBAL_POSE))
                        continue;

                    if(CreateInformServiceConnection(*id, 
                                                     JAUS_REPORT_GLOBAL_POSE,
                                                     pv,
                                                     5))
                    {
                        break;
                    }
                }
            }
        }

        // Pass to parent in case it needs the data too.
        return SubscriberComponent::ProcessDiscoveryEvent(subsystem, eventType);
    }
};

int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    MyComponent subscriber;

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
    for(Byte i = 1; i < 255; i++)
    {
        if(subscriber.Initialize("Subsystem Discovery Component", 
                                 Address(nodeID.mSubsystem, nodeID.mNode, 4, i)) == JAUS_OK)
        {
            break;
        }
    }
    if(subscriber.IsInitialized())
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        return 0;
    }

    Sleep(50);

    // Transition the component from the standy by state, 
    // which is default after initialization to 
    // a ready state.
    subscriber.SetPrimaryStatus(Component::Status::Ready);

    // Enable discovery, but only acquire information from
    // this subsystem and subsystems 8.
    std::set<Byte> toDiscover;
    // We do not need to add our own subsystem
    // number to this list, because it is added automatically.  Pass
    // a NULL value to discover all subssytems.
    toDiscover.insert(1);
    toDiscover.insert(8);
    toDiscover.insert(10);
    subscriber.EnableSubsystemDiscovery(true, &toDiscover);

	Platform::Map platforms;
    while(!gExitFlag)
    {
        // While the program is running, any time a 
        // new subsystem is discovered, updated, or dissapears
        // the ProcessDiscoveryEvent function will be called.

        // You can also access the current system configuration
        // from the component using GetSystemConfiguration function
        // of Subscriber Component.
		

		// Get a copy of system configuration discovered.
		platforms = subscriber.GetSystemConfiguration();

        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(200);
    }

    subscriber.Shutdown();

    return 0;
}


/*  End of File */
