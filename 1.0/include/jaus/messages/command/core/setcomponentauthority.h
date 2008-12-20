////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcomponentauthority.h
///  \brief This file contains data structures for the SET_COMPONENT_AUTHORITY message
///  belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 20 July 2007
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
#ifndef __JAUS_SET_COMPONENT_AUTHORITY_H
#define __JAUS_SET_COMPONENT_AUTHORITY_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetComponentAuthority
    ///   \brief Sets the command authority of the receiving component. 
    ///   Part of the core set of messages in JAUS.    
    ///   
    ///   This message shall set the command authority of the receiving 
    ///   component.  The authority bits range in value from 0 to 255, with
    ///   255 being the highest.
    ///
    ///   The System Commander reserves the authority of 255.  All Subsystem
    ///   Commander components default to the authority of 8 and can be set 
    ///   higher by the System Commander.  All other components default to
    ///   the authority of 0 and be changed by their respective Subsystem
    ///   Command component or by the system designer.
    ///
    ////////////////////////////////////////////////////////////////////////////////////    
    class JAUS_MSG_DLL SetComponentAuthority : public Message
    {
    public:
        SetComponentAuthority();
        SetComponentAuthority(const SetComponentAuthority &msg);
        virtual ~SetComponentAuthority();
        Byte GetAuthorityCode() const;
        int SetAuthorityCode(const Byte code);
        virtual int RunTestCase() const;
        virtual int WriteMessageBody(Stream &msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream &msg, const UShort version);
        virtual void Print() const;
        virtual void ClearMessageBody() { mAuthorityCode = 0; }
        virtual Message* Clone() const { return new SetComponentAuthority(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetComponentAuthority &operator=(const SetComponentAuthority &msg);
    protected:
        Byte mAuthorityCode;       ///<  Authority code for command authority [0-255]. (Field #1).
    };
}

#endif

#endif 

/*  End of File */
