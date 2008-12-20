////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalwaypoint.h
///  \brief This file contains the message structure for Report Global Waypoint
///
///  <br>Author(s): David Adams
///  <br>Created: 12 June 2008
///  <br>Last Modified: 12 June 2008
///  <br>Copyright (c) 2007
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#ifndef __JAUS_REPORT_GLOBAL_WAYPOINT_H
#define __JAUS_REPORT_GLOBAL_WAYPOINT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportGlobalWaypoint
    ///   \brief This message provides the receiver with the current values of the 
    ///          requested waypoint as specified by the data in Code 240Ch: Query 
    ///          Global Waypoint.  The message data and mapping of the presence vector 
    ///          of this message are identical to Code 040Ch: Set Global Waypoint.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportGlobalWaypoint : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                Altitude    = 0x01,
                Roll        = 0x02,
                Pitch       = 0x04,
                Yaw         = 0x08
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                Altitude    = 0,
                Roll,    
                Pitch,    
                Yaw        
            };
        };

        class JAUS_MSG_DLL Limits
        {
        public:
            const static UInt MinWaypointNumber;
            const static UInt MaxWaypointNumber;
            const static double MinLatitude;
            const static double MaxLatitude;
            const static double MinLongitude;
            const static double MaxLongitude;
            const static double MinAltitude;
            const static double MaxAltitude;
            const static double MinRoll;
            const static double MaxRoll;
            const static double MinPitch;
            const static double MaxPitch;
            const static double MinYaw;
            const static double MaxYaw;

        };
        ReportGlobalWaypoint();
        ReportGlobalWaypoint(const ReportGlobalWaypoint &msg);
        virtual ~ReportGlobalWaypoint();

        virtual int SetPresenceVector(const Byte val);
        virtual Byte GetPresenceVector()const;
        virtual int SetWaypointNumber(const UInt val);
        virtual UInt GetWaypointNumber()const;
        virtual int SetLatitude(const double val);
        virtual double GetLatitude()const;
        virtual int SetLongitude(const double val);
        virtual double GetLongitude()const;
        virtual int SetAltitude(const double val);
        virtual double GetAltitude()const;
        virtual int SetRoll(const double val);
        virtual double GetRoll()const;
        virtual int SetPitch(const double val);
        virtual double GetPitch()const;
        virtual int SetYaw(const double val);
        virtual double GetYaw()const;

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportGlobalWaypoint(*this); }
        ReportGlobalWaypoint &operator=(const ReportGlobalWaypoint &msg);
    protected:
        Byte mPresenceVector;    ///< Byte, The presence vector applies to all waypoints.
        UInt mWaypointNumber;    ///< Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535.
        double mLatitude;        ///< Integer, Degrees, Scaled Integer, Lower Limit = -90, Upper Limit = 90.
        double mLongitude;       ///< Integer, Degrees, Scaled Integer, Lower Limit = -180, Upper Limit = 180.
        double mAltitude;        ///< Integer, Meters, Scaled Integer, Lower Limit = -10,000, Upper Limit = 35,000.
        double mRoll;            ///< Short Integer, Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI.
        double mPitch;           ///< Short Integer, Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI.
        double mYaw;             ///< Short Integer, Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI.

    };
}

#endif

#endif 

/*  End of File */
