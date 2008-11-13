////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalpathsegment.h
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
#ifndef __JAUS_REPORT_LOCAL_PATH_SEGMENT_H
#define __JAUS_REPORT_LOCAL_PATH_SEGMENT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportLocalPathSegment
    ///   \brief This message provides the receiver the current values of the requested 
    ///             path segment specified by the data in Code 2410h: Query Local Path 
    ///          Segment.  The message data and mapping of the presence vector of this 
    ///          message are identical to Code 0410h: Set Local Path Segment.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportLocalPathSegment : public Message
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
                P1Z =        0x01,
                P2Z =        0x02
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
                P1Z =        0,
                P2Z    
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Class containing upper and lower bounds of fields used in this
        ///          message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static UShort MinPathSegmentNumber;    ///< Lower Limit = 0. 
            const static UShort MaxPathSegmentNumber;    ///< Upper Limit = 65,535.
            const static double MinP1X;                    ///< Lower Limit = -100,000. 
            const static double MaxP1X;                    ///< Upper Limit = 100,000.
            const static double MinP1Y;                    ///< Lower Limit = -100,000. 
            const static double MaxP1Y;                    ///< Upper Limit = 100,000.
            const static double MinP1Z;                    ///< Lower Limit =  -10,000.
            const static double MaxP1Z;                    ///< Upper Limit = 35,000.
            const static double MinP2X;                    ///< Lower Limit = -100,000.
            const static double MaxP2X;                    ///< Upper Limit = 100,000.
            const static double MinP2Y;                    ///< Lower Limit = -100,000.
            const static double MaxP2Y;                    ///< Upper Limit = 100,000.
            const static double MinP2Z;                    ///< Lower Limit =  -10,000. 
            const static double MaxP2Z;                    ///< Upper Limit = 35,000.
            const static double MinWeightingFactor;        ///< Range 0 to 500. Zero = a straight line.
            const static double MaxWeightingFactor;        ///< Range 0 to 500. Zero = a straight line.
        };
        ReportLocalPathSegment();
        ReportLocalPathSegment(const ReportLocalPathSegment &msg);
        ~ReportLocalPathSegment();
        int SetPresenceVector(const Byte pv);
        int SetPathSegmentNumber(const UInt val);
        int SetP1X(const double val);
        int SetP1Y(const double val);
        int SetP1Z(const double val);
        int SetP2X(const double val);
        int SetP2Y(const double val);
        int SetP2Z(const double val);
        int SetWeightingFactor(const double val);

        Byte GetPresenceVector() const { return mPresenceVector; }
        UInt GetPathSegmentNumber() const { return mPathSegmentNumber; }
        double GetP1X() const { return mP1X; }
        double GetP1Y() const { return mP1Y; }
        double GetP1Z() const;
        double GetP2X() const { return mP2X; }
        double GetP2Y() const { return mP2Y; }
        double GetP2Z() const;
        double GetWeightingFactor() const { return mWeightingFactor; }

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportLocalPathSegment(*this); }
        ReportLocalPathSegment &operator=(const ReportLocalPathSegment &msg);
    protected:
        Byte mPresenceVector;       ///<  Presence vector associated with message.
        UInt mPathSegmentNumber;    ///<  Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535.
        double mP1X;                ///<  Integer, Meters, Scaled Integer, Lower Limit =  -100,000, Upper Limit = 100,000.
        double mP1Y;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -100,000, Upper Limit = 100,000.
        double mP1Z;                ///<  Integer, Meters, Scaled Integer, Lower Limit =  -10,000, Upper Limit = 35,000.
        double mP2X;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -100,000, Upper Limit = 100,000.
        double mP2Y;                ///<  Integer, Meters, Scaled Integer, Lower Limit = -100,000, Upper Limit = 100,000.
        double mP2Z;                ///<  Integer, Meters, Scaled Integer, Lower Limit =  -10,000, Upper Limit = 35,000.
        double mWeightingFactor;    ///<  Unsigned Short Integer, Scaled Integer, Range 0 to 500. Zero = a straight line. 
    };
}

#endif

#endif 

/*  End of File */
