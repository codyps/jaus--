////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportsicklidar.h
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
#ifndef __JAUS_EXPERIMENTAL_REPORT_SICK_LIDAR__H
#define __JAUS_EXPERIMENTAL_REPORT_SICK_LIDAR__H

#include "jaus/messages/message.h"
#include <vector>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportSickLidar
    ///   \brief This custom message encapsulates the output from a SICK LIDAR
    ///          range sensor.
    ///
    ///   LIDAR scan data start with 0 degrees being the right side of the sesnor
    ///   and 180 being the left side, 90 degrees facing forward.  Values in the
    ///   scan map to an angle based on the Type.  Units are either in millimeters
    ///   or centimeters.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportSickLidar : public Message
    {
    public:
        enum Type
        {
            OneHundredDegreesOneRes = 0,  // 100 degree scan, 1 degree increments (101 points).
            OneHundredDegreesHalfRes,     // 100 degree scan, .5 degree increments (201 points).
            OneHundredDegreesQuarterRes,  // 100 degree scan, .25 degree increments (401 points).
            OneEightyDegreesOneRes,       // 180 degree scan, 1 degree increments (181 points).
            OneEightyDegreesHalfRes       // 180 degree scan, .5 degree increments (361 points).
        };
        enum Units
        {
            Millimeter = 0,               // Millimeter resolution (divide by 100 to convert to meters).
            Centimeter                    // Centimeter resolution (divide by 10 to convert to cm).
        };
        typedef std::vector<UShort> Scan;
        ReportSickLidar();
        ReportSickLidar(const ReportSickLidar& query);
        virtual ~ReportSickLidar();
        virtual int WriteMessageBody(Stream& msg,
                                     const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, 
                                    const UShort version);
        virtual Message* Clone() const { return new ReportSickLidar(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody();
        void SetScanData(const Scan& data, 
                         const Time& timestamp, 
                         const Units units, 
                         const Type type);
        Scan* GetScanData() const { return (Scan*)(&mDataScan); }
        Units GetUnits() const { return mUnits; }
        Type GetScanType() const { return mType; }
        Time GetTimeStamp() const { return mTimeStamp; }
        ReportSickLidar& operator=(const ReportSickLidar& query);
    protected:
        Scan mDataScan;     ///< DataScan.
        Time mTimeStamp;    ///< Time Stamp.
        Units mUnits;       ///< Units for converting range values to meters.
        Type mType;         ///< Type of scan received.
    };
}



#endif
/*  End of File */
