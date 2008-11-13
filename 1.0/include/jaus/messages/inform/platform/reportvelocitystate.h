////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportvelocitystate.h
///  \brief This file contains the message class for Report Velocity State.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 12 May 2008
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
#ifndef __JAUS_REPORT_VELOCITY_STATE__H
#define __JAUS_REPORT_VELOCITY_STATE__H

#include "jaus/messages/message.h"
#include "jaus/messages/time.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportVelocityState
    ///   \brief This message provides the linear velocity and rotational rate of the
    ///          platform.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportVelocityState : public Message
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
                VelocityX = 0x0001,
                VelocityY = 0x0002,
                VelocityZ = 0x0004,
                VelocityRms = 0x0008,
                RollRate = 0x0010,
                PitchRate = 0x0020,
                YawRate = 0x0040,
                RateRms = 0x0080,
                TimeStamp = 0x0100
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
                VelocityX = 0,
                VelocityY,
                VelocityZ,
                VelocityRms,
                RollRate,
                PitchRate,
                YawRate,
                RateRms,
                TimeStamp
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief This class contains constants for limits of some values used in
        ///          this class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxVelocity;    ///<  Maximum velocity in m/s.
            const static double MinVelocity;    ///<  Minimum velocity in m/s.
            const static double MaxRate;        ///<  Maximum rotational rate rad/s.
            const static double MinRate;        ///<  Minimum rotational rate rad/s.
        };
        ReportVelocityState();
        ReportVelocityState(const ReportVelocityState& msg);
        ~ReportVelocityState();
        int SetVelocityX(const double value);
        int SetVelocityY(const double value);
        int SetVelocityZ(const double value);
        int SetVelocityRms(const double value);
        int SetRollRate(const double value);
        int SetPitchRate(const double value);
        int SetYawRate(const double value);
        int SetRateRms(const double value);
        int SetTimeStamp(const Time& tstamp);
        double GetVelocityX() const { return mVelocityX; }      // Check presence vector 1st!
        double GetVelocityY() const { return mVelocityY; }      // Check presence vector 1st!
        double GetVelocityZ() const { return mVelocityZ; }      // Check presence vector 1st!
        double GetVelocityRms() const { return mVelocityRms; }  // Check presence vector 1st!
        double GetRollRate() const { return mRollRate; }        // Check presence vector 1st!
        double GetPitchRate() const { return mPitchRate; }      // Check presence vector 1st!
        double GetYawRate() const { return mYawRate; }          // Check presence vector 1st!
        double GetRateRms() const { return mRateRms; }          // Check presence vector 1st!
        Time GetTimeStamp() const { return mTimeStamp; }        // Check presence vector 1st!
        UShort GetPresenceVector() const { return mPresenceVector; }
        void ClearFields(const UShort mask);
        virtual void Print() const;
        virtual void PrintVelocityState() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportVelocityState(*this); }
        ReportVelocityState& operator=(const ReportVelocityState& msg);
    protected:
        UShort mPresenceVector; ///<  Bit vector for fields present.
        double mVelocityX;      ///<  Meters per second [-65.534,65.534].
        double mVelocityY;      ///<  Meters per second [-65.534,65.534].
        double mVelocityZ;      ///<  Meters per second [-65.534,65.534].
        double mVelocityRms;    ///<  Validity of velocity data [0,100].
        double mRollRate;       ///<  Roll rate [-32.767,32.767] rad/s.
        double mPitchRate;      ///<  Roll rate [-32.767,32.767] rad/s.
        double mYawRate;        ///<  Roll rate [-32.767,32.767] rad/s.
        double mRateRms;        ///<  Validity of rotational data [0,100].
        Time mTimeStamp;        ///<  Time Stamp data.
    };
}

#endif
/*  End of File */
