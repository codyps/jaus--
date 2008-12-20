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
#include "jaus/messages/experimental/sick/reportsicklidar.h"
#include "jaus/messages/experimental/experimentalcodes.h"


using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Consructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSickLidar::ReportSickLidar() : Message(JAUS_REPORT_SICK_LIDAR)
{
    mUnits = Millimeter;
    mType = OneEightyDegreesHalfRes;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy consructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSickLidar::ReportSickLidar(const ReportSickLidar& report) : Message(JAUS_REPORT_SICK_LIDAR)
{
    *this = report;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSickLidar::~ReportSickLidar()
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
int ReportSickLidar::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;
        expected = JAUS_UINT_SIZE + JAUS_BYTE_SIZE*2;
        switch(mType)
        {
        case OneHundredDegreesOneRes:
            expected += JAUS_USHORT_SIZE*101;
            break;
        case OneHundredDegreesHalfRes:
            expected += JAUS_USHORT_SIZE*201;
            break;
        case OneHundredDegreesQuarterRes:
            expected += JAUS_USHORT_SIZE*401;
            break;
        case OneEightyDegreesOneRes:
            expected += JAUS_USHORT_SIZE*181;
            break;
        default:
            expected += JAUS_USHORT_SIZE*361;
            break;
        };

        written += msg.Write(mTimeStamp.ToUInt());
        written += msg.WriteByte((Byte)(mUnits));
        written += msg.WriteByte((Byte)(mType));
        Scan::const_iterator r;
        for(r = mDataScan.begin();
            r != mDataScan.end();
            r++)
        {
            written += msg.Write(*r);
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
int ReportSickLidar::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        unsigned int count = 0;
        expected = JAUS_UINT_SIZE + JAUS_BYTE_SIZE*2;
        UInt tstamp = 0;
        Byte temp = 0;
        read += msg.Read(tstamp);
        mTimeStamp.SetTime(tstamp);
        read += msg.Read(temp);
        mUnits = (Units)temp;
        read += msg.Read(temp);
        mType = (Type)temp;
        switch(mType)
        {
        case OneHundredDegreesOneRes:
            count = 101;
            expected += JAUS_USHORT_SIZE*101;
            break;
        case OneHundredDegreesHalfRes:
            count = 201;
            expected += JAUS_USHORT_SIZE*201;
            break;
        case OneHundredDegreesQuarterRes:
            count = 401;
            expected += JAUS_USHORT_SIZE*401;
            break;
        case OneEightyDegreesOneRes:
            count = 181;
            expected += JAUS_USHORT_SIZE*181;
            break;
        default:
            count = 361;
            expected += JAUS_USHORT_SIZE*361;
            break;
        };
        UShort r = 0;
        mDataScan.clear();
        for(unsigned int i = 0; i < count; i++)
        {
            read += msg.Read(r);
            mDataScan.push_back(r);
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
///   \brief Sets the contents of the message using data from a SICK LIDAR.
///
///   \param[in] data Range scan data originating from the sensor.  Data is 
///              relative to the sensor, not the platform.
///   \param[in] timestamp The time when the data was captured (UTC).
///   \param[in] units The units for the range data.
///   \param[in] type The scan type.  This value will determine how many data
///                   points should be in the scan vector.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportSickLidar::SetScanData(const Scan& data, 
                                  const Time& timestamp, 
                                  const Units units, 
                                  const Type type)
{
    mDataScan = data;
    mTimeStamp = timestamp;
    mUnits = units;
    mType = type;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportSickLidar::ClearMessageBody()
{
    mDataScan.clear();
    mTimeStamp = Time();
    mUnits = Millimeter;
    mType = OneEightyDegreesHalfRes;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSickLidar& ReportSickLidar::operator=(const ReportSickLidar& report) 
{
    CopyHeaderData(&report);
    mDataScan = report.mDataScan;
    mTimeStamp = report.mTimeStamp;
    mUnits = report.mUnits;
    mType = report.mType;
    return *this;
}

/* End of File */
