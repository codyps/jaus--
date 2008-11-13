////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalwaypoint.cpp
///  \brief This file contains the message structure for Report Local Waypoint
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

#include "jaus/messages/inform/platform/reportlocalwaypoint.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UInt ReportLocalWaypoint::Limits::MinWaypointNumber =    0;
const UInt ReportLocalWaypoint::Limits::MaxWaypointNumber =    65535;
const double ReportLocalWaypoint::Limits::MinX =            -100000;                            
const double ReportLocalWaypoint::Limits::MaxX =            100000;            
const double ReportLocalWaypoint::Limits::MinY =            -100000;            
const double ReportLocalWaypoint::Limits::MaxY =            100000;            
const double ReportLocalWaypoint::Limits::MinZ =            -10000;            
const double ReportLocalWaypoint::Limits::MaxZ =            35000;            
const double ReportLocalWaypoint::Limits::MinRoll =            -JAUS_PI;                        
const double ReportLocalWaypoint::Limits::MaxRoll =            JAUS_PI;        
const double ReportLocalWaypoint::Limits::MinPitch =        -JAUS_PI;        
const double ReportLocalWaypoint::Limits::MaxPitch =        JAUS_PI;        
const double ReportLocalWaypoint::Limits::MinYaw =            -JAUS_PI;                
const double ReportLocalWaypoint::Limits::MaxYaw =            JAUS_PI;                
    

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalWaypoint::ReportLocalWaypoint(): Message(JAUS_REPORT_LOCAL_WAYPOINT)
{
    ClearMessageBody();
    
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalWaypoint::ReportLocalWaypoint(const ReportLocalWaypoint &msg): Message(JAUS_REPORT_LOCAL_WAYPOINT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalWaypoint::~ReportLocalWaypoint()
{

}

int ReportLocalWaypoint::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Waypoint number.
///
///   \param val Unsigned Integer, [0,65535].
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetWaypointNumber(const UInt val)
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
///   \brief Sets a X position value.
///
///   \param val Integer, Meters, Scaled Integer, 
///                 Lower Limit = -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetX(const double val)
{
    if(val >= Limits::MinX && val <= Limits::MaxX)
    {
        mX=val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Y position value.
///
///   \param val Integer, Meters, Scaled Integer, 
///                 Lower Limit = -100,000, Upper Limit = 100,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetY(const double val)
{
    if(val >= Limits::MinY && val <= Limits::MaxY)
    {
        mY=val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Z position value.
///
///   \param val Integer, Meters, Scaled Integer, 
///                 Lower Limit = -10,000, Upper Limit = 35,000
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetZ(const double val)
{
    if(val >= Limits::MinZ && val <= Limits::MaxZ)
    {
        mZ=val;
        mPresenceVector |= VectorMask::Z;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Roll value 
///
///   \param val Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI 
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetRoll(const double val)
{
    if(val >= Limits::MinRoll && val <= Limits::MaxRoll)
    {
        mRoll=val;
        mPresenceVector |= VectorMask::Roll;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Pitch value 
///
///   \param val Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI 
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetPitch(const double val)
{
    if(val >= Limits::MinPitch && val <= Limits::MaxPitch)
    {
        mPitch=val;
        mPresenceVector |= VectorMask::Pitch;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a Yaw value 
///
///   \param val Radians, Scaled Integer, Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI 
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalWaypoint::SetYaw(const double val)
{
    if(val >= Limits::MinYaw && val <= Limits::MaxYaw)
    {
        mYaw=val;
        mPresenceVector |= VectorMask::Yaw;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

UInt ReportLocalWaypoint::GetWaypointNumber()const
{
    return mWaypointNumber;
}
double ReportLocalWaypoint::GetX()const
{ 
    return mX; 
}

double ReportLocalWaypoint::GetY()const
{
    return mY;
}

double ReportLocalWaypoint::GetZ()const
{ 
    if(mPresenceVector & VectorMask::Z)
    {
        return mZ;
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}

double ReportLocalWaypoint::GetRoll()const
{ 
    if(mPresenceVector & VectorMask::Roll)
    {
        return mRoll; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}

double ReportLocalWaypoint::GetPitch()const
{ 
    if(mPresenceVector & VectorMask::Pitch)
    {
        return mPitch; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent); 
}

double ReportLocalWaypoint::GetYaw()const
{ 
    if(mPresenceVector & VectorMask::Yaw)
    {
        return mYaw; 
    }
    return SetJausError(ErrorCodes::FieldNotPresent);
}

Byte ReportLocalWaypoint::GetPresenceVector() const 
{ 
    return mPresenceVector; 
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
int ReportLocalWaypoint::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int write = 0;
        
        write += msg.Write(mPresenceVector);
        expected = JAUS_BYTE_SIZE;
       
        write += msg.Write(mX, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;
       
        write += msg.Write(mY, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        write += msg.Write(mZ, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        if(mPresenceVector&VectorMask::Roll) 
        {
            write += msg.Write(mRoll, Limits::MaxRoll, Limits::MinRoll, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }
        
        if(mPresenceVector&VectorMask::Pitch) 
        {
            write += msg.Write(mPitch, Limits::MaxPitch, Limits::MinPitch, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }

        if(mPresenceVector&VectorMask::Yaw) 
        {
            write += msg.Write(mYaw, Limits::MaxYaw, Limits::MinYaw, ScaledInteger::Short);
            expected += JAUS_SHORT_SIZE;
        }

        if( expected == write ) 
        {
            return write;
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
int ReportLocalWaypoint::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        
        read += msg.Read(mPresenceVector);
        expected = JAUS_BYTE_SIZE;
       
        read += msg.Read(mX, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;
       
        read += msg.Read(mY, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        read += msg.Read(mZ, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

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
///   \brief Sets all local variables to 0.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportLocalWaypoint::ClearMessageBody()
{
    mPresenceVector =    0;
    mX =                0;
    mY =                0;
    mZ =                0;
    mRoll =                0;
    mPitch =            0;
    mYaw =                0;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalWaypoint &ReportLocalWaypoint::operator=(const ReportLocalWaypoint &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector =    msg.mPresenceVector;   
    mX =                msg.mX;                
    mY =                msg.mY;                
    mZ =                msg.mZ;                
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
UShort ReportLocalWaypoint::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }

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
UInt ReportLocalWaypoint::GetPresenceVectorMask(const UShort version) const { return 0xF; }

// End of File
