////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalpose.h
///  \brief This file contains a JAUS message structure for encoding
///  and decoding JAUS_REPORT_GLOBAL_POSE messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 17 July 2007
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
#ifndef _JAUS_REPORT_GLOBAL_POSE_H
#define _JAUS_REPORT_GLOBAL_POSE_H

#include "jaus/messages/message.h"
#include "jaus/messages/common/platform/globalpose.h"

#ifdef __cplusplus

namespace Jaus
{

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportGlobalPose
    ///   \brief Writes/Reads JAUS Report Global Pose Message, and stores
    ///   related data.
    ///
    //////////////////////////////////////////////////////////////////////////////////// 
    class JAUS_MSG_DLL ReportGlobalPose : public Message, public GlobalPose
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
                Latitude        = 0x0001,
                Longitude       = 0x0002,
                Elevation       = 0x0004,
                PositionRms     = 0x0008,
                Roll            = 0x0010,
                Pitch           = 0x0020,
                Yaw             = 0x0040,
                AttitudeRms     = 0x0080,
                TimeStamp       = 0x0100
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
        ReportGlobalPose();
        ReportGlobalPose(const ReportGlobalPose &gp);
        ~ReportGlobalPose();
        int GetGlobalPose(GlobalPose &p) { p = (GlobalPose)(*this); return JAUS_OK; }
        void SetGlobalPose(const GlobalPose &p) { CopyFrom(p); }
        GlobalPose GetGlobalPose() const { return (GlobalPose)(*this); }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual Message* Clone() const { return new ReportGlobalPose(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        operator GlobalPose() const { return *( (GlobalPose*)(this) ); }
        ReportGlobalPose &operator=(const ReportGlobalPose &gp);
    };


}

#endif

#endif
/* End of File */
