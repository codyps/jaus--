////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcameraformatoptions.h
///  \brief This file contains the message to set Camera Format Options for a Visual
///  Sensor.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 13 August 2008
///  <br>Last Modified: 13 August 2008
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
#ifndef __JAUS_SET_CAMERA_FORMAT_OPTIONS__H
#define __JAUS_SET_CAMERA_FORMAT_OPTIONS__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class SetCameraFormatOptions
    ///  \brief Message to set camera capabilities.
    ///
    ///  The Set Camera Format Options message commands the Visual Component to use the 
    ///  format output as specified for the currently selected camera
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetCameraFormatOptions : public Message
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
                AudioFormat     = 0x0001,
                ImageFormat     = 0x0002,
                FormatOption    = 0x0004,
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
                AudioFormat = 0,
                ImageFormat,
                FormatOption,
            };
        };



        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AudioFormats
        ///   \brief Class continaing enumerations of different audio formats to use
        ///          within message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL AudioFormats
        {
        public:
            enum Type
            {
                Unused = 0,
                RAW,
                PCM,
                AU,
                WAV,
                MID,
                MP3,
                MP2,
                ASF
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ImageFormats
        ///   \brief Class continaing enumerations of different image formats to use
        ///          within message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ImageFormats
        {
        public:
            enum Type
            {
                Unused = 0,
                MPEG2,
                MPEG4,
                MJPEG,
                NTSC,
                PAL,
                TIFF,
                JPEG,
                GIF,
                H263,
                H264,
                PNG,
                BMP,
                RAW,
                PPM,
                PGM,
                PNM,
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Contains constants for limit values of data members of class. Only
        ///          values currently defined in the RA are value. Values outside this
        ///          range are currently reserved.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static Byte MaxAudioFormat;   ///<  Upper limit for Audio Format (8)
            const static Byte MinAudioFormat;   ///<  Lower limit for focus. (0)
            const static Byte MaxImageFormat;   ///<  Upper limit for iris (100)
            const static Byte MinImageFormat;   ///<  Lower limit for iris. (0)
        };

        SetCameraFormatOptions();
        SetCameraFormatOptions(const SetCameraFormatOptions& msg);
        ~SetCameraFormatOptions();

        int SetCameraID(const Byte id);
        int SetAudioFormat(const Byte value);
        int SetImageFormat(const Byte value);
        int SetFormatOption(const UInt value);

        UShort GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const         { return mCameraID; }
        Byte GetAudioFormat() const      { return mAudioFormat; }    // Check presence vector first!
        Byte GetImageFormat() const      { return mImageFormat; }    // Check presence vector first!
        UInt GetFormatOption() const     { return mFormatOption; }    // Check presence vector first!

        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const;
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetCameraFormatOptions& operator=(const SetCameraFormatOptions& msg);
        virtual int RunTestCase() const;

    protected:
        Byte mPresenceVector;
        Byte mCameraID;                ///< Camera ID : [1, 355], 0 is reserved
        Byte mAudioFormat;            ///< The format of the audio:
                                    ///< 0-Unused
                                    ///< 1-RAW
                                    ///< 2-PCM
                                    ///< 3-AU
                                    ///< 4-WAV
                                    ///< 5-MID
                                    ///< 6-MPEG1 Layer 3 (MP3)
                                    ///< 7-MP2
                                    ///< 8-Advanced Streaming Format (ASF)
                                    ///< 9-255 Reserved

        Byte mImageFormat;            ///< The format of the image:
                                    ///< 0 – Unused
                                    ///< 1 – MPEG2
                                    ///< 2 – MPEG4
                                    ///< 3 – MJPEG
                                    ///< 4 – NTSC
                                    ///< 5 – PAL
                                    ///< 6 – TIFF
                                    ///< 7 – JPEG
                                    ///< 8 – GIF
                                    ///< 9 – H263
                                    ///< 10 – H264
                                    ///< 11 – PNG
                                    ///< 12 – BMP
                                    ///< 13 – RAW
                                    ///< 14 – PPM
                                    ///< 15 – PGM
                                    ///< 16 – PNM
                                    ///< 17 – 255 Reserved

        UInt mFormatOption;            ///< The value contained in this field shall not impact interoperability.  
                                    ///< Image format options beyond those published in this message may be 
                                    ///< necessary.  One example use of this field is for the compression 
                                    ///< ratio for video.
    };
}


#endif
/*  End of File */
