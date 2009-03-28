////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportsicklidar.cpp
///  \brief This file contains a custom experimental message to report from a 
///  SICK LIDAR sensor.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 19 December 2008
///  <br>Copyright (c) 2008
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
#include "jaus/messages/experimental/video/reportpixelonscreen.h"
#include "jaus/messages/experimental/experimentalcodes.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Consructor for the Data subclass
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::Data::Data()
{
    mX = 0.0f;
    mY = 0.0f;
    mOrientation.Set(0, 0, 0);
    mPosition.Set(0, 0, 0);
    mDistance = 0.0f;
    mAngle = 0.0f;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy Consructor for the Data subclass
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::Data::Data(const Data& data)
{
    *this = data;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deconsructor for the Data subclass
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::Data::~Data()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::Data& ReportPixelOnScreen::Data::operator=(const Data& data) 
{
    mX = data.mX;
    mY = data.mY;
    mModelType = data.mModelType;
    mActorName = data.mActorName;   
    mOrientation = data.mOrientation;
    mPosition = data.mPosition;
    mVehiclePosition = data.mVehiclePosition;
    mDistance = data.mDistance;
    mAngle = data.mAngle;

    return *this;
}




////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Consructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::ReportPixelOnScreen() : Message(JAUS_REPORT_PIXEL_ON_SCREEN)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy consructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::ReportPixelOnScreen(const ReportPixelOnScreen& report) : Message(JAUS_REPORT_PIXEL_ON_SCREEN)
{
    *this = report;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen::~ReportPixelOnScreen()
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
int ReportPixelOnScreen::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;

        written += msg.Write(mTimeStamp.ToUInt());
        expected = JAUS_UINT_SIZE;
        
        written += msg.Write( (UInt)mDataList.size() );
        expected += JAUS_UINT_SIZE;

        Data::List::const_iterator data;
        for(data = mDataList.begin();
            data != mDataList.end();
            data++)
        {
            written += msg.Write(data->mX);
            expected += JAUS_FLOAT_SIZE;
            written += msg.Write(data->mY);
            expected += JAUS_FLOAT_SIZE;

            written += msg.Write( (UShort)(data->mModelType.size()) );
            expected += JAUS_USHORT_SIZE;
            written += msg.Write( (const unsigned char *)data->mModelType.c_str(), (unsigned int)data->mModelType.size());
            expected += (int)data->mModelType.size();

            written += msg.Write( (UShort)(data->mActorName.size()) );
            expected += JAUS_USHORT_SIZE;
            written += msg.Write( (const unsigned char *)data->mActorName.c_str(), (unsigned int)data->mActorName.size());
            expected += (int)data->mActorName.size();
            
            written += msg.Write((LongFloat)data->mOrientation.mX); // Roll
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mOrientation.mY); // Pitch
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mOrientation.mZ); // Yaw
            expected += JAUS_LONG_FLOAT_SIZE;

            written += msg.Write((LongFloat)data->mPosition.mLatitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mPosition.mLongitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mPosition.mElevation);
            expected += JAUS_LONG_FLOAT_SIZE;

            written += msg.Write((LongFloat)data->mVehiclePosition.mLatitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mVehiclePosition.mLongitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            written += msg.Write((LongFloat)data->mVehiclePosition.mElevation);
            expected += JAUS_LONG_FLOAT_SIZE;

            written += msg.Write(data->mDistance);
            expected += JAUS_FLOAT_SIZE;
            written += msg.Write(data->mAngle);
            expected += JAUS_FLOAT_SIZE;
        }

        if( written == expected) 
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
int ReportPixelOnScreen::ReadMessageBody(const Stream& msg, 
                                         const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;

        UInt tstamp = 0, count = 0;
        read += msg.Read(tstamp);
        expected = JAUS_UINT_SIZE;
        mTimeStamp.SetTime(tstamp);

        read += msg.Read( count );
        expected += JAUS_UINT_SIZE;
    
        mDataList.clear();

        for(UInt i = 0; i < count; i++)
        {
            Data data;
            read += msg.Read(data.mX);
            expected += JAUS_FLOAT_SIZE;
            read += msg.Read(data.mY);
            expected += JAUS_FLOAT_SIZE;

            UShort strlen = 0;
            read += msg.Read( strlen );
            expected += JAUS_USHORT_SIZE;
            read += msg.Read( data.mModelType, (unsigned int)strlen);
            expected += (int)strlen;

            strlen = 0;
            read += msg.Read( strlen );
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(data.mActorName, (unsigned int)strlen);
            expected += (int)strlen;
            
            read += msg.Read((LongFloat&)data.mOrientation.mX); // Roll
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mOrientation.mY); // Pitch
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mOrientation.mZ); // Yaw
            expected += JAUS_LONG_FLOAT_SIZE;

            read += msg.Read((LongFloat&)data.mPosition.mLatitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mPosition.mLongitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mPosition.mElevation);
            expected += JAUS_LONG_FLOAT_SIZE;

            read += msg.Read((LongFloat&)data.mVehiclePosition.mLatitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mVehiclePosition.mLongitude);
            expected += JAUS_LONG_FLOAT_SIZE;
            read += msg.Read((LongFloat&)data.mVehiclePosition.mElevation);
            expected += JAUS_LONG_FLOAT_SIZE;

            read += msg.Read(data.mDistance);
            expected += JAUS_FLOAT_SIZE;
            read += msg.Read(data.mAngle);
            expected += JAUS_FLOAT_SIZE;

            mDataList.push_back(data);
        }

        if( read == expected) 
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
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPixelOnScreen::ClearMessageBody()
{
    mDataList.clear();
    mTimeStamp = Time();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message body information to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPixelOnScreen::PrintData() const
{
    Data::List::const_iterator data;
    cout << "Found " << mDataList.size() << " Actors!\n";
    for(data = mDataList.begin();
        data != mDataList.end();
        data++)
    {
        cout << "Actor: " << data->mActorName << ", Model Type: " 
             << data->mModelType << endl;
        cout << "WGS[" << data->mPosition.mLatitude << ", " << data->mPosition.mLongitude << "] <" << data->mOrientation.mX << ", " << data->mOrientation.mY << ", " << data->mOrientation.mZ << endl;
        cout << "Distance: " << data->mDistance << ", Angle: " << CxUtils::CxToDegrees(data->mAngle) << " degrees. " << endl;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPixelOnScreen& ReportPixelOnScreen::operator=(const ReportPixelOnScreen& report) 
{
    CopyHeaderData(&report);
    mDataList = report.mDataList;
    mTimeStamp = report.mTimeStamp;
    return *this;
}

/* End of File */
