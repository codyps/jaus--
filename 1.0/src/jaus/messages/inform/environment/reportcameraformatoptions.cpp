////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcameraformatoptions.cpp
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
#include "jaus/messages/inform/environment/reportcameraformatoptions.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraFormatOptions::ReportCameraFormatOptions() : Message(JAUS_REPORT_CAMERA_FORMAT_OPTIONS)
{
    mCameraID = 0;         
    mAudioFormat1 = 0;     
    mAudioFormat2 = 0;     
    mImageFormat1 = 0;     
    mImageFormat2 = 0;     
    mImageFormat3 = 0;     
    mImageFormat4 = 0;     
    mFormatOption = 0;     
    mPresenceVector = 0;   
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraFormatOptions::ReportCameraFormatOptions(const ReportCameraFormatOptions &msg) : Message(JAUS_REPORT_CAMERA_FORMAT_OPTIONS)
{
    mCameraID = 0;         
    mAudioFormat1 = 0;     
    mAudioFormat2 = 0;     
    mImageFormat1 = 0;     
    mImageFormat2 = 0;     
    mImageFormat3 = 0;     
    mImageFormat4 = 0;     
    mFormatOption = 0;     
    mPresenceVector = 0; 
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraFormatOptions::~ReportCameraFormatOptions()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the camera ID.
///
///   \param value ID of the camera [1,255].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetCameraID(const Byte value)
{
    mCameraID = value;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the first type of audio format supported by component.
///
///   \param value Audio format type 1 supported by component. See AudioFormat
///   class for enumerations of possible values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetAudioFormat1(const Byte value)
{
    mAudioFormat1 = value;
    mPresenceVector |= VectorMask::AudioFormat1;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the second type of audio format supported by component.
///
///   \param value Audio format type 1 supported by component. See AudioFormat
///   class for enumerations of possible values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetAudioFormat2(const Byte value)
{
    mAudioFormat2 = value;
    mPresenceVector |= VectorMask::AudioFormat2;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set a supported image format for image transmission used by the
///          sensor.
///
///   \param value Image format to set that is supported by your component.  See
///                ImageFormats class for enumeration values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetImageFormat1(const Byte value)
{
    mImageFormat1 = value;
    mPresenceVector |= VectorMask::ImageFormat1;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set a supported image format for image transmission used by the
///          sensor.
///
///   \param value Image format to set that is supported by your component.  See
///                ImageFormats class for enumeration values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetImageFormat2(const Byte value)
{
    mImageFormat2 = value;
    mPresenceVector |= VectorMask::ImageFormat2;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set a supported image format for image transmission used by the
///          sensor.
///
///   \param value Image format to set that is supported by your component.  See
///                ImageFormats class for enumeration values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetImageFormat3(const Byte value)
{
    mImageFormat3 = value;
    mPresenceVector |= VectorMask::ImageFormat3;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set a supported image format for image transmission used by the
///          sensor.
///
///   \param value Image format to set that is supported by your component.  See
///                ImageFormats class for enumeration values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetImageFormat4(const Byte value)
{
    mImageFormat4 = value;
    mPresenceVector |= VectorMask::ImageFormat4;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Currently this is a rserved field to be changed by a future RA.
///          This field shall not impact interoperability.  Image format options
///          beyond those published in this message may be necessary.  One example
///          use of this field is for the compression ratio of video.
///
///   \param value Reserved value for additional information for message.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::SetFormatOption(const UInt value)
{
    mFormatOption = value;
    mPresenceVector |= VectorMask::FormatOption;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message body data to the stream.
///
///   \param msg The stream to write to.
///   \param version The desired version of the message to write.
///
///   \return Number of bytes written on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::WriteMessageBody(Stream& msg, 
                                                const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;
        expected = JAUS_BYTE_SIZE*2;
        written += msg.Write(mPresenceVector);
        written += msg.Write(mCameraID);

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::AudioFormat1))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mAudioFormat1);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::AudioFormat2))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mAudioFormat2);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat1))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mImageFormat1);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat2))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mImageFormat2);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat3))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mImageFormat3);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat4))
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mImageFormat4);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::FormatOption))
        {
            expected += JAUS_UINT_SIZE;
            written += msg.Write(mFormatOption);
        }

        if( written == expected) 
        {
            return written;
        }
        else
        {
            SetJausError(ErrorCodes::WriteFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message body data from the stream.
///
///   \param msg The stream to read from.
///   \param version The desired version of the message to read.
///
///   \return Number of bytes read on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraFormatOptions::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        expected = JAUS_BYTE_SIZE*2;
        read += msg.Read(mPresenceVector);
        read += msg.Read(mCameraID);

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::AudioFormat1))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mAudioFormat1);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::AudioFormat2))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mAudioFormat2);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat1))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mImageFormat1);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat2))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mImageFormat2);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat3))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mImageFormat3);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ImageFormat4))
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mImageFormat4);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::FormatOption))
        {
            expected += JAUS_UINT_SIZE;
            read += msg.Read(mFormatOption);
        }

        if( read == expected) 
        {
            return read;
        }
        else
        {
            SetJausError(ErrorCodes::ReadFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportCameraFormatOptions::ClearMessageBody()
{
    mCameraID = 0;         
    mAudioFormat1 = 0;     
    mAudioFormat2 = 0;     
    mImageFormat1 = 0;     
    mImageFormat2 = 0;     
    mImageFormat3 = 0;     
    mImageFormat4 = 0;     
    mFormatOption = 0;     
    mPresenceVector = 0; 
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraFormatOptions &ReportCameraFormatOptions::operator=(const ReportCameraFormatOptions &msg)
{
    CopyHeaderData(&msg);
    mCameraID =          msg.mCameraID;
    mAudioFormat1 =      msg.mAudioFormat1;
    mAudioFormat2 =      msg.mAudioFormat2;
    mImageFormat1 =      msg.mImageFormat1;
    mImageFormat2 =      msg.mImageFormat2;
    mImageFormat3 =      msg.mImageFormat3;
    mImageFormat4 =      msg.mImageFormat4;
    mFormatOption =      msg.mFormatOption;
    mPresenceVector =    msg.mPresenceVector;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size in bytes of the presence vector used by
///          the message.
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  size.  Defaults is current version of library.
///
///   \return Size in bytes of presence vector associated with message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort ReportCameraFormatOptions::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the bit mask associated with the message indicating the
///          bits used in the presence vector of this message (if it has one).
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  mask.  Defaults is current version of library.
///
///   \return Presence vector mask associated with message.  This value
///           is used to determine what bits are used in the presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
UInt ReportCameraFormatOptions::GetPresenceVectorMask(const UShort version) const { return 0x7F; }

/*  End of File */
