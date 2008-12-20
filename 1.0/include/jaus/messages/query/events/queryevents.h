////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryevents.h
///  \brief This file contains the message Query Events.
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
#ifndef __JAUS_QUERY_EVENTS__H
#define __JAUS_QUERY_EVENTS__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class QueryEvents
    ///  \brief Message to query JAUS events.
    ///
    ///  The Query Events message is used to request details on events.
    ///
    ///  Field 2 indicates the message code in question.  If left out, all message
    ///  codes should be returned.  Field 3 indicates the event type to report on.
    ///  If left out, all event types should be considered.  Field 4 indicates a
    ///  specific Event ID.  If left out, all event IDs should be considered. 
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryEvents : public Message
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
                MessageCode  = 0x01,
                EventType    = 0x02,
                EventID      = 0x04
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
                MessageCode = 0,
                EventType,
                EventID
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum EventType
        ///   \brief Enumeration of the different event typs to query.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum EventType
        {
            Periodic = 0,
            EveryChange,
            FirstChange
        };
        QueryEvents();
        QueryEvents(const QueryEvents& msg);
        virtual ~QueryEvents();
        
        int SetMessageCode(const UShort code);
        int SetEventType(const QueryEvents::EventType type);
        int SetEventID(const Byte id);
       
        Byte GetPresenceVector() const { return mPresenceVector; }
        UShort GetMessageCode() const { return mMessageCode; }
        Byte GetEventID() const { return mEventID; }
        Byte GetEventType() const { return mEventType; }

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
        virtual Message* Clone() const { return new QueryEvents(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        QueryEvents& operator=(const QueryEvents& msg);
    protected:
        Byte mPresenceVector;           ///< Presence vector for message.
        Byte mEventType;                ///< Event types to query.
        UShort mMessageCode;            ///< Message code of the event message recieving component generates.
        Byte mEventID;                  ///< Event ID number.  
    };
}

#endif
/*  End of File */
