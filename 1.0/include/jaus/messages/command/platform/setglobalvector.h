////////////////////////////////////////////////////////////////////////////////////
///
///  \file setglobalvector.h
///  \brief This file contains the message class for Set Global Vector.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#ifndef __JAUS_SET_GLOBAL_VECTOR__H
#define __JAUS_SET_GLOBAL_VECTOR__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetGlobalVector
    ///   \brief Sets the desired global vector of a platform.  The desired heading 
    ///   angle is defined in a right hand sense about the Z axis of the global
    ///   coordinate system (Z axis points downward) where north is defined as
    ///   zero degrees.  The Elevation (above seal level (ASL)) is in accordance with
    ///   the WGS 84 standard.  The desired roll angle is alos in a right hand sense
    ///   about the X axis of the global coordinate system, and pitch is right
    ///   hand around the Y axis of the global coordinate system.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetGlobalVector : public Message
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
                Speed = 0x01,
                Elevation = 0x02,
                Heading = 0x04,
                Roll = 0x08,
                Pitch = 0x10
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
                Speed = 0,
                Elevation,
                Heading,
                Roll,
                Pitch
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
            const static double MinSpeed;       ///<  Minimum speed. (0)
            const static double MaxSpeed;       ///<  Maximum speed. (10000)
            const static double MinElevation;   ///<  Minimum altitude. (-10000)
            const static double MaxElevation;   ///<  Maximum altitude. (35000)
            const static double MinAngle;       ///<  Minimum angle value in radians. (-PI)
            const static double MaxAngle;       ///<  Maximum angle value in radians. (PI)
        };
        SetGlobalVector();
        SetGlobalVector(const SetGlobalVector& vector);
        virtual ~SetGlobalVector();
        int SetSpeed(const double value);
        int SetElevation(const double value);
        int SetHeading(const double value);
        int SetRoll(const double value);
        int SetPitch(const double value);
        Byte GetPresenceVector() const;
        double GetSpeed() const { return mSpeed; }          // Check presence vector 1st!
        double GetElevation() const { return mElevation; }  // Check presence vector 1st!
        double GetHeading() const { return mHeading; }      // Check presence vector 1st!
        double GetRoll() const { return mRoll; }            // Check presence vector 1st!
        double GetPitch() const { return mPitch; }          // Check presence vector 1st!
        virtual void Print() const;
        virtual void PrintGlobalVector() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new SetGlobalVector(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetGlobalVector& operator=(const SetGlobalVector& msg);
    protected:
        Byte mPresenceVector;       ///<  Bit vector for fields present.
        double mSpeed;              ///<  Desired speed in m/s [0, 10,000].
        double mElevation;          ///<  Desired altitude (ASL) [-10,000, 35,000].
        double mHeading;            ///<  Desired compass heading in radians [-pi, pi].
        double mRoll;               ///<  Desired roll in radians [-pi, pi].
        double mPitch;              ///<  Desired pitch in radians [-pi, pi].
    };
}



#endif
/*  End of File */
