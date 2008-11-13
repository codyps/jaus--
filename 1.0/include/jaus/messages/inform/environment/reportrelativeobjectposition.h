////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportrelativeobjectposition.h
///  \brief This file contains the message structure for Report Camera Pose.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 18 August 2008
///  <br>Last Modified: 18 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cholland@ist.ucf.edu
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
#ifndef __JAUS_REPORT_RELATIVE_OBJECT_POSITION_H
#define __JAUS_REPORT_RELATIVE_OBJECT_POSITION_H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportRelativeObjectPosition
    ///   \brief For the Report Relative Object Position message, the parameters Range, 
    ///          Bearing, and Inclination indicate the relative offset to the object 
    ///          in the current platform coordinate system.  All times are in Coordinated 
    ///          Universal Time (UTC).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportRelativeObjectPosition : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                Range               = 0x01,
                RangeError          = 0x02,
                Bearing             = 0x04,
                BearingError        = 0x08,
                Inclination         = 0x10,
                InclinationError    = 0x20,
                Confidence          = 0x40,
                ObjectID            = 0x80,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                Range               = 0,
                RangeError          = 1,
                Bearing             = 2,
                BearingError        = 3,
                Inclination         = 4,
                InclinationError    = 5,
                Confidence          = 6,
                ObjectID            = 7,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Contains constants for limit values of data members of class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxRange;            ///<  Upper limit:  10,000 meters
            const static double MinRange;            ///<  Lower limit: -10,000 meters
            const static double MaxRangeError;       ///<  Upper limit:  1,000  meters
            const static double MinRangeError;       ///<  Lower limit:  0        meters
            const static double MaxBearing;          ///<  Upper Limit:  Pi        radians
            const static double MinBearing;          ///<  Lower Limit: -Pi        radians
            const static double MaxBearingError;     ///<  Upper Limit:  Pi        radians
            const static double MinBearingError;     ///<  Lower Limit:  0        radians
            const static double MaxInclination;      ///<  Upper Limit:  Pi        radians
            const static double MinInclination;      ///<  Lower Limit: -Pi        radians
            const static double MaxInclinationError; ///<  Upper Limit:  Pi        radians
            const static double MinInclinationError; ///<  Lower limit:  0        radians
            const static Byte MinConfidence;         ///<  Lower limit:  0
            const static Byte MaxConfidence;         ///<  Upper Limit:  255
        };

        ReportRelativeObjectPosition();
        ReportRelativeObjectPosition(const ReportRelativeObjectPosition &msg);
        ~ReportRelativeObjectPosition();

        int SetTimeStamp(const Time value);
        int SetRange(const double value);
        int SetRangeError(const double value);
        int SetBearing(const double value);
        int SetBearingError(const double value);
        int SetInclination(const double value);
        int SetInclinationError(const double value);
        int SetConfidence(const Byte value);
        int SetObjectID(const UShort value);

        Byte GetPresenceVector() const        { return mPresenceVector; }
        Time GetTimeStamp() const            { return mTimeStamp; }
        double GetRange() const                { return mRange; }            // Check presence vector first!
        double GetRangeError() const        { return mRangeError; }          // Check presence vector first!
        double GetBearing() const            { return mBearing; }            // Check presence vector first!
        double GetBearingError() const        { return mBearingError; }      // Check presence vector first!
        double GetInclination() const        { return mInclination; }        // Check presence vector first!
        double GetInclinationError() const    { return mInclinationError; }  // Check presence vector first!
        Byte GetConfidence() const            { return mConfidence; }        // Check presence vector first!
        UShort GetObjectID() const            { return mObjectID; }          // Check presence vector first!
        void ClearFields(const UShort mask);

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportRelativeObjectPosition(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ReportRelativeObjectPosition &operator=(const ReportRelativeObjectPosition &msg);

        virtual int RunTestCase() const;

    protected:
        Byte mPresenceVector;       ///<  Bit vector for items being reported.
        Time mTimeStamp;            ///<  Time Stamp of the data
        double mRange;              ///<  Range of the object, [-10,000, 10,000] meters.
        double mRangeError;         ///<  Error value for the range, [0, 1000] meters
        double mBearing;            ///<  Bearing of the object, [-Pi, Pi] radians.
        double mBearingError;       ///<  Error value for the bearing, [0, Pi] meters
        double mInclination;        ///<  Inclination of the object, [-Pi, Pi] radians.
        double mInclinationError;   ///<  Error value for the inclination, [0, Pi] meters
        Byte mConfidence;           ///<  Confidence of how accurate the data is, [0, 255] with higher indicating more confidence
        UShort mObjectID;           ///<  Object identifier
    };
}

#endif

/*  End of File */
