////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalposesensor.h
///  \brief This file contains software for creating a JAUS Global Pose
///  Sensor component (ID 38).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 15 April 2008
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
#ifndef __JAUS_GLOBAL_POSE_SENSOR__H
#define __JAUS_GLOBAL_POSE_SENSOR__H

#include "jaus/components/informcomponent.h"
#include "jaus/services/srvclibdll.h"
#include <cxutils/mutex.h>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class GlobalPoseSensor
    ///   \brief Global Pose Sensor Component structure. (Component ID 38).
    ///
    ///   This component is used to distribute Global Pose data to other JAUS
    ///   components.  Just update the JGlobalPose data, and the component will take
    ///   care of the rest automatically.  Component also supports 
    ///   service connections.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL GlobalPoseSensor : public InformComponent
    {
    public:
        GlobalPoseSensor(const double updateRate = 5);
        virtual ~GlobalPoseSensor();
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        int SetGlobalPose(const GlobalPose& gp);
        int SetSensorUpdateRate(const double r);
        int ProcessQueryMessage(const Message* msg);
        void ClearGlobalPose();        
        GlobalPose GetGlobalPose() const;
        double GetSensorUpdateRate() const;
        virtual int GenerateEvent(const Event* eventInfo);
        virtual int ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                        Byte& responseValue,
                                        double& confirmedRate,
                                        std::string& errorMessage) const;
        virtual int ProcessServiceConnectionRequest(const Jaus::CreateServiceConnection& command,
                                                    Byte& responseValue,
                                                    double& confirmedRate) const;
        virtual int GenerateServiceConnectionMessage(const ServiceConnection& sc);
    private:
        int RespondToQuery(const QueryGlobalPose* query);
        double mMaxUpdateRate;              ///<  Update rate of global pose sensor.
        GlobalPose mGlobalPose;             ///<  Current global pose.
        CxUtils::Mutex mGlobalPoseMutex;    ///<  Mutex for thread protection of global pose data.
    };
}

#endif
/*  End of File */
