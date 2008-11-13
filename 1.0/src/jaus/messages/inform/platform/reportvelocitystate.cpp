////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportvelocitystate.cpp
///  \brief This file contains the message class for Report Velocity State.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 12 May 2008
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
#include "jaus/messages/inform/platform/reportvelocitystate.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const double ReportVelocityState::Limits::MaxVelocity = 65.534;
const double ReportVelocityState::Limits::MinVelocity = -65.534;
const double ReportVelocityState::Limits::MaxRate = 32.767;
const double ReportVelocityState::Limits::MinRate = -32.767;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportVelocityState::ReportVelocityState() : Message(JAUS_REPORT_VELOCITY_STATE)
{
    mPresenceVector = 0;
    mVelocityX = 0;
    mVelocityY = 0;
    mVelocityZ = 0;
    mVelocityRms = 0;
    mRollRate = 0;
    mPitchRate = 0;
    mYawRate = 0;
    mRateRms = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportVelocityState::ReportVelocityState(const ReportVelocityState &vs) : Message(JAUS_REPORT_VELOCITY_STATE)
{
    *this = vs;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportVelocityState::~ReportVelocityState()
{   
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetVelocityX(const double value)
{
    if(value >= Limits::MinVelocity && value <= Limits::MaxVelocity)
    {
        mVelocityX = value;
        mPresenceVector |= VectorMask::VelocityX;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetVelocityY(const double value)
{
    if(value >= Limits::MinVelocity && value <= Limits::MaxVelocity)
    {
        mVelocityY = value;
        mPresenceVector |= VectorMask::VelocityY;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetVelocityZ(const double value)
{
    if(value >= Limits::MinVelocity && value <= Limits::MaxVelocity)
    {
        mVelocityZ = value;
        mPresenceVector |= VectorMask::VelocityZ;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetVelocityRms(const double value)
{
    if(value >= 0 && value <= 100.0)
    {
        mVelocityRms = value;
        mPresenceVector |= VectorMask::VelocityRms;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetRollRate(const double value)
{
    if(value >= Limits::MinRate && value <= Limits::MaxRate)
    {
        mRollRate = value;
        mPresenceVector |= VectorMask::RollRate;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetPitchRate(const double value)
{
    if(value >= Limits::MinRate && value <= Limits::MaxRate)
    {
        mPitchRate = value;
        mPresenceVector |= VectorMask::PitchRate;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetYawRate(const double value)
{
    if(value >= Limits::MinRate && value <= Limits::MaxRate)
    {
        mYawRate = value;
        mPresenceVector |= VectorMask::YawRate;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetRateRms(const double value)
{
    if(value >= 0 && value <= 100.0)
    {
        mRateRms = value;
        mPresenceVector |= VectorMask::RateRms;
        return JAUS_OK;
    }
    SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and update presence vector.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportVelocityState::SetTimeStamp(const Time& tstamp)
{
    mTimeStamp = tstamp;
    mPresenceVector |= VectorMask::TimeStamp;
    return JAUS_OK;
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
int ReportVelocityState::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected = JAUS_USHORT_SIZE;
        written += msg.Write(mPresenceVector);
        
        if(mPresenceVector&VectorMask::VelocityX) 
        {
            written += msg.Write(mVelocityX, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityY) 
        {
            written += msg.Write(mVelocityY, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityZ) 
        {
            written += msg.Write(mVelocityZ, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityRms) 
        {
            written += msg.Write(mVelocityRms, 100, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::RollRate) 
        {
            written += msg.Write(mRollRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::PitchRate) 
        {
            written += msg.Write(mPitchRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::YawRate) 
        {
            written += msg.Write(mYawRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::RateRms) 
        {
            written += msg.Write(mRateRms, 100, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::TimeStamp) 
        {
            written += msg.Write(mTimeStamp.ToUInt());
            expected += JAUS_UINT_SIZE;
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
int ReportVelocityState::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        expected = JAUS_USHORT_SIZE;
        read += msg.Read(mPresenceVector);
        
        if(mPresenceVector&VectorMask::VelocityX) 
        {
            read += msg.Read(mVelocityX, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityY) 
        {
            read += msg.Read(mVelocityY, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityZ) 
        {
            read += msg.Read(mVelocityZ, Limits::MaxVelocity, Limits::MinVelocity, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::VelocityRms) 
        {
            read += msg.Read(mVelocityRms, 100, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::RollRate) 
        {
            read += msg.Read(mRollRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::PitchRate) 
        {
            read += msg.Read(mPitchRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::YawRate) 
        {
            read += msg.Read(mYawRate, Limits::MaxRate, Limits::MinRate, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::RateRms) 
        {
            read += msg.Read(mRateRms, 100, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::TimeStamp) 
        {
            UInt t = 0;
            read += msg.Read(t);
            mTimeStamp.SetTime(t);
            expected += JAUS_UINT_SIZE;
        }

        if( expected == read ) 
        {
            return read;
        }
        else
        {
            SetJausError(ErrorCodes::ReadFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to console.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportVelocityState::Print() const
{
    Message::Print();
    PrintVelocityState();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the fields based on the presence vector mask passed.
///
///   \param mask Any bit position set in the mask will clear a field
///               in the message.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportVelocityState::ClearFields(const UShort mask)
{
    mPresenceVector &= ~mask;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message body data to console.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportVelocityState::PrintVelocityState() const
{
    if(mPresenceVector&VectorMask::VelocityX) 
    {
        cout << "Velocity X: " << mVelocityX << " ";
    }
    if(mPresenceVector&VectorMask::VelocityY) 
    {
        cout << "Velocity Y: " << mVelocityY << " ";
    }
    if(mPresenceVector&VectorMask::VelocityZ) 
    {
        cout << "Velocity Z: " << mVelocityZ << " ";
    }
    if(mPresenceVector&VectorMask::VelocityRms) 
    {
        cout << "Velocity RMS: " << mVelocityRms << " ";
    }
    cout << endl;
    if(mPresenceVector&VectorMask::RollRate) 
    {
        cout << "Roll Rate: " << JAUS_RAD2DEG(mRollRate) << " ";
    }
    if(mPresenceVector&VectorMask::PitchRate) 
    {
        cout << "Pitch Rate: " << JAUS_RAD2DEG(mPitchRate) << " ";
    }
    if(mPresenceVector&VectorMask::YawRate) 
    {
        cout << "Yaw Rate: " << JAUS_RAD2DEG(mYawRate) << " ";
    }
    if(mPresenceVector&VectorMask::RateRms) 
    {
        cout << "Rate RMS: " << mRateRms << " ";
    }
    if(mPresenceVector&VectorMask::TimeStamp) 
    {
        mTimeStamp.PrintTime();
    }   
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportVelocityState::ClearMessageBody()
{
    mPresenceVector = 0;
    mVelocityX = 0;
    mVelocityY = 0;
    mVelocityZ = 0;
    mVelocityRms = 0;
    mRollRate = 0;
    mPitchRate = 0;
    mYawRate = 0;
    mRateRms = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportVelocityState &ReportVelocityState::operator =(const ReportVelocityState &msg)
{
    this->CopyHeaderData(&msg);

    mPresenceVector = msg.mPresenceVector;
    mVelocityX =    msg.mVelocityX;   
    mVelocityY =    msg.mVelocityY;   
    mVelocityZ =    msg.mVelocityZ;   
    mVelocityRms =  msg.mVelocityRms; 
    mRollRate =     msg.mRollRate;    
    mPitchRate =    msg.mPitchRate;   
    mYawRate =      msg.mYawRate;        
    mRateRms =      msg.mRateRms;
    mTimeStamp =    msg.mTimeStamp;

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
UShort ReportVelocityState::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }

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
UInt ReportVelocityState::GetPresenceVectorMask(const UShort version) const { return 0x1FF; }

/*  End of File */
