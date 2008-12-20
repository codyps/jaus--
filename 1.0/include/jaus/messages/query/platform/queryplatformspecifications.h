////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryplatformspecifications.h
///  \brief This file contains the message structure for Query Platform 
///         Specifications.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 5 April 2008
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
#ifndef __JAUS_QUERY_PLATFORM_SPECIFICATIONS_H
#define __JAUS_QUERY_PLATFORM_SPECIFICATIONS_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class QueryPlatformSpecifications
    ///   \brief This message shall cause the receiving component to reply to the
    ///   requestor with a Report Global Pose message.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryPlatformSpecifications : public Message
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
        ///          presence vector of this structure.
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
        QueryPlatformSpecifications();
        QueryPlatformSpecifications(const QueryPlatformSpecifications &msg);
        virtual ~QueryPlatformSpecifications();
        int SetPresenceVector(const UShort pv);
        UInt GetPresenceVector() const { return mPresenceVector; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new QueryPlatformSpecifications(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        QueryPlatformSpecifications &operator=(const QueryPlatformSpecifications &msg);
    protected:
        UInt mPresenceVector; ///<  Presence vector associated with message.
    };
}

#endif

#endif 

/*  End of File */
