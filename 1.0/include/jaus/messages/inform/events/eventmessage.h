////////////////////////////////////////////////////////////////////////////////////
///
///  \file eventmessage.h
///  \brief This file contains the Event message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 March 2008
///  <br>Last Modified: 25 March 2008
///  <br>Copyright (c) 2008
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
#ifndef __JAUS_REPORT_EVENT_MESSAGE__H
#define __JAUS_REPORT_EVENT_MESSAGE__H

#include "jaus/messages/message.h"


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class EventMessage
    ///  \brief Message to generate a JAUS event.
    ///
    ///  The EventMessage message is sent when an event is triggered.  It includes the
    ///  EventMessage ID and a sequence number to allow the client to keep track of
    ///  event processing.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL EventMessage : public Message
    {
    public:
        EventMessage();
        EventMessage(const EventMessage& msg);
        virtual ~EventMessage();
        int SetEventID(const Byte id);
        int SetEventMessageCode(const UShort mcode);
        int SetEventSequenceNumber(const Byte sn);
        int SetEventMessage(const Message* msg, const bool clone = true);
        int SetEventMessageBody(const Stream& msg);
        Byte GetEventID() const { return mEventID; }
        UShort GetEventMessageCode() const { return mEventMessageCode; }
        Byte GetEventSequenceNumber() const { return mEventSequenceNumber; }
        const Message* GetEventMessage() const { return mpMessage; }
        const Stream* GetEventMessageBody() const { return &mMessageBody; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual int RunTestCase() const;
        virtual Message* Clone() const;
        virtual void Print() const; 
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        EventMessage& operator=(const EventMessage& msg);
    protected:
        Byte mEventID;              ///<  EventMessage ID (unique identifier).
        UShort mEventMessageCode;   ///<  Message code of the enclosed event message.
        Byte mEventSequenceNumber;  ///<  Message sequence number (sequention count).
        Stream mMessageBody;        ///<  Message body data.
        Message* mpMessage;         ///<  Pointer to de-serialized message body data.
    };
}

#endif
/*  End of File */
