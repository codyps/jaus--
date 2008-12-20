////////////////////////////////////////////////////////////////////////////////////
///
///  \file querycameracapabilities.cpp
///  \brief This file contains the message structure for Query Camera Pose.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 14 August 2008
///  <br>Last Modified: 14 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cdhollan@ist.ucf.edu
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
#ifndef __JAUS_QUERY_CAMERA_CAPABILITIES_H
#define __JAUS_QUERY_CAMERA_CAPABILITIES_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class QueryCameraCapabilities
    ///   \brief Used to query and image from a Visual Sensor Component.   
    ///   
    ///   This message shall cause the receiving component to reply to the requestor 
    ///      with a Code 4805h: Report Camera Capabilities message to report the 
    ///   capabilities of the selected camera.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryCameraCapabilities : public Message
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
                Description             = 0x01,
                MaxHorizontalFOV        = 0x02,
                MinHorizontalFOV        = 0x04,
                MaxVerticalFOV          = 0x08,
                MinVerticalFOV          = 0x10,
                MaxHorizontalResolution = 0x20,
                MinHorizontalResolution = 0x40,
                MaxVerticalResolution   = 0x80,
                MinVerticalResolution   = 0x100,
                MinFrameRate            = 0x200,
                MaxFrameRate            = 0x400,
                MinShutter              = 0x800,
                MaxShutter              = 0x1000,
                ImageryControl          = 0x2000,
                AudioControl            = 0x4000,
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
                Description             = 0,
                MaxHorizontalFOV        = 1,
                MinHorizontalFOV        = 2,
                MaxVerticalFOV          = 3,
                MinVerticalFOV          = 4,
                MaxHorizontalResolution = 5,
                MinHorizontalResolution = 6,
                MaxVerticalResolution   = 7,
                MinVerticalResolution   = 8,
                MinFrameRate            = 9,
                MaxFrameRate            = 10,
                MinShutter              = 11,
                MaxShutter              = 12,
                ImageryControl          = 13,
                AudioControl            = 14,
            };
        };

        QueryCameraCapabilities();
        QueryCameraCapabilities(const QueryCameraCapabilities &msg);
        virtual ~QueryCameraCapabilities();

        int SetPresenceVector(const UShort pv);
        int SetCameraID(const Byte id);
        UShort GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const { return mCameraID; }
        
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new QueryCameraCapabilities(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        QueryCameraCapabilities &operator=(const QueryCameraCapabilities &msg);
        virtual int RunTestCase() const;
    protected:
        UShort mPresenceVector; ///<  Bit vector for items being queried.
        Byte   mCameraID;       ///<  ID of camera being queried 
    };
}

#endif

#endif 

/*  End of File */
