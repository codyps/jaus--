////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalwaypoint.h
///  \brief This file contains the message structure for Report Local Waypoint
///
///  <br>Author(s): David Adams
///  <br>Created: 16 June 2008
///  <br>Last Modified: 16 June 2008
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
#ifndef __JAUS_REPORT_LOCAL_WAYPOINT_H
#define __JAUS_REPORT_LOCAL_WAYPOINT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportLocalWaypoint
    ///   \brief This message provides the receiver with the current values of the 
    ///             requested waypoint as specified by the data in Code 240Dh: Query Local 
    ///          Waypoint.  The message data and mapping of the presence vector of this 
    ///          message are identical to Code 040Dh: Set Local Waypoint.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportLocalWaypoint : public Message
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
                Z            = 0x01,
                Roll        = 0x02,
                Pitch        = 0x04,
                Yaw         = 0x08
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Masks
            {            
                Z            = 0,
                Roll,
                Pitch,
                Yaw,
 
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static UInt MinWaypointNumber;    ///< Lower Limit = 0.
            const static UInt MaxWaypointNumber;    ///< Upper Limit = 65,535.
            const static double MinX;                ///< Lower Limit = -100,000.
            const static double MaxX;                ///< Upper Limit = 100,000.
            const static double MinY;                ///< Lower Limit = -100,000.
            const static double MaxY;                ///< Upper Limit = 100,000.                                        
            const static double MinZ;                ///< Lower Limit = -10,000.
            const static double MaxZ;                ///< Upper Limit = 35,000.                
            const static double MinRoll;            ///< Lower Limit = -PI.  
            const static double MaxRoll;            ///< Upper Limit = PI.
            const static double MinPitch;            ///< Lower Limit = -PI.
            const static double MaxPitch;            ///< Upper Limit = PI.
            const static double MinYaw;                ///< Lower Limit = -PI.
            const static double MaxYaw;                ///< Upper Limit = PI.

        };
        ReportLocalWaypoint();
        ReportLocalWaypoint(const ReportLocalWaypoint &msg);
        virtual ~ReportLocalWaypoint();
        
        virtual int SetPresenceVector(const Byte pv);
        virtual int SetWaypointNumber(const UInt val);
        virtual int SetX(const double val);
        virtual int SetY(const double val);
        virtual int SetZ(const double val);
        virtual int SetRoll(const double val);
        virtual int SetPitch(const double val);
        virtual int SetYaw(const double val);

        virtual Byte GetPresenceVector() const;
        virtual UInt GetWaypointNumber()const;
        virtual double GetX()const;
        virtual double GetY()const;
        virtual double GetZ()const;
        virtual double GetRoll()const;
        virtual double GetPitch()const;
        virtual double GetYaw()const;

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportLocalWaypoint(*this); }
        ReportLocalWaypoint &operator=(const ReportLocalWaypoint &msg);
    protected:
        Byte mPresenceVector;    
        UInt mWaypointNumber;    ///<  Unsigned Short Integer, N/A    Lower Limit = 0, Upper Limit = 65,535.
        double mX;                ///<  Integer, Scaled Integer, Meters, Lower Limit = -100,000, Upper Limit = 100,000.
        double mY;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -100,000, Upper Limit = 100,000.
        double mZ;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -10,000, Upper Limit = 35,000.
        double mRoll;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI. 
        double mPitch;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI.
        double mYaw;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI.  

    };
}

#endif

#endif 

/*  End of File */
