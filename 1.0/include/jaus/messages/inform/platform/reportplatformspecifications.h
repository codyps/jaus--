////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportplatformspecifications.h
///  \brief This file contains the message class for Report Platform
///  Specifications.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#ifndef __JAUS_REPORT_PLATFORM_SPECIFICATIONS__H
#define __JAUS_REPORT_PLATFORM_SPECIFICATIONS__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportPlatformSpecifications
    ///   \brief This message provides the receiver specifications of the platform
    ///   including its name, dimensions, location of its center of gravity, turning
    ///   radius, wheel base, track width, static pitch and roll over angles, and
    ///   maximum velocities and angular rates the platform is capable of achieving.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportPlatformSpecifications : public Message
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
                MobilityPlatformName    = 0x00000001,
                Front                   = 0x00000002,
                Back                    = 0x00000004,
                Right                   = 0x00000008,
                Left                    = 0x00000010,
                Bottom                  = 0x00000020,
                Top                     = 0x00000040,
                Xcg                     = 0x00000080,
                Ycg                     = 0x00000100,
                Zcg                     = 0x00000200,
                TurningRadius           = 0x00000400,
                WheelBase               = 0x00000800,
                TrackWidth              = 0x00001000,
                StaticPitchOver         = 0x00002000,
                StaticRollOver          = 0x00004000,
                MaximumVelocityX        = 0x00008000,
                MaximumVelocityY        = 0x00010000,
                MaximumVelocityZ        = 0x00020000,
                MaximumRollRate         = 0x00040000,
                MaximumPitchRate        = 0x00080000,
                MaximumYawRate          = 0x00100000
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
                MobilityPlatformName    = 0,
                Front,
                Back,
                Right,
                Left,
                Bottom,
                Top,
                Xcg,
                Ycg,
                Zcg,
                TurningRadius,
                WheelBase,
                TrackWidth,
                StaticPitchOver,
                StaticRollOver,
                MaximumVelocityX,
                MaximumVelocityY,
                MaximumVelocityZ,
                MaximumRollRate,
                MaximumPitchRate,
                MaximumYawRate
            };
        };
        ReportPlatformSpecifications();
        ReportPlatformSpecifications(const ReportPlatformSpecifications& msg);
        virtual ~ReportPlatformSpecifications();
        int SetMobilityPlatformName(const std::string& name);
        int SetDistanceToFront(const double val);
        int SetDistanceToBack(const double val);
        int SetDistanceToRight(const double val);
        int SetDistanceToLeft(const double val);
        int SetDistanceToBottom(const double val);
        int SetDistanceToTop(const double val);
        int SetXcg(const double val);
        int SetYcg(const double val);
        int SetZcg(const double val);
        int SetTurningRadius(const double val);
        int SetWheelBase(const double val);
        int SetTrackWidth(const double val);
        int SetStaticPitchOver(const double val);
        int SetStaticRollOver(const double val);
        int SetMaximumVelocityX(const double val);
        int SetMaximumVelocityY(const double val);
        int SetMaximumVelocityZ(const double val);
        int SetMaximumRollRate(const double val);
        int SetMaximumPitchRate(const double val);
        int SetMaximumYawRate(const double val);
        UInt GetPresenceVector() const { return mPresenceVector; }
        // Check presence vector before to see if data is valid.
        inline std::string GetMobilityPlatformName() const { return mMobilityPlatformName; }
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToFront() const { return mFront; }
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToBack() const { return mBack; }
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToRight() const { return mRight; } 
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToLeft() const { return mLeft; }
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToBottom() const { return mBottom; }
        // Check presence vector before to see if data is valid.
        inline double GetDistanceToTop() const { return mTop; }
        // Check presence vector before to see if data is valid.
        inline double GetXcg() const { return mXcg; }
        // Check presence vector before to see if data is valid.
        inline double GetYcg() const { return mYcg; }
        // Check presence vector before to see if data is valid.
        inline double GetZcg() const { return mZcg; }
        // Check presence vector before to see if data is valid.
        inline double GetTurningRadius() const { return mTurningRadius; }
        // Check presence vector before to see if data is valid.
        inline double GetWheelBase() const { return mWheelBase; }
        // Check presence vector before to see if data is valid.
        inline double GetTrackWidth() const { return mTrackWidth; }
        // Check presence vector before to see if data is valid.
        inline double GetStaticPitchOver() const { return mStaticPitchOver; }
        // Check presence vector before to see if data is valid.
        inline double GetStaticRollOver() const { return mStaticRollOver; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumVelocityX() const { return mMaximumVelocityX; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumVelocityY() const { return mMaximumVelocityY; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumVelocityZ() const { return mMaximumVelocityZ; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumRollRate() const { return mMaximumRollRate; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumPitchRate() const { return mMaximumPitchRate; }
        // Check presence vector before to see if data is valid.
        inline double GetMaximumYawRate() const { return mMaximumYawRate; }
        virtual void Print() const;
        virtual void PrintPlatformSpecifications() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportPlatformSpecifications(*this); }
        ReportPlatformSpecifications& operator=(const ReportPlatformSpecifications& msg);
    protected:
        UInt mPresenceVector;                 ///<  Bit vector for fields present.
        std::string mMobilityPlatformName;    ///<  Name of the platform.
        double mFront;              ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].             
        double mBack;               ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].
        double mRight;              ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].
        double mLeft;               ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].
        double mBottom;             ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].
        double mTop;                ///<  Distance to bounding planes from vehicle origin [0, 32.767 Meters].
        double mXcg;                ///<  Center of gravity coordinates w/r/t mobility platform origin. [0, 32.767] Meters.
        double mYcg;                ///<  Center of gravity coordinates w/r/t mobility platform origin. [0, 32.767] Meters.
        double mZcg;                ///<  Center of gravity coordinates w/r/t mobility platform origin. [0, 32.767] Meters.
        double mTurningRadius;      ///<  Minimum turning radius [0, 65.535] Meters.
        double mWheelBase;          ///<  Distance between fore-most and aft-most axle [0, 65.535] Meters.
        double mTrackWidth;         ///<  Distance between left and right most driving devices [0, 65.535] Meters.
        double mStaticPitchOver;    ///<  Pitch over angle at 0 velocity [0.256] Radians.
        double mStaticRollOver;     ///<  Roll over angle at 0 velocity [0, 2.56] Radians.
        double mMaximumVelocityX;   ///<  [0, 65.535 Meters per second.
        double mMaximumVelocityY;   ///<  [0, 65.535 Meters per second.
        double mMaximumVelocityZ;   ///<  [0, 65.535 Meters per second.
        double mMaximumRollRate;    ///<  [0, 32.767] Radians per Second.
        double mMaximumPitchRate;   ///<  [0, 32.767] Radians per Second.
        double mMaximumYawRate;     ///<  [0, 32.767] Radians per Second.
    };
}

#endif
/*  End of File */
