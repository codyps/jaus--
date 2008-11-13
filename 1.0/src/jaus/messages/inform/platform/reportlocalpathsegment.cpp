////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalpathsegment.cpp
///  \brief This file contains the message structure for Query Local Path Segment
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
#include "jaus/messages/inform/platform/reportlocalpathsegment.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UShort ReportLocalPathSegment::Limits::MinPathSegmentNumber =  0;    
const UShort ReportLocalPathSegment::Limits::MaxPathSegmentNumber =  65535;    
const double ReportLocalPathSegment::Limits::MinP1X =                -100000;        
const double ReportLocalPathSegment::Limits::MaxP1X =                100000;
const double ReportLocalPathSegment::Limits::MinP1Y =                -100000;        
const double ReportLocalPathSegment::Limits::MaxP1Y =                100000;    
const double ReportLocalPathSegment::Limits::MinP1Z =                -10000;
const double ReportLocalPathSegment::Limits::MaxP1Z =                35000;    
const double ReportLocalPathSegment::Limits::MinP2X =                -100000;        
const double ReportLocalPathSegment::Limits::MaxP2X =                100000;
const double ReportLocalPathSegment::Limits::MinP2Y =                -100000;        
const double ReportLocalPathSegment::Limits::MaxP2Y =                100000;    
const double ReportLocalPathSegment::Limits::MinP2Z =                -10000;
const double ReportLocalPathSegment::Limits::MaxP2Z =                35000;    
const double ReportLocalPathSegment::Limits::MinWeightingFactor =        0;        
const double ReportLocalPathSegment::Limits::MaxWeightingFactor =        500;        

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalPathSegment::ReportLocalPathSegment() : 
                        Message(JAUS_REPORT_LOCAL_PATH_SEGMENT)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalPathSegment::ReportLocalPathSegment(const ReportLocalPathSegment &msg) : 
                        Message(JAUS_REPORT_LOCAL_PATH_SEGMENT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalPathSegment::~ReportLocalPathSegment()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the presence for the query.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a path segment number
///
///   \param val Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetPathSegmentNumber(const UInt val)
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
///   \brief Sets P1X
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP1X(const double val)
{
    if(val >= Limits::MinP1X && val <= Limits::MaxP1X)
    {
        mP1X = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets P1Y
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP1Y(const double val)
{
    if(val >= Limits::MinP1Y && val <= Limits::MaxP1Y)
    {
        mP1Y = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets P1Z
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP1Z(const double val)
{
    if(val >= Limits::MinP1Z && val <= Limits::MaxP1Z)
    {
        mP1Z = val;
        mPresenceVector |= VectorMask::P1Z;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets P2X
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP2X(const double val)
{
    if(val >= Limits::MinP2X && val <= Limits::MaxP2X)
    {
        mP2X = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets P2Y
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP2Y(const double val)
{
    if(val >= Limits::MinP2Y && val <= Limits::MaxP2Y)
    {
        mP2Y = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets P2Z
///
///   \param val Integer, Meters, Scaled Integer,
///                 Lower Limit =  -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetP2Z(const double val)
{
    if(val >= Limits::MinP2Z && val <= Limits::MaxP2Z)
    {
        mP2Z = val;
        mPresenceVector |= VectorMask::P2Z;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets weighing factor
///
///   \param val Unsigned Short Integer, Scaled Integer, Range 0 to 500. 
///                 Zero = a straight line
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalPathSegment::SetWeightingFactor(const double val)
{
    if(val >= Limits::MinWeightingFactor && val <= Limits::MaxWeightingFactor)
    {
        mWeightingFactor = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

double ReportLocalPathSegment::GetP1Z() const
{
    if(mPresenceVector & VectorMask::P1Z)
    {
        return mP1Z;
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}
double ReportLocalPathSegment::GetP2Z() const
{
    if(mPresenceVector & VectorMask::P2Z)
    {
        return mP2Z;
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
int ReportLocalPathSegment::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;

        written +=  msg.Write(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        written +=  msg.Write(mPathSegmentNumber);
        expected += JAUS_UINT_SIZE;

        written +=  msg.Write(mP1X,Limits::MaxP1X,Limits::MinP1X,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written +=  msg.Write(mP1Y,Limits::MaxP1Y,Limits::MinP1Y,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P1Z)
        {
            written +=  msg.Write(mP1Z,Limits::MaxP1Z,Limits::MinP1Z,ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        written +=  msg.Write(mP2X,Limits::MaxP2X,Limits::MinP2X,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written +=  msg.Write(mP2Y,Limits::MaxP2Y,Limits::MinP2Y,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector & VectorMask::P2Z)
        {
            written +=  msg.Write(mP2Z,Limits::MaxP2Z,Limits::MinP2Z,ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }

        written +=  msg.Write(mWeightingFactor,Limits::MaxWeightingFactor,Limits::MinWeightingFactor,ScaledInteger::UShort);
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
int ReportLocalPathSegment::ReadMessageBody(const Stream& msg, 
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
void ReportLocalPathSegment::ClearMessageBody()
{
    mPresenceVector =        0;
    mPathSegmentNumber =    0;
    mP1X =                    0;            
    mP1Y =                    0;            
    mP1Z =                    0;            
    mP2X =                    0;            
    mP2Y =                    0;            
    mP2Z =                    0;            
    mWeightingFactor =        0;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalPathSegment &ReportLocalPathSegment::operator=(const ReportLocalPathSegment &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector =        msg.mPresenceVector;    
    mPathSegmentNumber =    msg.mPathSegmentNumber;
    mP1X =                    msg.mP1X;
    mP1Y =                    msg.mP1Y;
    mP1Z =                    msg.mP1Z;
    mP2X =                    msg.mP2X;
    mP2Y =                    msg.mP2Y;
    mP2Z =                    msg.mP2Z;
    mWeightingFactor =        msg.mWeightingFactor;
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
UShort ReportLocalPathSegment::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }

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
UInt ReportLocalPathSegment::GetPresenceVectorMask(const UShort version) const { return 0x3; }


/*  End of File */
