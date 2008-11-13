////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalpose.h
///  \brief This file contains the message structure for Report Local Pose
///
///  <br>Author(s): David Adams
///  <br>Created: 10 June 2008
///  <br>Last Modified: 10 June 2008
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
#ifndef __JAUS_REPORT_LOCAL_POSE_H
#define __JAUS_REPORT_LOCAL_POSE_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportLocalPose
    ///   \brief The Report Global Pose message provides the position and attitude of
    ///          the platform.  The position of the platform is given in latitude,
    ///          longitude, and altitude, in accordance with the WGS 84 standard.  All times are in Coordinated Universal Time (UTC).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportLocalPose : public Message
    {
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
                X            = 0x0001,
                Y            = 0x0002,
                Z            = 0x0004,
                PositionRms = 0x0008,
                Roll        = 0x0010,
                Pitch        = 0x0020,
                Yaw         = 0x0040,
                AttitudeRms = 0x0080,
                TimeStamp   = 0x0100
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
                X            = 0,
                Y,
                Z,
                PositionRms,
                Roll,
                Pitch,
                Yaw,
                AttitudeRms,
                TimeStamp
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
            const static double MinX;                ///< Lower Limit = -100,000.
            const static double MaxX;                ///< Upper Limit = 100,000.
            const static double MinY;                ///< Lower Limit = -100,000.
            const static double MaxY;                ///< Upper Limit = 100,000.
            const static double MinZ;                ///< Lower Limit = -10,000.
            const static double MaxZ;                ///< Upper Limit = 35,000.
            const static double MinPositionRms;        ///< Lower Limit = 0.
            const static double MaxPositionRms;        ///< Upper Limit = 100.
            const static double MinRoll;            ///< Lower Limit = -PI.
            const static double MaxRoll;            ///< Upper Limit = PI.
            const static double MinPitch;            ///< Lower Limit = -PI.
            const static double MaxPitch;            ///< Upper Limit = PI.
            const static double MinYaw;                ///< Lower Limit = -PI.
            const static double MaxYaw;                ///< Upper Limit = PI.
            const static double MinAttitudeRms;        ///< Lower Limit = 0.
            const static double MaxAttitudeRms;        ///< Upper Limit = PI.
        };
    public:
        ReportLocalPose();
        ReportLocalPose(const ReportLocalPose &msg);
        ~ReportLocalPose();

        virtual int SetPresenceVector(const UShort pv);
        virtual int SetX(const double val);
        virtual int SetY(const double val);
        virtual int SetZ(const double val);
        virtual int SetPositionRms(const double val);
        virtual int SetRoll(const double val);
        virtual int SetPitch(const double val);
        virtual int SetYaw(const double val);
        virtual int SetAttitudeRms(const double val);
        virtual int SetTimeStamp(const Time& tstamp);

        virtual double GetX()const;
        virtual double GetY()const;
        virtual double GetZ()const;
        virtual double GetPositionRms()const;
        virtual double GetRoll()const;
        virtual double GetPitch()const;
        virtual double GetYaw()const;
        virtual double GetAttitudeRms()const;
        virtual Time   GetTimeStamp() const;
        virtual UShort GetPresenceVector() const;

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportLocalPose(*this); }
        ReportLocalPose &operator=(const ReportLocalPose &msg);

    protected:
        UShort mPresenceVector;    ///<  Unsigned Short Integer, Presence vector associated with message.
        double mX;                ///<  Integer, Scaled Integer, Meters, Lower Limit = -100,000, Upper Limit = 100,000.
        double mY;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -100,000, Upper Limit = 100,000.
        double mZ;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -10,000, Upper Limit = 35,000.
        double mPositionRms;    ///<  Unsigned Integer, Meters, A Rms value indicating the validity of the position data.  Scaled Integer, Lower Limit = 0, Upper Limit = 100.
        double mRoll;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI.
        double mPitch;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI.
        double mYaw;            ///<  Short Integer, Radians, Scaled Integer, Lower Limit = -PI, Upper Limit = PI.
        double mAttitudeRms;    ///<  Unsigned Short Integer,    Radians, A Rms value indicating the validity of the Roll/Pitch/Yaw data, Scaled Integer, Lower Limit = 0, Upper Limit = PI.
        Time   mTimeStamp;      ///<  Unsigned Integer,    N/A    Bits 0-9:  milliseconds, range 0...999, Bits 10-15:  Seconds, range 0...59, Bits 16 – 21:  Minutes, range 0...59, Bits 22-26:  Hour (24 hour clock), range 0..23, Bits 27-31:  Day, range 1…31.
    };
}

#endif

#endif

/*  End of File */
