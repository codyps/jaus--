////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcameraformatoptions.h
///  \brief This file contains the message structure for Report Camera Formate
///  Options.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 6 June 2008
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
#ifndef __JAUS_REPORT_CAMERA_FORMAT_OPTIONS_H
#define __JAUS_REPORT_CAMERA_FORMAT_OPTIONS_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportCameraFormatOptions
    ///   \brief The Report Camera Format Options message allows the Visual Component
    ///   to report output formats available for the selected camera.  Note that up to
    ///   four image formats and two audio formats can be supported.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportCameraFormatOptions : public Message
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
                AudioFormat1 = 0x01,
                AudioFormat2 = 0x02,
                ImageFormat1 = 0x04,
                ImageFormat2 = 0x08,
                ImageFormat3 = 0x10,
                ImageFormat4 = 0x20,
                FormatOption = 0x40
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
                AudioFormat1 = 0,
                AudioFormat2,
                ImageFormat1,
                ImageFormat2,
                ImageFormat3,
                ImageFormat4,
                FormatOption
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
                AdvancedStreamingAudioFormat
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
        ReportCameraFormatOptions();
        ReportCameraFormatOptions(const ReportCameraFormatOptions &msg);
        ~ReportCameraFormatOptions();
        int SetCameraID(const Byte id);
        int SetAudioFormat1(const Byte value);
        int SetAudioFormat2(const Byte value);
        int SetImageFormat1(const Byte value);
        int SetImageFormat2(const Byte value);
        int SetImageFormat3(const Byte value);
        int SetImageFormat4(const Byte value);
        int SetFormatOption(const UInt value);
        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetCameraID() const { return mCameraID; }
        Byte GetAudioFormat1() const { return mAudioFormat1; } // Check presence vector first!
        Byte GetAudioFormat2() const { return mAudioFormat2; } // Check presence vector first!
        Byte GetImageFormat1() const { return mImageFormat1; } // Check presence vector first!
        Byte GetImageFormat2() const { return mImageFormat2; } // Check presence vector first!
        Byte GetImageFormat3() const { return mImageFormat3; } // Check presence vector first!
        Byte GetImageFormat4() const { return mImageFormat4; } // Check presence vector first!
        UInt GetFormatOption() const { return mFormatOption; } // Check presence vector first!
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportCameraFormatOptions(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ReportCameraFormatOptions &operator=(const ReportCameraFormatOptions &msg);
    protected:
        Byte mCameraID;         ///<  ID of camera to query.
        Byte mAudioFormat1;     ///<  Audio format supported by visual sensor.
        Byte mAudioFormat2;     ///<  Audio format supported by visual sensor.
        Byte mImageFormat1;     ///<  Image format supported by visual sensor.
        Byte mImageFormat2;     ///<  Image format supported by visual sensor.
        Byte mImageFormat3;     ///<  Image format supported by visual sensor.
        Byte mImageFormat4;     ///<  Image format supported by visual sensor.
        UInt mFormatOption;     ///<  Reserved field for future interoperability.
        Byte mPresenceVector;   ///<  Bit vector for items being queried.
    };
}

#endif

#endif 

/*  End of File */
