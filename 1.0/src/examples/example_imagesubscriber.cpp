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
#include <vld.h>
#include "jaus/jaus++.h"

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;
Image::Format gImageFormat = Image::JPEG;

////////////////////////////////////////////////////////////////////////////////////
///
///   \class VideoSubscriber
///   \brief Simple component whith the purpose of requesting events
///   from a Visual Sensor components.
///
///   This example shows how to use the SubscriberComponent class to create
///   a new class which requests services and events from another component.
///
////////////////////////////////////////////////////////////////////////////////////
class VideoSubscriber : public SubscriberComponent
{
public:
    VideoSubscriber()
    {
        mStartTimeMs = mUpdateTimeMs = mRecvCount = mTotalCount = 0;
    }
    ~VideoSubscriber()
    {
    }
    // Overload the method for processing Inform Messages recevied
    // by the class.
    virtual int ProcessInformMessage(const Message* msg)
    {
        int processed = JAUS_OK;
        switch(msg->GetCommandCode())
        {
        case JAUS_REPORT_IMAGE:
            {
                const Jaus::ReportImage* report = dynamic_cast<const ReportImage*>(msg);
                if(report)
                {
                    // Decompress the image data.
                    Image decompressed;

                    if(decompressed.Decompress(report->GetImageData(), report->GetDataSize(), gImageFormat))
                    {
                        if(mTotalCount == 0)
                        {
                            mStartTimeMs = mUpdateTimeMs = CxUtils::Timer::GetTimeMs();
                        }
                        else
                        {
                            mUpdateTimeMs = CxUtils::Timer::GetTimeMs();
                        }
                        mRecvCount++;
                        mTotalCount++;
                        cout << "Frame Number: " << mRecvCount << endl;
                    }
                }
            }
            break;
        default:
            // Let the parent class process the message.  It will
            // process a lot of messages for us. ALWAYS DO THIS!
            processed = SubscriberComponent::ProcessInformMessage(msg);
            break;
        };
        return processed;
    }
    ReportImage mReportImage;               ///<  The last global pose message received.
    volatile double mStartTimeMs;           ///<  Time of first message received.
    volatile double mUpdateTimeMs;          ///<  The last time in ms that a Report Global Pose message was received.
    volatile unsigned int mRecvCount;       ///<  The number of Report Global Pose in between FPS calculations.
    volatile unsigned int mTotalCount;      ///<  Total number of messages received.
};

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
        Sleep(100);
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
                                 JAUS_VISUAL_SENSOR_MESSAGE_BOX_SIZE) == JAUS_OK)
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
    bool createdEvent = false;
    while(!gExitFlag)
    {
        // If connected to Node Manager...
        if(subscriber.IsConnected())
        {
            // If no subscription has been made to any service provider, try create one.
            if(!createdEvent && subscriber.HaveEventSubscriptionsOfType(JAUS_REPORT_IMAGE) == false)
            {
                // Clear receive count information.
                subscriber.mTotalCount = 0;
                subscriber.mRecvCount = 0;
                subscriber.mUpdateTimeMs = subscriber.mStartTimeMs = 0;

                QueryConfiguration queryConfiguration;
                Receipt receipt;

                queryConfiguration.SetSourceID(subscriber.GetID());
                queryConfiguration.SetDestinationID(nodeID);
                queryConfiguration.SetQueryField(QueryConfiguration::Subsystem);

                // Get the configuration of the subsystem to see if a Global Pose Sensor
                // is present.
                if(subscriber.Send(&queryConfiguration, receipt) == JAUS_OK)
                {
                    const ReportConfiguration* reportConfiguration =
                                                    dynamic_cast<const ReportConfiguration*>(receipt.GetResponseMessage());
                    if(reportConfiguration)
                    {
                        // Find a visual sensor.
                        Address::List sensors =
                            reportConfiguration->GetConfiguration()->GetComponentsOfType((Byte)Service::VisualSensor);

                        if(sensors.size() > 0)
                        {
                            cout << "Create Event from Visual Sensor " << sensors.front().ToString() << "...";
                            CreateEventRequest createEvent;

                            createEvent.SetSourceID(subscriber.GetID());
                            createEvent.SetDestinationID(sensors.front());
                            createEvent.SetMessageCode(JAUS_REPORT_IMAGE);
                            createEvent.SetEventType(CreateEventRequest::EveryChange);

                            // Request the event.
                            if(subscriber.RequestEvent(createEvent) == JAUS_OK)
                            {
                                cout << "Success!\n";
                                createdEvent = true;
                                // At this point Global Pose data should start
                                // arriving to our component, and will be passed
                                // to the ProcessInformMessage function.
                            }
                            else
                            {
                                cout << "Failure!\n";
                            }
                        }
                    }
                }
            }
        }
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(100);
    }

    return 0;
}

/*  End of File */
