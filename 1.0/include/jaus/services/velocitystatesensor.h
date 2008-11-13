////////////////////////////////////////////////////////////////////////////////////
///
///  \file velocitystatesensor.h
///  \brief This file contains software for creating a JAUS Velocity State
///  Sensor component (ID 42).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 18 May 2008
///  <br>Last Modified: 18 May 2008
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
#ifndef __JAUS_VELOCITY_STATE_SENSOR__H
#define __JAUS_VELOCITY_STATE_SENSOR__H

#include "jaus/components/informcomponent.h"
#include "jaus/services/srvclibdll.h"
#include <cxutils/mutex.h>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class VelocityStateSensor
    ///   \brief Velocity State Sensor component structure. (Component ID 42).
    ///
    ///   The Velocity State Sensor has the responsibility of reporting the 
    ///   instantaneous velocity of the platform.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL VelocityStateSensor : public InformComponent
    {
    public:
        VelocityStateSensor(const double updateRate = 10);
        virtual ~VelocityStateSensor();
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        int SetVelocityState(const ReportVelocityState& state);
        int SetTravelSpeed(const double travelSpeed);
        int SetSensorUpdateRate(const double rate);
        double GetSensorUpdateRate() const { return mMaxUpdateRate; }
        double GetTravelSpeed() const;
        ReportVelocityState GetVelocityState() const;
        virtual int ProcessQueryMessage(const Message* msg);
        virtual int GenerateEvent(const Event* eventInfo);
        virtual int ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                        Byte& responseValue,
                                        double& confirmedRate,
                                        std::string& errorMessage) const;
    protected:
        int RespondToQuery(const QueryVelocityState* query);
        double mMaxUpdateRate;                  ///<  Update rate of the sensor.
        double mTravelSpeed;                    ///<  Travel speed of the platform.
        ReportVelocityState mVelocityState;     ///<  Current velocity state data. 
        CxUtils::Mutex mVelocityStateMutex;     ///<  Mutex for thread protection.
    };
}

#endif
/*  End of File */
