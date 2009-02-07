////////////////////////////////////////////////////////////////////////////////////
///
///  \file receipt.h
///  \brief Used to store information about a message that has been sent
///  and what the expect response to the message should be.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 16 March 2007
///  <br>Last Modified: 30 July 2007
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
#ifndef __JAUS_RECEIPT__H
#define __JAUS_RECEIPT__H

#include "jaus/messages/time.h"
#include "jaus/messages/message.h"
#include "jaus/components/cmplibdll.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Receipt
    ///   \brief Stores information about a message that is being sent and
    ///   its expected response.
    ///
    ///   This class stores information such as if an ACK/NACK response was received,
    ///   how long it took to get a response, and if the desired response was
    ///   received its stream and de-serialized message (if supported).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL Receipt
    {
        friend class ConnectionHandler;
        friend class Component;
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Status
        ///   \brief Status values of receipt including Success (received responses) or
        ///   Pending, Negative Acknowledge received, or Timeout.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Status
        {
            Success = 1,
            Pending,
            Nack,
            Timeout
        };
        Receipt();
        virtual ~Receipt();
        void Clear(const bool del = true);
        UInt GetResponseTime() const;
        UInt GetSendCount() const;
        Receipt::Status GetStatus() const;
        UShort GetSentMessageCode() const;
        UShort GetResponseMessageCode() const;
        bool ReceivedAcknowledge() const;
        const Stream* GetResponseStream() const;
        const Message* GetResponseMessage() const;
    protected:
        UInt mSendTimeMs;                       ///<  UTC time message was first sent.
        UInt mUpdateTimeMs;                     ///<  Last time the message was updated.
        UInt mTimeoutTimeMs;                    ///<  Timeout time in Ms.=
        UInt mSendCount;                        ///<  Number of times message has been sent.
        bool mAckNackFlag;                      ///<  Ack/Nack recieved?
        UShort mSentAckNack;                    ///<  Ack/Nack value of sent message.
        UShort mReceivedAckNack;                ///<  Ack/Nack value of received message.
        UShort mSentMessageCode;                ///<  Message code of the message sent.
        Address mDestinationID;                 ///<  Destination address of sent message.
        Receipt::Status mStatus;                ///<  Current status of the receipt.
        std::set<UShort> mResponseCodes;        ///<  Possible responses to the message.
        Stream mSendStream;                     ///<  Serialized message data sent.
        Stream* mpResponseStream;               ///<  Serialized response message received.
        Message* mpResponseMessage;             ///<  Pointer to response message structure.
    };
}

#endif

#endif 

/*  End of File */
