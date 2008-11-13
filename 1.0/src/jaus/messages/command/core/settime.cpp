////////////////////////////////////////////////////////////////////////////////////
///
///  \file settime.cpp
///  \brief This file contains data structures for the JAUS_SET_TIME
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 25 February 2008
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
#include "jaus/messages/command/core/settime.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/bitvector.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetTime::SetTime() : Message(JAUS_SET_TIME)
{
    mpTimeStamp = 0;
    mpDateStamp = 0;
    mPresenceVector = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetTime::SetTime(const SetTime &msg) : Message(JAUS_SET_TIME)
{
    mpTimeStamp = 0;
    mpDateStamp = 0;
    mPresenceVector = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetTime::~SetTime()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the time stamp.
///
///   \param t The time to use in the Set Time message.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetTime::SetTimeStamp(const Time& t)
{
    if(!mpTimeStamp)
        mpTimeStamp = new Time();

    *mpTimeStamp = t;
    BitVector::SetBit(mPresenceVector, VectorBit::Time, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the date stamp.
///
///   \param d The date to use in the Set Time message.
///
///   \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetTime::SetDateStamp(const Date& d)
{
    if(!mpDateStamp)
        mpDateStamp = new Date();

    *mpDateStamp = d;
    BitVector::SetBit(mPresenceVector, VectorBit::Date, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears out time stamp data.
///
////////////////////////////////////////////////////////////////////////////////////
void SetTime::ClearTime()
{
    if(mpTimeStamp)
        delete mpTimeStamp;

    mpTimeStamp = NULL;
    BitVector::SetBit(mPresenceVector, VectorBit::Time, false);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears out date stamp data.
///
////////////////////////////////////////////////////////////////////////////////////
void SetTime::ClearDate()
{
    if(mpDateStamp)
        delete mpDateStamp;

    mpDateStamp = NULL;
    BitVector::SetBit(mPresenceVector, VectorBit::Date, false);
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
int SetTime::WriteMessageBody(Stream& msg, 
                              const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        if( msg.Write(mPresenceVector) )
        {
            written = expected = 1;
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Time))
            {
                written += msg.Write(mpTimeStamp->ToUInt());
                expected += JAUS_UINT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Date))
            {
                written += msg.Write(mpDateStamp->ToUShort());
                expected += JAUS_USHORT_SIZE;
            }

            if( expected == written )
            {
                return written;
            }
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
int SetTime::ReadMessageBody(const Stream& msg, 
                             const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        ClearTime();
        ClearDate();
        if( msg.Read(mPresenceVector) )
        {
            expected = read = 1;
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Time))
            {
                UInt tstamp = 0;
                read += msg.Read(tstamp);
                expected += JAUS_UINT_SIZE;
                mpTimeStamp = new Time();
                mpTimeStamp->SetTime(tstamp);
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Date))
            {
                UShort dstamp = 0;
                read += msg.Read(dstamp);
                expected += JAUS_USHORT_SIZE;
                mpDateStamp = new Date();
                mpDateStamp->SetDate(dstamp);
            }

            if(expected == read)
            {
                return read;
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
///   \brief Clears all message contents.
///
////////////////////////////////////////////////////////////////////////////////////
void SetTime::ClearMessageBody()
{
    ClearTime();
    ClearDate();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
SetTime &SetTime::operator =(const SetTime &msg)
{
    CopyHeaderData(&msg);

    if(mpTimeStamp && !msg.mpTimeStamp)
    {
        delete mpTimeStamp;
        mpTimeStamp = 0;
    }
    if(!mpTimeStamp && msg.mpTimeStamp)
    {
        mpTimeStamp = new Time();
    }

    if(mpTimeStamp && msg.mpTimeStamp)
        *mpTimeStamp = *msg.mpTimeStamp;

    if(mpDateStamp && !msg.mpDateStamp)
    {
        delete mpDateStamp;
        mpDateStamp = 0;
    }
    if(!mpDateStamp && msg.mpDateStamp)
    {
        mpDateStamp = new Date();
    }

    if(mpDateStamp && msg.mpDateStamp)
        *mpDateStamp = *msg.mpDateStamp;

    mPresenceVector = msg.mPresenceVector;

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
UShort SetTime::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt SetTime::GetPresenceVectorMask(const UShort version) const { return 0x3; }

/*  End of File */
