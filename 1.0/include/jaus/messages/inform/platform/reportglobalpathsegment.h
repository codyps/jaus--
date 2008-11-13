////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportglobalpathsegment.h
///  \brief This file contains the message structure for Report Global Path Segment
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
#ifndef __JAUS_REPORT_GLOBAL_PATH_SEGMENT_H
#define __JAUS_REPORT_GLOBAL_PATH_SEGMENT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportGlobalPathSegment
    ///   \brief This message provides the receiver the current values of the requested 
    ///          path segment specified by the data in Code 240Fh: Query Global Path 
    ///          Segment.  The message data and mapping of the presence vector of this 
    ///          message are identical to Code 040Fh: Set Global Path Segment.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportGlobalPathSegment : public Message
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
                P1Altitude =        0x01,
                P2Altitude =        0x02
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
                P1Altitude =        0,
                P2Altitude    
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
            const static double MinP1Latitude;           ///< Lower Limit =  -90. 
            const static double MaxP1Latitude;           ///< Upper Limit = 90.
            const static double MinP1Longitude;          ///< Lower Limit =  -180.
            const static double MaxP1Longitude;          ///< Upper Limit = 180.
            const static double MinP1Altitude;           ///< Lower Limit =  -10,000.
            const static double MaxP1Altitude;           ///< Upper Limit = 35,000.
            const static double MinP2Latitude;           ///< Lower Limit =  -90.
            const static double MaxP2Latitude;           ///< Upper Limit = 90.
            const static double MinP2Longitude;          ///< Lower Limit = -180.
            const static double MaxP2Longitude;          ///< Upper Limit = 180.
            const static double MinP2Altitude;           ///< Lower Limit =  -10,000. 
            const static double MaxP2Altitude;           ///< Upper Limit = 35,000. 
            const static double MinWeighingFactor;       ///< Range 0 to 500. Zero = a straight line.
            const static double MaxWeighingFactor;       ///< Range 0 to 500. Zero = a straight line.
        };
        ReportGlobalPathSegment();
        ReportGlobalPathSegment(const ReportGlobalPathSegment &msg);
        ~ReportGlobalPathSegment();

        int SetPresenceVector(const Byte pv);
        int SetPathSegmentNumber(const UInt val);
        int SetP1Latitude(const double val);        
        int SetP1Longitude(const double val);    
        int SetP1Altitude(const double val);        
        int SetP2Latitude(const double val);        
        int SetP2Longitude(const double val);    
        int SetP2Altitude(const double val);        
        int SetWeighingFactor(const double val);

        Byte GetPresenceVector() const { return mPresenceVector; }
        UInt GetPathSegmentNumber() const { return mPathSegmentNumber; }
        double GetP1Latitude()const{ return mP1Latitude; }        
        double GetP1Longitude()const{ return mP1Longitude; }    
        double GetP1Altitude()const;        
        double GetP2Latitude()const{ return mP2Latitude; }        
        double GetP2Longitude()const{ return mP2Longitude; }    
        double GetP2Altitude()const;        
        double GetWeighingFactor()const{ return mWeighingFactor; }

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportGlobalPathSegment(*this); }
        ReportGlobalPathSegment &operator=(const ReportGlobalPathSegment &msg);
    protected:
        Byte mPresenceVector;       ///<  Presence vector associated with message.
        UInt mPathSegmentNumber;    ///<  Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535
        double mP1Latitude;         ///<  Scaled Integer, Integer, Degrees, Scaled Integer, Lower Limit =  -90, Upper Limit = 90.
        double mP1Longitude;        ///<  Scaled Integer, Integer, Degrees, Scaled Integer, Lower Limit =  -180, Upper Limit = 180.
        double mP1Altitude;         ///<  Scaled, Integer, Lower Limit =  -10,000, Upper Limit = 35,000.
        double mP2Latitude;         ///<  Scaled Integer, Lower Limit =  -90, Upper Limit = 90.
        double mP2Longitude;        ///<  Scaled Integer, Lower Limit = -180, Upper Limit = 180.
        double mP2Altitude;         ///<  Scaled Integer, Lower Limit =  -10,000, Upper Limit = 35,000.
        double mWeighingFactor;     ///<  Scaled Integer, Range 0 to 500. Zero = a straight line.
    };
}

#endif

#endif 

/*  End of File */
