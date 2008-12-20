////////////////////////////////////////////////////////////////////////////////////
///
///  \file createeventrequest.h
///  \brief This file contains the message Create Event.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 March 2008
///  <br>Last Modified: 8 March 2008
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
#ifndef __JAUS_CREATE_EVENT_REQUEST__H
#define __JAUS_CREATE_EVENT_REQUEST__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class CreateEventRequest
    ///  \brief Message to create a JAUS event.
    ///
    ///  This message is used to set up an event.  Required fields are presence
    ///  vector, request ID, message code, and event type.  The presence vector
    ///  provides a mapping for the other optional fields.
    ///
    ///  Before using any of the Get methods of this class, verify that the
    ///  data exists by checking the presence vector or using the IsFieldPresent
    ///  or AreFieldsPresent methods.  Failure to do this will result in garbage
    ///  data.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL CreateEventRequest : public Message
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
                EventBoundary  = 0x01,
                LimitDataField = 0x02,
                LowerLimit     = 0x04,
                UpperLimit     = 0x08,
                State          = 0x10,
                RequestedMinimumPeriodicRate = 0x20,
                RequestedPeriodicRate        = 0x40,
                QueryMessageBody             = 0x80
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
                EventBoundary = 0,
                LimitDataField,
                LowerLimit,
                UpperLimit,
                State,
                RequestedMinimumPeriodicRate,
                RequestedPeriodicRate,
                QueryMessageBody
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        /// 
        ///  \enum EventType
        ///  \brief Simple class for storing information about types of events.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum EventType
        {
            Periodic      = 0,   
            EveryChange,
            FirstChange,
            FirstChangeBoundaries,
            PeriodicWithoutReplacement,
            OneTime
        };
        ////////////////////////////////////////////////////////////////////////////////////
        /// 
        ///  \enum BoundaryType
        ///  \brief Simple class for storing information about types of boundaries used
        ///         for event generation.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum BoundaryType
        {
            Equal = 0,
            NotEqual,
            InsideInclusive,
            InsideExclusive,
            OutsideInclusive,
            OutsideExclusive,
            GreaterThanOrEqual,
            GreaterThan,
            LessThanOrEqual,
            LessThan                
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
        CreateEventRequest();
        CreateEventRequest(const CreateEventRequest& msg);
        virtual ~CreateEventRequest();
        
        int SetRequestID(const Byte id);
        int SetMessageCode(const UShort code);
        int SetEventType(const CreateEventRequest::EventType type);
        int SetEventBoundary(const CreateEventRequest::BoundaryType boundary);
        int SetLimitDataField(const Byte field);
        int SetLowerLimit(const VarType& limit);
        int SetUpperLimit(const VarType& limit);
        int SetState(const VarType& state);
        int SetMinimumPeriodicUpdateRate(const double rate);
        int SetRequestedPeriodicUpdateRate(const double rate);
        int SetQueryMessage(const Message* msg);
        int SetQueryMessageBody(const Stream& msg);
        
        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetRequestID() const { return mRequestID; }
        UShort GetMessageCode() const { return mMessageCode; }
        Byte GetEventType() const { return mType; }
        Byte GetEventBoundary() const { return mEventBoundary; }
        Byte GetLimitDataField() const { return mLimitDataField; }
        VarType GetLowerLimit() const { return mLowerLimit; }
        VarType GetUpperLimit() const { return mUpperLimit; }
        VarType GetState() const { return mState; }
        double GetMinimumPeriodicRate() const { return mMinimumPeriodicRate; }
        double GetRequestedPeriodicUpdateRate() const { return mRequestedPeriodicRate; }
        Stream* GetQueryMessageBody() const;
        Message* GetQueryMessage() const;

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
        virtual Message* Clone() const;
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        CreateEventRequest& operator=(const CreateEventRequest& msg);
    protected:
        Byte mPresenceVector;           ///< Presence vector for message.
        Byte mRequestID;                ///< Local request ID for use in event confirmation.
        UShort mMessageCode;            ///< Message code of the event message recieving component generates.
        Byte mType;                     ///< Type of event to generate.
        Byte mEventBoundary;            ///< Event boundary condition.
        Byte mLimitDataField;           ///< Field from event message to base trigger limit on.
        VarType mLowerLimit;            ///< Lower limit data for trigger (Variable Type).
        VarType mUpperLimit;            ///< Upper limit data for trigger (Variable Type).
        VarType mState;                 ///< Trigger value used for Equal Event Boundary type.
        double mMinimumPeriodicRate;    ///< Minimum periodic update rate.
        double mRequestedPeriodicRate;  ///< Periodic update rate desired.
        Stream* mpQueryMessageBody;     ///< Query message body/data.
        Message* mpQueryMessage;        ///< Query message structure.
    };
}

#endif
/* End of File */
