////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalpose.cpp
///  \brief This file contains a JAUS message structure for encoding
///  and decoding JAUS_REPORT_GLOBAL_POSE messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 24 July 2007
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
#include "jaus/messages/inform/platform/reportglobalpose.h"
#include "jaus/messages/inform/informcodes.h"

using namespace Jaus;

const double ReportGlobalPose::Limits::MaxLatitude = 90.0;
const double ReportGlobalPose::Limits::MaxLongitude = 180;
const double ReportGlobalPose::Limits::MaxElevation = 35000.0;
const double ReportGlobalPose::Limits::MinLatitude = -90.0;
const double ReportGlobalPose::Limits::MinLongitude = -180;
const double ReportGlobalPose::Limits::MinElevation = -10000.0;
const double ReportGlobalPose::Limits::MaxAttitude = JAUS_PI;
const double ReportGlobalPose::Limits::MinAttitude = -JAUS_PI;
const double ReportGlobalPose::Limits::MinPositionRms = 0.0;
const double ReportGlobalPose::Limits::MaxPositionRms = 100.0;
const double ReportGlobalPose::Limits::MaxAttitudeRms = JAUS_PI;
const double ReportGlobalPose::Limits::MinAttitudeRms = 0.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPose::ReportGlobalPose() : Message(JAUS_REPORT_GLOBAL_POSE)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPose::ReportGlobalPose(const ReportGlobalPose &gp) : Message(JAUS_REPORT_GLOBAL_POSE)
{
    *this = gp;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPose::~ReportGlobalPose()
{   
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
int ReportGlobalPose::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected = JAUS_USHORT_SIZE;
        written += msg.Write(mPresenceVector);
        
        if(mPresenceVector&VectorMask::Latitude) 
        {
            written += msg.Write(mLatitude, Limits::MaxLatitude, Limits::MinLatitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Longitude) 
        {
            written += msg.Write(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Elevation) 
        {
            written += msg.Write(mElevation, Limits::MaxElevation, Limits::MinElevation, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::PositionRms) 
        {
            written += msg.Write(mPositionRms, Limits::MaxPositionRms, Limits::MinPositionRms, ScaledInteger::UInt);
            expected += JAUS_UINT_SIZE;
        }
        if(mPresenceVector&VectorMask::Roll) 
        {
            written += msg.Write(mRoll, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Pitch) 
        {
            written += msg.Write(mPitch, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Yaw) 
        {
            written += msg.Write(mYaw, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::AttitudeRms) 
        {
            written += msg.Write(mAttitudeRms, Limits::MaxAttitudeRms, Limits::MinAttitudeRms, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::TimeStamp) 
        {
            written += msg.Write(((UInt)(mTimeStamp)));
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
int ReportGlobalPose::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        expected = JAUS_USHORT_SIZE;
        read += msg.Read(mPresenceVector);
        
        if(mPresenceVector&VectorMask::Latitude) 
        {
            read += msg.Read(mLatitude, Limits::MaxLatitude, Limits::MinLatitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Longitude) 
        {
            read += msg.Read(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Elevation) 
        {
            read += msg.Read(mElevation, Limits::MaxElevation, Limits::MinElevation, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::PositionRms) 
        {
            read += msg.Read(mPositionRms, Limits::MaxPositionRms, Limits::MinPositionRms, ScaledInteger::UInt);
            expected += JAUS_UINT_SIZE;
        }
        if(mPresenceVector&VectorMask::Roll) 
        {
            read += msg.Read(mRoll, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Pitch) 
        {
            read += msg.Read(mPitch, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Yaw) 
        {
            read += msg.Read(mYaw, Limits::MaxAttitude, Limits::MinAttitude, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::AttitudeRms) 
        {
            read += msg.Read(mAttitudeRms, Limits::MaxAttitudeRms, Limits::MinAttitudeRms, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::TimeStamp) 
        {
            UInt tstamp;
            read += msg.Read(tstamp);
            mTimeStamp.SetTime(tstamp);
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
///   \brief Clears message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportGlobalPose::ClearMessageBody()
{
    ClearGlobalPose();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPose &ReportGlobalPose::operator =(const ReportGlobalPose &msg)
{
    this->CopyHeaderData(&msg);
    CopyFrom((GlobalPose)(msg));
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
UShort ReportGlobalPose::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }
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
UInt ReportGlobalPose::GetPresenceVectorMask(const UShort version) const { return 0x1FF; }


/*  End of File */
