////////////////////////////////////////////////////////////////////////////////////
///
///  \file setglobalvector.cpp
///  \brief This file contains the message class for Set Global Vector.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#include "jaus/messages/command/platform/setglobalvector.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

const double Jaus::SetGlobalVector::Limits::MinSpeed = 0.0;
const double Jaus::SetGlobalVector::Limits::MaxSpeed = 10000;
const double Jaus::SetGlobalVector::Limits::MinElevation = -10000;
const double Jaus::SetGlobalVector::Limits::MaxElevation = 35000;
const double Jaus::SetGlobalVector::Limits::MinAngle = -JAUS_PI;
const double Jaus::SetGlobalVector::Limits::MaxAngle = JAUS_PI;

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalVector::SetGlobalVector() : Message(JAUS_SET_GLOBAL_VECTOR)
{
    mPresenceVector = 0;
    mSpeed = 0;
    mElevation = 0;
    mHeading = 0;
    mRoll = 0;
    mPitch = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalVector::SetGlobalVector(const SetGlobalVector& msg) : Message(JAUS_SET_GLOBAL_VECTOR)
{
    mPresenceVector = 0;
    mSpeed = 0;
    mElevation = 0;
    mHeading = 0;
    mRoll = 0;
    mPitch = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalVector::~SetGlobalVector()
{

}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Desired speed [0, 10000] m/s.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalVector::SetSpeed(const double value)
{
    if(value >= Limits::MinSpeed && value <= Limits::MaxSpeed)
    {
        mSpeed = value;
        mPresenceVector |= VectorMask::Speed;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Desired altitude [-10000, 35000] m ASL.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalVector::SetElevation(const double value)
{
    if(value >= Limits::MinElevation && value <= Limits::MaxElevation)
    {
        mElevation = value;
        mPresenceVector |= VectorMask::Elevation;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Desired heading [-pi, pi].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalVector::SetHeading(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mHeading = value;
        mPresenceVector |= VectorMask::Heading;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Desired roll [-pi, pi].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalVector::SetRoll(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mRoll = value;
        mPresenceVector |= VectorMask::Roll;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Desired pitch [-pi, pi].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalVector::SetPitch(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mPitch = value;
        mPresenceVector |= VectorMask::Pitch;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Presence vector for message.  Use this value to determine which
///           message fields are present.
///
////////////////////////////////////////////////////////////////////////////////////
Byte SetGlobalVector::GetPresenceVector() const
{
    return mPresenceVector;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void SetGlobalVector::Print() const
{
    Message::Print();
    PrintGlobalVector();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void SetGlobalVector::PrintGlobalVector() const
{
    cout << "Global Vector:\n";
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Speed))
        cout << "Speed: " << mSpeed << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Elevation))
        cout << "Elevation: " << mElevation << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Heading))
        cout << "Heading: " << JAUS_RAD2DEG(mHeading) << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Roll))
        cout << "Roll: " << JAUS_DEG2RAD(mRoll) << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch))
        cout << "Pitch: " << JAUS_DEG2RAD(mPitch) << endl;

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
int SetGlobalVector::WriteMessageBody(Stream& msg, 
                                      const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        expected = JAUS_BYTE_SIZE;
        written += msg.Write(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Speed))
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mSpeed, Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::UShort);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Elevation))
        {
            expected += JAUS_INT_SIZE;
            written += msg.Write(mElevation, Limits::MaxElevation, Limits::MinElevation, ScaledInteger::Int);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Heading))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mHeading, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Roll))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mRoll, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPitch, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }

        if(expected == written)
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
int SetGlobalVector::ReadMessageBody(const Stream& msg, 
                                     const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        expected = JAUS_BYTE_SIZE;
        read += msg.Read(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Speed))
        {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mSpeed, Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::UShort);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Elevation))
        {
            expected += JAUS_INT_SIZE;
            read += msg.Read(mElevation, Limits::MaxElevation, Limits::MinElevation, ScaledInteger::Int);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Heading))
        {
            expected += JAUS_SHORT_SIZE;
            read += msg.Read(mHeading, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Roll))
        {
            expected += JAUS_SHORT_SIZE;
            read += msg.Read(mRoll, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch))
        {
            expected += JAUS_SHORT_SIZE;
            read += msg.Read(mPitch, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
        }
        if(expected == read)
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
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void SetGlobalVector::ClearMessageBody()
{
    mPresenceVector = 0;
    mSpeed = 0;
    mElevation = 0;
    mHeading = 0;
    mRoll = 0;
    mPitch = 0;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalVector& SetGlobalVector::operator =(const SetGlobalVector& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mPresenceVector =  msg.mPresenceVector;
        mSpeed =           msg.mSpeed;
        mElevation =        msg.mElevation;
        mHeading =         msg.mHeading;
        mRoll =            msg.mRoll;
        mPitch =           msg.mPitch;
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
UShort SetGlobalVector::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt SetGlobalVector::GetPresenceVectorMask(const UShort version) const { return 0x1F; }


/*  End of File */
