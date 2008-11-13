////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportplatformoperationaldata.cpp
///  \brief This file contains the message structure for Report Platform Operational
///  data.
///
///  <br>Author(s): David Adams
///  <br>Created: 09 June 2008
///  <br>Last Modified: 09 June 2008
///  <br>Copyright (c) 2007
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#include "jaus/messages/inform/platform/reportplatformoperationaldata.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \class Limits
///   \brief Limit values for constants used by class.
///
////////////////////////////////////////////////////////////////////////////////////
const double ReportPlatformOperationalData::Limits::MinEngineTemperature =        -75;
const double ReportPlatformOperationalData::Limits::MaxEngineTemperature =        180;
const double ReportPlatformOperationalData::Limits::MinBatteryVoltage =            0;
const double ReportPlatformOperationalData::Limits::MaxBatteryVoltage =            127;
const double ReportPlatformOperationalData::Limits::MinPercentageFuelLevel =    0;
const double ReportPlatformOperationalData::Limits::MaxPercentageFuelLevel =    100;
const double ReportPlatformOperationalData::Limits::MinPercentageOilPressure =    0;
const double ReportPlatformOperationalData::Limits::MaxPercentageOilPressure =    127;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformOperationalData::ReportPlatformOperationalData() :
            Message(JAUS_REPORT_PLATFORM_OPERATIONAL_DATA)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformOperationalData::ReportPlatformOperationalData(const ReportPlatformOperationalData &msg) :
            Message(JAUS_REPORT_PLATFORM_OPERATIONAL_DATA)
{
    ClearMessageBody();
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformOperationalData::~ReportPlatformOperationalData()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPlatformOperationalData::ClearMessageBody()
{
    mPresenceVector = 0;
    mEngineTemperature = 0;
    mOdometer = 0;
    mBatteryVoltage = 0;
    mPercentageFuelLevel = 0;
    mPercentageOilPressure = 0;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Degrees Celsius, Scaled Integer,
///              Lower Limit = -75C, Upper Limit = 180C.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformOperationalData::SetEngineTemperature(const double val)
{
    if(val >=-Limits::MinEngineTemperature && val <=Limits::MaxEngineTemperature)
    {
        mEngineTemperature = val;
        BitVector::SetBit(mPresenceVector, VectorBit::EngineTemperature, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Meters, Cumulative distance traveled by vehicle
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformOperationalData::SetOdometer(const UInt val)
{
    mOdometer = val;
    BitVector::SetBit(mPresenceVector,VectorBit::Odometer,true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Percent, Scaled Integer,
///              Lower Limit = 0%, Upper Limit = 127%
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformOperationalData::SetBatteryVoltage(const double val)
{
    if(val>=Limits::MinBatteryVoltage && val<=Limits::MaxBatteryVoltage)
    {
        mBatteryVoltage =  val;
        BitVector::SetBit(mPresenceVector,VectorBit::BatteryVoltage,true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Percent, Scaled Integer,
///              Lower Limit = 0%, Upper Limit = 100%
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformOperationalData::SetPercentageFuelLevel(const double val)
{
    if(val>=Limits::MinPercentageFuelLevel && val<=Limits::MaxPercentageFuelLevel)
    {
        mPercentageFuelLevel = val;
        BitVector::SetBit(mPresenceVector,VectorBit::PercentageFuelLevel ,true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Percent, Scaled Integer,
///              Lower Limit = 0%, Upper Limit = 127%
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformOperationalData::SetPercentageOilPressure(const double val)
{
    if(val>=Limits::MinPercentageOilPressure && val<=Limits::MaxPercentageOilPressure)
    {
        mPercentageOilPressure = val;
        BitVector::SetBit(mPresenceVector,VectorBit::PercentageOilPressure ,true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

double ReportPlatformOperationalData::GetEngineTemperature()const
{
    if(mPresenceVector & VectorMask::EngineTemperature)
    {
        return mEngineTemperature;
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}
double ReportPlatformOperationalData::GetOdometer()const
{
    if(mPresenceVector & VectorMask::Odometer)
    {
        return mOdometer;
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}
double ReportPlatformOperationalData::GetBatteryVoltage()const
{
    if(mPresenceVector & VectorMask::BatteryVoltage)
    {
        return mBatteryVoltage;
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}
double ReportPlatformOperationalData::GetPercentageFuelLevel()const
{
    if(mPresenceVector & VectorMask::PercentageFuelLevel)
    {
        return mPercentageFuelLevel;
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}
double ReportPlatformOperationalData::GetPercentageOilPressure()const
{
    if(mPresenceVector & VectorMask::PercentageOilPressure)
    {
        return mPercentageOilPressure;
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
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
int ReportPlatformOperationalData::WriteMessageBody(Jaus::Stream& msg, const Jaus::UShort version) const
{
    if(version<=JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;

        expected = JAUS_BYTE_SIZE;
        written += msg.Write(mPresenceVector);

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EngineTemperature))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mEngineTemperature,180,-75,ScaledInteger::Short);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Odometer))
        {
            expected += JAUS_UINT_SIZE;
            written += msg.Write(mOdometer);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::BatteryVoltage))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mBatteryVoltage,127,0,ScaledInteger::Byte);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PercentageFuelLevel))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mPercentageFuelLevel,100,0,ScaledInteger::Byte);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PercentageOilPressure))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mPercentageOilPressure,127,0,ScaledInteger::Byte);
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
int ReportPlatformOperationalData::ReadMessageBody(const Jaus::Stream& msg, const Jaus::UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;

        expected = JAUS_BYTE_SIZE;
        read += msg.Read(mPresenceVector);

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::EngineTemperature))
        {
            expected += JAUS_SHORT_SIZE;
            read += msg.Read(mEngineTemperature,180.0,-75.0,ScaledInteger::Short);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Odometer))
        {
            expected += JAUS_UINT_SIZE;
            read += msg.Read(mOdometer);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::BatteryVoltage))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mBatteryVoltage,127,0,ScaledInteger::Byte);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PercentageFuelLevel))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mPercentageFuelLevel,100,0,ScaledInteger::Byte);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PercentageOilPressure))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mPercentageOilPressure,127,0,ScaledInteger::Byte);
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
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformOperationalData &ReportPlatformOperationalData::operator=(const ReportPlatformOperationalData &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector    =            msg.mPresenceVector;
    mEngineTemperature =        msg.mEngineTemperature;
    mOdometer =                    msg.mOdometer;
    mBatteryVoltage    =            msg.mBatteryVoltage;
    mPercentageFuelLevel =        msg.mPercentageFuelLevel;
    mPercentageOilPressure =    msg.mPercentageOilPressure;
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
UShort ReportPlatformOperationalData::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }

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
UInt ReportPlatformOperationalData::GetPresenceVectorMask(const UShort version) const { return 0x1F; }

/*  End of File */
