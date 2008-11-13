////////////////////////////////////////////////////////////////////////////////////
///
///  \file terminateserviceconnection.h
///  \brief This file contains data structures for the JAUS_TERMINATE_SERVICE_CONNECTION
///  message belonging to the CORE set of JAUS messages.
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
#ifndef __JAUS_TERMINATE_SERVICE_CONNECTION_H
#define __JAUS_TERMINATE_SERVICE_CONNECTION_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class TerminateServiceConnection
    ///   \brief Ends a service connection. Part of the core set of messages in JAUS.    
    ///   
    ///   This SC creator, or the node, uses this message to notify the
    ///   provider that the service provided on this connection shall be
    ///   terminated.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL TerminateServiceConnection : public Message
    {
    public:
        TerminateServiceConnection();
        TerminateServiceConnection(const TerminateServiceConnection &msg);
        ~TerminateServiceConnection();
        UShort GetMessageCode() const { return mScCommandCode; }
        Byte GetInstanceID() const { return mInstanceID; }
        int SetMessageCode(const UShort code);
        int SetInstanceID(const Byte id);
        virtual int WriteMessageBody(Stream &msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream &msg, const UShort version);
        virtual void ClearMessageBody() { mScCommandCode = 0; mInstanceID = 0; }
        virtual Message* Clone() const { return new TerminateServiceConnection(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        TerminateServiceConnection &operator=(const TerminateServiceConnection &msg);
    protected:
        UShort mScCommandCode;         ///<  Command code for SC.
        Byte mInstanceID;              ///<  SC instance ID.
    };
}

#endif

#endif 

/*  End of File */
