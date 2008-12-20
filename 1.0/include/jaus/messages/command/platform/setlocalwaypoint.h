////////////////////////////////////////////////////////////////////////////////////
///
///  \file setlocalwaypoint.h
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
#ifndef __JAUS_SET_LOCAL_WAYPOINT__H
#define __JAUS_SET_LOCAL_WAYPOINT__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetLocalWaypoint
    ///   \brief A local waypoint can have up to six fields to describe it completely.  
    ///          The waypoint is defined in a local coordinate system using the X, Y, 
    ///          and Z fields.  The desired orientation of the platform at the waypoint 
    ///          is defined using the roll, pitch, and yaw fields.  Only fields 1-4 
    ///          (presence vector, waypoint number, the X, and Y) are required for each 
    ///          waypoint.  The presence vector is used to specify if the remaining 
    ///          fields, i.e., Z, roll, pitch, and yaw, are used to further describe 
    ///          each waypoint and the desired orientation at that point.  The waypoint 
    ///          number starts at zero for the first waypoint, and must be incremented 
    ///          by one for each additional waypoint.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetLocalWaypoint : public Message
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
                Z       = 0x01,
                Roll    = 0x02,
                Pitch   = 0x04,
                Yaw     = 0x08
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
                Z = 0,
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

            const static double MinX;        ///< Lowest X value:    -100,000 meters
            const static double MaxX;        ///< Highest X value:     100,000 meters
            const static double MinY;        ///< Lowest Y value:    -100,000 meters
            const static double MaxY;        ///< Highest Y value:     100,000 meters
            const static double MinZ;        ///< Lowest Z value:    -10,000 meters
            const static double MaxZ;        ///< Highest Z value:     35,000 meters
            const static double MinAngle;    ///< Maximum angle:        -Pi         radians
            const static double MaxAngle;    ///< Minimum angle:         Pi         radians

        };

        SetLocalWaypoint();
        SetLocalWaypoint(const SetLocalWaypoint& vector);
        virtual ~SetLocalWaypoint();

        int SetWaypointNumber(const UShort value);
        int SetX(const double value);
        int SetY(const double value);
        int SetZ(const double value);
        int SetRoll(const double value);
        int SetPitch(const double value);
        int SetYaw(const double value);

        Byte GetPresenceVector() const     { return mPresenceVector; }
        UShort GetWaypointNumber() const { return mWaypointNumber; }    
        double GetX() const                 { return mX; }            
        double GetY() const                 { return mY; }            
        double GetZ() const                 { return mZ; }                // Check presence vector 1st!
        double GetRoll() const             { return mRoll; }              // Check presence vector 1st!
        double GetPitch() const             { return mPitch; }            // Check presence vector 1st!
        double GetYaw() const             { return mYaw; }                // Check presence vector 1st!
        
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new SetLocalWaypoint(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetLocalWaypoint& operator=(const SetLocalWaypoint& msg);

        virtual int RunTestCase() const;

    protected:
        Byte mPresenceVector;       ///< Bit vector for fields present.
        UShort mWaypointNumber;     ///< The number of the waypoint, between 0 and 65535
        double mX;                  ///< The X coordinate, [-100000, 100000] meters
        double mY;                  ///< The Y coordinate, [-100000, 100000] meters
        double mZ;                  ///< The Z coordinate, [-100000, 100000] meters
        double mRoll;               ///< The roll orientation of the platform, [-Pi, Pi] radians
        double mPitch;              ///< The pitch orientation of the platform, [-Pi, Pi] radians
        double mYaw;                ///< The yaw orientation of the platform, [-Pi, Pi] radians
    };
}

#endif
/*  End of File */
