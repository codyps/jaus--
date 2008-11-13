////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalpose.cpp
///  \brief This file contains a simple data structure for global pose
///  data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 24 March 2008
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
#include "jaus/messages/common/platform/globalpose.h"
#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace std;
using namespace Jaus;

const double GlobalPose::Limits::MaxLatitude = 90.0;
const double GlobalPose::Limits::MaxLongitude = 180;
const double GlobalPose::Limits::MaxElevation = 35000.0;
const double GlobalPose::Limits::MinLatitude = -90.0;
const double GlobalPose::Limits::MinLongitude = -180;
const double GlobalPose::Limits::MinElevation = -10000.0;
const double GlobalPose::Limits::MaxAttitude = JAUS_PI;
const double GlobalPose::Limits::MinAttitude = -JAUS_PI;
const double GlobalPose::Limits::MinPositionRms = 0.0;
const double GlobalPose::Limits::MaxPositionRms = 100.0;
const double GlobalPose::Limits::MaxAttitudeRms = JAUS_PI;
const double GlobalPose::Limits::MinAttitudeRms = 0.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose::GlobalPose()
{
    mYaw = mPitch = mRoll = 0;
    mLatitude = mLongitude = mElevation = 0;
    mPositionRms = mAttitudeRms = 0;
    mPresenceVector = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose::GlobalPose(const GlobalPose &pose)
{
    *this = pose;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose::~GlobalPose()
{
    ClearGlobalPose();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position value. Performs checking for
///   valid values.
///
///   \param lat Latitude [-90, 90] degrees.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetLatitude(const double lat)
{
    if(lat >= Limits::MinLatitude && lat <= Limits::MaxLatitude)
    {
        mPresenceVector |= VectorMask::Latitude;
        mLatitude = lat;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position value. Performs checking for
///   valid values.
///
///   \param lon Longitude [-180, 180] degrees.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetLongitude(const double lon)
{
    if(lon >= Limits::MinLongitude && lon <= Limits::MaxLongitude)
    {
        mPresenceVector |= VectorMask::Longitude;
        mLongitude = lon;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the elevation value. Performs checking for
///   valid values.
///
///   \param elev Elevation [-10,000, 30,000] meters.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetElevation(const double elev)
{
    if(elev >= Limits::MinElevation && elev <= Limits::MaxElevation)
    {
        mPresenceVector |= VectorMask::Elevation;
        mElevation = elev;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position data. Performs checking for
///   valid values.
///
///   \param lat Latitude [-90, 90] degrees.
///   \param lon Longitude [-180, 180] degrees.
///   \param elev Elevation [-10,000, 30,000] meters.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetPosition(const double lat, const double lon, const double elev)
{
    if( (lat >= GlobalPose::Limits::MinLatitude && lat <= GlobalPose::Limits::MaxLatitude) &&
        (lon >= GlobalPose::Limits::MinLongitude && lon <= GlobalPose::Limits::MaxLongitude) &&
        (elev >= GlobalPose::Limits::MinElevation && elev <= GlobalPose::Limits::MaxElevation) )
    {
        mElevation = elev;
        mLatitude = lat;
        mLongitude = lon;
        mPresenceVector |= VectorMask::Latitude;
        mPresenceVector |= VectorMask::Longitude;
        mPresenceVector |= VectorMask::Elevation;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the yaw value for pose.
///
///   \param yaw Yaw angle in radians [-pi,pi] (rotation around z-axis).
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetYaw(const double yaw)
{
    if(yaw >= Limits::MinAttitude && yaw <= Limits::MaxAttitude)
    {
        mPresenceVector |= VectorMask::Yaw;
        mYaw = yaw;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the pitch value for pose.
///
///   \param pitch Pitch angle in radians [-pi,pi] (rotation around y-axis).
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetPitch(const double pitch)
{
    if(pitch >= Limits::MinAttitude && pitch <= Limits::MaxAttitude)
    {
        mPresenceVector |= VectorMask::Pitch;
        mPitch = pitch;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the roll value for pose.
///
///   \param roll Roll angle in radians [-pi,pi] (rotation around x-axis).
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetRoll(const double roll)
{
    if(roll >= Limits::MinAttitude && roll <= Limits::MaxAttitude)
    {
        mPresenceVector |= VectorMask::Roll;
        mRoll = roll;
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the attitude data. Performs checking for
///   valid values.
///
///   \param yaw Yaw angle in radians [-pi,pi] (rotation around z-axis).
///   \param pitch Pitch angle in radians [-pi,pi] (rotation around y-axis).
///   \param roll Roll angle in radians [-pi,pi] (rotation around x-axis).
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE if nothing set.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetAttitude(const double roll, const double pitch, const double yaw)
{
    if( (roll >= Limits::MinAttitude && roll <= Limits::MaxAttitude) &&
        (yaw >= Limits::MinAttitude && yaw <= Limits::MaxAttitude) &&
        (pitch >= Limits::MinAttitude && pitch <= Limits::MaxAttitude) )
    {
        mYaw = yaw;
        mPitch = pitch;
        mRoll = roll;

        mPresenceVector |= VectorMask::Yaw;
        mPresenceVector |= VectorMask::Pitch;
        mPresenceVector |= VectorMask::Roll;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position root-mean-square error.
///
///   \param prms Position rms [JAUS_MIN_POSITION_RMS,JAUS_MAX_POSITION_RMS]
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetPositionRms(const double prms)
{
    if(prms >= Limits::MinPositionRms && prms <= Limits::MaxPositionRms)
    {
        mPresenceVector |= VectorMask::PositionRms;
        mPositionRms = prms;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the attitude root-mean-square error.
///
///   \param arms Attitude rms [JAUS_MIN_ATTITUDE_RMS,JAUS_MAX_ATTITUDE_RMS]
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetAttitudeRms(const double arms)
{
    if(arms >= Limits::MinAttitudeRms && arms <= Limits::MaxAttitudeRms)
    {
        mPresenceVector |= VectorMask::AttitudeRms;
        mAttitudeRms = arms;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the timestamp.
///
///   \param tstamp Timestamp of data.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::SetTimeStamp(const Time &tstamp)
{
    mTimeStamp = tstamp;
    mPresenceVector |= VectorMask::TimeStamp;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
///   \param lat Latitude [-90, 90] degrees.
///
///   \return JAUS_OK if value retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetLatitude(double &lat) const
{
    lat = mLatitude;
    return (mPresenceVector&VectorMask::Latitude) ? JAUS_OK : JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
///   \param lon Longitude [-180, 180] degrees.
///
///   \return JAUS_OK if value retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetLongitude(double &lon) const
{
    lon = mLongitude;
    return (mPresenceVector&VectorMask::Longitude) ? JAUS_OK : JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
///   \param elev Elevation [-10,000, 30,000] meters.
///
///   \return JAUS_OK if value retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetElevation(double &elev) const
{
    elev = mElevation;
    return (mPresenceVector&VectorMask::Elevation) ? JAUS_OK : JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the attitude data.
///
///   \param yaw Yaw angle in radians [-pi,pi] (rotation around z-axis).
///
///   \return JAUS_OK if retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetYaw(double &yaw) const
{
    yaw = mYaw;
    return (mPresenceVector&VectorMask::Yaw) ? JAUS_OK : JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the attitude data.
///
///   \param pitch Pitch angle in radians [-pi,pi] (rotation around y-axis).
///
///   \return JAUS_OK if retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetPitch(double &pitch) const
{
    pitch = mPitch;
    return (mPresenceVector&VectorMask::Pitch) ? JAUS_OK : JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the attitude data.
///
///   \param roll Roll angle in radians [-pi,pi] (rotation around x-axis).
///
///   \return JAUS_OK if retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetRoll(double &roll) const
{
    roll = mRoll;
    return (mPresenceVector&VectorMask::Roll) ? JAUS_OK : JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the presence vector.  This determines what information
///   is present within the data structure.
///
///   \param v Presence vector.
///
///   \return JAUS_OK if retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetPresenceVector(UShort& v) const
{
    v = mPresenceVector;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the rms value.
///
///   \param prms Position rms.
///
///   \return JAUS_OK if data available, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetPositionRms(double &prms) const
{
    prms = mPositionRms;
    return (mPresenceVector&VectorMask::PositionRms) ? JAUS_OK : JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the rms value.
///
///   \param arms Attitude rms.
///
///   \return JAUS_OK if data available, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetAttitudeRms(double &arms) const
{
    arms = mAttitudeRms;
    return (mPresenceVector&VectorMask::AttitudeRms) ? JAUS_OK : JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
///   \param tstamp Time and Date stamp.
///
///   \return JAUS_OK if data available, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPose::GetTimeStamp(Time &tstamp) const
{
    tstamp = mTimeStamp;
    return (mPresenceVector&VectorMask::TimeStamp) ? JAUS_OK : JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out global pose data to console.
///
///   \param degrees If true attitude is displayed in degrees, otherwise radians.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::PrintGlobalPose(const bool degrees) const
{
    if( (mPresenceVector& 0xFFFF) == 0 )
        return;

    cout << "Global Pose:" << endl;
    if(mPresenceVector&VectorMask::Latitude)
        cout << setprecision(10) << "Latitude: " << mLatitude << " ";
    if(mPresenceVector&VectorMask::Longitude)
        cout << setprecision(10) << "Longitude: " << mLongitude << " ";
    if(mPresenceVector&VectorMask::Elevation)
        cout << "Elevation: " << mElevation << " ";
    if(mPresenceVector&VectorMask::PositionRms)
        cout << "Position RMS: " << mPositionRms;

    cout << setprecision(4) << endl;

    if( degrees )
    {
        if(mPresenceVector&VectorMask::Yaw)
            cout << "Yaw: " << JAUS_RAD2DEG(mYaw) << " ";
        if(mPresenceVector&VectorMask::Pitch)
            cout << "Pitch: " << JAUS_RAD2DEG(mPitch) << " ";
        if(mPresenceVector&VectorMask::Roll)
            cout << "Roll: " << JAUS_RAD2DEG(mRoll) << " ";
    }
    else
    {
        if(mPresenceVector&VectorMask::Yaw)
            cout << "Yaw: " << mYaw << " ";
        if(mPresenceVector&VectorMask::Pitch)
            cout << "Pitch: " << mPitch << " ";
        if(mPresenceVector&VectorMask::Roll)
            cout << "Roll: " << mRoll << " ";
    }
    if(mPresenceVector&VectorMask::AttitudeRms)
        cout << "Attitude RMS: " << mAttitudeRms;

    cout << endl;

    if(mPresenceVector&VectorMask::TimeStamp)
    {
        mTimeStamp.PrintTime();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears global pose data.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearGlobalPose()
{
    mPresenceVector = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears global pose data.
///
///   \param field Bit position in the presence vector. (0x0001, 0x0002, 0x0004,..)
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearFields(const UShort field)
{
    mPresenceVector &= ~field;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears global pose data.
///
///   \param bit The bit number/field number.
///
///   \return True if the data if present for the field specified, otherwise
///           false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalPose::IsFieldPresent(const VectorBit::Bits bit) const
{
    return BitVector::IsBitSet(mPresenceVector, bit);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Check if multiple fields are present with a presence vector.
///
///   \param pv Presence vector. (0x0001 | 0x0002 | 0x0004)
///
///   \return True if all the fields in the pv passed are present in the structure
////          otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool GlobalPose::AreFieldsPresent(const UShort pv) const
{
    return (mPresenceVector&pv) == pv ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears yaw, pitch, and roll.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearAttitude()
{
    mYaw = mPitch = mRoll = 0;
    mPresenceVector &=  ~( VectorMask::Roll |
                           VectorMask::Pitch |
                           VectorMask::Yaw );
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearPitch()
{
    mPitch = 0;
    mPresenceVector &=  ~VectorMask::Pitch;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearRoll()
{
    mRoll = 0;
    mPresenceVector &=  ~VectorMask::Roll;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearYaw()
{
    mYaw = 0;
    mPresenceVector &=  ~VectorMask::Yaw;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears yaw, pitch, and roll.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearPosition()
{
    mLatitude = mLongitude = mElevation = 0;
    mPresenceVector &=  ~( VectorMask::Elevation |
                           VectorMask::Longitude |
                           VectorMask::Latitude );
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearElevation()
{
    mElevation = 0;
    mPresenceVector &=  ~VectorMask::Elevation;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearLongitude()
{
    mLongitude = 0;
    mPresenceVector &=  ~VectorMask::Longitude;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearLatitude()
{
    mLatitude = 0;
    mPresenceVector &=  ~VectorMask::Latitude;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearPositionRms()
{
    mPositionRms = 0;
    mPresenceVector &=  ~VectorMask::PositionRms;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearAttitudeRms()
{
    mAttitudeRms = 0;
    mPresenceVector &=  ~VectorMask::AttitudeRms;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPose::ClearTimeStamp()
{
    mTimeStamp.SetTime(0, 0, 0, 0, 0);
    mPresenceVector &=  ~VectorMask::TimeStamp;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose& GlobalPose::CopyFrom(const GlobalPose& gp)
{
    if(this != &gp) {
        mPresenceVector = gp.mPresenceVector;
        mYaw = gp.mYaw;
        mRoll = gp.mRoll;
        mPitch = gp.mPitch;
        mLatitude = gp.mLatitude;
        mLongitude = gp.mLongitude;
        mElevation = gp.mElevation;
        mPositionRms = gp.mPositionRms;
        mAttitudeRms = gp.mAttitudeRms;
        mTimeStamp = gp.mTimeStamp;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the attitude.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose &GlobalPose::operator=(const Time &tstamp)
{
    mTimeStamp = tstamp;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose &GlobalPose::operator=(const GlobalPose &gp)
{
    return CopyFrom(gp);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose &GlobalPose::operator=(const GlobalPosition& wp)
{
    SetPosition(wp.GetLatitude(), wp.GetLongitude(), wp.GetElevation());
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPose &GlobalPose::operator=(const Attitude& att)
{
    SetAttitude(att.GetRoll(), att.GetPitch(), att.GetYaw());
    return *this;
}

/*  End of File */
