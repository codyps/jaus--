////////////////////////////////////////////////////////////////////////////////////
///
///  \file rejecteventrequest.h
///  \brief This file contains the message Reject Event Request.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 March 2008
///  <br>Last Modified: 10 March 2008
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
#ifndef __JAUS_REJECT_EVENT_REQUEST__H
#define __JAUS_REJECT_EVENT_REQUEST__H

#include "jaus/messages/message.h"
#include <string>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class RejectEventRequest
    ///  \brief Message to reject a JAUS event.
    ///
    ///  The Reject Event Request message is used to reject an Event creation, 
    ///  update, or cancellation.  Field 2 represents the Request ID from the
    ///  Create, Update, or Cancel message that initiated this message.  The
    ///  Rerquest ID's scope is local to the requesting client only.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL RejectEventRequest : public Message
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
                ErrorMessage = 0x01,
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
                ErrorMessage = 0,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum ResponseCode
        ///   \brief Enumeration of the different responses that this message can
        ///          have.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum ResponseCode
        {
            PeriodicEventsNotSupported = 0,
            ChangeBasedEventsNotSupported,
            ConnectionRefused,
            InvalidEventSetup,
            MessageNotSupported,
            InvalidEventID
        };
        RejectEventRequest();
        RejectEventRequest(const RejectEventRequest& msg);
        virtual ~RejectEventRequest();
        
        int SetRequestID(const Byte id);
        int SetResponseCode(const RejectEventRequest::ResponseCode code);
        int SetErrorMessage(const std::string& msg);

        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetRequestID() const { return mRequestID; }
        Byte GetResponseCode() const { return mResponseCode; }
        const std::string* GetErrorMessage() const { return &mErrorMessage; }

        void ClearField(const VectorBit::Bits bit);
        void ClearFields(const Byte mask);
        bool IsFieldPresent(const VectorBit::Bits bit) const 
                        { return BitVector::IsBitSet(mPresenceVector, bit); }
        bool AreFieldsPresent(const Byte mask) const
                        { return (mPresenceVector&mask) == mask ? true : false; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual int RunTestCase() const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new RejectEventRequest(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        RejectEventRequest& operator=(const RejectEventRequest& msg);
    protected:
        Byte mPresenceVector;           ///< Presence vector for message.
        Byte mRequestID;                ///< Local request ID for use in event rejection.
        Byte mResponseCode;             ///< Response type in event rejection.
        std::string mErrorMessage;      ///< Error message associated with rejection.
    };
}

#endif
/* End of File */
