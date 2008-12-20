////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalwaypointdriver.h
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
#ifndef __JAUS_SERVICE_GLOBAL_WAYPOINT_DRIVER__H
#define __JAUS_SERVICE_GLOBAL_WAYPOINT_DRIVER__H

#include "jaus/services/srvclibdll.h"
#include "jaus/components/commandcomponent.h"
#include "jaus/messages/command/platform/setglobalwaypoint.h"
#include "jaus/messages/query/platform/queryglobalwaypoint.h"
#include "jaus/messages/query/platform/querywaypointcount.h"
#include "jaus/messages/inform/platform/reportglobalwaypoint.h"
#include "jaus/messages/inform/platform/reportwaypointcount.h"
#include "jaus/messages/common/platform/globalpose.h"
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class GlobalWaypointDriver
    ///   \brief Interface for creating Global Waypoint Driver Components (ID 34).
    ///
    ///   The Global Waypoint Driver class performs closed loop control of a 
    ///   platform to reach single or multiple destination waypoints.
    ///
    ///   This component will automatically get necessary sensor data to know the
    ///   vehicles current position, however if you need additional sensor data, feel
    ///   free to acquire it also.  At  minimum Global Pose data is retrived so that
    ///   Global Vector commands can be generated to send to a Global Vector Driver
    ///   which will move the vehicle.
    ///
    ///   If you need a Global Waypoint Driver that controls something other than
    ///   a Global Vector Driver, than you'll need to create your own service.  However
    ///   use this as an example of how to create your own interface that can be
    ///   re-used.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL GlobalWaypointDriver : public CommandComponent
    {
    public:
        typedef std::map<UShort, Jaus::SetGlobalWaypoint> WaypointList;
        GlobalWaypointDriver();
        virtual ~GlobalWaypointDriver();
        // Returns a presence vector indicating what fields of the Set Global Waypoint Command are supported.
        virtual UShort GetSetGlobalWaypointPresenceVector() const = 0;
        // Returns a presence vector indicating what fields of the Set Global Vector Command are supported.
        virtual Byte GetSetGlobalVectorPresenceVector() const = 0;
        // Method takes the desired destination and generates a command for the Global Vector Driver.
        virtual int GenerateGlobalVector(const Jaus::SetGlobalWaypoint& desiredDestination, Jaus::SetGlobalVector& desiredVector) = 0;
        // Method checks to see if the waypoint destination has been acheived.
        virtual bool IsWaypointAcheived(const Jaus::SetGlobalWaypoint& desiredDestination) = 0;
        // Initialize the waypoint driver
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        // Performs shutdown of the service.
        virtual int Shutdown();
        // Sets up the messages supported by this service (Global Waypoint Driver)
        virtual int SetupService();
        // Sets how often to generate a new command to reach a desired waypoint.
        virtual int SetUpdateRate(const double rate);
        // Processes command messages.
        virtual int ProcessCommandMessage(const Message* msg,
                                          const Byte commandAuthority);
        // Processes query messages.
        virtual int ProcessQueryMessage(const Message* msg);
        // Process inform messages.
        virtual int ProcessInformMessage(const Message* msg);
        // Sets a global waypoint to reach.
        virtual int SetGlobalWaypoint(const Jaus::SetGlobalWaypoint& waypointCommand);
        // Sets the ID of the Global Vector Driver to use to get to a destination.
        void SetGlobalVectorDriver(const Address& id);
        // Sets the ID of the Global Pose Sensor to get data from (latitude, longitude, attitude data, etc.)
        void SetGlobalPoseSensor(const Address& id);
        // Gets the update rate for generating commands.
        double GetUpdateRate() const;
        // Gets the current known global pose of platform.
        GlobalPose GetGlobalPose() const;
        // Gets the list of waypoints the driver must get to.
        WaypointList GetWaypointList() const;
        // Gets the current waypoint being driven to.
        Jaus::SetGlobalWaypoint GetCurrentDesiredGlobalWaypoint() const;
    protected:
        static void GlobalWaypointDriverThread(void *args);
        bool IsGlobalPoseSubscriptionReady();
        bool HaveControlOfGlobalVectorDriver();
        CxUtils::Mutex mGlobalWaypointDriverMutex;   ///<  Mutex for thread protection of data.
        CxUtils::Thread mGlobalWaypointDriverThread; ///<  Thread object for generating Set Global Vector commands.
        Address mGlobalVectorDriverID;               ///<  ID of Global Vector Driver to command.
        Address mGlobalPoseSensorID;                 ///<  ID of Global Pose Sensor to get data from.
        ReportGlobalPose mCurrentGlobalPose;         ///<  Current Global Pose of the platform.
        WaypointList mWaypointList;                  ///<  List of waypoints to travel to.
        double mGlobalWaypointDriverUpdateRateHz;    ///<  Update rate for the global waypoint driver's generation of commands.
        double mDesiredTravelSpeed;                  ///<  The speed at which the platform should move to a waypoint
    };
}


#endif
/*  End of File */
