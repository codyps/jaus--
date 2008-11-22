////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcameracapabilities.h
///  \brief This file contains the message structure for Report Camera Pose.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 20 August 2008
///  <br>Last Modified: 20 August 2008
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
#ifndef __JAUS_REPORT_CAMERA_CAPABILITIES_H
#define __JAUS_REPORT_CAMERA_CAPABILITIES_H

#include <string>

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportRelativeObjectPosition
    ///   \brief The Report Camera Capabilities message allows the Visual Component to
    ///          report the capabilities for the selected camera.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportCameraCapabilities : public Message
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
                Description                 = 0x01,
                MaxHorizontalFOV            = 0x02,
                MinHorizontalFOV            = 0x04,
                MaxVerticalFOV              = 0x08,
                MinVerticalFOV              = 0x10,
                MaxHorizontalResolution     = 0x20,
                MinHorizontalResolution     = 0x40,
                MaxVerticalResolution       = 0x80,
                MinVerticalResolution       = 0x100,
                MinFrameRate                = 0x200,
                MaxFrameRate                = 0x400,
                MinShutter                  = 0x800,
                MaxShutter                  = 0x1000,
                ImageryControl              = 0x2000,
                AudioControl                = 0x4000,
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
                Description                 = 0,
                MaxHorizontalFOV            = 1,
                MinHorizontalFOV            = 2,
                MaxVerticalFOV              = 3,
                MinVerticalFOV              = 4,
                MaxHorizontalResolution     = 5,
                MinHorizontalResolution     = 6,
                MaxVerticalResolution       = 7,
                MinVerticalResolution       = 8,
                MinFrameRate                = 9,
                MaxFrameRate                = 10,
                MinShutter                  = 11,
                MaxShutter                  = 12,
                ImageryControl              = 13,
                AudioControl                = 14,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ImageryControlFlags
        ///   \brief Class continaing enumerations of different image options
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ImageryControlFlags
        {
        public:
            enum Flags
            {
                AutoFocus               = 0x01,
                AutoExposureOrIris      = 0x02,
                ImageStabilization      = 0x04,
                WhiteBalance            = 0x08,
                SyncFlashOrStrobe       = 0x10,
                RedEye                  = 0x20,
                AutoShutter             = 0x40,
                AutoGain                = 0x80,
                Interlaced              = 0x100
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ImageryControlBits
        ///   \brief Class continaing enumerations of different image options
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ImageryControlBits
        {
        public:
            enum Bits
            {
                AutoFocus = 0,
                AutoExposureOrIris,
                ImageStabilization,
                WhiteBalance,
                SyncFlashOrStrobe,
                RedEye,
                AutoShutter,
                AutoGain,
                Interlaced
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AudioControlFlags
        ///   \brief Class continaing enumerations of different audio options
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL AudioControlFlags
        {
        public:
            enum Flags
            {
                Audio               = 0x01,
                AutoGain            = 0x02,
                Stero               = 0x04,
                Directional         = 0x08,
                FrontMicrophone     = 0x10,
                RearMicrophone      = 0x20,
                LeftMicrophone      = 0x40,
                RightMicrophone     = 0x80
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AudioControlBits
        ///   \brief Class continaing enumerations of different audio options
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL AudioControlBits
        {
        public:
            enum Bits
            {
                Audio = 0,
                AutoGain,
                Stero,
                Directional,
                FrontMicrophone,
                RearMicrophone,
                LeftMicrophone,
                RightMicrophone
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
            const static double HorizontalFOVUpperLimit;    ///<  Upper limit:  Pi radians
            const static double HorizontalFOVLowerLimit;    ///<  Lower limit:  0    radians
            const static double VerticalFOVUpperLimit;      ///<  Upper limit:  Pi radians
            const static double VerticalFOVLowerLimit;      ///<  Lower limit:  0    radians

            const static unsigned int MaxDescriptionLength;            ///<  50 characters
        };

        ReportCameraCapabilities();
        ReportCameraCapabilities(const ReportCameraCapabilities &msg);
        ~ReportCameraCapabilities();

        int SetCameraID(const Byte value);
        int SetDescription(const std::string& value);
        int SetMaxHorizontalFOV(const double value);
        int SetMinHorizontalFOV(const double value);
        int SetMaxVerticalFOV(const double value);
        int SetMinVerticalFOV(const double value);
        int SetMaxHorizontalResolution(const UShort value);
        int SetMinHorizontalResolution(const UShort value);
        int SetMaxVerticalResolution(const UShort value);
        int SetMinVerticalResolution(const UShort value);
        int SetMinFrameRate(const UShort value);
        int SetMaxFrameRate(const UShort value);
        int SetMinShutter(const UShort value);
        int SetMaxShutter(const UShort value);
        int SetImageryControl(const UShort value);
        int SetAudioControl(const UShort value);


        UShort GetPresenceVector() const    { return mPresenceVector; }
        Byte GetCameraID() const            { return mCameraID; }
        std::string GetDescription()        { return mDescription; }             // Check presence vector first!
        double GetMaxHorizontalFOV()        { return mMaxHorizontalFOV; }        // Check presence vector first!
        double GetMinHorizontalFOV()        { return mMinHorizontalFOV; }        // Check presence vector first!
        double GetMaxVerticalFOV()            { return mMaxVerticalFOV; }               // Check presence vector first!
        double GetMinVerticalFOV()            { return mMinVerticalFOV; }               // Check presence vector first!
        UShort GetMaxHorizontalResolution()    { return mMaxHorizontalResolution; }     // Check presence vector first!
        UShort GetMinHorizontalResolution()    { return mMinHorizontalResolution; }     // Check presence vector first!
        UShort GetMaxVerticalResolution()    { return mMaxVerticalResolution; }         // Check presence vector first!
        UShort GetMinVerticalResolution()    { return mMinVerticalResolution; }         // Check presence vector first!
        UShort GetMinFrameRate()            { return mMinFrameRate; }            // Check presence vector first!
        UShort GetMaxFrameRate()            { return mMaxFrameRate; }            // Check presence vector first!
        UShort GetMinShutter()                { return mMinShutter; }                // Check presence vector first!
        UShort GetMaxShutter()                { return mMaxShutter; }                // Check presence vector first!
        UShort GetImageryControl()            { return mImageryControl; }            // Check presence vector first!
        UShort GetAudioControl()            { return mAudioControl; }            // Check presence vector first!


        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportCameraCapabilities(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ReportCameraCapabilities &operator=(const ReportCameraCapabilities &msg);

        virtual int RunTestCase() const;

    protected:
        UShort mPresenceVector;          ///< Bit vector for items being reported.
        Byte mCameraID;                  ///< ID of camera this information describes.
        std::string mDescription;        ///< 50 characters containing the human readable description of the Camera.  This shall be a null terminated ASCII String.
        double mMaxHorizontalFOV;        ///< Max Horizontal FOV supported by the camaera, [0, Pi] radians
        double mMinHorizontalFOV;        ///< Min Horizontal FOV supported by the camaera, [0, Pi] radians
        double mMaxVerticalFOV;          ///< Max Vertical FOV supported by the camaera, [0, Pi] radians
        double mMinVerticalFOV;          ///< Min Vertical FOV supported by the camaera, [0, Pi] radians
        UShort mMaxHorizontalResolution; ///< The maximum number of possible horizontal scan lines produced by the camera
        UShort mMinHorizontalResolution; ///< The minimum number of possible horizontal scan lines produced by the camera
        UShort mMaxVerticalResolution;   ///< The maximum number of possible vertical scan lines produced by the camera
        UShort mMinVerticalResolution;   ///< The minimum number of possible vertical scan lines produced by the camera
        UShort mMinFrameRate;            ///< For video imagery this field indicates the minimum frame rate possible by the camera system in frames per second.  Frame rates slower than 1 frame per second are not available.
        UShort mMaxFrameRate;            ///< For video imagery this field indicates the maximum frame rate possible by the camera system in frames per second.
        UShort mMinShutter;              ///< For Still imagery this field indicates the slowest possible shutter speed as the inverse of the field value.  For example 500 would indicate a 1/500th of a second shutter speed.  Speeds slower than one second are not available.
        UShort mMaxShutter;              ///< For Still imagery this field indicates the fastest possible shutter speed as the inverse of the field value.  For example 500 would indicate a 1/500th of a second shutter speed.
        UShort mImageryControl;          ///< A bit field that describes additional image capabilities of the camera
        UShort mAudioControl;            ///< A bit field that describes additional audio capabilities of the camera
    };
}

#endif

/*  End of File */
