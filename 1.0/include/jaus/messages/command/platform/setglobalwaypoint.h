////////////////////////////////////////////////////////////////////////////////////
///
///  \file setglobalwaypoint.h
///  \brief This file contains the message class for Set Global Waypoint.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 21 August 2008
///  <br>Last Modified: 21 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cholland@ist.ucf.edu
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
#ifndef __JAUS_SET_GLOBAL_WAYPOINT__H
#define __JAUS_SET_GLOBAL_WAYPOINT__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetGlobalWaypoint
    ///   \brief A global waypoint can have up to six fields to describe it completely.  
    ///          The waypoint is defined in the global coordinate system using the 
    ///          latitude, longitude, and altitude, fields.  The desired orientation of 
    ///          the platform at the waypoint is defined using the roll, pitch, and yaw 
    ///          fields.  Only fields 1-4 (presence vector, waypoint number, latitude, 
    ///          and longitude) are required for each waypoint.  The presence vector is 
    ///          used to specify if the remaining fields, i.e., altitude, roll, pitch, 
    ///          and yaw, are used to further describe the waypoint and the desired 
    ///          orientation at that point.  The waypoint number starts at zero for the 
    ///          first waypoint, and must be incremented by one for each additional 
    ///          waypoint.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetGlobalWaypoint : public Message
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
                Altitude = 0x01,
                Roll     = 0x02,
                Pitch    = 0x04,
                Yaw      = 0x08
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
                Altitude = 0,
                Roll,
                Pitch,
                Yaw
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Contains constants for limit values of data members of class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            //don't ask, the spec defines it this way, probably so that it's still valid if computer architecture change
            const static UShort WaypointNumberLowerLimit;   ///< Lower bound for the number of waypoints:    0
            const static UShort WaypointNumberUpperLimit;   ///< Upper bound for the number of waypoints:    65535

            const static double MinLatitude;    ///< Lowest possible latitude:        -90 degrees
            const static double MaxLatitude;    ///< Highest possible latitude:         90 degrees
            const static double MinLongitude;   ///< Lowest possible longitude:        -180 degrees
            const static double MaxLongitude;   ///< Highest possible longitude:     180 degrees
            const static double MinAltitude;    ///< Lowest possible altitude:        -10,000 meters
            const static double MaxAltitude;    ///< Highest possible altitude:         35,000 meters
            const static double MinAngle;       ///< Maximum angle:                    -Pi radians
            const static double MaxAngle;       ///< Minimum angle:                     Pi radians

        };

        SetGlobalWaypoint();
        SetGlobalWaypoint(const SetGlobalWaypoint& vector);
        ~SetGlobalWaypoint();

        int SetWaypointNumber(const UShort value);
        int SetLatitude(const double value);
        int SetLongitude(const double value);
        int SetAltitude(const double value);
        int SetRoll(const double value);
        int SetPitch(const double value);
        int SetYaw(const double value);

        Byte GetPresenceVector() const     { return mPresenceVector; }
        UShort GetWaypointNumber() const { return mWaypointNumber; }    
        double GetLatitude() const         { return mLatitude; }            
        double GetLongitude() const         { return mLongitude; }            
        double GetAltitude() const         { return mAltitude; }          // Check presence vector 1st!
        double GetRoll() const             { return mRoll; }              // Check presence vector 1st!
        double GetPitch() const             { return mPitch; }            // Check presence vector 1st!
        double GetYaw() const             { return mYaw; }                // Check presence vector 1st!
        
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new SetGlobalWaypoint(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetGlobalWaypoint& operator=(const SetGlobalWaypoint& msg);

        virtual int RunTestCase() const;

    protected:
        Byte mPresenceVector;       ///< Bit vector for fields present.
        UShort mWaypointNumber;     ///< The number of the waypoint, between 0 and 65535
        double mLatitude;           ///< The latitude coordinate, [-90, 90] degrees
        double mLongitude;          ///< The longitude coordinate, [-180, 180] degrees
        double mAltitude;           ///< The altitude coordinate, [-10000, 35000] meters
        double mRoll;               ///< The roll orientation of the platform, [-Pi, Pi] radians
        double mPitch;              ///< The pitch orientation of the platform, [-Pi, Pi] radians
        double mYaw;                ///< The yaw orientation of the platform, [-Pi, Pi] radians
    };
}

#endif
/*  End of File */
