////////////////////////////////////////////////////////////////////////////////////
///
///  \file querywrencheffort.h
///  \brief This file contains the message structure for Query Wrench Effort.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 1 May 2008
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
#ifndef __JAUS_QUERY_WRENCH_EFFORT_H
#define __JAUS_QUERY_WRENCH_EFFORT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class QueryWrenchEffort
    ///   \brief This message shall cause the receiving component to reply to the
    ///   requestor with a Report Wrench Effort message.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryWrenchEffort : public Message
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
                PropulsiveLinearEffortX = 0x0001,
                PropulsiveLinearEffortY = 0x0002,
                PropulsiveLinearEffortZ = 0x0004,
                PropulsiveRotationalEffortX = 0x0008,
                PropulsiveRotationalEffortY = 0x0010,
                PropulsiveRotationalEffortZ = 0x0020,
                ResistiveLinearEffortX = 0x0040,
                ResistiveLinearEffortY = 0x0080,
                ResistiveLinearEffortZ = 0x0100,
                ResistiveRotationalEffortX = 0x0200,
                ResistiveRotationalEffortY = 0x0400,
                ResistiveRotationalEffortZ = 0x0800,
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
                PropulsiveLinearEffortX = 0,
                PropulsiveLinearEffortY,
                PropulsiveLinearEffortZ,
                PropulsiveRotationalEffortX,
                PropulsiveRotationalEffortY,
                PropulsiveRotationalEffortZ,
                ResistiveLinearEffortX,
                ResistiveLinearEffortY,
                ResistiveLinearEffortZ,
                ResistiveRotationalEffortX,
                ResistiveRotationalEffortY,
                ResistiveRotationalEffortZ,
            };
        };
        QueryWrenchEffort();
        QueryWrenchEffort(const QueryWrenchEffort &msg);
        ~QueryWrenchEffort();
        int SetPresenceVector(const UShort pv);
        UShort GetPresenceVector() const { return mPresenceVector; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new QueryWrenchEffort(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        QueryWrenchEffort &operator=(const QueryWrenchEffort &msg);
    protected:
        UShort mPresenceVector; ///<  Presence vector associated with message.
    };
}

#endif

#endif 

/*  End of File */
