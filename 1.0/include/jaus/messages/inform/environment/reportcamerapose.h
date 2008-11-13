////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcamerapose.h
///  \brief This file contains the message structure for Report Camera Pose.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 14 August 2008
///  <br>Last Modified: 14 August 2008
///  <br>Copyright (c) 2007
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
#ifndef __JAUS_REPORT_CAMERA_POSE_H
#define __JAUS_REPORT_CAMERA_POSE_H

#include <string>

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportCameraPose
    ///   \brief This message provides the receiver with the current values of the
    ///          camera pose.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportCameraPose : public Message
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
                CameraName            = 0x01,
                XCameraOrigin         = 0x02,
                YCameraOrigin         = 0x04,
                ZCameraOrigin         = 0x08,
                XCameraAxisDirCosineX = 0x10,
                XCameraAxisDirCosineY = 0x20,
                XCameraAxisDirCosineZ = 0x40,
                ZCameraAxisDirCosineX = 0x80,
                ZCameraAxisDirCosineY = 0x100,
                ZCameraAxisDirCosineZ = 0x200
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
                CameraName            = 0,
                XCameraOrigin         = 1,
                YCameraOrigin         = 2,
                ZCameraOrigin         = 3,
                XCameraAxisDirCosineX = 4,
                XCameraAxisDirCosineY = 5,
                XCameraAxisDirCosineZ = 6,
                ZCameraAxisDirCosineX = 7,
                ZCameraAxisDirCosineY = 8,
                ZCameraAxisDirCosineZ = 9
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
            const static double MaxCameraOrigin;      ///<  Upper limit for the camera origin. (32.767)
            const static double MinCameraOrigin;      ///<  Lower limit for the camera origin. (-32.767)
            const static double MaxCameraAxisCosine;  ///<  Upper limit for the camera axis cosine. (1.0)
            const static double MinCameraAxisCosine;  ///<  Lower limit for the camera axis cosine. (-1.0)
            const static unsigned int MaxCameraNameLength;  ///<  Maximum number of characters in a name. (15)
        };

        ReportCameraPose();
        ReportCameraPose(const ReportCameraPose &msg);
        ~ReportCameraPose();

        int SetCameraID(const Byte id);
        int SetCameraName(const std::string name);
        int SetXCameraOrigin(const double value);
        int SetYCameraOrigin(const double value);
        int SetZCameraOrigin(const double value);
        int SetXCameraAxisDirCosineX(const double value);
        int SetXCameraAxisDirCosineY(const double value);
        int SetXCameraAxisDirCosineZ(const double value);
        int SetZCameraAxisDirCosineX(const double value);
        int SetZCameraAxisDirCosineY(const double value);
        int SetZCameraAxisDirCosineZ(const double value);

        UShort GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const { return mCameraID; }
        std::string GetCameraName() const { return mCameraName; }                        // Check presence vector first!
        double GetXCameraOrigin() const { return mXCameraOrigin; }                        // Check presence vector first!
        double GetYCameraOrigin() const { return mYCameraOrigin; }                        // Check presence vector first!
        double GetZCameraOrigin() const { return mZCameraOrigin; }                        // Check presence vector first!
        double GetXCameraAxisDirCosineX() const { return mXCameraAxisDirCosineX; }        // Check presence vector first!
        double GetXCameraAxisDirCosineY() const { return mXCameraAxisDirCosineY; }        // Check presence vector first!
        double GetXCameraAxisDirCosineZ() const { return mXCameraAxisDirCosineZ; }        // Check presence vector first!
        double GetZCameraAxisDirCosineX() const { return mZCameraAxisDirCosineX; }        // Check presence vector first!
        double GetZCameraAxisDirCosineY() const { return mZCameraAxisDirCosineY; }        // Check presence vector first!
        double GetZCameraAxisDirCosineZ() const { return mZCameraAxisDirCosineZ; }        // Check presence vector first!

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportCameraPose(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ReportCameraPose &operator=(const ReportCameraPose &msg);

        virtual int RunTestCase() const;

    protected:
        UShort mPresenceVector;         ///<  Bit vector for items being reported.
        Byte mCameraID;                 ///<  ID of camera to query.
        std::string mCameraName;        ///<  15 character fixed length string
        double mXCameraOrigin;          ///<  Location of the camera’s coordinate frame origin with respect to the vehicle in meters. [-32.767, 32.767]
        double mYCameraOrigin;          ///<  Location of the camera’s coordinate frame origin with respect to the vehicle in meters. [-32.767, 32.767]
        double mZCameraOrigin;          ///<  Location of the camera’s coordinate frame origin with respect to the vehicle in meters. [-32.767, 32.767]
        double mXCameraAxisDirCosineX;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
        double mXCameraAxisDirCosineY;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
        double mXCameraAxisDirCosineZ;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
        double mZCameraAxisDirCosineX;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
        double mZCameraAxisDirCosineY;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
        double mZCameraAxisDirCosineZ;  ///<  The Camera Axis Direction Cosines define the orientation of the camera coordinate frame with respect to the vehicle coordinate frame. [-1.0, 1.0]
    };
}

#endif

/*  End of File */
