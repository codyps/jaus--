////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcameracapabilities.h
///  \brief This file contains the message to set a Camera Capabilties for a Visual
///  Sensor.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 11 August 2008
///  <br>Last Modified: 11 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cdhollan@ist.ucf.edu
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
#ifndef __JAUS_SET_CAMERA_CAPABILITIES__H
#define __JAUS_SET_CAMERA_CAPABILITIES__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class SetCameraCapabilities
    ///  \brief Message to set camera capabilities.
    ///
    ///  The Set Camera Capabilities message commands the Visual Component to use 
    ///  the parameters specified for the currently selected camera
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetCameraCapabilities : public Message
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
                HorizontalFOV        = 0x0001,
                VerticalFOV          = 0x0002,
                HorizontalResolution = 0x0004,
                VerticalResolution   = 0x0008,
                Focus                 = 0x0010,
                Iris                 = 0x0020,
                Gain                 = 0x0040,
                FrameRate             = 0x0080,
                Shutter                 = 0x0100,
                ImageryControl         = 0x0200,
                AudioControl         = 0x0400,
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
                HorizontalFOV = 0,
                VerticalFOV,
                HorizontalResolution,
                VerticalResolution,
                Focus,
                Iris,
                Gain,
                FrameRate,
                Shutter,
                ImageryControl,
                AudioControl,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ImageryControlMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          imagery control vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ImageryControlMask
        {
        public:
            enum Masks
            {
                AutoFocus            = 0x0001,
                AutoExposureIris    = 0x0002,
                ImageStabilization    = 0x0004,
                WhiteBalance        = 0x0008,
                SyncFlashStrobe        = 0x0010,
                RedEye                = 0x0020,
                AutoShutter            = 0x0040,
                AutoGain            = 0x0080,
                Interlaced            = 0x0100,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ImageryControlBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          imagery control vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ImageryControlBit
        {
        public:
            enum Bits
            {
                AutoFocus            = 0,
                AutoExposureIris    = 1,
                ImageStabilization    = 2,
                WhiteBalance        = 3,
                SyncFlashStrobe        = 4,
                RedEye                = 5,
                AutoShutter            = 6,
                AutoGain            = 7,
                Interlaced            = 8,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AudioControlMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          audio control vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL AudioControlMask
        {
        public:
            enum Masks
            {
                Audio            = 0x0001,
                AutoGain        = 0x0002,
                Stero            = 0x0004,
                Directional        = 0x0008,
                FrontMicrophone    = 0x0010,
                RearMicrophone    = 0x0020,
                LeftMicrophone    = 0x0040,
                RightMicrophone    = 0x0080,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AudioControlBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          audio control vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL AudioControlBit
        {
        public:
            enum Bits
            {
                Audio            = 0,
                AutoGain        = 1,
                Stero            = 2,
                Directional        = 3,
                FrontMicrophone    = 4,
                RearMicrophone    = 5,
                LeftMicrophone    = 6,
                RightMicrophone    = 7,
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
            const static double MaxFocalLength;     ///<  Upper limit for focus. (100)
            const static double MinFocalLength;     ///<  Lower limit for focus. (0)
            const static double MaxApertureWidth;   ///<  Upper limit for iris (100)
            const static double MinApertureWidth;   ///<  Lower limit for iris. (0)
            const static double MaxGain;            /// < Upper limit for gain. (100)
            const static double MinGain;            ///<  Lower limit for gain. (0)
        };

        SetCameraCapabilities();
        SetCameraCapabilities(const SetCameraCapabilities& msg);
        virtual ~SetCameraCapabilities();

        int SetCameraID(const Byte id);
        int SetHorizontalFOV(const double value);
        int SetVerticalFOV(const double value);
        int SetHorizontalResolution(const UShort value);
        int SetVerticalResolution(const UShort value);
        int SetFocus(const double value);
        int SetIris(const double value);
        int SetGain(const double value);
        int SetFrameRate(const UShort value);
        int SetShutter(const UShort value);
        int SetImageryControl(const UShort value);
        int SetImageryControlBit(const UShort flag, const bool value);
        int SetAudioControl(const UShort value);
        int SetAudioControlBit(const UShort flag, const bool value);


        UShort GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const { return mCameraID; }
        double GetHorizontalFOV() const { return mHorizontalFOV; }                    // Check presence vector first!
        double GetVerticalFOV() const { return mVerticalFOV; }                        // Check presence vector first!
        UShort GetHorizontalResolution() const { return mHorizontalResolution; }    // Check presence vector first!
        UShort GetVerticalResolution() const { return mVerticalResolution; }        // Check presence vector first!
        double GetFocus() const { return mFocus; }                                    // Check presence vector first!
        double GetIris() const { return mIris; }                                    // Check presence vector first!
        double GetGain() const { return mGain; }                                    // Check presence vector first!
        UShort GetFrameRate() const { return mFrameRate; }                            // Check presence vector first!
        UShort GetShutter() const { return mShutter; }                                // Check presence vector first!
        UShort GetImageryControl() const { return mImageryControl; }                // Check presence vector first!
        UShort GetAudioControl() const { return mAudioControl; }                    // Check presence vector first!

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const;
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetCameraCapabilities& operator=(const SetCameraCapabilities& msg);
        virtual int RunTestCase() const;

    protected:
        UShort mPresenceVector;
        Byte mCameraID;                    ///< Camera ID : [1, 355], 0 is reserved
        double mHorizontalFOV;            ///< Horizontal field of view in RADIANS : [0, Pi]
        double mVerticalFOV;            ///< Vertical field of view in RADIANS : [0, Pi]
        UShort mHorizontalResolution;   ///< Number of horizontal scan lines produced by the camera
        UShort mVerticalResolution;     ///< Number of vertical scan lines produced by the camera
        double mFocus;                    ///< [0, 100] % of possible focal length, 0 equates to the focal point nearest the camera
        double mIris;                    ///< [0, 100] % of possible aperture width where 0 equates to a closed aperture and 100 is full open.
        double mGain;                    ///< [0, 100] % of possible gain where 0 equates to the lowest level of allowable visual noise and 100 is the maximum.
        UShort mFrameRate;                ///< For Motion Picture (Video) cameras this value represents the actual Frame Rate in Frames per Second.
        UShort mShutter;                ///< For Still imagery this field is defined as the inverse of the field value. For example 500 would indicate a 1/500th of a second shutter speed. Speeds slower than one second are not available.
        UShort mImageryControl;            ///< Bit field for image control
        UShort mAudioControl;            ///< Bit field for audio control
    };
}


#endif
/*  End of File */
