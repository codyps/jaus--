////////////////////////////////////////////////////////////////////////////////////
///
///  \file attitude.cpp
///  \brief This file contains a simle data structure for representing/storing
///  attitude information in JAUS.  The primary information is yaw, pitch, and
///  roll data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 6 February 2007
///  <br>Last Modified: 17 July 2007
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
#include "jaus/messages/common/platform/attitude.h"
#include <iostream>
#include <assert.h>
#include <math.h>

using namespace Jaus;
using namespace std;

const double Attitude::Limits::MaxAttitude = JAUS_PI;
const double Attitude::Limits::MinAttitude = -JAUS_PI;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Attitude::Attitude()
{
    mRoll = mPitch = mYaw = 0.0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the attitude information.
///
///   \param roll Roll in radians [-PI,PI].
///   \param pitch Pitch in radians [-PI,PI].
///   \param yaw yaw in radians [-PI,PI].
///
////////////////////////////////////////////////////////////////////////////////////
Attitude::Attitude(const double roll, 
                   const double pitch,
                   const double yaw)
{
    mRoll = mPitch = mYaw = 0.0;
    SetAttitude(roll, pitch, yaw);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Attitude::Attitude(const Attitude &att)
{
    *this = att;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Attitude::~Attitude() 
{
    ClearAttitude();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the attitude information.
///
///   \param roll Roll in radians [-PI,PI].
///   \param pitch Pitch in radians [-PI,PI].
///   \param yaw yaw in radians [-PI,PI].
///
///   \return 1 if attitude set, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::SetAttitude(const double roll, 
                          const double pitch,
                          const double yaw)
{
    if(roll >= Limits::MinAttitude  && roll <= Limits::MaxAttitude &&
       pitch >= Limits::MinAttitude && pitch <= Limits::MaxAttitude &&
       yaw >= Limits::MinAttitude   && yaw <= Limits::MaxAttitude)
    {
        mRoll = roll;
        mPitch = pitch;
        mYaw = yaw;
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the attitude information.
///
///   \param roll Roll in radians [-PI,PI].
///   \param pitch Pitch in radians [-PI,PI].
///   \param yaw yaw in radians [-PI,PI].
///
///   \return 1 if all attitude data was available, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::GetAttitude(double &roll, 
                           double &pitch, 
                           double &yaw) const
{
    roll = mRoll;
    pitch = mPitch;
    yaw = mYaw;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value of the attitude data.
///
///   \param pitch Pitch in radians.
///
///   \return 1 if able to get data or data exists, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::GetPitch(double &pitch) const
{
    pitch = mPitch;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value of the attitude data.
///
///   \param roll Roll in radians.
///
///   \return 1 if able to get data or data exists, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::GetRoll(double &roll) const
{
    roll = mRoll;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value of the attitude data.
///
///   \param yaw Yaw in radians.
///
///   \return 1 if able to get data or data exists, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::GetYaw(double &yaw) const
{
    yaw = mYaw;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the attitude data.
///
///   \param pitch Pitch to set/add to Attitude structure in radians [-PI,PI].
///
///   \return 1 if data is added, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::SetPitch(const double pitch)
{
    if(pitch >= Limits::MinAttitude && pitch <= Limits::MaxAttitude)
    {
        mPitch= pitch;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the attitude data.
///
///   \param roll Roll to set/add to Attitude structure in radians [-PI,PI].
///
///   \return 1 if data is added, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::SetRoll(const double roll)
{
    if(roll >= Limits::MinAttitude && roll <= Limits::MaxAttitude)
    {
        mRoll = roll;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the attitude data.
///
///   \param yaw Yaw to set/add to Attitude structure in radians [-PI,PI].
///
///   \return 1 if data is added, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int Attitude::SetYaw(const double yaw)
{
    if(yaw >= Limits::MinAttitude && yaw <= Limits::MaxAttitude)
    {
        mYaw = yaw;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any attitude data.
///
////////////////////////////////////////////////////////////////////////////////////
void Attitude::ClearAttitude()
{
    mPitch = mYaw = mRoll = 0.0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any attitude data.
///
////////////////////////////////////////////////////////////////////////////////////
void Attitude::ClearRoll()
{
    mRoll = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any attitude data.
///
////////////////////////////////////////////////////////////////////////////////////
void Attitude::ClearPitch()
{
    mPitch = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any attitude data.
///
////////////////////////////////////////////////////////////////////////////////////
void Attitude::ClearYaw()
{
    mYaw = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out attitude data to console window.
///
///   \param degrees Print values in degrees true/false.
///
////////////////////////////////////////////////////////////////////////////////////
void Attitude::PrintAttitude(const bool degrees) const
{
    cout << "Yaw: " << mYaw << ", Pitch: " << mPitch << ", Roll: " << mRoll << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value if it exits, otherwise return 0.0.  Use HavePitch
///   function to determine the data is available.
///
///   \return Pitch value if it exists in radians, otherwise 0.0.  Use HavePitch
///   function to determine if data is available.
///
////////////////////////////////////////////////////////////////////////////////////
double Attitude::GetPitch() const
{
    return mPitch;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value if it exits, otherwise return 0.0.  Use HaveRoll
///   function to determine the data is available.
///
///   \return Roll value if it exists in radians, otherwise 0.0.  Use HaveRoll
///   function to determine if data is available.
///
////////////////////////////////////////////////////////////////////////////////////
double Attitude::GetRoll() const
{
    return mRoll;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value if it exits, otherwise return 0.0.  Use HaveYaw
///   function to determine the data is available.
///
///   \return Yaw value if it exists in radians, otherwise 0.0.  Use HaveYaw
///   function to determine if data is available.
///
////////////////////////////////////////////////////////////////////////////////////
double Attitude::GetYaw() const
{
    return mYaw;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator. (sets equal to).
///
////////////////////////////////////////////////////////////////////////////////////
Attitude &Attitude::operator =(const Attitude &att)
{
    mRoll = att.mRoll;
    mPitch = att.mPitch;
    mYaw = att.mYaw;

    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the differance between two heading angles, with the differance
///   being relative to the first heading.
///
///   For example, if a = 10 degrees and b = 40 degrees, than the angle differance
///   will be 30.  If a = 10 degrees and b = -20, than the result will be -30.
///   This is useful when trying to determine if a platform needs to rotate to
///   the left or right of an angle.
///
///   \param currentAngle The angle currently being faced along an axis. 
///                      [-180, 180], [-pi, pi] depending on degrees.
///   \param desiredAngle The angle to get the differance to, relative to  
///                       currentAngle. [-180, 180], or [-pi, pi] depending 
///                       on degrees.
///   \param degrees If true, currentAngle and desiredAngle are in degrees, 
///                  otherwise radians.
///
///   \return The differance in the angles relative to angle currentAngle.  
///           Depending on value of degrees parameter, returned number is in
///           degrees or radians.
///
////////////////////////////////////////////////////////////////////////////////////
double Attitude::GetHeadingDifferance(const double currentAngle, const double desiredAngle, const bool degrees) 
{
    double curAngleRadians = currentAngle;
    double desAngleRadians = desiredAngle;

    double desPos = 0;
    double desNeg = 0;

    double errPos = 0;
    double errNeg = 0;

    if(degrees) {
        curAngleRadians = JAUS_DEG2RAD(currentAngle);
        desAngleRadians = JAUS_DEG2RAD(desiredAngle);
    }

    if(desAngleRadians > 0)
    {
        desPos = desAngleRadians;
        desNeg = desAngleRadians - 2 * JAUS_PI;
    }
    else
    {
        desPos = desAngleRadians + 2 * JAUS_PI;
        desNeg = desAngleRadians;
    }

    errPos = desPos - curAngleRadians;
    errNeg = desNeg - curAngleRadians;

    if(fabs(errPos) < fabs(errNeg))
    {
        return errPos;
    }
    else
    {
        return errNeg;
    }

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a specific angle to a heading/pitch/roll value and
///   if the resulting addition rolls passed the plus or minus 180, then this
///   method will wrap around.
///
///   \param currentAngle The current angle to add some data to.
///   \param angleDelta How much to add [-PI/PI].
///   \param degrees If true, then the input parameters are in degrees and
///                  the result will be in degrees, otherwise radians.
///
///   \return The new angle after adding the angleDelta.
///
////////////////////////////////////////////////////////////////////////////////////
double Attitude::AddAngle(const double currentAngle, const double angleDelta, const bool degrees)
{
    double result = currentAngle + angleDelta;

    if(degrees)
    {
        if(result > 180)
        {
            result -= 360;
        }
        else if(result < -180)
        {
            result += 360;
        }
    }
    else
    {
        if(result > JAUS_PI)
        {
            result -= JAUS_TWO_PI;
        }
        else if(result < -JAUS_PI)
        {
            result += JAUS_TWO_PI;
        }
    }

    return result;
}

/*  End of File */
