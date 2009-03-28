////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_imagesubscriber.cpp
///  \brief Example demonstrating how to get image data from a Visual Sensor
///  component using the JAUS++ Video Library.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 March 2008
///  <br>Last Modified: 12 June 2008
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
#include <vector>
#ifdef WIN32
#include <vld.h>
#endif
#include "jaus/video/videosubscriber.h"

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;
unsigned int gFrameCount = 0;

void ImageCallback(const Address& source, const Image& rawImage, void* args)
{
    cout << "Frame Number: " << gFrameCount++ << endl; 
}


int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    VideoSubscriber subscriber;
    Image image;

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
        CxUtils::SleepMs(100);
    }
    if(nodeID.IsValid() == false)
    {
        cout << "Failure.\n";
        cout << "Exiting...";
        return 0;
    }

    cout << "Initializing Image Subscriber...";

    // Initialize the component with any instance ID.

    //****************************************************************
    //****************************************************************
    // You must create a larger message inbox for receiving image data
    // at higher resolutions!!
    //****************************************************************
    //****************************************************************
    for(Byte i = 1; i < 255; i++)
    {
        if(subscriber.Initialize("Video Subscriber",
                                 Address(nodeID.mSubsystem, nodeID.mNode, 15, i),
                                 JAUS_VIDEO_SUBSCRIBER_MESSAGE_BOX_SIZE) == JAUS_OK)
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

    Address visualSensorID;
    if(argc > 1)
    {
        int a, b, c, d;
        if(sscanf(argv[1], "%d.%d.%d.%d", &a, &b, &c, &d) == 4)
        {
            visualSensorID((Byte)a, (Byte)b, (Byte)c, (Byte)d);
        }
    }

    CxUtils::SleepMs(50);

    // Set a callback to get the data as it arrives.  The other method
    // to get image data is to inherit from VideoSubscriber and overload
    // the ProcessImage method.
    subscriber.RegisterVideoCallback(ImageCallback, NULL);

    // Transition the component from the standy by state,
    // which is default after initialization to
    // a ready state.
    subscriber.SetPrimaryStatus(Component::Status::Ready);
    bool createdSubscription = false;
    while(!gExitFlag)
    {
        // If connected to Node Manager...
        if(subscriber.IsConnected())
        {
            if(createdSubscription == false)
            {
                bool foundSensor = false;
                if(visualSensorID.IsValid())
                {
                    foundSensor = subscriber.FindVisualSensors(visualSensorID.mSubsystem);
                }
                else
                {
                    foundSensor = subscriber.FindVisualSensors(subscriber.GetID().mSubsystem);
                }

                if(foundSensor)
                {
                    VideoSubscriber::SensorInfo info = subscriber.GetVisualSensorInfo(0);
                    if(subscriber.CreateVideoSubscription(info.mID))
                    {
                        createdSubscription = true;
                    }
                }
            }
        }
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        CxUtils::SleepMs(100);
    }

    subscriber.CancelVideoSubscription();

    return 0;
}

/*  End of File */
