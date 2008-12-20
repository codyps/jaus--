////////////////////////////////////////////////////////////////////////////////////
///
///  \file confirmeventrequest.h
///  \brief This file contains the message Confirm Event Request.
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
#ifndef __JAUS_CONFIRM_EVENT_REQUEST__H
#define __JAUS_CONFIRM_EVENT_REQUEST__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class ConfirmEventRequest
    ///  \brief Message to confirm a JAUS event.
    ///
    ///  The Confirm Event message is used to confirm an Event has been created/updated
    ///  or cancelled.  Field 2 represents the request ID from the Create, Update or
    ///  Cancel message that initiated this message.  The Request ID's scope is local to
    ///  the requesting client only.  Field 4, Event ID, is a globally unique ID that
    ///  is established for the event.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ConfirmEventRequest : public Message
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
                ConfirmedPeriodicUpdateRate = 0x01,
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
                ConfirmedPeriodicUpdateRate = 0,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        /// 
        ///  \class Limits
        ///  \brief Class defining the limits of values used in message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxPeriodicRate;    ///<  Maximum value for periodic rate.
            const static double MinPeriodicRate;    ///<  Minimum value for periodic rate.
        };
        ConfirmEventRequest();
        ConfirmEventRequest(const ConfirmEventRequest& msg);
        virtual ~ConfirmEventRequest();
        
        int SetRequestID(const Byte id);
        int SetMessageCode(const UShort code);
        int SetEventID(const Byte id);
        int SetConfirmedPeriodicUpdateRate(const double rate);
        int SetResponseCode(const Byte responseCode); // Deprecate in v4.0
        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetRequestID() const { return mRequestID; }
        UShort GetMessageCode() const { return mMessageCode; }
        Byte GetEventID() const { return mEventID; }
        Byte GetResponseCode() const { return mResponseCode; } // Deprecate in v4.0
        double GetConfirmedPeriodicUpdateRate() const { return mConfirmedPeriodicUpdateRate; }

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
        virtual Message* Clone() const { return new ConfirmEventRequest(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ConfirmEventRequest& operator=(const ConfirmEventRequest& msg);
    protected:
        Byte mPresenceVector;           ///< Presence vector for message.
        Byte mRequestID;                ///< Local request ID for use in event confirmation.
        UShort mMessageCode;            ///< Message code of the event message recieving component generates.
        Byte mEventID;                  ///< Event ID number.  
        Byte mResponseCode;             ///< Event response code (deprecated in v4.0).
        double mConfirmedPeriodicUpdateRate; ///<  Confirmed periodic update rate.
    };
}

#endif
/* End of File */
