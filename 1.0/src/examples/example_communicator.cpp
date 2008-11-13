////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_communicator.cpp
///  \brief Example program showing how to use the Communicator class to 
///  add custom/new Data Link connections for communication with other
///  subsystems.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 August 2008
///  <br>Last Modified: 12 August 2008
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
#include "jaus/components/node/nodemanager.h"
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace CxUtils;
using namespace Jaus;

bool gExitFlag = false;

// Create a custom data link for the Communicator. Data links
// are used to communicate to other subsystems.

class CustomDataLink : public Communicator::DataLink
{
public:
    CustomDataLink()
    {
        mLinkState = Communicator::DataLink::Off;
    }

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief The SetState method is used to turn on/off or put a Data Link into
    ///   Standby operation.
    ///
    ///   \param state Desired state of the data link.
    ///
    ///   \return True if successfully transitioned to the desired state, false on
    ///           failure.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual bool SetState(const Communicator::DataLink::State state)
    {
        bool result = true;

        if(state == Communicator::DataLink::On)
        {
            mLinkState = Communicator::DataLink::On;
            // Add code here to turn on the data link.  This
            // could be creating UDP connections, etc.
        }
        else if(state == Communicator::DataLink::Off)
        {
            mLinkState = Communicator::DataLink::Off;
            // Add code here to turn off the data link.  This could
            // be closing sockets or disconnecting from hardware interfacets.
        }
        else if(state == Communicator::DataLink::Standby)
        {
            mLinkState = Communicator::DataLink::Standby;
            // Put link in standby so that no transmission takes place.  But 
            // maintain the link.
        }

        return result;
    }
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief The Transmit method sends a serialized JAUS message over the
    ///   data link to other subsystems.  Overload this method to implement how
    ///   data is sent over the link and to add any additional encrypition or
    ///   transport layer data to the JAUS message before transmission.
    ///
    ///   \param data Serialzed JAUS message data to transmit.
    ///
    ///   \return True if the message was sent over the data link, false on failure.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    virtual bool Transmit(const Stream& data)
    {
        if(mLinkState == Communicator::DataLink::On)
        {
            // Add any additional transport information to the JAUS message
            // or apply encryption etc. and then send the data out over
            // the data link you've created.
            return true;
        }

        return false;
    }
    // Gets the state of the link.

    virtual Communicator::DataLink::State GetState() const { return mLinkState; }
protected:
    Communicator::DataLink::State mLinkState;
};

int main(int argc, char *argv[])
{
    string input;
    NodeManager node;
    
    // Initialize the node manager.
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
        // be routed.  The default size is 2MB. Make this value
        // larger depending the volume of traffice you expect.
        else if(node.Initialize(1, 1, false, JAUS_NODE_SM_DEFAULT_SIZE))
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

    // By default the Commnicator contained within the Node Manager
    // had a Default Data Link (DefaultDataLink class) which uses 
    // UDP multicast/broadcast and unicast for communication with
    // other subsystems.  The following steps can be used to 
    // add a new data link and disable the default one.

    // Lets add a new data link to it.
    CustomDataLink* newLink = new CustomDataLink();
    // Set a link ID
    newLink->SetID(3);
    
    // Perform any operations you need to on your data link
    // to initialize it or provide default values.
    

    // Now add the link to the Communicator. (communicator now owns pointer).
    node.GetCommunicator()->AddDataLink(newLink);
    newLink = NULL;
    // Lets disable the default link.
    node.GetCommunicator()->SetDataLinkState(Communicator::DataLink::Off);
    // Switch to the new data link we just added.
    node.GetCommunicator()->SetDataLinkSelect(3);
    // Turn on the data link.
    node.GetCommunicator()->SetDataLinkState(Communicator::DataLink::On);

    // Done, that's it you've added a new communication link to subsystems.

    while(!gExitFlag)
    {
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        // Display node configuration data and current time.
        Jaus::Time::GetUtcTime().PrintTime();
        node.GetSubsystemConfiguration().Print();
        CxUtils::SleepMs(1000);
    }

    return 0;
}


/*  End of File */
