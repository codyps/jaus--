////////////////////////////////////////////////////////////////////////////////////
///
///  \file systemcommander.cpp
///  \brief This file contains a basic system commander interface.  It will
///  discover basic information about JAUS subsystem it can talk to including
///  Global Pose.  It also incorporates a joystick interface for telemanipulation
///  of platforms.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 18 December 2008
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
#include "jaus/services/systemcommander.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SystemCommander::SystemCommander() : mDesiredPeriodicUpdateRate(30),
                                     mMinimumPeriodicUpdateRate(0.5)
{
    EnableSubsystemDiscovery(true);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SystemCommander::~SystemCommander()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the System Commander component.
///
///   This method overrides the parent class Initialize function, filling in
///   the component name and component type number.
///
///   \param subsystem Subsystem ID number.
///   \param node Node ID number.
///   \param instance Instance ID to use.  Set to 0 to find first available
///             instance.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SystemCommander::Initialize(const Byte subsystem,
                                const Byte node,
                                const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(CommandComponent::Initialize("System Commander",
                                            Address(subsystem,
                                            node,
                                            (Byte)(Service::SystemCommander),
                                            i)))
            {
                EnableSubsystemDiscovery(true, NULL);
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        if(CommandComponent::Initialize("System Commander",
                                        Address(subsystem,
                                        node,
                                        (Byte)(Service::SystemCommander),
                                        instance)) )
        {
            EnableSubsystemDiscovery(true, NULL);
            return JAUS_OK;
        }
    }
    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets up the service information provided by the System Commander.
///
////////////////////////////////////////////////////////////////////////////////////
int SystemCommander::SetupService()
{
    Service service;

    service.SetType(Service::SystemCommander);
    
    service.AddInputMessage(JAUS_REPORT_WAYPOINT_COUNT, 0);
    service.AddInputMessage(JAUS_REPORT_GLOBAL_WAYPOINT, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_GLOBAL_WAYPOINT));
    service.AddInputMessage(JAUS_REPORT_PLATFORM_SPECIFICATIONS, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_PLATFORM_SPECIFICATIONS));
    service.AddInputMessage(JAUS_REPORT_TRAVEL_SPEED, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_TRAVEL_SPEED));
    service.AddInputMessage(JAUS_REPORT_VELOCITY_STATE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_VELOCITY_STATE));
    service.AddInputMessage(JAUS_REPORT_GLOBAL_POSE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_GLOBAL_POSE));
 
    service.AddOutputMessage(JAUS_QUERY_WAYPOINT_COUNT, 0);
    service.AddOutputMessage(JAUS_QUERY_GLOBAL_WAYPOINT, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_GLOBAL_WAYPOINT));
    service.AddOutputMessage(JAUS_QUERY_PLATFORM_SPECIFICATIONS, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_PLATFORM_SPECIFICATIONS));
    service.AddOutputMessage(JAUS_QUERY_TRAVEL_SPEED, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_TRAVEL_SPEED));
    service.AddOutputMessage(JAUS_QUERY_VELOCITY_STATE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_VELOCITY_STATE));
    service.AddOutputMessage(JAUS_QUERY_GLOBAL_POSE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_GLOBAL_POSE));
    
    service.AddInputMessage(JAUS_REPORT_CAMERA_FORMAT_OPTIONS, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_FORMAT_OPTIONS));
    service.AddInputMessage(JAUS_REPORT_CAMERA_CAPABILITIES, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_CAPABILITIES));
    service.AddInputMessage(JAUS_REPORT_CAMERA_COUNT, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_COUNT));
    service.AddInputMessage(JAUS_REPORT_CAMERA_POSE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_CAMERA_POSE));
    service.AddInputMessage(JAUS_REPORT_SELECTED_CAMERA, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_SELECTED_CAMERA));
    service.AddInputMessage(JAUS_REPORT_IMAGE, MessageCreator::GetPresenceVectorMask(JAUS_REPORT_IMAGE));
    
    service.AddOutputMessage(JAUS_QUERY_CAMERA_FORMAT_OPTIONS, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_FORMAT_OPTIONS));
    service.AddOutputMessage(JAUS_QUERY_CAMERA_CAPABILITIES, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_CAPABILITIES));
    service.AddOutputMessage(JAUS_QUERY_CAMERA_COUNT, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_COUNT));
    service.AddOutputMessage(JAUS_QUERY_CAMERA_POSE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_CAMERA_POSE));
    service.AddOutputMessage(JAUS_QUERY_SELECTED_CAMERA, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_SELECTED_CAMERA));
    service.AddOutputMessage(JAUS_QUERY_IMAGE, MessageCreator::GetPresenceVectorMask(JAUS_QUERY_IMAGE));

    service.AddOutputMessage(JAUS_SELECT_CAMERA, MessageCreator::GetPresenceVectorMask(JAUS_SELECT_CAMERA));
    service.AddOutputMessage(JAUS_SET_CAMERA_FORMAT_OPTIONS, MessageCreator::GetPresenceVectorMask(JAUS_SET_CAMERA_FORMAT_OPTIONS));
    service.AddOutputMessage(JAUS_SET_CAMERA_POSE, MessageCreator::GetPresenceVectorMask(JAUS_SET_CAMERA_POSE));

    service.AddOutputMessage(JAUS_SET_GLOBAL_WAYPOINT, MessageCreator::GetPresenceVectorMask(JAUS_SET_GLOBAL_WAYPOINT));
    service.AddOutputMessage(JAUS_SET_WRENCH_EFFORT, MessageCreator::GetPresenceVectorMask(JAUS_SET_WRENCH_EFFORT));
    service.AddOutputMessage(JAUS_SET_GLOBAL_PATH_SEGMENT, MessageCreator::GetPresenceVectorMask(JAUS_SET_GLOBAL_PATH_SEGMENT));
    service.AddOutputMessage(JAUS_SET_GLOBAL_VECTOR, MessageCreator::GetPresenceVectorMask(JAUS_SET_GLOBAL_VECTOR));
    service.AddOutputMessage(JAUS_SET_TRAVEL_SPEED, MessageCreator::GetPresenceVectorMask(JAUS_SET_TRAVEL_SPEED));   

    AddService(service);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief When discovery is enabled, anytime a subsystem/platform updates this
