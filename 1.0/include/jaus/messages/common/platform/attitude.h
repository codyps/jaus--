////////////////////////////////////////////////////////////////////////////////////
///
///  \file attitude.h
///  \brief This file contains a simple data structure for representing/storing
///  attitude information in JAUS.  The primary information is yaw, pitch, and
///  roll data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 6 February 2007
///  <br>Last Modified: 26 August 2007
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
#ifndef __JAUS_ATTITUDE_H
#define __JAUS_ATTITUDE_H

#include "jaus/messages/types.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Attitude
    ///   \brief Simple structure for storing attitude information in JAUS.  Includes
    ///   functions for converting to and from radians.  All attitude information
    ///   is stored in radians [-pi,pi].
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Attitude
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxAttitude;    ///<  Maximum angle.
            const static double MinAttitude;    ///<  Minimum angle.
        };
        Attitude();
        Attitude(const double roll,
                  const double pitch,
                  const double yaw);
        Attitude(const Attitude &att);
        virtual ~Attitude();
        virtual int SetAttitude(const double roll,
                                const double pitch,
                                const double yaw);
        virtual int SetPitch(const double pitch);
        virtual int SetRoll(const double roll);
        virtual int SetYaw(const double yaw);

        virtual int GetAttitude(double &roll,
                        double &pitch,
                        double &yaw) const;
        virtual int GetPitch(double &pitch) const;
        virtual int GetRoll(double &roll) const;
        virtual int GetYaw(double &yaw) const;
        
        virtual void ClearAttitude();
        virtual void ClearPitch();
        virtual void ClearRoll();
        virtual void ClearYaw();

        virtual void PrintAttitude(const bool degrees = true) const;
        virtual double GetPitch() const;
        virtual double GetRoll() const;
        virtual double GetYaw() const;
        static double GetHeadingDifferance(const double currentAngle, 
                                           const double desiredAngle, 
                                           const bool degrees = false);
        static double AddAngle(const double currentAngle, 
                               const double angleDelta, 
                               const bool degrees = false);
        Attitude &operator=(const Attitude &att);
    protected:
        double mRoll;           ///<  Roll in radians [-PI,PI].  Rotation around x-axis.
        double mPitch;          ///<  Pitch in radians [-PI,PI].  Rotation around y-axis.
        double mYaw;            ///<  Yaw in radians [-PI,PI].  Rotation around z-axis.
    };
}

#endif

#endif
/*  End of File */
