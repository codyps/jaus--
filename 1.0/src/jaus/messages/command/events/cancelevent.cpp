////////////////////////////////////////////////////////////////////////////////////
///
///  \file cancelevent.cpp
///  \brief This file contains the message Cancel Event.
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
#include "jaus/messages/command/events/cancelevent.h"
#include "jaus/messages/command/commandcodes.h"
#include <math.h>
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CancelEvent::CancelEvent() : Message(JAUS_CANCEL_EVENT)
{
    mPresenceVector = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mEventID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CancelEvent::CancelEvent(const CancelEvent& msg) : Message(JAUS_CANCEL_EVENT)
{
    mPresenceVector = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mEventID = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CancelEvent::~CancelEvent()
{
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
int CancelEvent::SetRequestID(const Byte id)
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
int CancelEvent::SetMessageCode(const UShort code)
{
    mMessageCode = code;
    BitVector::SetBit(mPresenceVector, VectorBit::MessageCode, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param id Unique identifier associated with event.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int CancelEvent::SetEventID(const Byte id)
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
void CancelEvent::ClearField(const VectorBit::Bits bit)
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
void CancelEvent::ClearFields(const Byte mask)
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
int CancelEvent::WriteMessageBody(Stream& msg, 
                                  const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected = JAUS_BYTE_SIZE*2;
        written += msg.Write(mPresenceVector);
        written += msg.Write(mRequestID);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MessageCode))
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMessageCode);
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
int CancelEvent::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    ClearMessageBody();
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        
        expected = JAUS_BYTE_SIZE*2;
        read += msg.Read(mPresenceVector);
        read += msg.Read(mRequestID);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MessageCode))
        {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMessageCode);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EventID))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mEventID);
        }
        if( expected == read )
        {
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
int CancelEvent::RunTestCase() const
{
    int result = JAUS_OK;
    Stream packet;
    CancelEvent msg1, msg2;

    msg1.SetRequestID(1);
    msg1.SetMessageCode(0x03);
    msg1.SetEventID(3);
    // Test periodic event
    if( msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg1.GetRequestID() == msg2.GetRequestID() &&
        msg1.GetMessageCode() == msg2.GetMessageCode() &&
        msg1.GetEventID() == msg2.GetEventID() &&
        (msg2.GetPresenceVector() & 0x03) == 0x03)
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
void CancelEvent::ClearMessageBody()
{
    mPresenceVector = 0;
    mRequestID = 0;
    mMessageCode = 0;
    mEventID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the data.
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::CancelEvent& CancelEvent::operator=(const CancelEvent& msg)
{
    if(this != &msg)
    {
        ClearMessage();
        CopyHeaderData(&msg);
        mPresenceVector = msg.mPresenceVector;
        mRequestID = msg.mRequestID;
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
UShort CancelEvent::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt CancelEvent::GetPresenceVectorMask(const UShort version) const { return 0x3; }
/*  End of File */

