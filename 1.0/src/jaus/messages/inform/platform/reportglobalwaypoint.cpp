////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalwaypoint.cpp
///  \brief This file contains the message structure for Report Global Waypoint
///
///  <br>Author(s): David Adams
///  <br>Created: 12 June 2008
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
#include "jaus/messages/inform/platform/reportglobalwaypoint.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UInt ReportGlobalWaypoint::Limits::MinWaypointNumber =    0;
const UInt ReportGlobalWaypoint::Limits::MaxWaypointNumber =    65535;
const double ReportGlobalWaypoint::Limits::MinLatitude =        -90;
const double ReportGlobalWaypoint::Limits::MaxLatitude =        90;
const double ReportGlobalWaypoint::Limits::MinLongitude =        -180;
const double ReportGlobalWaypoint::Limits::MaxLongitude =        180;
const double ReportGlobalWaypoint::Limits::MinAltitude =        -10000;
const double ReportGlobalWaypoint::Limits::MaxAltitude =        35000;
const double ReportGlobalWaypoint::Limits::MinRoll =            -JAUS_PI;
const double ReportGlobalWaypoint::Limits::MaxRoll =            JAUS_PI;
const double ReportGlobalWaypoint::Limits::MinPitch =            -JAUS_PI;
const double ReportGlobalWaypoint::Limits::MaxPitch =            JAUS_PI;
const double ReportGlobalWaypoint::Limits::MinYaw =                -JAUS_PI;
const double ReportGlobalWaypoint::Limits::MaxYaw =                JAUS_PI;



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalWaypoint::ReportGlobalWaypoint() : 
            Message(JAUS_REPORT_GLOBAL_WAYPOINT)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalWaypoint::ReportGlobalWaypoint(const ReportGlobalWaypoint &msg) : 
            Message(JAUS_REPORT_GLOBAL_WAYPOINT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportGlobalWaypoint::~ReportGlobalWaypoint()
{
}

void ReportGlobalWaypoint::ClearMessageBody()
{
    mPresenceVector = 0;    
    mWaypointNumber = 0;    
    mLatitude = 0;        
    mLongitude = 0;        
    mAltitude = 0;        
    mRoll = 0;            
    mPitch = 0;            
    mYaw = 0;                
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the presence for the query.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetPresenceVector(const Byte val)
{
    mPresenceVector = val;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a waypoint number
///
///   \param val Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetWaypointNumber(const UInt val)
{
    if(val >= Limits::MinWaypointNumber && val <= Limits::MaxWaypointNumber)
    {
        mWaypointNumber = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a latitude
///
///   \param val Integer, Degrees, Scaled Integer,    
///              Lower Limit = -90, Upper Limit = 90
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetLatitude(const double val)
{
    if(val >= Limits::MinLatitude && val <= Limits::MinLatitude)
    {
        mLatitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a longitude
///
///   \param val Integer, Degrees, Scaled Integer,    
///              Lower Limit = -180, Upper Limit = 180
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetLongitude(const double val)
{
    if(val >= Limits::MinLongitude && val <= Limits::MinLongitude)
    {
        mLongitude = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a altitude
///
///   \param val Integer, Degrees, Scaled Integer,    
///              Lower Limit = -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetAltitude(const double val)
{
    if(val >= Limits::MinAltitude && val <= Limits::MaxAltitude)
    {
        mAltitude = val;
        mPresenceVector |= VectorMask::Altitude;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a roll
///
///   \param val Short Integer, Radians, Scaled Integer, 
///                 Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetRoll(const double val)
{
    if(val >= Limits::MinRoll && val <= Limits::MaxRoll)
    {
        mRoll = val;
        mPresenceVector |= VectorMask::Roll;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a pitch
///
///   \param val Short Integer, Radians, Scaled Integer, 
///                 Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetPitch(const double val)
{
    if(val >= Limits::MinPitch && val <= Limits::MaxPitch)
    {
        mPitch = val;
        mPresenceVector |= VectorMask::Pitch;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a yaw
///
///   \param val Short Integer, Radians, Scaled Integer, 
///                 Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportGlobalWaypoint::SetYaw(const double val)
{
    if(val >= Limits::MinYaw && val <= Limits::MaxYaw)
    {
        mYaw = val;
        mPresenceVector |= VectorMask::Yaw;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}
Byte ReportGlobalWaypoint::GetPresenceVector()const
{
    return mPresenceVector;
} 
UInt ReportGlobalWaypoint::GetWaypointNumber()const
{
    return mWaypointNumber;
}
double ReportGlobalWaypoint::GetLatitude()const
{
    return mLatitude;
}
double ReportGlobalWaypoint::GetLongitude()const
{
    return mLongitude;
}
double ReportGlobalWaypoint::GetAltitude()const
{
    if(mPresenceVector & VectorMask::Altitude)
    {
        return mAltitude; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}
double ReportGlobalWaypoint::GetRoll()const
{
    if(mPresenceVector & VectorMask::Roll)
    {
        return mRoll; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}
double ReportGlobalWaypoint::GetPitch()const
{
    if(mPresenceVector & VectorMask::Pitch)
    {
        return mPitch; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}
double ReportGlobalWaypoint::GetYaw()const
{
    if(mPresenceVector & VectorMask::Yaw)
    {
        return mYaw; 
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
int ReportGlobalWaypoint::WriteMessageBody(Stream& msg, 
                                       const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected += JAUS_BYTE_SIZE;
        written += msg.Write(mPresenceVector);
        
        written += msg.Write(mWaypointNumber);
        expected += JAUS_USHORT_SIZE;
        
        written += msg.Write(mLatitude,Limits::MaxLatitude,Limits::MinLatitude,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written += msg.Write(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector&VectorMask::Altitude) 
        {
            written += msg.Write(mAltitude, Limits::MaxAltitude, Limits::MinAltitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Roll) 
        {
            written += msg.Write(mRoll, Limits::MaxRoll, Limits::MinRoll, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Pitch) 
        {
            written += msg.Write(mPitch, Limits::MaxPitch, Limits::MinPitch, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Yaw) 
        {
            written += msg.Write(mYaw, Limits::MaxYaw, Limits::MinYaw, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
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
int ReportGlobalWaypoint::ReadMessageBody(const Stream& msg, 
                                                  const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        expected += JAUS_BYTE_SIZE;
        read += msg.Read(mPresenceVector);
        
        read += msg.Read(mWaypointNumber);
        expected += JAUS_USHORT_SIZE;
        
        read += msg.Read(mLatitude,Limits::MaxLatitude,Limits::MinLatitude,ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        read += msg.Read(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector&VectorMask::Altitude) 
        {
            read += msg.Read(mAltitude, Limits::MaxAltitude, Limits::MinAltitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;
        }
        if(mPresenceVector&VectorMask::Roll) 
        {
            read += msg.Read(mRoll, Limits::MaxRoll, Limits::MinRoll, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Pitch) 
        {
            read += msg.Read(mPitch, Limits::MaxPitch, Limits::MinPitch, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        if(mPresenceVector&VectorMask::Yaw) 
        {
            read += msg.Read(mYaw, Limits::MaxYaw, Limits::MinYaw, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
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
ReportGlobalWaypoint &ReportGlobalWaypoint::operator=(const ReportGlobalWaypoint &msg)
{
    CopyHeaderData(&msg);   
    mPresenceVector    =    msg.mPresenceVector;
    mWaypointNumber =    msg.mWaypointNumber;
    mLatitude =            msg.mLatitude;    
    mLongitude =        msg.mLongitude;    
    mAltitude =            msg.mAltitude;    
    mRoll =                msg.mRoll;        
    mPitch =            msg.mPitch;        
    mYaw =                msg.mYaw;        
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
UShort ReportGlobalWaypoint::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }

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
UInt ReportGlobalWaypoint::GetPresenceVectorMask(const UShort version) const { return 0xF; }


/*  End of File */
