////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalvectordriver.h
///  \brief This file contains software for creating a JAUS Global Vector
///  Driver component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 April 2008
///  <br>Created: 18 May 2008
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
#include "jaus/services/globalvectordriver.h"
#include "jaus/messages/command/core/resume.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   Default update rate for generation of wrench efforts is 10 Hz.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalVectorDriver::GlobalVectorDriver() : mGlobalVectorDriverUpdateRateHz(10),
                                           mpSpeed(0),
                                           mpElevation(0),
                                           mpHeading(0),
                                           mpRoll(0),
                                           mpPitch(0),
                                           mpDesiredVector(0)

{
    SetControllable(true);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalVectorDriver::~GlobalVectorDriver()
{
    mGlobalVectorDriverThread.StopThread(2500);
    Shutdown();
    ClearCurrentVector();
    if(mpDesiredVector)
    {
        delete mpDesiredVector;
        mpDesiredVector = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the Global Vector Driver component.
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
int GlobalVectorDriver::Initialize(const Byte subsystem,
                                   const Byte node,
                                   const Byte instance)
{
    if(instance == 0)
    {
        for(Byte i = 1; i < 255; i++)
        {
            if(CommandComponent::Initialize("Global Vector Driver",
                                            Address(subsystem,
                                            node,
                                            (Byte)(Service::GlobalVectorDriver),
                                            i)))
            {
                mGlobalVectorDriverThread.CreateThread(GlobalVectorDriverThread, this);
                mGlobalVectorDriverThread.SetThreadName("Global Vector Driver");
                return JAUS_OK;
            }
        }
        return JAUS_FAILURE;
    }
    else
    {
        if(CommandComponent::Initialize("Global Vector Driver",
                                        Address(subsystem,
                                        node,
                                        (Byte)(Service::GlobalVectorDriver),
                                        instance)) )
        {
            mGlobalVectorDriverThread.CreateThread(GlobalVectorDriverThread, this);
            mGlobalVectorDriverThread.SetThreadName("Global Vector Driver");
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
int GlobalVectorDriver::Shutdown()
{
    // Stop active threads, clear desired
    // values.
    mGlobalVectorDriverThread.StopThread(1000);
    ClearCurrentVector();
    mGlobalVectorDriverMutex.Enter();
    if(mpDesiredVector)
    {
        delete mpDesiredVector;
        mpDesiredVector = NULL;
    }
    if(mPrimitiveDriverID.IsValid())
    {
        int count = 0;
        while(HaveComponentControl(mPrimitiveDriverID))
        {
            if(JAUS_OK == ReleaseComponentControl(mPrimitiveDriverID))
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
    if(mVelocityStateSensorID.IsValid())
    {
        CancelEvents(mVelocityStateSensorID);
    }
    mGlobalVectorDriverMutex.Leave();
    // Call the shutdown method of parent class.
    return CommandComponent::Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets up the service information supported for the Global Vector
///          Driver Component.
///
///   This method uses the information provided by the
///   GetSetGlobalVectorPresenceVector method and the
///   GetSetWrenchEffortPresenceVector method to determine what type of
///   wrench effort messages will get generated by this component, what type of
///   vector commands can be recieved, and what type of global pose and travel
///   speed data is required as input.  You do not need to overload this method
///   unless you wish to add additional supported messages.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::SetupService()
{
    UInt wrenchEffortPV = GetSetWrenchEffortPresenceVector();
    UInt globalVectorPV = GetSetGlobalVectorPresenceVector();
    UInt globalPosePV = 0;

    Service globalVectorDriverService;
    globalVectorDriverService.SetType(Service::GlobalVectorDriver);

    if(BitVector::IsBitSet(globalVectorPV, Jaus::SetGlobalVector::VectorBit::Heading))
    {
        globalPosePV |= ReportGlobalPose::VectorMask::Yaw;
    }
    if(BitVector::IsBitSet(globalVectorPV, Jaus::SetGlobalVector::VectorBit::Roll))
    {
        globalPosePV |= ReportGlobalPose::VectorMask::Roll;
    }
    if(BitVector::IsBitSet(globalVectorPV, Jaus::SetGlobalVector::VectorBit::Pitch))
    {
        globalPosePV |= ReportGlobalPose::VectorMask::Pitch;
    }
    if(BitVector::IsBitSet(globalVectorPV, Jaus::SetGlobalVector::VectorBit::Elevation))
    {
        globalPosePV |= ReportGlobalPose::VectorMask::Elevation;
    }

    // Input messages related to the Global Vector Driver Service.
    globalVectorDriverService.AddInputMessage(JAUS_SET_GLOBAL_VECTOR, globalVectorPV);
    globalVectorDriverService.AddInputMessage(JAUS_QUERY_GLOBAL_VECTOR, globalVectorPV);
    globalVectorDriverService.AddInputMessage(JAUS_REPORT_GLOBAL_POSE, globalPosePV);
    globalVectorDriverService.AddInputMessage(JAUS_REPORT_TRAVEL_SPEED, 0);
    globalVectorDriverService.AddInputMessage(JAUS_REPORT_WRENCH_EFFORT, wrenchEffortPV);

    // Output messages related to the Global Vector Driver Service.
    globalVectorDriverService.AddOutputMessage(JAUS_QUERY_GLOBAL_POSE, globalPosePV);
    globalVectorDriverService.AddOutputMessage(JAUS_QUERY_TRAVEL_SPEED, 0);
    globalVectorDriverService.AddOutputMessage(JAUS_SET_WRENCH_EFFORT, wrenchEffortPV);
    globalVectorDriverService.AddOutputMessage(JAUS_QUERY_WRENCH_EFFORT, wrenchEffortPV);

    AddService(globalVectorDriverService);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the update rate for generating Set Wrench Effort commands for
///          Global Vector Driving.
///
///   \param rate Update rate in Hz. [0.016,1092].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int  GlobalVectorDriver::SetUpdateRate(const double rate)
{
    if(rate >= 0.016 && rate <= 1092)
    {
        mGlobalVectorDriverUpdateRateHz = rate;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Overloaded method to process command messages.
///
///   This method will process Set Global Vector, Set Travel Speed,
///   commands, or pass the message to the parent class for processing.
///
///   \param msg The command message to process.
///   \param commandAuthority The authority level of the component sending
///          the command.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::ProcessCommandMessage(const Message* msg, const Byte commandAuthority)
{
    int result = JAUS_OK;

    if(commandAuthority >= GetComponentAuthority())
    {
        switch(msg->GetCommandCode())
        {
        case JAUS_SET_GLOBAL_VECTOR:
            {
                const Jaus::SetGlobalVector* command =
                    dynamic_cast<const Jaus::SetGlobalVector*>(msg);
                if(command && IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, command->GetPresenceVector()))
                {
                    mGlobalVectorDriverMutex.Enter();
                    if(mpDesiredVector)
                    {
                        *mpDesiredVector = *command;
                    }
                    else
                    {
                        mpDesiredVector = (Jaus::SetGlobalVector *)command->Clone();
                    }
                    mGlobalVectorDriverMutex.Leave();
                }
            };
            break;
        case JAUS_SET_TRAVEL_SPEED:
            {
                const Jaus::SetTravelSpeed* command =
                    dynamic_cast<const Jaus::SetTravelSpeed*>(msg);
                if(command && IsInputMessageSupported(JAUS_SET_TRAVEL_SPEED, 0))
                {
                    mGlobalVectorDriverMutex.Enter();
                    if(mpDesiredVector)
                    {
                        mpDesiredVector->SetSpeed(command->GetSpeed());
                    }
                    else
                    {
                        mpDesiredVector = new Jaus::SetGlobalVector();
                        mpDesiredVector->SetSpeed(command->GetSpeed());
                    }
                    mGlobalVectorDriverMutex.Leave();
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
///   \brief Overloaded method to process query messages.
///
///   This method will process Query Global Vector commands, or pass the message
///   to the parent class for processing.
///
///   \param msg The message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::ProcessQueryMessage(const Message* msg)
{
    int result = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_QUERY_GLOBAL_VECTOR:
        {
            const Jaus::QueryGlobalVector* query =
                dynamic_cast<const Jaus::QueryGlobalVector*>(msg);
            if(query)
            {
                Jaus::ReportGlobalVector report;
                if(HaveDesiredGlobalVector())
                {
                    mGlobalVectorDriverMutex.Enter();
                    if(mpSpeed && BitVector::IsBitSet(query->GetPresenceVector(), QueryGlobalVector::VectorBit::Speed)) { report.SetSpeed(*mpSpeed); }
                    if(mpElevation && BitVector::IsBitSet(query->GetPresenceVector(), QueryGlobalVector::VectorBit::Elevation)) { report.SetElevation(*mpElevation); }
                    if(mpHeading && BitVector::IsBitSet(query->GetPresenceVector(), QueryGlobalVector::VectorBit::Heading)) { report.SetHeading(*mpHeading); }
                    if(mpRoll && BitVector::IsBitSet(query->GetPresenceVector(), QueryGlobalVector::VectorBit::Roll)) { report.SetRoll(*mpRoll); }
                    if(mpPitch && BitVector::IsBitSet(query->GetPresenceVector(), QueryGlobalVector::VectorBit::Pitch)) { report.SetPitch(*mpPitch); }
                    mGlobalVectorDriverMutex.Leave();
                }
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
///   \brief Overloaded method to process inform messages.
///
///   This method will process Report Global Pose,
///   Report Travel Speed, or pass the message
///   to the parent class for processing.
///
///   \param msg The message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::ProcessInformMessage(const Message* msg)
{
    int result = JAUS_OK;
    switch(msg->GetCommandCode())
    {
    case JAUS_REPORT_GLOBAL_POSE:
        {
            const Jaus::ReportGlobalPose* report =
                dynamic_cast<const Jaus::ReportGlobalPose*>(msg);
            if(report)
            {
                mGlobalVectorDriverMutex.Enter();
                if(report->HaveElevation())
                {
                    if(mpElevation == NULL)
                    {
                        mpElevation = new double;
                    }
                    *mpElevation = report->GetElevation();
                }
                if(report->HaveYaw())
                {
                    if(mpHeading == NULL)
                    {
                        mpHeading = new double;
                    }
                    *mpHeading = report->GetYaw();
                    //cout << "Heading: " << JAUS_RAD2DEG(*mpHeading) << endl;
                }
                if(report->HaveRoll())
                {
                    if(mpRoll == NULL)
                    {
                        mpRoll = new double;
                    }
                    *mpRoll = report->GetRoll();
                }
                if(report->HavePitch())
                {
                    if(mpPitch == NULL)
                    {
                        mpPitch = new double;
                    }
                    *mpPitch = report->GetPitch();
                }
                mGlobalVectorDriverMutex.Leave();
            }
        }
        break;
    case JAUS_REPORT_VELOCITY_STATE:
        {
            const Jaus::ReportVelocityState* report = 
                dynamic_cast<const Jaus::ReportVelocityState*>(msg);
            if(report)
            {
                mGlobalVectorDriverMutex.Enter();

                if(mpSpeed == NULL)
                {
                    mpSpeed = new double;
                }
                *mpSpeed = report->GetTravelSpeed();

                mGlobalVectorDriverMutex.Leave();
            }
        }
        break;
    case JAUS_REPORT_TRAVEL_SPEED:
        {
            const Jaus::ReportTravelSpeed* report =
                dynamic_cast<const Jaus::ReportTravelSpeed*>(msg);
            if(report)
            {
                mGlobalVectorDriverMutex.Enter();

                if(mpSpeed == NULL)
                {
                    mpSpeed = new double;
                }
                *mpSpeed = report->GetSpeed();

                mGlobalVectorDriverMutex.Leave();
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
///   \brief Method to set the desired global vector to follow.
///
///   \param command Global Vector Driver command
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::SetGlobalVector(const Jaus::SetGlobalVector* command)
{
    if(!IsControlled() ||
        (IsControlled() && command->GetSourceID() == GetControllerID()))
    {
        mGlobalVectorDriverMutex.Enter();
        if(mpDesiredVector)
        {
            *mpDesiredVector = *command;
        }
        else
        {
            mpDesiredVector = (Jaus::SetGlobalVector *)command->Clone();
        }
        mGlobalVectorDriverMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the Global Pose Sensor to use to acquire
///          attitude and elevation data for vector control.
///
///   If this method is used, then the Global Vector Driver class will automatically
///   create/maintain subscriptions to pose data from this sensor.  If not set,
///   then you must create subscriptions or provide the current vector of the
///   platform in another way.
///
///   If you create your own subscriptions to sensor data, the Process Inform
///   message of this class will use the inform messages as they arrive.
///
///   \param id The ID of the sensor to get pose data from.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::SetGlobalPoseSensorID(const Address& id)
{
    if(id.IsValid())
    {
        mGlobalVectorDriverMutex.Enter();
        mGlobalPoseSensorID = id;
        mGlobalVectorDriverMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the Velocity State Sensor to use to acquire
///          speed data for vector control.
///
///   If this method is used, then the Global Vector Driver class will automatically
///   create/maintain subscriptions to data from this sensor.  If not set,
///   then you must create subscriptions or provide the current vector of the
///   platform in another way.
///
///   If you create your own subscriptions to sensor data, the Process Inform
///   message of this class will use the inform messages as they arrive.
///
///   \param id The ID of the sensor to get pose data from.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::SetVelocityStateSensorID(const Address& id)
{
    if(id.IsValid())
    {
        mGlobalVectorDriverMutex.Enter();
        mVelocityStateSensorID = id;
        mGlobalVectorDriverMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the Primitive Driver component to use for vector
///   driving.
///
///   \param id The ID of the primitive driver to control.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalVectorDriver::SetPrimitiveDriverID(const Address& id)
{
    if(id.IsValid())
    {
        mGlobalVectorDriverMutex.Enter();
        mPrimitiveDriverID = id;
        mGlobalVectorDriverMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Update rate in Hz used for wrench effort command generation.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalVectorDriver::GetUpdateRate() const
{
    return mGlobalVectorDriverUpdateRateHz;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Current known speed of platform.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalVectorDriver::GetSpeed() const
{
    double value = 0;

    mGlobalVectorDriverMutex.Enter();
    if(mpSpeed)
    {
        value = *mpSpeed;
    }
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Current known elevation of platform.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalVectorDriver::GetElevation() const
{
    double value = 0;

    mGlobalVectorDriverMutex.Enter();
    if(mpElevation)
    {
        value = *mpElevation;
    }
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if a desired global vector is set, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalVectorDriver::HaveDesiredGlobalVector() const
{
    bool value = false;

    mGlobalVectorDriverMutex.Enter();
    if(mpDesiredVector)
    {
        value = true;
    }
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the current known attitude information received.
///
///   This data will not be valid if no pose information has been received.
///
///   If any component of the attitude (roll, pitch, or yaw) is unknown or not
///   received by a global pose sensor, etc. the value will be 0.
///
///   \return Current known platform attitude data.
///
////////////////////////////////////////////////////////////////////////////////////
Attitude GlobalVectorDriver::GetAttitude() const
{
    Attitude value;

    mGlobalVectorDriverMutex.Enter();
    if(mpHeading)
    {
        value.SetYaw(*mpHeading);
    }
    if(mpRoll)
    {
        value.SetRoll(*mpRoll);
    }
    if(mpPitch)
    {
        value.SetPitch(*mpPitch);
    }
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the current desired global vector command sent and being used
///          by the component.
///
///   This command will be blank if none set/received/or component is not in a
///   ready state.
///
///   \return Desired global vector to follow.
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::SetGlobalVector GlobalVectorDriver::GetDesiredGlobalVector() const
{
    Jaus::SetGlobalVector value;

    mGlobalVectorDriverMutex.Enter();
    if(mpDesiredVector)
    {
        value = *mpDesiredVector;
    }
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the ID of the global pose sensor ID set to get pose data
///          for maintaining vectors.
///
///   \return ID of component set.  If not set, ID will be
///           invalid.
///
////////////////////////////////////////////////////////////////////////////////////
Address GlobalVectorDriver::GetGlobalPoseSensorID() const
{
    Address value;

    mGlobalVectorDriverMutex.Enter();
    value = mGlobalPoseSensorID;
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the ID of the velocity state sensor set to be used for
///          getting speed values.
///
///   \return ID of velocity state sensor component set.  If not set, ID will be
///           invalid.
///
////////////////////////////////////////////////////////////////////////////////////
Address GlobalVectorDriver::GetVelocityStateSensorID() const
{
    Address value;

    mGlobalVectorDriverMutex.Enter();
    value = mVelocityStateSensorID;
    mGlobalVectorDriverMutex.Leave();

    return value;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the ID of the primitive driver set to be used for
///          driving vectors.
///
///   \return ID of primitive driver component set.  If not set, ID will be
///           invalid.
///
////////////////////////////////////////////////////////////////////////////////////
Address GlobalVectorDriver::GetPrimitiveDriverID() const
{
    Address value;

    mGlobalVectorDriverMutex.Enter();
    value = mPrimitiveDriverID;
    mGlobalVectorDriverMutex.Leave();

    return value;
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
bool GlobalVectorDriver::IsGlobalPoseSubscriptionReady()
{
    bool ready = true;
    // Double check that we have all the necessary command/control
    // and sensor information.
    if(mGlobalPoseSensorID.IsValid())
    {
        if(mGlobalVectorDriverThread.QuitThreadFlag() == false &&
            HaveEventSubscription(mGlobalPoseSensorID,
                                 JAUS_REPORT_GLOBAL_POSE) == false)
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
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Heading))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Yaw;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Roll))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Roll;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Pitch))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Pitch;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Elevation))
            {
                presenceVector |= QueryGlobalPose::VectorMask::Elevation;
            }
            queryGlobalPose.SetPresenceVector(presenceVector);
            createEvent.SetQueryMessage(&queryGlobalPose);
            if(mGlobalVectorDriverThread.QuitThreadFlag() == false &&
                RequestEvent(createEvent) == JAUS_OK)
            {
                ready = true;
            }
        }
    }
    else
    {
        ready = false;
        // Check and see if someone manually created event subscriptions
        // to this data (didn't use automatic creation).
        if(HaveEventSubscriptionsOfType(JAUS_REPORT_GLOBAL_POSE))
        {
            ready = true;
            // Find out the vector values this driver supports
            // and then request the needed sensor information for generation of
            // those vectors.
            mGlobalVectorDriverMutex.Enter();
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Heading) &&
                !mpHeading)
            {
                ready = false;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Roll) &&
                !mpRoll)
            {
                ready = false;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Pitch) &&
                !mpPitch)
            {
                ready = false;
            }
            if(IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Elevation) &&
                !mpElevation)
            {
                ready = false;
            }
            mGlobalVectorDriverMutex.Leave();
        }
    }
    return ready;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks that the component has subscriptions/data from the
///          velocity state sensor.
///
///   This method will automatically create the necessary subscriptions if a
///   Velocity State Sensor ID is set based on the type of global vector commands
///   supported by this driver.
///
///   \return True if ready, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalVectorDriver::IsTravelSpeedSubscriptionReady()
{
    bool ready = true;
    // Make sure we have all the necessary speed information.
    if(mVelocityStateSensorID.IsValid())
    {
        // Only get travel speed/velocity state information if needed.
        if(mGlobalVectorDriverThread.QuitThreadFlag() == false &&
            IsInputMessageSupported(JAUS_SET_GLOBAL_VECTOR, Jaus::SetGlobalVector::VectorMask::Speed) &&
            (HaveEventSubscription(mVelocityStateSensorID,
                                  JAUS_REPORT_TRAVEL_SPEED) == false &&
             HaveEventSubscription(mVelocityStateSensorID, 
                                   JAUS_REPORT_VELOCITY_STATE) == false))
        {
            ready = false;
            // Try to get velocity state information, and only if that isn't supported
            // request travel speed.  Travel speed is less precise than velocity state
            // data.
            Jaus::QueryVelocityState queryVelocityState;
            Jaus::Receipt receipt;
            UShort velocityStatePresenceVector = 0;
            queryVelocityState.SetSourceID(GetID());
            queryVelocityState.SetDestinationID(mVelocityStateSensorID);
            queryVelocityState.SetPresenceVector((UShort)queryVelocityState.GetPresenceVectorMask());
            
            
            if(Send(&queryVelocityState, receipt, 0, 250, 1))
            {
                // See what fields are available.
                const Jaus::ReportVelocityState* report = 
                    dynamic_cast<const Jaus::ReportVelocityState*>(receipt.GetResponseMessage());
                velocityStatePresenceVector = Jaus::ReportVelocityState::VectorMask::VelocityX |
                                              Jaus::ReportVelocityState::VectorMask::VelocityY |
                                              Jaus::ReportVelocityState::VectorMask::VelocityZ;
                if( (velocityStatePresenceVector & report->GetPresenceVector()) == 0 )
                {
                    velocityStatePresenceVector = 0;
                }
            }
            Jaus::CreateEventRequest createEvent;
            // Try create event subscription.
            createEvent.SetSourceID(GetID());
            createEvent.SetDestinationID(mVelocityStateSensorID);
            createEvent.SetRequestID(EventManager::GenerateRequestID());
            if(velocityStatePresenceVector > 0)
            {
                createEvent.SetMessageCode(JAUS_REPORT_VELOCITY_STATE);
                queryVelocityState.SetPresenceVector(velocityStatePresenceVector);
            }
            else
            {
                createEvent.SetMessageCode(JAUS_REPORT_TRAVEL_SPEED);
            }
            createEvent.SetEventType(CreateEventRequest::EveryChange);

            if(mGlobalVectorDriverThread.QuitThreadFlag() == false &&
                RequestEvent(createEvent) == JAUS_OK)
            {
                ready = true;
            }
        }
    }
    else
    {
        ready = false;
        // Check and see if someone manually created event subscriptions
        // to this data (didn't use automatic creation).
        if( (HaveEventSubscriptionsOfType(JAUS_REPORT_TRAVEL_SPEED) || 
             HaveEventSubscriptionsOfType(JAUS_REPORT_VELOCITY_STATE) ) && mpSpeed)
        {
            ready = true;
        }

    }
    return ready;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes the current known vector data.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalVectorDriver::ClearCurrentVector()
{
    mGlobalVectorDriverMutex.Enter();
    if(mpSpeed)
    {
        delete mpSpeed;
        mpSpeed = NULL;
    }
    if(mpElevation)
    {
        delete mpElevation;
        mpElevation = NULL;
    }
    if(mpHeading)
    {
        delete mpHeading;
        mpHeading = NULL;
    }
    if(mpRoll)
    {
        delete mpRoll;
        mpRoll = NULL;
    }
    if(mpPitch)
    {
        delete mpPitch;
        mpPitch = NULL;
    }
    mGlobalVectorDriverMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks that the primitive driver ID has been set, and attempts to
///          take control of it if not already done so.
///
///   \return True if ready (have control of a primitive driver), otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalVectorDriver::HaveControlOfPrimitiveDriver()
{
    bool ready = true;
    if(mPrimitiveDriverID.IsValid())
    {
        if(HaveComponentControl(mPrimitiveDriverID) == false)
        {
            ready = false;
            //  Request control of primitive driver
           if(RequestComponentControl(mPrimitiveDriverID) == JAUS_OK)
           {
               QueryComponentStatus query;
               query.SetSourceID(GetID());
               query.SetDestinationID(mPrimitiveDriverID);
               Receipt receipt;
               if(Send(&query, receipt) == JAUS_OK)
               {
                   const ReportComponentStatus* report = 
                       dynamic_cast<const ReportComponentStatus*>(receipt.GetResponseMessage());
                   if(report && report->GetPrimaryStatusCode() == Component::Status::Standby)
                   {
                       Jaus::Resume resume;
                       resume.SetSourceID(GetID());
                       resume.SetDestinationID(mPrimitiveDriverID);
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


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This thread continuously checks the status of the Global Vector
///          driver based on the Update Rate set using (SetUpdateRate).  If
///          in a Ready state, this thread will take control of the primitive
///          driver set and generate wrench efforts based on the current
///          desired vector.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalVectorDriver::GlobalVectorDriverThread(void *args)
{
    GlobalVectorDriver* driver = (GlobalVectorDriver*)(args);
    SetWrenchEffort wrenchEffort;
    while(driver && !driver->mGlobalVectorDriverThread.QuitThreadFlag())
    {
        // If we are in a ready state, and we have the necessary
        // sensor data required, and control of a primitive driver
        // then generate wrench efforts!
        if(driver->GetPrimaryStatus() == Component::Status::Ready &&
            driver->IsGlobalPoseSubscriptionReady() &&
            driver->IsTravelSpeedSubscriptionReady() &&
            driver->HaveControlOfPrimitiveDriver() &&
            driver->HaveDesiredGlobalVector())
        {
            // Generate the wrench effort to send to the
            // primitive driver component.
            driver->GenerateWrench(driver->GetDesiredGlobalVector(), wrenchEffort);
            wrenchEffort.SetDestinationID(driver->GetPrimitiveDriverID());
            wrenchEffort.SetSourceID(driver->GetID());
            // Send the command.
            driver->Send(&wrenchEffort);
        }
        else if(driver->GetPrimaryStatus() != Component::Status::Ready)
        {
            // If not in a ready state, then we must release control
            // of all components being commanded and stop
            // any active subscriptions (this will reduce bandwidth).

            /*  Keep events for now, may add this code back in later.
            if(driver->mGlobalPoseSensorID.IsValid())
            {
                driver->CancelEvents(driver->mGlobalPoseSensorID);
            }
            if(driver->mVelocityStateSensorID.IsValid())
            {
                driver->CancelEvents(driver->mVelocityStateSensorID);
            }
            */

            if(driver->mPrimitiveDriverID.IsValid() &&
                driver->HaveComponentControl(driver->mPrimitiveDriverID))
            {
                driver->ReleaseComponentControl(driver->mPrimitiveDriverID);
                driver->SendStandbyCommand(driver->mPrimitiveDriverID);
            }
            driver->ClearCurrentVector();
            driver->mGlobalVectorDriverMutex.Enter();
            if(driver->mpDesiredVector)
            {
                delete driver->mpDesiredVector;
                driver->mpDesiredVector = NULL;
            }
            driver->mGlobalVectorDriverMutex.Leave();
        }
        // Delay refresh based on update rate set.
        Sleep( (unsigned int)(1000.0/driver->mGlobalVectorDriverUpdateRateHz) );
    }
    // If not in a ready state, then we must release control
    // of all components being commanded and stop
    // any active subscriptions (this will reduce bandwidth).
    if(driver->mGlobalPoseSensorID.IsValid())
    {
        driver->CancelEvents(driver->mGlobalPoseSensorID);
    }
    if(driver->mVelocityStateSensorID.IsValid())
    {
        driver->CancelEvents(driver->mVelocityStateSensorID);
    }
    if(driver->mPrimitiveDriverID.IsValid() &&
        driver->HaveComponentControl(driver->mPrimitiveDriverID))
    {
        driver->SendStandbyCommand(driver->mPrimitiveDriverID);
        driver->ReleaseComponentControl(driver->mPrimitiveDriverID);
    }
    driver->ClearCurrentVector();
    driver->mGlobalVectorDriverMutex.Enter();
    if(driver->mpDesiredVector)
    {
        delete driver->mpDesiredVector;
        driver->mpDesiredVector = NULL;
    }
    driver->mGlobalVectorDriverMutex.Leave();
}

/*  End of File */
