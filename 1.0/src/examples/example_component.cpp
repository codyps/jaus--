////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_component.cpp
///  \brief Example/Test Program demonstrating how to use the Component class to
///         send and receive messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 March 2008
///  <br>Last Modified: 25 Aprile 2008
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

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false; // If true, while loops and program exit.

// Temporary global variable used to test
// if a function callback was activated or not.
bool gCallbackHeartbeatPulseFlag = false;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This is an example function callback that is used to
///          demonstrate how to use a callback to receive messages sent
///          to a component.
///
///   \param msg The message received that the callback is mapped to.
///   \param args Any additional arguments used by your callback function.
///
////////////////////////////////////////////////////////////////////////////////////
void TestMessageFunctionCallback(const Message* msg, void* args)
{
    if(msg->GetCommandCode() == JAUS_REPORT_HEARTBEAT_PULSE)
    {
        const ReportHeartbeatPulse* report = dynamic_cast<const ReportHeartbeatPulse*>(msg);
        if(report)
        {
            //cout << "Function Callback received Report Heartbeat Pulse message!\n";
            gCallbackHeartbeatPulseFlag = true;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This is an example program testing/demonstrating the
///          component class.  It shows how to initialize a component, check if
///          it is connected to a Node Manager, how to send messages, and the
///          different ways to get responses/messages sent to the component.
///
////////////////////////////////////////////////////////////////////////////////////
int main(int argc , char *argv[])
{
    Address nodeID;     // ID of the node manager.
    Component component;// Component.

/*
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
*/
    nodeID(1, 2, 1, 1);

    cout << "Initializing component...";
    // Initialize the component.  Every component has
    // a name associated with it, and an ID.  The third parameter
    // used here is the number of bytes to use for buffering incomming
    // messages.  Depending on the number of messages you expect to
    // receive or the size (like video) you may want to make this number larger.
    // by default the buffer is large enough to hold 10 JAUS_MAX_PACKET_SIZE messages.
    if(component.Initialize("My Example Component",
                            Address(nodeID.mSubsystem, nodeID.mNode, 2, 1),
                            JAUS_SHARED_MEMORY_DEFAULT_SIZE))
    {
        cout << "Success!\n";
        // Now lets wait until we are connected to the node manger.
        // Initialization only allocates memory, and verifies there is not
        // another component running with the same ID.  If the node manager is
        // already running (as is the case in this example if we got here) then we
        // should already be connected, however it is possible to initialize a component
        // and then start up a node manger later.  The node manger included with JAUS++ will
        // automatically identify running components and connect to them.
        while(!component.IsConnected())
        {
            Sleep(100);
        }

        // If the component is ready, set the status.
        component.SetPrimaryStatus(Component::Status::Ready);

        // Keep looping until exit flag is set to true, or
        // we are no longer connected to node manager.
        while(gExitFlag == false && component.IsConnected())
        {
            QueryServices queryServices;
            QueryHeartbeatPulse queryHeartbeatPulse;
            Receipt receipt;
            nodeID.mNode = 1;
            queryServices.SetAckNack(Header::AckNack::Request);
            queryServices.SetDestinationID(nodeID);
            queryServices.SetSourceID(component.GetID());

            // This example shows how you can send a message and
            // wait until the response is received.  The response
            // message is stored in a receipt structure.  Note: When the
            // receipt is deleted (or goes out of scope) the message
            // data stored within it will be deleted also.
            cout << "Sending Query message with receipt (blocking)...";
            if(component.Send(&queryServices, receipt) == JAUS_OK)
            {
                // At this point the response message is stored in
                // the Receipt (receipt) which has other information such
                // as how long it took to get the response, etc.

                // See if we received an Acknowledge message also, since
                // we set Ack/Nack to Request Acknowledge.  This is just a test
                // to verify interface performance.  If send count is how many times
                // the message was sent while attempting to get a response.  The
                // maximum send count will always be 3.
                if(receipt.ReceivedAcknowledge() && receipt.GetSendCount() == 1)
                {
                    cout << "Success!\n";
                    // Display received data to console.
                    //const ReportServices* report = dynamic_cast<const ReportServices*>(receipt.GetResponseMessage());
                    //report->Print();
                }
                else
                {
                    cout << "Failure.\n";
                    break;
                }
            }
            else
            {
                cout << "Failure.\n";
            }

            // Check for user input to exit program.
            if(CxUtils::GetChar() == 27)
            {
                gExitFlag = true;
                break;
            }

            // Send a message without using a receipt.  Any responses
            // generated by the receiving components will be sent
            // to any registered callback messages or the ProcessCommandMessage,
            // ProcessInformMessage, ProcessAckNackMessage, or
            // ProcessExperimentalMessage functions of the
            // Component class (depending on the type of message sent).
            queryHeartbeatPulse.SetDestinationID(nodeID);
            queryHeartbeatPulse.SetSourceID(component.GetID());
            queryHeartbeatPulse.SetAckNack(Header::AckNack::Request);

            cout << "Sending Query without blocking...";
            if(component.Send(&queryHeartbeatPulse) == JAUS_OK)
            {
                // Message sent successfully.  Responses from
                // the receiving component (in this example the
                // Node Manager) will be sent to the component
                // and passed to the ProcessAckNackMessage, then
                // ProcessInformMessage function.  This is because
                // the query sent requested Acknowledgement, and the
                // response is an Inform messae.  Users should inherit
                // from the Component class and overload these functions
                // to add additional functionality.

                cout << "Success!\n";
            }
            else
            {
                cout << "Failure.\n";
                break;
            }

            // Check for user input to exit program.
            if(CxUtils::GetChar() == 27)
            {
                gExitFlag = true;
                break;
            }

            Sleep(10);

            // If you need to get a specific type of message, and don't want
            // to inherit from component and overload one of its
            // Process Message Functions, you can use a callback.
            // There are two types of callbacks in this library,
            // function callbacks, or you can inherity from the
            // MessageCallback class and overload its virtual method.
            // This example demonstrates getting a message
            // using a function callback, this feature was added for those who
            // may need it, but you would probably be better off inheriting from the
            // Component class instead.
            cout << "Testing function for messages callbacks...";
            gCallbackHeartbeatPulseFlag = false;
            component.RegisterCallback(JAUS_REPORT_HEARTBEAT_PULSE,
                                       &TestMessageFunctionCallback,
                                       NULL);
            // Now send the message, and wait until the callback
            // function signals to continue
            if(component.Send(&queryHeartbeatPulse) == JAUS_OK)
            {
                // Keep looping, the function callback will set
                // the flag to true if it is called with a
                // Report Hearbeat Pulse message.
                unsigned int loopCount = 0;
                while(gCallbackHeartbeatPulseFlag == false)
                {
                    // Give up after 100 loops.
                    if(++loopCount == 100)
                    {
                        cout << "Failure.\n";
                        component.PrintJausError();
                        break;
                    }
                    Sleep(10);
                }
                cout << "Success!\n";
            }
            else
            {
                cout << "Failure.\n";
                component.PrintJausError();
                break;
            }
            // If we are done with the callback, unregister it.  If you
            // want it to always be called when this type of message
            // is received, do not remove it from the Component object.
            component.RemoveCallback(JAUS_REPORT_HEARTBEAT_PULSE);

            // Check for user input to exit program.
            if(CxUtils::GetChar() == 27)
            {
                gExitFlag = true;
                break;
            }
            Sleep(1500);
        }

        component.Shutdown();
    }
    else
    {
        cout << "Failure!\n";
        // Display the reason for failure.
        component.PrintJausError();
    }

    // Shutdown the component.
    component.Shutdown();

    return 0;
}


/*  End of File */
