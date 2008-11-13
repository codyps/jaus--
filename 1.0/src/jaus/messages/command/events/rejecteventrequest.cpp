////////////////////////////////////////////////////////////////////////////////////
///
///  \file rejecteventrequest.cpp
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
#include "jaus/messages/command/events/rejecteventrequest.h"
#include "jaus/messages/command/commandcodes.h"
#include <math.h>
#include <string.h>
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
RejectEventRequest::RejectEventRequest() : Message(JAUS_REJECT_EVENT_REQUEST)
{
    mPresenceVector = 0;
    mRequestID = 0;
    mResponseCode = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
RejectEventRequest::RejectEventRequest(const RejectEventRequest& msg) : Message(JAUS_REJECT_EVENT_REQUEST)
{
        mPresenceVector = 0;
    mRequestID = 0;
    mResponseCode = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
RejectEventRequest::~RejectEventRequest()
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
int RejectEventRequest::SetRequestID(const Byte id)
{
    mRequestID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the error message, and updates presence
///          vector to reflect inclusion.
///
///   \param msg Error message to send with rejection of event.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RejectEventRequest::SetErrorMessage(const std::string& msg)
{
    mErrorMessage = msg;
    BitVector::SetBit(mPresenceVector, VectorBit::ErrorMessage, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value.
///
///   \param code Confirmation response code.  See ResponseCode enumerations.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int RejectEventRequest::SetResponseCode(const RejectEventRequest::ResponseCode code)
{
    mResponseCode = (Byte)(code);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the field specified from presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
void RejectEventRequest::ClearField(const VectorBit::Bits bit)
{
    BitVector::SetBit(mPresenceVector, bit, false);
    if(bit == VectorBit::ErrorMessage)
    {
        mErrorMessage.clear();
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
void RejectEventRequest::ClearFields(const Byte mask)
{
    mPresenceVector &= ~mask;
    if(!BitVector::IsBitSet(mPresenceVector, VectorBit::ErrorMessage))
    {
        mErrorMessage.clear();
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
int RejectEventRequest::WriteMessageBody(Stream& msg, 
                                          const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected = JAUS_BYTE_SIZE*3;
        written += msg.Write(mPresenceVector);
        written += msg.Write(mRequestID);
        written += msg.Write(mResponseCode);
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ErrorMessage))
        {
            expected += (unsigned int)mErrorMessage.size();
            written += msg.Write((unsigned char *)mErrorMessage.c_str(),
                                 (unsigned int)mErrorMessage.size());
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
int RejectEventRequest::ReadMessageBody(const Stream& msg, 
                                         const UShort version)
{
    ClearMessageBody();
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        
        expected = JAUS_BYTE_SIZE*3;
        read += msg.Read(mPresenceVector);
        read += msg.Read(mRequestID);
        read += msg.Read(mResponseCode);
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ErrorMessage))
        {
            unsigned int len = 0;
            unsigned char *buff = NULL;
            len = msg.Length() - msg.GetReadPos();
            
            if( len == 0 )
            {
                SetJausError(ErrorCodes::BadPacket); return -1;
            }
            expected += len;
            buff = new unsigned char[len + 1];
            memset(buff, 0, len + 1);
            read += msg.Read(buff, len);
            mErrorMessage = (char *)buff;
            delete[] buff;
            buff = NULL;
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
int RejectEventRequest::RunTestCase() const
{
    int result = JAUS_OK;
    Stream packet;
    RejectEventRequest msg1, msg2;

    msg1.SetRequestID(1);
    msg1.SetResponseCode(RejectEventRequest::InvalidEventID);
    msg1.SetErrorMessage("Bad Data");

    if( msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg1.GetRequestID() == msg2.GetRequestID() &&
        *msg1.GetErrorMessage() == *msg2.GetErrorMessage())
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
void RejectEventRequest::ClearMessageBody()
{
    mPresenceVector = 0;
    mRequestID = 0;
    mResponseCode = 0;
    mErrorMessage.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the data.
///
////////////////////////////////////////////////////////////////////////////////////
Jaus::RejectEventRequest& RejectEventRequest::operator=(const RejectEventRequest& msg)
{
    if(this != &msg)
    {
        ClearMessage();
        CopyHeaderData(&msg);
        mPresenceVector = msg.mPresenceVector;
        mRequestID = msg.mRequestID;
        mResponseCode = msg.mResponseCode;
        mErrorMessage = msg.mErrorMessage;
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
UShort RejectEventRequest::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt RejectEventRequest::GetPresenceVectorMask(const UShort version) const { return 0x1; }
/*  End of File */

