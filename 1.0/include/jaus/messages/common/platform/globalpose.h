////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalpose.h
///  \brief This file contains a simple data structure for global pose
///  data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 24 March 2008
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
#ifndef _JAUS_GLOBAL_POSE_DATA_H
#define _JAUS_GLOBAL_POSE_DATA_H

#include "jaus/messages/time.h"
#include "jaus/messages/date.h"
#include "jaus/messages/bitvector.h"
#include "jaus/messages/common/platform/globalposition.h"
#include "jaus/messages/common/platform/attitude.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class GlobalPose
    ///   \brief Simple global pose structure.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL GlobalPose : public GlobalPosition, 
                                    public Attitude
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                Latitude  = 0x0001,
                Longitude = 0x0002,
                Elevation = 0x0004,
                PositionRms = 0x0008,
                Roll = 0x0010,
                Pitch = 0x0020,
                Yaw = 0x0040,
                AttitudeRms = 0x0080,
                TimeStamp = 0x0100
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
            enum Bits
            {
                Latitude = 0,
                Longitude,
                Elevation,
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
            const static double MaxLatitude;
            const static double MaxLongitude;
            const static double MaxElevation;
            const static double MinLatitude;
            const static double MinLongitude;
            const static double MinElevation;
            const static double MaxAttitude;
            const static double MinAttitude; 
            const static double MinPositionRms;
            const static double MaxPositionRms;
            const static double MinAttitudeRms;
            const static double MaxAttitudeRms;
        };
        GlobalPose();
        GlobalPose(const GlobalPose& pose);
        virtual ~GlobalPose();

        virtual int SetLatitude(const double lat);
        virtual int SetLongitude(const double lon);
        virtual int SetElevation(const double elev);
        virtual int SetPosition(const double lat, const double lon, const double elev = 0);
        virtual int SetYaw(const double yaw);
        virtual int SetPitch(const double pitch);
        virtual int SetRoll(const double roll);
        virtual int SetAttitude(const double roll, const double pitch, const double yaw);
        int SetPositionRms(const double prms);
        int SetAttitudeRms(const double arms);
        int SetTimeStamp(const Time& tstamp);
        
        virtual int GetLatitude(double& lat) const;
        virtual int GetLongitude(double& lon) const;
        virtual int GetElevation(double& elev) const;
        virtual int GetYaw(double& yaw) const;
        virtual int GetPitch(double& pitch) const;
        virtual int GetRoll(double& roll) const;
        int GetPositionRms(double& prms) const;
        int GetAttitudeRms(double& arms) const;
        int GetTimeStamp(Time& tstamp) const;
        int GetPresenceVector(UShort& v) const;
        
        void PrintGlobalPose(const bool degrees = true) const;
        void ClearGlobalPose();
        void ClearField(const VectorBit::Bits bit);
        void ClearFields(const UShort pv);
        
        bool IsFieldPresent(const VectorBit::Bits bit) const;
        bool AreFieldsPresent(const UShort pv) const;
        bool HaveYaw() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Yaw); }
        bool HavePitch() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch); }
        bool HaveRoll() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Roll); }
        bool HaveLatitude() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Latitude); }
        bool HaveLongitude() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Longitude); }
        bool HaveElevation() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::Elevation); }
        bool HavePositionRms() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::PositionRms); }
        bool HaveAttitudeRms() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::AttitudeRms); }
        bool HaveTimeStamp() const { return BitVector::IsBitSet(mPresenceVector, VectorBit::TimeStamp); }
        bool HavePosition() const { if(HaveLatitude() && HaveLongitude() && HaveElevation()) { return true; } return false; }
        bool HaveAttitude() const { if(HaveRoll() && HavePitch() && HaveYaw()) { return true; } return false; }
        
        double GetPositionRms() const { return mPositionRms; }
        double GetAttitudeRms() const { return mAttitudeRms; }
        Time GetTimeStamp() const { return mTimeStamp; }
        UShort GetPresenceVector() const { return mPresenceVector; }

        virtual void ClearAttitude();
        virtual void ClearPitch();
        virtual void ClearRoll();
        virtual void ClearYaw();
        virtual void ClearPosition();
        virtual void ClearLatitude();
        virtual void ClearLongitude();
        virtual void ClearElevation();
        virtual void ClearPositionRms();
        virtual void ClearAttitudeRms();
        virtual void ClearTimeStamp();

        inline virtual double GetPitch() const { return mPitch; }
        inline virtual double GetRoll() const { return mRoll; }
        inline virtual double GetYaw() const { return mYaw; }
        inline virtual double GetLatitude() const { return mLatitude; }
        inline virtual double GetLongitude() const { return mLongitude; }
        inline virtual double GetElevation() const { return mElevation; }

        GlobalPose& CopyFrom(const GlobalPose& p);
        GlobalPose& operator=(const Time& time);
        GlobalPose& operator=(const GlobalPose& gp);
        GlobalPose& operator=(const GlobalPosition& wp);
        GlobalPose& operator=(const Attitude& att);
    protected:
        UShort mPresenceVector; ///<  Presence vector.
        double mPositionRms;    ///<  Position root-mean-square error.
        double mAttitudeRms;    ///<  Atttitude root-mean-square error.
        Time mTimeStamp;        ///<  Timestamp information.
    };
}

#endif

#endif
/*  End of File */
