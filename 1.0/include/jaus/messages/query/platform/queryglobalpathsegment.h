////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryglobalpathsegment.h
///  \brief This file contains the message structure for Query Global Path Segment
///
///  <br>Author(s): David Adams
///  <br>Created: 11 June 2008
///  <br>Last Modified: 11 June 2008
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
#ifndef __JAUS_QUERY_GLOBAL_PATH_SEGMENT_H
#define __JAUS_QUERY_GLOBAL_PATH_SEGMENT_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class QueryGlobalPathSegment
    ///   \brief This message shall cause the receiving component to reply to the 
    ///             requestor with a Code 440Fh: Report Global Path Segment message.  
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryGlobalPathSegment : public Message
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
        QueryGlobalPathSegment();
        QueryGlobalPathSegment(const QueryGlobalPathSegment &msg);
        virtual ~QueryGlobalPathSegment();
        int SetPresenceVector(const Byte pv);
        int SetPathSegmentNumber(const UInt val);
        Byte GetPresenceVector() const { return mPresenceVector; }
        UInt GetPathSegmentNumber() const { return mPathSegmentNumber; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new QueryGlobalPathSegment(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        QueryGlobalPathSegment &operator=(const QueryGlobalPathSegment &msg);
    protected:
        Byte mPresenceVector;       ///<  Presence vector associated with message.
        UInt mPathSegmentNumber;    ///<  Unsigned Short Integer, Lower Limit = 0, Upper Limit = 65,535
    };
}

#endif

#endif 

/*  End of File */
