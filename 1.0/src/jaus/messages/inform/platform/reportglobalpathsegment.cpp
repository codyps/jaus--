////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalpathsegment.cpp
///  \brief This file contains the message structure for Report Global Path Segment
///
///  <br>Author(s): David Adams
///  <br>Created: 16 June 2008
///  <br>Last Modified: 16 June 2008
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
#include "jaus/messages/inform/platform/reportglobalpathsegment.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UShort ReportGlobalPathSegment::Limits::MinPathSegmentNumber = 0;    
const UShort ReportGlobalPathSegment::Limits::MaxPathSegmentNumber = 65535;    
const double ReportGlobalPathSegment::Limits::MinP1Latitude =        -90;            
const double ReportGlobalPathSegment::Limits::MaxP1Latitude =        90;            
const double ReportGlobalPathSegment::Limits::MinP1Longitude =        -180;            
const double ReportGlobalPathSegment::Limits::MaxP1Longitude =        180;            
const double ReportGlobalPathSegment::Limits::MinP1Altitude =        -10000;            
const double ReportGlobalPathSegment::Limits::MaxP1Altitude =        35000;            
const double ReportGlobalPathSegment::Limits::MinP2Latitude =        -90;            
const double ReportGlobalPathSegment::Limits::MaxP2Latitude =        90;            
const double ReportGlobalPathSegment::Limits::MinP2Longitude =        -180;            
const double ReportGlobalPathSegment::Limits::MaxP2Longitude =        180;            
const double ReportGlobalPathSegment::Limits::MinP2Altitude =        -10000;            
const double ReportGlobalPathSegment::Limits::MaxP2Altitude =        35000;            
const double ReportGlobalPathSegment::Limits::MinWeighingFactor =    0;        
const double ReportGlobalPathSegment::Limits::MaxWeighingFactor =    500;        

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPathSegment::ReportGlobalPathSegment() : 
                        Message(JAUS_REPORT_GLOBAL_PATH_SEGMENT)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPathSegment::ReportGlobalPathSegment(const ReportGlobalPathSegment &msg) : 
                        Message(JAUS_REPORT_GLOBAL_PATH_SEGMENT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPathSegment::~ReportGlobalPathSegment()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the presence for the query.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a path segment number value 
///
///   \param val Unsigned Short Integer    N/A    Lower Limit = 0, Upper Limit = 65,535
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetPathSegmentNumber(const UInt val)
{
    if(val >= Limits::MinPathSegmentNumber && val <= Limits::MaxPathSegmentNumber)
    {
        mPathSegmentNumber = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P1 latitude
///
///   \param val Scaled Integer, Degrees, Scaled Integer, 
///                    Lower Limit =  -90, Upper Limit = 90.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP1Latitude(const double val)
{
    if(val >= Limits::MinP1Latitude && val <= Limits::MaxP1Latitude)
    {
        mP1Latitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P1 Longitude
///
///   \param val Scaled Integer, Degrees, Scaled Integer, 
///                    Lower Limit =  -180, Upper Limit = 180.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP1Longitude(const double val)
{
    if(val >= Limits::MinP1Longitude && val <= Limits::MaxP1Longitude)
    {
        mP1Longitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P1 Altitude
///
///   \param val Scaled Integer, Lower Limit =  -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP1Altitude(const double val)
{
    if(val >= Limits::MinP1Altitude && val <= Limits::MaxP1Altitude)
    {
        mPresenceVector |= VectorMask::P1Altitude;
        mP1Altitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P2 latitude
///
///   \param val Scaled Integer, Degrees, Scaled Integer, 
///                    Lower Limit =  -90, Upper Limit = 90.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP2Latitude(const double val)
{
    if(val >= Limits::MinP2Latitude && val <= Limits::MaxP2Latitude)
    {
        mP2Latitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P2 Longitude
///
///   \param val Scaled Integer, Degrees, Scaled Integer, 
///                    Lower Limit =  -180, Upper Limit = 180.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP2Longitude(const double val)
{
    if(val >= Limits::MinP2Longitude && val <= Limits::MaxP2Longitude)
    {
        mP2Longitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a P2 Altitude
///
///   \param val Scaled Integer, Lower Limit =  -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetP2Altitude(const double val)
{
    if(val >= Limits::MinP2Altitude && val <= Limits::MaxP2Altitude)
    {
        mPresenceVector |= VectorMask::P2Altitude;
        mP2Altitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Weighting factor
///
///   \param val Unsigned Short Integer, Scaled Integer, Range 0 to 500.
///               Zero = a straight line
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalPathSegment::SetWeighingFactor(const double val)
{
    if(val >= Limits::MinWeighingFactor && val <= Limits::MaxWeighingFactor)
    {
        mWeighingFactor = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

double ReportGlobalPathSegment::GetP1Altitude()const
{
    if(mPresenceVector & VectorMask::P1Altitude)
    {
        return mP1Altitude;
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}

double ReportGlobalPathSegment::GetP2Altitude()const
{
    if(mPresenceVector & VectorMask::P2Altitude)
    {
        return mP2Altitude;
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
int ReportGlobalPathSegment::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;
        written += msg.Write(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        written +=msg.Write(mPathSegmentNumber);
        expected += JAUS_UINT_SIZE;

        written += msg.Write(mP1Latitude, Limits::MaxP1Latitude, Limits::MinP1Latitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written += msg.Write(mP1Longitude, Limits::MaxP1Longitude, Limits::MinP1Longitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P1Altitude)
        {
            written +=msg.Write(mP1Altitude, Limits::MaxP1Altitude, Limits::MinP1Altitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        written += msg.Write(mP2Latitude, Limits::MaxP2Latitude, Limits::MinP2Latitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written +=msg.Write(mP2Longitude, Limits::MaxP2Longitude, Limits::MinP2Longitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P1Altitude)
        {
            written += msg.Write(mP2Altitude, Limits::MaxP1Altitude, Limits::MinP1Altitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        written +=msg.Write(mWeighingFactor, Limits::MaxWeighingFactor, Limits::MinWeighingFactor, ScaledInteger::UShort);
        expected += JAUS_USHORT_SIZE;

        if(expected == written)
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
int ReportGlobalPathSegment::ReadMessageBody(const Stream& msg, 
                                                  const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        read += msg.Read(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        read +=msg.Read(mPathSegmentNumber);
        expected += JAUS_UINT_SIZE;

        read += msg.Read(mP1Latitude, Limits::MaxP1Latitude, Limits::MinP1Latitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        read += msg.Read(mP1Longitude, Limits::MaxP1Longitude, Limits::MinP1Longitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P1Altitude)
        {
            read +=msg.Read(mP1Altitude, Limits::MaxP1Altitude, Limits::MinP1Altitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        read += msg.Read(mP2Latitude, Limits::MaxP2Latitude, Limits::MinP2Latitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        read +=msg.Read(mP2Longitude, Limits::MaxP2Longitude, Limits::MinP2Longitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P1Altitude)
        {
            read += msg.Read(mP2Altitude, Limits::MaxP1Altitude, Limits::MinP1Altitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        read +=msg.Read(mWeighingFactor, Limits::MaxWeighingFactor, Limits::MinWeighingFactor, ScaledInteger::UShort);
        expected += JAUS_USHORT_SIZE;

        if(expected == read)
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
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportGlobalPathSegment::ClearMessageBody()
{
    mPresenceVector =        0;
    mPathSegmentNumber =    0;
    mP1Latitude =            0;
    mP1Longitude =            0;
    mP1Altitude =            0;
    mP2Latitude =            0;
    mP2Longitude =            0;
    mP2Altitude =            0;
    mWeighingFactor =        0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalPathSegment &ReportGlobalPathSegment::operator=(const ReportGlobalPathSegment &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector =        msg.mPresenceVector;    
    mPathSegmentNumber =    msg.mPathSegmentNumber;
    mP1Latitude =            msg.mP1Latitude;        
    mP1Longitude =            msg.mP1Longitude;
    mP1Altitude =            msg.mP1Altitude;    
    mP2Latitude =            msg.mP2Latitude;        
    mP2Longitude =            msg.mP2Longitude;
    mP2Altitude =            msg.mP2Altitude;    
    mWeighingFactor =        msg.mWeighingFactor;
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
UShort ReportGlobalPathSegment::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt ReportGlobalPathSegment::GetPresenceVectorMask(const UShort version) const { return 0x3; }

/*  End of File */
