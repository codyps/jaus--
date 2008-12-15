////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_eventsubscriber.cpp
///  \brief Example program showing how to use the SubscriberComponent class
///  to create a component that subscribes to Global Pose information using
///  events.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 April 2007
///  <br>Last Modified: 25 April 2008
///  <br>Copyright (c) 2007
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
bool gUsePeriodicEvents = false;
double gPeriodicRate = 2.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \class GlobalPoseEventSubscriber
///   \brief Simple component whith the purpose of requesting events
///   from a Global Pose Sensor components.
///
///   This example shows how to use the SubscrberComponent class to create
///   a new class which requests services and events from another component.
///
////////////////////////////////////////////////////////////////////////////////////
class GlobalPoseEventSubscriber : public SubscriberComponent
{
public:
    GlobalPoseEventSubscriber()
    {
        mStartTimeMs = mUpdateTimeMs = mRecvCount = mTotalCount = 0;
    }
    ~GlobalPoseEventSubscriber()
    {
    }
    // Overload the method for processing Inform Messages recevied
    // by the class.
    virtual int ProcessInformMessage(const Message* msg)
    {
        int processed = JAUS_OK;
        switch(msg->GetCommandCode())
        {
        case JAUS_REPORT_GLOBAL_POSE:
            {
                const ReportGlobalPose* report = dynamic_cast<const ReportGlobalPose*>(msg);
                if(report)
                {
                    mReportGlobalPose = *report;

                    // If a periodic event was created, spit out
                    // some stats about update rate, etc.
                    if(gUsePeriodicEvents)
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
                        // If we've received 10 messages, calculate the upate
                        // rate, so we can verify we are getting information at
                        // the rate requested.
                        if(mRecvCount > gPeriodicRate*2)
                        {
                            double fps = (mTotalCount - 1)*1000.0/(mUpdateTimeMs - mStartTimeMs);
                            mRecvCount = 0;
                            mStartTimeMs = CxUtils::Timer::GetTimeMs();
                            mTotalCount = 0;
                            cout << "\n=======================================================\n";
                            cout << "Update Rate: " << fps << " FPS" << endl;
                            cout << "=======================================================\n\n";
                        }
                    }
                }
                // Display global pose values, but not at high speeds (50 Hz) when using periodic events.
                if(gUsePeriodicEvents == false || (gUsePeriodicEvents && gPeriodicRate < 15))
                {
                    mReportGlobalPose.PrintGlobalPose();
                }
            }
            break;
        case JAUS_REPORT_VELOCITY_STATE:
            {
                const ReportVelocityState* report = dynamic_cast<const ReportVelocityState*>(msg);
                if(report)
                {
                    report->PrintVelocityState();
                }
            }
            break;
        case JAUS_REPORT_TRAVEL_SPEED:
            {
                const ReportTravelSpeed* report = dynamic_cast<const ReportTravelSpeed*>(msg);
                if(report)
                {
                    cout << "Travel Speed: " << report->GetSpeed() << endl;
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
    ReportGlobalPose mReportGlobalPose;     ///<  The last global pose message received.
    volatile double mStartTimeMs;           ///<  Time of first message received.
    volatile double mUpdateTimeMs;          ///<  The last time in ms that a Report Global Pose message was received.
    volatile unsigned int mRecvCount;       ///<  The number of Report Global Pose in between FPS calculations.
    volatile unsigned int mTotalCount;      ///<  Total number of messages received.
};

int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    GlobalPoseEventSubscriber subscriber;

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

    cout << "Initializing Global Pose Subscriber...";

    // Initialize the component with any instance ID.
    for(Byte i = 1; i < 255; i++)
    {
        if(subscriber.Initialize("Global Pose Subscriber",
                                 Address(nodeID.mSubsystem, nodeID.mNode, 3, i)) == JAUS_OK)
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
    bool createdVelocityEvent = false;
    while(!gExitFlag)
    {
        // If connected to Node Manager...
        if(subscriber.IsConnected())
        {
            // If no subscription has been made to any service provider, try create one.
            if(!createdEvent && subscriber.HaveEventSubscriptionsOfType(JAUS_REPORT_GLOBAL_POSE) == false)
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
                        // Find a global pose sensors.
                        Address::List sensors =
                            reportConfiguration->GetConfiguration()->GetComponentsOfType((Byte)Service::GlobalPoseSensor);

                        if(sensors.size() > 0)
                        {
                            cout << "Create Event from Global Pose Sensor " << sensors.front().ToString() << "...";
                            // Try Create an Event for Global Pose data.
                            // Depending on the component instance of this component
                            // create a different kind of event.  This is done to
                            // show how to create (and test) the different types of events.
                            UShort presenceVector = 0;
                            CreateEventRequest createEvent;
                            QueryGlobalPose queryGlobalPose;

                            createEvent.SetSourceID(subscriber.GetID());
                            createEvent.SetDestinationID(sensors.front());
                            createEvent.SetMessageCode(JAUS_REPORT_GLOBAL_POSE);

                            // Set the query options for the event. (we want latitude and longitude).
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Latitude);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Longitude);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Elevation);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Roll);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Pitch);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::Yaw);
                            BitVector::SetBit(presenceVector, QueryGlobalPose::VectorBit::TimeStamp);
                            queryGlobalPose.SetPresenceVector(presenceVector);
                            createEvent.SetQueryMessage(&queryGlobalPose);

                            // Depending on the instance ID of the subscriber, create events
                            // on any change, or periodic.  This is only done for testing purposes.
                            switch(subscriber.GetID().mInstance)
                            {
                            case 2:
                                gUsePeriodicEvents = true;
                                createEvent.SetEventType(CreateEventRequest::Periodic);
                                createEvent.SetRequestedPeriodicUpdateRate(gPeriodicRate);
                                createEvent.SetMinimumPeriodicUpdateRate(1.0);
                                break;
                            case 3:
                                createEvent.SetEventType(CreateEventRequest::EveryChange);
                            case 4:
                                gUsePeriodicEvents = true;
                                createEvent.SetEventType(CreateEventRequest::Periodic);
                                createEvent.SetRequestedPeriodicUpdateRate(gPeriodicRate);
                                createEvent.SetMinimumPeriodicUpdateRate(1.0);
                                break;
                            default:
                                createEvent.SetEventType(CreateEventRequest::EveryChange);
                                break;
                            };
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
            // If no subscription has been made to any service provider, try create one.
            if(!createdVelocityEvent && subscriber.HaveEventSubscriptionsOfType(JAUS_REPORT_VELOCITY_STATE) == false)
            {
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
                        // Find a global pose sensors.
                        Address::List sensors =
                            reportConfiguration->GetConfiguration()->GetComponentsOfType((Byte)Service::VelocityStateSensor);

                        if(sensors.size() > 0)
                        {
                            cout << "Create Event from Velocity State Sensor " << sensors.front().ToString() << "...";
                            // Try Create an Event for Global Pose data.
                            // Depending on the component instance of this component
                            // create a different kind of event.  This is done to
                            // show how to create (and test) the different types of events.
                            CreateEventRequest createEvent;

                            createEvent.SetSourceID(subscriber.GetID());
                            createEvent.SetDestinationID(sensors.front());
                            createEvent.SetMessageCode(JAUS_REPORT_TRAVEL_SPEED);

                            createEvent.SetEventType(CreateEventRequest::EveryChange);

                            // Request the event.
                            if(subscriber.RequestEvent(createEvent) == JAUS_OK)
                            {
                                cout << "Success!\n";
                                createdVelocityEvent = true;
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
        Sleep(200);
    }

    subscriber.Shutdown();

    return 0;
}


/*  End of File */