///          method is called.
///
///   \param[in] subsystem Subsystem information.
///   \param[in] eventType The type of event.
///
///   \return OK if processed, otherwise FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SystemCommander::ProcessDiscoveryEvent(const Platform& subsystem,
                                           const SubscriberComponent::DiscoveryEvents eventType)
{
    if(eventType != SubscriberComponent::SubsystemDisconnect)
    {
        // Create global pose subscriptions.
        if(subsystem.HaveIdentification() && 
           subsystem.GetIdentification()->GetIdentification().empty() == false)
        {
            UShort currentPresenceVector = 0, desiredPresenceVector = 0;
            desiredPresenceVector = QueryGlobalPose::VectorMask::Latitude |
                                    QueryGlobalPose::VectorMask::Longitude |
                                    QueryGlobalPose::VectorMask::Elevation |
                                    QueryGlobalPose::VectorMask::Roll |
                                    QueryGlobalPose::VectorMask::Pitch |
                                    QueryGlobalPose::VectorMask::Yaw;

            // If we have no global pose data, or we don't have all the data we want.
            if(subsystem.HaveGlobalPose() == false || 
                (subsystem.GetGlobalPose()->GetPresenceVector()& desiredPresenceVector) != desiredPresenceVector)
            {
                // Mark what data we already have.
                if(subsystem.HaveGlobalPose())
                {
                    currentPresenceVector = subsystem.GetGlobalPose()->GetPresenceVector();
                }

                Jaus::Address::List globalPoseSensors = subsystem.GetConfiguration()->GetComponentsOfType(Jaus::Service::GlobalPoseSensor);

                if(globalPoseSensors.size() > 0)
                {
                    // See what they support.
                    Jaus::QueryGlobalPose queryGlobalPose;
                    Jaus::Receipt receipt;                    

                    queryGlobalPose.SetSourceID(GetID());
                    queryGlobalPose.SetPresenceVector(desiredPresenceVector);
                    bool foundAllData = false;
                    // Since it is possible that there are multiple sensors on a vehicle each with a piece
                    // of the data we want, we may have to create multiple subscriptions, so go through all
                    // available global pose sensors and create the necessary events we want.
                    for(unsigned int i = 0; i < (unsigned int)globalPoseSensors.size() && !foundAllData; i++)
                    {
                        if(!HaveEventSubscription(globalPoseSensors[i], JAUS_REPORT_GLOBAL_POSE))
                        {
                            queryGlobalPose.SetDestinationID(globalPoseSensors[i]);
                            if(Send(&queryGlobalPose, receipt, 0, 250, 1) == OK)
                            {
                                const ReportGlobalPose* reportGlobalPose = dynamic_cast<const ReportGlobalPose*>(receipt.GetResponseMessage());
                                if( (reportGlobalPose->GetPresenceVector() & desiredPresenceVector) > 0)
                                {
                                    // Create an event to this sensor.
                                    CreateEventRequest eventRequest;

                                    eventRequest.SetSourceID(GetID());
                                    eventRequest.SetDestinationID(globalPoseSensors[i]);
                                    eventRequest.SetMessageCode(JAUS_REPORT_GLOBAL_POSE);
                                    eventRequest.SetEventType(CreateEventRequest::Periodic);
                                    eventRequest.SetRequestedPeriodicUpdateRate(mDesiredPeriodicUpdateRate);
                                    eventRequest.SetMinimumPeriodicUpdateRate(mMinimumPeriodicUpdateRate);
                                    queryGlobalPose.SetPresenceVector( desiredPresenceVector & (~currentPresenceVector ) );
                                    eventRequest.SetQueryMessage(&queryGlobalPose);

                                    if(RequestEvent(eventRequest) == OK)
                                    {
                                        currentPresenceVector |= (desiredPresenceVector & (!currentPresenceVector));
                                        if( (currentPresenceVector & desiredPresenceVector) == desiredPresenceVector )
                                        {
                                            foundAllData = true;
                                        }
                                        break;
                                    }
                                } // If the sensor can provide the data I need
                            } // If the sensor responded
                        } // If we don't already have a subscription.
                    }// For each global pose sensor on system
                } // If the platform has Global Pose Sensors
            } // If we don't have any global pose data for the platform.
        } // If we have identification information for the vehicle.
    } // If this is not a disconnect event.

    // Pass up the chain.
    return JoystickDriver::ProcessDiscoveryEvent(subsystem, eventType);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method sets what rates to request for updates from platforms
///   within the system.
///
///   \param[in] desired Desired update rate (Hz) from global pose sensors.
///   \param[in] minimum Minimum accepted rate (Hz) from sensors.
///
////////////////////////////////////////////////////////////////////////////////////
void SystemCommander::SetSubscriptionPeriodicUpdateRate(const double desired, const double minimum)
{
    if(desired >= 0 && minimum >= 0 && minimum <= desired)
    {
        mDesiredPeriodicUpdateRate = desired;
        mMinimumPeriodicUpdateRate = minimum;
    }
}

/*  End of File */
