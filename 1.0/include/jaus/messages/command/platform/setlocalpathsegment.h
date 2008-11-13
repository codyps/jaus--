////////////////////////////////////////////////////////////////////////////////////
///
///  \file setlocalpathsegment.h
///  \brief This file contains the message class for Set Global Path Segment.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 21 August 2008
///  <br>Last Modified: 21 August 2008
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
#ifndef __JAUS_SET_LOCAL_PATH_SEGMENT__H
#define __JAUS_SET_LOCAL_PATH_SEGMENT__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetLocalPathSegment
    ///   \brief A local path segment is defined in this message using three points, 
    ///             P0, P1 and P2 and a weighting factor.  The path segment number starts 
    ///             at zero for the first path segment, and must be incremented by one for 
    ///             each additional path segment.  For the first path segment, i.e. where 
    ///             the path segment number is equal to zero, P0 is assumed to be the 
    ///             current location of the platform as defined by Report Local Pose.  
    ///             For each successive path segments, i.e. where the path segment number 
    ///             is greater than zero, P0 is equal to the previous path segment’s P2.  
    ///             Therefore, for each message, only P1, P2, and a weighting factor must 
    ///             be set in order to define a path segment.  Each point is defined in 
    ///             the Local Coordinate System by setting its X, Y, and Z.  Both the X 
    ///             and Y are required fields, but the Z field is optional.  Bit 0 in the 
    ///             presence vector is used to determine if the Z field is used.  
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetLocalPathSegment : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                P1Z    = 0x01,
                P2Z    = 0x02,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                P1Z = 0,
                P2Z = 1
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
            //don't ask, the spec defines it this way, probably so that it's still valid if computer architecture change
            const static UShort PathSegmentNumberLowerLimit;   ///< Lower bound for the number of segments:    0
            const static UShort PathSegmentNumberUpperLimit;   ///< Upper bound for the number of segments:    65535

            const static double MinX;    ///< Lowest possible X:        -100,000    degrees
            const static double MaxX;    ///< Highest possible X:     100,000    degrees
            const static double MinY;    ///< Lowest possible Y:        -100,000    degrees
            const static double MaxY;    ///< Highest possible Y:     100,000    degrees
            const static double MinZ;    ///< Lowest possible Z:        -10,000        meters
            const static double MaxZ;    ///< Highest possible Z:     35,000        meters
        };

        SetLocalPathSegment();
        SetLocalPathSegment(const SetLocalPathSegment& vector);
        ~SetLocalPathSegment();

        int SetPathSegmentNumber(const UShort value);
        int SetP1X(const double value);
        int SetP1Y(const double value);
        int SetP1Z(const double value);
        int SetP2X(const double value);
        int SetP2Y(const double value);
        int SetP2Z(const double value);
        int SetWeightingFactor(const double value);


        Byte GetPresenceVector() const        { return mPresenceVector; }
        UShort getPathSegmentNumber() const { return mPathSegmentNumber; }    
        double GetP1X() const                { return mP1X; }            
        double GetP1Y() const                { return mP1Y; }            
        double GetP1Z() const                { return mP1Z; }            // Check presence vector 1st!
        double GetP2X() const                { return mP2X; }            
        double GetP2Y() const                { return mP2Y; }            
        double GetP2Z() const                { return mP2Z; }            // Check presence vector 1st!
        double GetWeightingFactor() const    { return mWeightingFactor; }

        
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new SetLocalPathSegment(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetLocalPathSegment& operator=(const SetLocalPathSegment& msg);

        virtual int RunTestCase() const;

    protected:
        Byte mPresenceVector;       ///< Bit vector for fields present.
        UShort mPathSegmentNumber;  ///< The number of the waypoint, between 0 and 65535
        double mP1X;                ///< The X coordinate of P1, [-100000, 100000] degrees
        double mP1Y;                ///< The Y coordinate of P1, [-100000, 100000] degrees
        double mP1Z;                ///< The Z coordinate of P1, [-10000, 35000] meters
        double mP2X;                ///< The X coordinate of P2, [-100000, 100000] degrees
        double mP2Y;                ///< The Y coordinate of P2, [-100000, 100000] degrees
        double mP2Z;                ///< The Z coordinate of P2, [-10000, 35000] meters
        double mWeightingFactor;    ///< How straight or curved the path is, [0, 500] with 0 being a straight line
    };
}

#endif
/*  End of File */
