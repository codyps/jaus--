////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalwaypointdriver.cpp
///  \brief This file contains software for creating a JAUS Global Waypoint
///  Driver component.
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

#include "jaus/services/srvclibdll.h"
#include "jaus/components/commandcomponent.h"
#include "jaus/messages/command/platform/setglobalwaypoint.h"
#include "jaus/messages/query/platform/queryglobalwaypoint.h"
#include "jaus/messages/query/platform/querywaypointcount.h"
#include "jaus/messages/inform/platform/reportglobalwaypoint.h"
#include "jaus/messages/inform/platform/reportwaypointcount.h"
#include "jaus/messages/common/platform/globalpose.h"
#include <map>

#include "jaus/services/globalwaypointdriver.h"

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   Default update rate for generation of wrench efforts is 10 Hz.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalWaypointDriver::GlobalWaypointDriver() : mGlobalWaypointDriverUpdateRateHz(0)
{
    SetControllable(true);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalWaypointDriver::~GlobalWaypointDriver()
{
    mGlobalWaypointDriverThread.StopThread(2500);
    Shutdown();
    mWaypointList.clear();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the Global Waypoint Driver component.
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
int GlobalWaypointDriver::Initialize(const Byte subsystem,
                                     const Byte node,
                                     const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(CommandComponent::Initialize("Global Waypoint Driver",
                                            Address(subsystem,
                                            node,
                                            (Byte)(Service::GlobalWaypointDriver),
                                            i)))
            {
                mGlobalWaypointDriverThread.CreateThread(GlobalWaypointDriverThread, this);
                mGlobalWaypointDriverThread.SetThreadName("Global Waypoint Driver");
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        if(CommandComponent::Initialize("Global Waypoint Driver",
                                        Address(subsystem,
                                        node,
                                        (Byte)(Service::GlobalWaypointDriver),
                                        instance)) )
        {
            mGlobalWaypointDriverThread.CreateThread(GlobalWaypointDriverThread, this);
            mGlobalWaypointDriverThread.SetThreadName("Global Waypoint Driver");
            return JAUS_OK;
        }
    }
    Shutdown();
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalWaypointDriver::Shutdown()
{
    // Stop active threads, clear desired
    // values.
    mGlobalWaypointDriverThread.StopThread(1000);

    mGlobalWaypointDriverMutex.Enter();
    if(!mWaypointList.empty())
    {
        mWaypointList.clear();
    }
    if(mGlobalVectorDriverID.IsValid())
    {
        int count = 0;
        while(HaveComponentControl(mGlobalVectorDriverID))
        {
            if(JAUS_OK == ReleaseComponentControl(mGlobalVectorDriverID))
            {
                break;
            }
            Sleep(1);
            if(++count == 5)
            {
                break;
            }
        }
    }
    if(mGlobalPoseSensorID.IsValid())
    {
        CancelEvents(mGlobalPoseSensorID);
    }
    
    mGlobalWaypointDriverMutex.Leave();
    // Call the shutdown method of parent class.
    return CommandComponent::Shutdown();

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets up the service information supported for the Global Waypoint
///          Driver Component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////

int GlobalWaypointDriver::SetupService()
{
    UInt globalWaypointPV = GetSetGlobalWaypointPresenceVector();
    UInt globalVectorPV = GetSetGlobalVectorPresenceVector();

    Service globalWaypointDriverService;
    globalWaypointDriverService.SetType(Service::GlobalWaypointDriver);


    // Input messages related to the Global Waypoint Driver Service.
    globalWaypointDriverService.AddInputMessage(JAUS_SET_TRAVEL_SPEED, 0);
    globalWaypointDriverService.AddInputMessage(JAUS_SET_GLOBAL_WAYPOINT, globalWaypointPV);
    globalWaypointDriverService.AddInputMessage(JAUS_QUERY_TRAVEL_SPEED, 0);
    globalWaypointDriverService.AddInputMessage(JAUS_QUERY_WAYPOINT_COUNT, 0);
    globalWaypointDriverService.AddInputMessage(JAUS_QUERY_GLOBAL_WAYPOINT, globalWaypointPV);


    // Output messages related to the Global Vector Driver Service.
    globalWaypointDriverService.AddOutputMessage(JAUS_REPORT_TRAVEL_SPEED, 0);
    globalWaypointDriverService.AddOutputMessage(JAUS_REPORT_WAYPOINT_COUNT, 0);
    globalWaypointDriverService.AddOutputMessage(JAUS_REPORT_GLOBAL_WAYPOINT, globalWaypointPV);

    globalWaypointDriverService.AddOutputMessage(JAUS_SET_GLOBAL_VECTOR, globalVectorPV);
   

    AddService(globalWaypointDriverService);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the update rate for generating Set Global Vector commands for
///          Global Waypoint Driving.
///
///   \param rate Update rate in Hz. [0.016,1092].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalWaypointDriver::SetUpdateRate(const double rate)
{
    if(rate >= 0.016 && rate <= 1092)
    {
        mGlobalWaypointDriverUpdateRateHz = rate;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
// Processes command messages.
int GlobalWaypointDriver::ProcessCommandMessage(const Message* msg,
                                  const Byte commandAuthority)
{
    int result = JAUS_OK;

    if(commandAuthority >= GetComponentAuthority())
    {
        switch(msg->GetCommandCode())
        {
        case JAUS_SET_GLOBAL_WAYPOINT:
            {
                const Jaus::SetGlobalWaypoint* command = dynamic_cast<const Jaus::SetGlobalWaypoint*>(msg);
                if(command && IsInputMessageSupported(JAUS_SET_GLOBAL_WAYPOINT, command->GetPresenceVector()))
                {
                    mGlobalWaypointDriverMutex.Enter();
                    SetGlobalWaypoint(*command);
                    mGlobalWaypointDriverMutex.Leave();
                }
            };
            break;
        case JAUS_SET_TRAVEL_SPEED:
            {
                const Jaus::SetTravelSpeed* command = dynamic_cast<const Jaus::SetTravelSpeed*>(msg);
                if(command && IsInputMessageSupported(JAUS_SET_TRAVEL_SPEED, 0))
                {
                    mGlobalWaypointDriverMutex.Enter();
                    SetTravelSpeed(command->GetSpeed());
                    mGlobalWaypointDriverMutex.Leave();
                }
            };
            break;
        default:
            result = JAUS_FAILURE;
            break;
        }
    }

    if(result == JAUS_FAILURE)
    {
        result = CommandComponent::ProcessCommandMessage(msg, commandAuthority);
    }
    else
    {
        // Always run parent process command in case it needs the data too.
        CommandComponent::ProcessCommandMessage(msg, commandAuthority);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
// Processes query messages.
int GlobalWaypointDriver::ProcessQueryMessage(const Message* msg)
{
    int result = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_TRAVEL_SPEED:
        {
            const Jaus::QueryTravelSpeed* query = dynamic_cast<const Jaus::QueryTravelSpeed*>(msg);
            if(query)
            {
                Jaus::ReportTravelSpeed report;
                report.SetSpeed(mDesiredTravelSpeed);

                report.SetSourceID(GetID());
                report.SetDestinationID(query->GetSourceID());
                Send(&report);
            }
        }
        break;
    case JAUS_QUERY_WAYPOINT_COUNT:
        {
            const Jaus::QueryWaypointCount* query = dynamic_cast<const Jaus::QueryWaypointCount*>(msg);
            if(query)
            {
                Jaus::ReportWaypointCount report;
                report.SetWaypointCount(mWaypointList.size());

                report.SetSourceID(GetID());
                report.SetDestinationID(query->GetSourceID());
                Send(&report);
            }
        }
        break;
    case JAUS_QUERY_GLOBAL_WAYPOINT:
        {
            const Jaus::QueryGlobalWaypoint* query = dynamic_cast<const Jaus::QueryGlobalWaypoint*>(msg);
            if(query)
            {
                Jaus::ReportGlobalWaypoint report;
                Jaus::SetGlobalWaypoint waypoint = mWaypointList.find(query->GetWaypointNumber())->second;

                report.SetWaypointNumber(waypoint.GetWaypointNumber());
                report.SetLatitude(waypoint.GetLatitude());
                report.SetLongitude(waypoint.GetLongitude());

                if(BitVector::IsBitSet(waypoint.GetPresenceVector(), ReportGlobalWaypoint::VectorBit::Altitude))
                    report.SetAltitude(waypoint.GetAltitude());
                if(BitVector::IsBitSet(waypoint.GetPresenceVector(), ReportGlobalWaypoint::VectorBit::Roll))
                    report.SetRoll(waypoint.GetRoll());
                if(BitVector::IsBitSet(waypoint.GetPresenceVector(), ReportGlobalWaypoint::VectorBit::Pitch))
                    report.SetPitch(waypoint.GetPitch());
                if(BitVector::IsBitSet(waypoint.GetPresenceVector(), ReportGlobalWaypoint::VectorBit::Yaw))
                    report.SetYaw(waypoint.GetYaw());
                
                report.SetSourceID(GetID());
                report.SetDestinationID(query->GetSourceID());
                Send(&report);
            }
        }
        break;
    default:
        result = JAUS_FAILURE;
        break;
    }

    if(result == JAUS_FAILURE)
    {
        result = CommandComponent::ProcessQueryMessage(msg);
    }
    else
    {
        // Always run parent process command in case it needs the data too.
        CommandComponent::ProcessQueryMessage(msg);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
////////////////////////////////////////////////////////////////////////////////////
// Process inform messages.
// 
// No idea what inform messages the driver should process
int GlobalWaypointDriver::ProcessInformMessage(const Message* msg)
{
    int result = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_REPORT_GLOBAL_POSE:
        {
            const Jaus::ReportGlobalPose* report = dynamic_cast<const Jaus::ReportGlobalPose*>(msg);
            if(report)
            {
                mGlobalWaypointDriverMutex.Enter();

                mCurrentGlobalPose.SetLatitude(report->GetLatitude());
                mCurrentGlobalPose.SetLongitude(report->GetLongitude());
                
                if(report->HaveElevation())
                {
                    mCurrentGlobalPose.SetElevation(report->GetElevation());
                }
                if(report->HaveYaw())
                {
                    mCurrentGlobalPose.SetYaw(report->GetYaw());
                }
                if(report->HaveRoll())
                {
                    mCurrentGlobalPose.SetRoll(report->GetRoll());
                }
                if(report->HavePitch())
                {
                    mCurrentGlobalPose.SetPitch(report->GetPitch());
                }

                mGlobalWaypointDriverMutex.Leave();
            }
        }
        break;
    default:
        result = JAUS_FAILURE;
        break;
    }

    // Still let parent class process (in case dynamic discovery is
    // enabled and parent class needs this data too.
    if(result == JAUS_FAILURE)
    {
        result = CommandComponent::ProcessInformMessage(msg);
    }
    else
    {
        // Always run parent process command in case it needs the data too.
        CommandComponent::ProcessInformMessage(msg);
    }
    return result;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   Adds a new global waypoint to the desintation list. The waypoint will be
///   processed in the order that it is received. 
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalWaypointDriver::SetGlobalWaypoint(const Jaus::SetGlobalWaypoint& waypointCommand)
{
    mWaypointList.insert( std::pair<UInt, Jaus::SetGlobalWaypoint>(waypointCommand.GetWaypointNumber(), waypointCommand) );
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the Global Vector Driver to use to acquire
///          attitude and elevation data for vector control.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalWaypointDriver::SetGlobalVectorDriverID(const Address& id)
{
    mGlobalVectorDriverID = id;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the Global Pose Sensor to use to acquire
///          attitude and elevation data for vector control.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalWaypointDriver::SetGlobalPoseSensorID(const Address& id)
{
    mGlobalPoseSensorID = id;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Update rate in Hz used for wrench effort command generation.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalWaypointDriver::GetUpdateRate() const
{
    return mGlobalWaypointDriverUpdateRateHz;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return The current known Global pose of the platform
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose GlobalWaypointDriver::GetGlobalPose() const
{
    return mCurrentGlobalPose;
}

////////////////////////////////////////////////////////////////////////////////////
///
///    Gets the list of waypoints the driver must get to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalWaypointDriver::WaypointList GlobalWaypointDriver::GetWaypointList() const
{
    return mWaypointList;
}

////////////////////////////////////////////////////////////////////////////////////
///
///    Gets the current waypoint being driven to. (The first one on the list)
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::SetGlobalWaypoint GlobalWaypointDriver::GetCurrentDesiredGlobalWaypoint() const
{
    Jaus::SetGlobalWaypoint result;

    if(!mWaypointList.empty())
    {
        WaypointList::const_iterator map_itr;
        map_itr = mWaypointList.begin();

        result = (*map_itr).second;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///    Sets the desired speed the platform should move to a waypoint
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalWaypointDriver::SetTravelSpeed(const double speed)
{
    mDesiredTravelSpeed = speed;
}

////////////////////////////////////////////////////////////////////////////////////
///
///    Gets the desired speed the platform should move to a waypoint
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalWaypointDriver::GetTravelSpeed() const
{
    return mDesiredTravelSpeed;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This thread continuously checks the status of the Global Waypoint
///          driver based on the Update Rate set using (SetUpdateRate).  If
///          in a Ready state, this thread will take control of the vector
///          driver set and generate wrench efforts based on the current
///          desired waypoint.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalWaypointDriver::GlobalWaypointDriverThread(void *args)
{
    
    GlobalWaypointDriver* driver = (GlobalWaypointDriver*)(args);
    Jaus::SetGlobalWaypoint globalWaypoint;
    Jaus::SetGlobalVector globalVector;
    while(driver && !driver->mGlobalWaypointDriverThread.QuitThreadFlag())
    {
        // If we are in a ready state, and we have the necessary
        // sensor data required, and control of a primitive driver
        // then generate wrench efforts!
        if(driver->GetPrimaryStatus() == Component::Status::Ready &&
            driver->IsGlobalPoseSubscriptionReady() &&
            driver->HaveControlOfGlobalVectorDriver() &&
            !driver->mWaypointList.empty())
        {
            // Generate the global vector to send to the
            // global vector driver component.
            if(driver->IsWaypointAcheived(driver->GetCurrentDesiredGlobalWaypoint()))
            {
                // Remove current waypoint
                WaypointList::iterator wpl_itr = driver->mWaypointList.begin();
                driver->mWaypointList.erase(wpl_itr);
            }
            
            //if this is true, it means that we just removed the last waypoint from the list.
            if(driver->mWaypointList.empty())
            {
                globalVector.SetSpeed(0.0);
            }
            else
            {
                driver->GenerateGlobalVector(driver->GetCurrentDesiredGlobalWaypoint(), globalVector);
            }
            
            globalVector.SetDestinationID(driver->GetGlobalVectorDriverID());
            globalVector.SetSourceID(driver->GetID());
            // Send the command.
            driver->Send(&globalVector);
        }
        else if(driver->GetPrimaryStatus() != Component::Status::Ready)
        {
            // If not in a ready state, then we must release control
            // of all components being commanded and stop
            // any active subscriptions (this will reduce bandwidth).
            if(driver->mGlobalPoseSensorID.IsValid())
            {
                driver->CancelEvents(driver->mGlobalPoseSensorID);
            }
            if(driver->mGlobalVectorDriverID.IsValid())
            {
                driver->CancelEvents(driver->mGlobalVectorDriverID);
            }
            driver->mGlobalWaypointDriverMutex.Enter();
            driver->mWaypointList.clear();
            driver->mGlobalWaypointDriverMutex.Leave();
        }
        // Delay refresh based on update rate set.
        Sleep( (unsigned int)(1000.0/driver->mGlobalWaypointDriverUpdateRateHz) );
    }
    // If not in a ready state, then we must release control
    // of all components being commanded and stop
    // any active subscriptions (this will reduce bandwidth).
    if(driver->mGlobalPoseSensorID.IsValid())
    {
        driver->CancelEvents(driver->mGlobalPoseSensorID);
    }
    if(driver->mGlobalVectorDriverID.IsValid())
    {
        driver->CancelEvents(driver->mGlobalVectorDriverID);
    }

    driver->mGlobalWaypointDriverMutex.Enter();
    driver->mWaypointList.clear();
    driver->mGlobalWaypointDriverMutex.Leave();
    
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks that the component has subscriptions/data from the
///          global pose sensor.
///
///   This method will automatically create the necessary subscriptions if a
///   Global Pose Sensor ID is set based on the type of global vector commands
///   supported by this driver.
///
///   If multiple Global Pose Sensors are needed to gather the necessary
///   sensor data, then you must create those event subscriptions manually.
///
///   \return True if ready, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalWaypointDriver::IsGlobalPoseSubscriptionReady()
{
    bool ready = true;
    // Double check that we have all the necessary command/control
    // and sensor information.
    if(mGlobalPoseSensorID.IsValid())
    {
        if(mGlobalWaypointDriverThread.QuitThreadFlag() == false &&
           HaveEventSubscription(mGlobalPoseSensorID, JAUS_REPORT_GLOBAL_POSE) == false)
        {
            ready = false;
            Jaus::CreateEventRequest createEvent;
            // Try create event subscription.
            createEvent.SetSourceID(GetID());
            createEvent.SetDestinationID(mGlobalPoseSensorID);
            createEvent.SetRequestID(EventManager::GenerateRequestID());
            createEvent.SetMessageCode(JAUS_REPORT_GLOBAL_POSE);
            createEvent.SetEventType(CreateEventRequest::EveryChange);
            QueryGlobalPose queryGlobalPose;
            UShort presenceVector = 0;
            // Find out the vector values this driver supports
            // and then request the needed sensor information for generation of
            // those vectors.
            presenceVector |= QueryGlobalPose::VectorMask::Latitude;
            presenceVector |= QueryGlobalPose::VectorMask::Longitude;
            
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_WAYPOINT, Jaus::SetGlobalWaypoint::VectorMask::Altitude))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Elevation;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_WAYPOINT, Jaus::SetGlobalWaypoint::VectorMask::Roll))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Roll;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_WAYPOINT, Jaus::SetGlobalWaypoint::VectorMask::Pitch))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Pitch;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_WAYPOINT, Jaus::SetGlobalWaypoint::VectorMask::Yaw))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Yaw;
            }
            queryGlobalPose.SetPresenceVector(presenceVector);
            createEvent.SetQueryMessage(&queryGlobalPose);
            if(mGlobalWaypointDriverThread.QuitThreadFlag() == false &&
                RequestEvent(createEvent) == JAUS_OK)
            {
                ready = true;
            }
        }
    }
    else
    {
        ready = false;
    }
    return ready;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks that the vector driver ID has been set, and attempts to
///          take control of it if not already done so.
///
///   \return True if ready (have control of a vector driver), otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalWaypointDriver::HaveControlOfGlobalVectorDriver()
{
    bool ready = true;
    if(mGlobalVectorDriverID.IsValid())
    {
        if(HaveComponentControl(mGlobalVectorDriverID) == false)
        {
            ready = false;
            //  Request control of primitive driver
           if(RequestComponentControl(mGlobalVectorDriverID) == JAUS_OK)
           {
               QueryComponentStatus query;
               query.SetSourceID(GetID());
               query.SetDestinationID(mGlobalVectorDriverID);
               Receipt receipt;
               if(Send(&query, receipt) == JAUS_OK)
               {
                   const ReportComponentStatus* report = 
                       dynamic_cast<const ReportComponentStatus*>(receipt.GetResponseMessage());
                   if(report && report->GetPrimaryStatusCode() == Component::Status::Standby)
                   {
                       Jaus::Resume resume;
                       resume.SetSourceID(GetID());
                       resume.SetDestinationID(mGlobalVectorDriverID);
                       Send(&resume);
                   }
               }
               ready = true;
           }
        }
    }
    else
    {
        ready = false;
    }

    return ready;
}


/*  End of File */
