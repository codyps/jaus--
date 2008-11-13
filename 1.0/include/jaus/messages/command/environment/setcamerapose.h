////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcamerapose.h
///  \brief This file contains the message to set a Camera Pose for a Visual
///  Sensor.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 July 2008
///  <br>Last Modified: 30 July 2008
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
#ifndef __JAUS_SET_CAMERA_POSE__H
#define __JAUS_SET_CAMERA_POSE__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class SetCameraPose
    ///  \brief Message to set a camera pose.
    ///
    ///  This message is used to control the position and orientation of cameras with
    ///  translational and/or rotational capabilities.  An example is a camera mounted
    ///  on a pan-tilt mechanism.  
    ///
    ///  The translation and rotation of the camera are with respect to the camera's
    ///  coordinate frame.  In this frame, panning is a rotation of the camera about
    ///  its Y-axis.  A positive Y-axis rotation will rotate the camera to the viewers
    ///  right.  Tilt is rotation of the camera about its X-axis.  A positive X-axis
    ///  rotation will rotate the camera up.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetCameraPose : public Message
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
                XDisplacementOrLinearRate = 0x01,
                YDisplacementOrLinearRate = 0x02,
                ZDisplacementOrLinearRate       = 0x04,
                XAngleOrAngularRotationRate     = 0x08,
                YAngleOrAngularRotationRate     = 0x10,
                ZAngleOrAngularRotationRate     = 0x20,
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
                XDisplacementOrLinearRate = 0,
                YDisplacementOrLinearRate,
                ZDisplacementOrLinearRate,
                XAngleOrAngularRotationRate,
                YAngleOrAngularRotationRate,
                ZAngleOrAngularRotationRate,
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
            const static double DisplacementOrLinearRateUpperLimit;     ///<  Upper limit for displacement or linear rate. (100)
            const static double DisplacementOrLinearRateLowerLimit;     ///<  Lower limit for displacement or linear rate. (-100)
            const static double AngleOrAngularRotationRateUpperLimit;   ///<  Upper limit for angle or angular rotationrate. (100)
            const static double AngleOrAngularRotationRateLowerLimit;   ///<  Lower limit for angle or angular rotation rate. (-100)
        };

        SetCameraPose();
        SetCameraPose(const SetCameraPose& msg);
        ~SetCameraPose();
        int SetCameraID(const Byte id);
        int SetXDisplacementOrLinearRate(const double value, const bool displacementRate);
        int SetYDisplacementOrLinearRate(const double value, const bool displacementRate);
        int SetZDisplacementOrLinearRate(const double value, const bool displacementRate);
        int SetXAngleOrAngularRotationRate(const double value, const bool angleRate);
        int SetYAngleOrAngularRotationRate(const double value, const bool angleRate);
        int SetZAngleOrAngularRotationRate(const double value, const bool angleRate);
        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const { return mCameraID; }
        Byte GetModeIndicator() const { return mModeIndicator; }
        double GetXDisplacementOrLinearRate() const; // Check presence vector first!
        double GetYDisplacementOrLinearRate() const; // Check presence vector first!
        double GetZDisplacementOrLinearRate() const; // Check presence vector first!
        double GetXAngleOrAngularRotationRate() const; // Check presence vector first!
        double GetYAngleOrAngularRotationRate() const; // Check presence vector first!
        double GetZAngleOrAngularRotationRate() const; // Check presence vector first!
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual void PrintCameraPose() const;
        virtual Message* Clone() const;
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetCameraPose& operator=(const SetCameraPose& msg);
        virtual int RunTestCase() const;
    protected:
        Byte mPresenceVector;                ///<  Indicates what fields will be present in message.
        Byte mCameraID;                         ///<  Valid values are [1, 255], 0 is reserved
        Byte mModeIndicator;                 ///<  Indicates what mode a parameter is.  Bits 0-5 1/0 Rate/Position.
        double mXDisplacementOrLinearRate;   ///<  +/- 100% of max limit.
        double mYDisplacementOrLinearRate;   ///<  +/- 100% of max limit.
        double mZDisplacementOrLinearRate;   ///<  +/- 100% of max limit.
        double mXAngleOrAngularRotationRate; ///<  +/- 100% of max limit.
        double mYAngleOrAngularRotationRate; ///<  +/- 100% of max limit.
        double mZAngleOrAngularRotationRate; ///<  +/- 100% of max limit.
    };
}


#endif
/*  End of File */
