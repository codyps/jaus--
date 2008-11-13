////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalposition.cpp
///  \brief This file contains a simple data structure for global position
///  data in JAUS.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
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
#include "jaus/messages/common/platform/globalposition.h"
#include <assert.h>
#include <iostream>

using namespace std;
using namespace Jaus;

const double GlobalPosition::Limits::MaxLatitude = 90.0;
const double GlobalPosition::Limits::MaxLongitude = 180;
const double GlobalPosition::Limits::MaxElevation = 35000.0;
const double GlobalPosition::Limits::MinLatitude = -90.0;
const double GlobalPosition::Limits::MinLongitude = -180;
const double GlobalPosition::Limits::MinElevation = -10000.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition::GlobalPosition()
{
    mHaveElevation = false;
    mLatitude = mLongitude = 0;
    mElevation = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position values.
///
///   \param lat Latitude [-90, 90] degrees.
///   \param lon Longitude [-180, 180] degrees
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition::GlobalPosition(const double lat,
                               const double lon)
{
    mLatitude = mLongitude = 0;
    mElevation = 0;
    SetLatitude(lat);
    SetLongitude(lon);
    mHaveElevation = false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position values.
///
///   \param lat Latitude [-90, 90] degrees.
///   \param lon Longitude [-180, 180] degrees.
///   \param elev Elevation [-10000,30000] meters above sea level.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition::GlobalPosition(const double lat,
                               const double lon,
                               const double elev)
{
    mLatitude = mLongitude = 0;
    mElevation = 0;
    SetPosition(lat, lon, elev);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition::GlobalPosition(const GlobalPosition &pos)
{
    *this = pos;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition::~GlobalPosition() 
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position values with a single function. Performs checking for
///   valid values.
///
///   \param lat Latitude [-90, 90] degrees.
///   \param lon Longitude [-180, 180] degrees.
///   \param elev Elevation [-10000,30000] meters above sea level.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::SetPosition(const double lat,
                                const double lon,
                                const double elev)
{
    if(lat >= Limits::MinLatitude && lat <= Limits::MaxLatitude &&
       lon >= Limits::MinLongitude && lon <= Limits::MaxLongitude &&
       elev >= Limits::MinElevation && elev <= Limits::MaxElevation)
    {
        mLatitude = lat;
        mLongitude = lon;
        mElevation = elev;
        mHaveElevation = true;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position value. Performs checking for
///   valid values.  If value is invalid, then value not set.
///
///   \param lat Latitude [-90, 90] degrees.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::SetLatitude(const double lat)
{
    if(lat >= Limits::MinLatitude && lat <= Limits::MaxLatitude)
    {
        mLatitude = lat;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position value. Performs checking for
///   valid values.  If value is invalid, then value not set.
///
///   \param lon Longitude [-180, 180] degrees.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::SetLongitude(const double lon)
{
    if(lon >= Limits::MinLongitude && lon <= Limits::MaxLongitude)
    {
        mLongitude = lon;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the position value. Performs checking for
///   valid values.  If value is invalid, then value not set.
///
///   \param elev Elevation [Limits::MinElevation, Limits::MaxElevation] meters.
///
///   \return JAUS_OK if values set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::SetElevation(const double elev)
{
    if(elev >= Limits::MinElevation && elev <= Limits::MaxElevation)
    {
        mElevation = elev;
        mHaveElevation = true;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the position values with a single function.  Elevation
///   data may not have been set (see HaveElevation() ).  If this is the
///   case than the function will fail.
///
///   \param lat Latitude [-90, 90] degrees.
///   \param lon Longitude [-90, 90] degrees.
///   \param elev Elevation [Limits::MinElevation, Limits::MaxElevation] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::GetPosition(double &lat,
                                double &lon,
                                double &elev) const
{
    lat = mLatitude;
    lon = mLongitude;
    elev = mElevation;
    if(!mHaveElevation)
        return JAUS_FAILURE;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the position value.
///
///   \param lat Latitude [-90,90] degrees.
///
///   \return JAUS_OK if OK, otherwise JAUS_FAILURE if could not get value.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::GetLatitude(double &lat) const
{
    lat = mLatitude;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the position value.
///
///   \param lon Longitude [-180,180] degrees.
///
///   \return JAUS_OK if OK, otherwise JAUS_FAILURE if could not get value.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::GetLongitude(double &lon) const
{
    lon = mLongitude;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the position value.
///
///   \param elev Elevation [Limits::MinElevation, Limits::MaxElevation] meters.
///
///   \return JAUS_OK if OK, otherwise JAUS_FAILURE if could not get value.
///
////////////////////////////////////////////////////////////////////////////////////
int GlobalPosition::GetElevation(double &elev) const
{
    if(mHaveElevation) {
        elev = mElevation;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all Position data.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPosition::ClearPosition()
{
    mLatitude = mLongitude = 0;
    mElevation = 0;
    mHaveElevation = false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPosition::ClearLatitude()
{
    mLatitude = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPosition::ClearLongitude()
{
    mLongitude = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the value.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPosition::ClearElevation()
{
    mElevation = 0;
    mHaveElevation = false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out any data to the console window if it exists.
///
////////////////////////////////////////////////////////////////////////////////////
void GlobalPosition::PrintPosition() const
{
        cout << "Latitude: " << mLatitude << " "
             << "Longitude: " << mLongitude << " ";
        if(mHaveElevation)
             cout << "Elevation: " << mElevation << "\n";
        else
            cout << "\n";
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalPosition::GetLatitude() const
{
    return mLatitude;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalPosition::GetLongitude() const
{
    return mLongitude;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the value.  If no elevation data has been set, than
///   junk data values will be returned.
///
////////////////////////////////////////////////////////////////////////////////////
double GlobalPosition::GetElevation() const
{
    return mElevation;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
GlobalPosition &GlobalPosition::operator=(const GlobalPosition &pos)
{
    mLatitude = pos.mLatitude;
    mLongitude = pos.mLongitude;
    mElevation = pos.mElevation;
    mHaveElevation = pos.mHaveElevation;
    return *this;
}

/*  End of File */
