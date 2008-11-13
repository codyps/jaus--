////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryevents.cpp
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
#include "jaus/messages/query/events/queryevents.h"
#include "jaus/messages/query/querycodes.h"
#include <math.h>
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryEvents::QueryEvents() : Message(JAUS_QUERY_EVENTS)
{
    mPresenceVector = 0;
    mMessageCode = 0;
    mEventID = 0;
    mEventID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryEvents::QueryEvents(const QueryEvents& msg) : Message(JAUS_QUERY_EVENTS)
{
    mPresenceVector = 0;
    mMessageCode = 0;
    mEventID = 0;
    mEventID = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryEvents::~QueryEvents()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param code Message code of the event messages to query.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryEvents::SetMessageCode(const UShort code)
{
    mMessageCode = code;
    BitVector::SetBit(mPresenceVector, VectorBit::MessageCode, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param type The type of events to query.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryEvents::SetEventType(const QueryEvents::EventType type)
{
    mEventType = (Byte)type;
    BitVector::SetBit(mPresenceVector, VectorBit::EventType, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param id Unique identifier associated with events to query.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryEvents::SetEventID(const Byte id)
{
    mEventID = id;
    BitVector::SetBit(mPresenceVector, VectorBit::EventID, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the field specified from presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryEvents::ClearField(const VectorBit::Bits bit)
{
    BitVector::SetBit(mPresenceVector, bit, false);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears multiple bits in the presence vector using a bit mask.
///
///   The mask can be a bitwise OR of multiple VectorMask::Masks to clear
///   multiple values at once.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryEvents::ClearFields(const Byte mask)
{
    mPresenceVector &= ~mask;
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
int QueryEvents::WriteMessageBody(Stream& msg, 
                                  const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected = JAUS_BYTE_SIZE;
        written += msg.Write(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MessageCode))
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMessageCode);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventType))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mEventType);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventID))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mEventID);
        }
        if( expected == written ) 
        {
            return written;
        }
        
        SetJausError(ErrorCodes::WriteFailure); return -1;
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
int QueryEvents::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    ClearMessageBody();
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        
        expected = JAUS_BYTE_SIZE;
        read += msg.Read(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MessageCode))
        {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMessageCode);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventType))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mEventType);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventID))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mEventID);
        }
        if( expected == read ) 
        {
            return read;
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
int QueryEvents::RunTestCase() const
{
    int result = JAUS_OK;
    Stream packet;
    QueryEvents msg1, msg2;

    msg1.SetMessageCode(0x03);
    msg1.SetEventID(3);
    // Test periodic event
    if( msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg1.GetMessageCode() == msg2.GetMessageCode() &&
        msg1.GetEventID() == msg2.GetEventID() &&
        BitVector::IsBitSet(msg2.GetPresenceVector(), VectorBit::EventID) &&
        BitVector::IsBitSet(msg2.GetPresenceVector(), VectorBit::MessageCode))
    {
        result = JAUS_OK;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryEvents::ClearMessageBody()
{
    mPresenceVector = 0;
    mMessageCode = 0;
    mEventID = 0;
    mEventType = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the data.
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::QueryEvents& QueryEvents::operator=(const QueryEvents& msg)
{
    if(this != &msg)
    {
        ClearMessage();
        CopyHeaderData(&msg);
        mPresenceVector = msg.mPresenceVector;
        mEventType = msg.mEventType;
        mMessageCode = msg.mMessageCode;
        mEventID = msg.mEventID;
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
UShort QueryEvents::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt QueryEvents::GetPresenceVectorMask(const UShort version) const { return 0x7; }

/*  End of File */
