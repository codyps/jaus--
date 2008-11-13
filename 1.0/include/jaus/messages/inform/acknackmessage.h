////////////////////////////////////////////////////////////////////////////////////
///
///  \file acknackmessage.h
///  \brief Message structure for Acknowledge/Negative Acknowledge messages.
///
///  Ack/Nack messages have no message body data, have the AckNack flag
///  set to either Header::AckNack::Ack or Header::AckNack::Nack and
///  have an identical message code to the original message 
///  received which was requesting Ack/Nack.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 16 March 2008
///  <br>Created: 16 March 2008
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
#ifndef __JAUS_ACK_NACK_MESSAGE
#define __JAUS_ACK_NACK_MESSAGE

#include "informcodes.h"
#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class AckNackMessage
    ///   \brief Simple message structure for representing Ack/Nack responses to
    ///   messages.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class AckNackMessage : public Message
    {
    public:
        AckNackMessage(const UShort messageCode) : Message(messageCode) {}
        AckNackMessage(const Header& header) : Message(header.mCommandCode) { CopyFromHeader(header); }
        AckNackMessage(const AckNackMessage& acknack) : Message(acknack.GetCommandCode()) { *this = acknack; }
        ~AckNackMessage() {}
        int SetAckNack(const UShort ack)
        {
            if(ack == Header::AckNack::Ack || ack == Header::AckNack::Nack)
            {
                return Message::SetAckNack(ack);
            }
            return JAUS_FAILURE;
        }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const { return 0; }
        virtual int ReadMessageBody(const Stream& msg, const UShort version) { return 0; }
        virtual Message* Clone() const { return new AckNackMessage(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody() {}
        AckNackMessage &operator=(const AckNackMessage &msg) { CopyHeaderData(&msg); return *this; }
    };
}

#endif

/*  End of File */
