////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalvectordriver.h
///  \brief This file contains software for creating a JAUS Global Vector
///  Driver component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 April 2008
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
#ifndef __JAUS_GLOBAL_VECTOR_DRIVER__H
#define __JAUS_GLOBAL_VECTOR_DRIVER__H

#include "jaus/services/srvclibdll.h"
#include "jaus/components/commandcomponent.h"
#include "jaus/messages/command/platform/setglobalvector.h"
#include "jaus/messages/query/platform/queryglobalvector.h"
#include "jaus/messages/inform/platform/reportglobalvector.h"
#include "jaus/messages/common/platform/globalpose.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class GlobalVectorDriver
    ///   \brief Interface for creating Global Vector Driver Components (ID 34).
    ///
    ///   The Global Vector Driver component is to perform closed loop control of
    ///   the desired global heading, altitude, and speed of a mobile platform.
    ///
    ///   This component takes the desired heading of the platform as
    ///   measured with respect ot the global coordinate system and the desired
    ///   speed of the platform.  For ground-based systems, the altitude ASL field
    ///   is ignored.  The Global Vector Driver component also receives data from
    ///   the Global Pose Sensor component and the Velocity State Sensor component.
    ///   This information allows the Global Vector Driver component to perform
    ///   cloased loop control on the platform's global heading, altitude, and
    ///   speed.
    ///
    ///   The output of the Global Vector Driver component is a wrench message that
    ///   is typically sent to the Primitive Driver component.  This message is
    ///   interpreted by the Primitive Driver component in order to enact 
    ///   platform motion.  The operator, acting as the system commander, could
    ///   set the desired speed, altitude, and heading.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL GlobalVectorDriver : public CommandComponent
    {
    public:
        GlobalVectorDriver();
        virtual ~GlobalVectorDriver();
        // Intializes component.
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        // Shutsdown the component.
        virtual int Shutdown();
        // Sets up the Global Vector Driver service information.
        virtual int SetupService();
        // Sets the rate at which Set Wrench Effort messages should be generated.
        virtual int SetUpdateRate(const double rate);
        // Process command messages.
        virtual int ProcessCommandMessage(const Message* msg, 
                                          const Byte commandAuthority);
        // Process query messages.
        virtual int ProcessQueryMessage(const Message* msg);
        // Process inform messages.
        virtual int ProcessInformMessage(const Message* msg);
        // Sets the global vector.
        virtual int SetGlobalVector(const Jaus::SetGlobalVector* command);
        // Generate a wrench effort based on the desired global vector.
        virtual int GenerateWrench(const Jaus::SetGlobalVector& command, SetWrenchEffort& wrench) = 0;
        //  Return the presence vector indicating what fields of Set Global Vector message are supported.
        virtual Byte GetSetGlobalVectorPresenceVector() const = 0;
        // Return the presence vector indicating what fields of Set Wrench Effort message are supported.
        virtual UShort GetSetWrenchEffortPresenceVector() const = 0;
        // Set the ID of the global pose sensor to use (if not set, one is found automatically).
        int SetGlobalPoseSensorID(const Address& id);
        // Set the ID of the velcoity state sensor to use (if not set, one is found automatically).
        int SetVelocityStateSensorID(const Address& id);
        // Set the ID of the primitive driver component to command.
        int SetPrimitiveDriverID(const Address& id);
        // Get the update rate at which wrench effort messages are generated.
        double GetUpdateRate() const;
        // Get the current known speed of platform.
        double GetSpeed() const;
        // Get the current known altitude of the platform.
        double GetElevation() const;
        // Check if a global vector has been set.
        bool HaveDesiredGlobalVector() const;
        // Get the known attitude of the platform.
        Attitude GetAttitude() const;
        // Gets the desired global vector to implement.
        Jaus::SetGlobalVector GetDesiredGlobalVector() const;
        // Get the ID of the global pose sensor being used.
        Address GetGlobalPoseSensorID() const;
        // Get the ID of the velocity state sensor being used.
        Address GetVelocityStateSensorID() const;
        // Get the ID of the primitive driver component to command.
        Address GetPrimitiveDriverID() const;
    private:
        void ClearCurrentVector();
        bool IsGlobalPoseSubscriptionReady();
        bool IsTravelSpeedSubscriptionReady();
        bool HaveControlOfPrimitiveDriver();        
        static void GlobalVectorDriverThread(void *args);
        CxUtils::Mutex  mGlobalVectorDriverMutex;   ///<  Mutex for thread protection.
        CxUtils::Thread mGlobalVectorDriverThread;  ///<  Thread object for vector driver.
        double mGlobalVectorDriverUpdateRateHz;     ///<  Update rate for generating wrench efforts.
        double* mpSpeed;                            ///<  Speed of the platform.
        double* mpElevation;                        ///<  Elevation of the platform.
        double* mpHeading;                          ///<  Current platform heading in radians.
        double* mpRoll;                             ///<  Current platform roll in radians.
        double* mpPitch;                            ///<  Current platform pitch in radians.
        Jaus::SetGlobalVector* mpDesiredVector;     ///<  Desired global vector.
        Address mGlobalPoseSensorID;                ///<  Sensors to get global pose data from.
        Address mVelocityStateSensorID;             ///<  Sensors to get velocity state data from.
        Address mPrimitiveDriverID;                 ///<  ID of the primitive driver to control.
    };
}

#endif
/*  End of File */
