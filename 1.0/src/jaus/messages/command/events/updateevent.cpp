////////////////////////////////////////////////////////////////////////////////////
///
///  \file updateevent.cpp
///  \brief This file contains the message Update Event.
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
#include "jaus/messages/command/events/updateevent.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/messagecreator.h"
#include <math.h>
using namespace Jaus;

const double UpdateEvent::Limits::MaxPeriodicRate = 1092.0;
const double UpdateEvent::Limits::MinPeriodicRate = 0.0;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
UpdateEvent::UpdateEvent() : Message(JAUS_UPDATE_EVENT)
{
    mPresenceVector = 0;
    mEventID = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mType = 0;
    mEventBoundary = 0;
    mLimitDataField = 0;
    mMinimumPeriodicRate = 0;
    mRequestedPeriodicRate = 0;
    mpQueryMessageBody = NULL;
    mpQueryMessage = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
UpdateEvent::UpdateEvent(const UpdateEvent& msg) : Message(JAUS_UPDATE_EVENT)
{
    mPresenceVector = 0;
    mEventID = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mType = 0;
    mEventBoundary = 0;
    mLimitDataField = 0;
    mMinimumPeriodicRate = 0;
    mRequestedPeriodicRate = 0;
    mpQueryMessageBody = NULL;
    mpQueryMessage = NULL;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
UpdateEvent::~UpdateEvent()
{
    if(mpQueryMessageBody)
    {
        delete mpQueryMessageBody;
    }
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param id Local request ID for use in confirm event.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetRequestID(const Byte id)
{
    mRequestID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param code Message code of the event message.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetMessageCode(const UShort code)
{
    mMessageCode = code;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param type Type of event.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetEventType(const UpdateEvent::EventType type)
{
    mType = (Byte)(type);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param boundary Boundary condition on event trigger.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetEventBoundary(const UpdateEvent::BoundaryType boundary)
{
    mEventBoundary = (Byte)(boundary);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the event being updated.
///
///   \param id Unique identifier of existing event to update.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetEventID(const Byte id)
{
    mEventID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param field The field from report message to base trigger limit on.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetLimitDataField(const Byte field)
{
    mLimitDataField = field;
    BitVector::SetBit(mPresenceVector, VectorBit::LimitDataField, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param limit The lower limit value that the limit data field should meet
///                for event triggering.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetLowerLimit(const VarType& limit)
{
    mLowerLimit = limit;
    BitVector::SetBit(mPresenceVector, VectorBit::LowerLimit, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param limit The upper limit value that the limit data field should meet
///                for event triggering.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetUpperLimit(const VarType& limit)
{
    mUpperLimit = limit;
    BitVector::SetBit(mPresenceVector, VectorBit::UpperLimit, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param state Trigger value to use for OnEqual event boundary.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetState(const VarType& state)
{
    mState = state;
    BitVector::SetBit(mPresenceVector, VectorBit::State, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param rate Requested minimum periodic rate.  For Periodic events, used to
///               throttle messages if the value is not changing.  [0,1092].
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetMinimumPeriodicUpdateRate(const double rate)
{
    if(rate >= UpdateEvent::Limits::MinPeriodicRate && rate <= UpdateEvent::Limits::MaxPeriodicRate)
    {
        mMinimumPeriodicRate = rate;
        BitVector::SetBit(mPresenceVector, VectorBit::RequestedMinimumPeriodicRate, true);
        return JAUS_OK;
    }
    BitVector::SetBit(mPresenceVector, VectorBit::RequestedMinimumPeriodicRate, false);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param rate Desired update periodic rate.  For Periodic events, used to
///               throttle messages if the value is not changing.  [0,1092].
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetRequestedPeriodicUpdateRate(const double rate)
{
    if(rate >= UpdateEvent::Limits::MinPeriodicRate && rate <= UpdateEvent::Limits::MaxPeriodicRate)
    {
        mRequestedPeriodicRate = rate;
        BitVector::SetBit(mPresenceVector, VectorBit::RequestedPeriodicRate, true);
        return JAUS_OK;
    }
    BitVector::SetBit(mPresenceVector, VectorBit::RequestedPeriodicRate, false);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param msg Pointer to a query message to create message body data from.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetQueryMessage(const Message* msg)
{
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
        mpQueryMessage = NULL;
    }
    if(msg)
    {
        if(mpQueryMessageBody == NULL)
        {
            mpQueryMessageBody = new Stream();
        }
        if( msg->WriteMessageBody(*mpQueryMessageBody, JAUS_DEFAULT_VERSION) > 0 )
        {
            // Clone the message.
            mpQueryMessage = msg->Clone();
            BitVector::SetBit(mPresenceVector, VectorBit::QueryMessageBody, true);
            return JAUS_OK;
        }
    }
    BitVector::SetBit(mPresenceVector, VectorBit::QueryMessageBody, false);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param msg The query message data to use to create the event message.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::SetQueryMessageBody(const Stream& msg)
{
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
        mpQueryMessage = NULL;
    }
    if(mpQueryMessageBody)
    {
        delete mpQueryMessageBody;
        mpQueryMessageBody = NULL;
    }
    if(msg.Length() > 0)
    {
        mpQueryMessageBody = msg.Clone();
        BitVector::SetBit(mPresenceVector, VectorBit::QueryMessageBody, true);
        return JAUS_OK;
    }

    BitVector::SetBit(mPresenceVector, VectorBit::QueryMessageBody, false);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to message body data if present, otherwise NULL.
///
////////////////////////////////////////////////////////////////////////////////////
Stream* UpdateEvent::GetQueryMessageBody() const
{
    if( mpQueryMessageBody &&
        BitVector::IsBitSet(mPresenceVector, VectorBit::QueryMessageBody) )
    {
        return mpQueryMessageBody;
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to Message structure if Query Message Body data is present,
///           and able to be read by MessageCreator.  Otherwise NULL.
///
////////////////////////////////////////////////////////////////////////////////////
Message* UpdateEvent::GetQueryMessage() const
{
    if( mpQueryMessageBody &&
        BitVector::IsBitSet(mPresenceVector, VectorBit::QueryMessageBody) )
    {
        if( mpQueryMessage == NULL )
        {
            Message** msg = (Message **)&mpQueryMessage;
            *msg = MessageCreator::CreateMessage( MessageCreator::GetInformQueryType(mMessageCode) );
            if( *msg )
            {
                if((*msg)->ReadMessageBody(*mpQueryMessageBody, mVersion) > 0)
                {
                    return *msg;
                }
                else
                {
                    delete *msg;
                    *msg = NULL;
                    return NULL;
                }
            }
        }
        return (Message*)mpQueryMessage;
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the field specified from presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
void UpdateEvent::ClearField(const VectorBit::Bits bit)
{
    BitVector::SetBit(mPresenceVector, bit, false);
    if( bit == VectorBit::QueryMessageBody )
    {
        if(mpQueryMessage)
        {
            delete mpQueryMessage;
        }
        if(mpQueryMessageBody)
        {
            delete mpQueryMessageBody;
        }
        mpQueryMessage = NULL;
        mpQueryMessageBody = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears multiple bits in the presence vector using a bit mask.
///
///   The mask can be a bitwise OR of multiple VectorMask::Masks to clear
///   multiple values at once.
///
////////////////////////////////////////////////////////////////////////////////////
void UpdateEvent::ClearFields(const Byte mask)
{
    mPresenceVector &= ~mask;
    if( BitVector::IsBitSet(mPresenceVector, VectorBit::QueryMessageBody) == false )
    {
        if(mpQueryMessage)
        {
            delete mpQueryMessage;
        }
        if(mpQueryMessageBody)
        {
            delete mpQueryMessageBody;
        }
        mpQueryMessage = NULL;
        mpQueryMessageBody = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message body data to the stream.
///
///   \param msg The stream to write to.
///   \param version The desired version of the message to write.
///
///   \return Number of bytes written on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::WriteMessageBody(Stream& msg, 
                                  const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected = JAUS_BYTE_SIZE*3 + JAUS_USHORT_SIZE;
        written += msg.Write(mPresenceVector);
        written += msg.Write(mRequestID);
        written += msg.Write(mMessageCode);
        written += msg.Write(mType);

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventBoundary))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mEventBoundary);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::LimitDataField))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mLimitDataField);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::LowerLimit))
        {
            expected += JAUS_BYTE_SIZE + mLowerLimit.Size();
            written += msg.Write(mLowerLimit);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::UpperLimit))
        {
            expected += JAUS_BYTE_SIZE + mUpperLimit.Size();
            written += msg.Write(mUpperLimit);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::State))
        {
            expected += JAUS_BYTE_SIZE + mState.Size();
            written += msg.Write(mState);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::RequestedMinimumPeriodicRate))
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMinimumPeriodicRate, 
                                 Limits::MaxPeriodicRate, 
                                 Limits::MinPeriodicRate, 
                                 ScaledInteger::UShort);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::RequestedPeriodicRate))
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mRequestedPeriodicRate, 
                                 Limits::MaxPeriodicRate, 
                                 Limits::MinPeriodicRate, 
                                 ScaledInteger::UShort);
        }
        expected += JAUS_BYTE_SIZE;
        written += msg.Write(mEventID);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::QueryMessageBody))
        {
            if(mpQueryMessageBody == NULL ||
                mpQueryMessageBody && mpQueryMessageBody->Length() == 0)
            {
                SetJausError(ErrorCodes::InvalidValue); return -1;
            }

            expected += JAUS_UINT_SIZE + mpQueryMessageBody->Length();
            written += msg.Write((UInt)mpQueryMessageBody->Length());
            written += msg.Write(*mpQueryMessageBody);
        }
        if( expected == written ) 
        {
            return written;
        }
        else
        {
            SetJausError(ErrorCodes::WriteFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message body data from the stream.
///
///   \param msg The stream to read from.
///   \param version The desired version of the message to read.
///
///   \return Number of bytes read on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    ClearMessageBody();
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        
        expected = JAUS_BYTE_SIZE*3 + JAUS_USHORT_SIZE;
        read += msg.Read(mPresenceVector);
        read += msg.Read(mRequestID);
        read += msg.Read(mMessageCode);
        read += msg.Read(mType);
        
        if( expected == read )
        {
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventBoundary))
            {
                expected += JAUS_BYTE_SIZE;
                read += msg.Read(mEventBoundary);
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::LimitDataField))
            {
                expected += JAUS_BYTE_SIZE;
                read += msg.Read(mLimitDataField);
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::LowerLimit))
            {
                expected += JAUS_BYTE_SIZE;
                if(msg.Read(mLowerLimit) > 0)
                {
                    expected += mLowerLimit.Size();
                    read += JAUS_BYTE_SIZE + mLowerLimit.Size();
                }
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::UpperLimit))
            {
                expected += JAUS_BYTE_SIZE;
                if(msg.Read(mUpperLimit) > 0)
                {
                    expected += mUpperLimit.Size();
                    read += JAUS_BYTE_SIZE + mUpperLimit.Size();
                }             
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::State))
            {
                expected += JAUS_BYTE_SIZE;
                if(msg.Read(mState) > 0)
                {
                    expected += mState.Size();
                    read += JAUS_BYTE_SIZE + mState.Size();
                }
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::RequestedMinimumPeriodicRate))
            {
                expected += JAUS_USHORT_SIZE;
                read += msg.Read(mMinimumPeriodicRate, 
                                 Limits::MaxPeriodicRate, 
                                 Limits::MinPeriodicRate, 
                                 ScaledInteger::UShort);
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::RequestedPeriodicRate))
            {
                expected += JAUS_USHORT_SIZE;
                read += msg.Read(mRequestedPeriodicRate, 
                                 Limits::MaxPeriodicRate, 
                                 Limits::MinPeriodicRate, 
                                 ScaledInteger::UShort);
            }
            
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mEventID);

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::QueryMessageBody))
            {
                UInt len = 0;
                expected += JAUS_UINT_SIZE;
                read += msg.Read(len);
                if(len > 0)
                {
                    mpQueryMessageBody = new Stream();
                    expected += len;
                    read += msg.Read(*mpQueryMessageBody, len);
                    if( expected == read )
                    {
                        mpQueryMessage = MessageCreator::CreateQueryMessage(MessageCreator::GetInformQueryType(mMessageCode));
                        if( mpQueryMessage )
                        {
                            if(mpQueryMessage->ReadMessageBody(*mpQueryMessageBody, version) <= 0)
                            {
                                delete mpQueryMessage;
                                mpQueryMessage = NULL;
                            }
                        }
                    }
                }
            }
            if(expected == read)
            {
                return expected;
            }
        }
        
        SetJausError(ErrorCodes::ReadFailure); return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs a test case to validate the message class.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int UpdateEvent::RunTestCase() const
{
    int result = JAUS_OK;
    Stream packet;
    UpdateEvent msg1, msg2;

    msg1.SetEventType(UpdateEvent::Periodic);
    msg1.SetRequestID(1);
    msg1.SetMinimumPeriodicUpdateRate(100);
    msg1.SetRequestedPeriodicUpdateRate(200);
    msg1.SetMessageCode(JAUS_REPORT_TIME);
    // Test periodic event
    if( msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 )
    {
        if( msg1.GetPresenceVector() == msg2.GetPresenceVector() &&
            msg1.GetEventType() == msg2.GetEventType() &&
            fabs(msg1.GetMinimumPeriodicRate() - msg2.GetMinimumPeriodicRate()) < .01 &&
            fabs(msg1.GetRequestedPeriodicUpdateRate() - msg2.GetRequestedPeriodicUpdateRate()) < .01 &&
            msg1.GetRequestID() == msg2.GetRequestID())
        {
            Stream query;
            // Test on change event
            msg1.ClearMessageBody();
            msg1.SetRequestID(2);
            msg1.SetEventType(UpdateEvent::EveryChange);
            msg1.SetMessageCode(JAUS_REPORT_TIME);            
            query.WriteByte( 0x01 ); // only get timestamp.
            msg1.SetQueryMessageBody(query);

            if(msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
                msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0)
            {
                Byte t = VectorMask::UpperLimit |
                                          VectorMask::LowerLimit |
                                          VectorMask::State |
                                          VectorMask::EventBoundary;
                if( msg2.GetPresenceVector() == msg1.GetPresenceVector() &&
                    msg2.GetQueryMessage() &&
                    msg2.GetQueryMessage()->GetCommandCode() == JAUS_QUERY_TIME &&
                    msg2.GetEventType() == UpdateEvent::EveryChange &&
                    msg2.GetRequestID() == 2 &&
                    msg2.AreFieldsPresent(VectorMask::QueryMessageBody) &&
                    msg2.AreFieldsPresent(t) == false)
                {
                    // Now do on limit field.
                    msg1.ClearMessageBody();
                    msg1.ClearMessageBody();
                    msg1.SetRequestID(2);
                    msg1.SetEventType(UpdateEvent::FirstChange);
                    msg1.SetLimitDataField(1);
                    msg1.SetMessageCode(JAUS_REPORT_TIME);            
                    if( msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION)> 0 &&
                        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
                        msg2.IsFieldPresent(VectorBit::LimitDataField) &&
                        msg2.GetLimitDataField() == 1 )
                    {
                        result = JAUS_OK;
                    }
                }
            }
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void UpdateEvent::ClearMessageBody()
{
    mPresenceVector = 0;
    mEventID = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mType = 0;
    mEventBoundary = 0;
    mLimitDataField = 0;
    mMinimumPeriodicRate = 0;
    mRequestedPeriodicRate = 0;
    if(mpQueryMessageBody)
    {
        delete mpQueryMessageBody;
    }
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
    }
    mpQueryMessageBody = NULL;
    mpQueryMessage = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the data.
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::UpdateEvent& UpdateEvent::operator=(const UpdateEvent& msg)
{
    if(this != &msg)
    {
        ClearMessage();
        CopyHeaderData(&msg);
        mEventID = msg.mEventID;
        mPresenceVector = msg.mPresenceVector;
        mRequestID = msg.mRequestID;
        mMessageCode = msg.mMessageCode;
        mType = msg.mType;
        mEventBoundary = msg.mEventBoundary;
        mLimitDataField = msg.mLimitDataField;
        mMinimumPeriodicRate = msg.mMinimumPeriodicRate;
        mRequestedPeriodicRate = msg.mRequestedPeriodicRate;
        if(msg.mpQueryMessageBody)
        {
            mpQueryMessageBody = msg.mpQueryMessageBody->Clone();
        }
        if(msg.mpQueryMessage)
        {
            mpQueryMessage = msg.mpQueryMessage->Clone();
        }
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size in bytes of the presence vector used by
///          the message.
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  size.  Defaults is current version of library.
///
///   \return Size in bytes of presence vector associated with message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort UpdateEvent::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the bit mask associated with the message indicating the
///          bits used in the presence vector of this message (if it has one).
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  mask.  Defaults is current version of library.
///
///   \return Presence vector mask associated with message.  This value
///           is used to determine what bits are used in the presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
UInt UpdateEvent::GetPresenceVectorMask(const UShort version) const { return 0xFF; }
/*  End of File */

