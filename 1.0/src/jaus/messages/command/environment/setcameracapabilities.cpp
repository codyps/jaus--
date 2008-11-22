////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcameracapabilities.cpp
///  \brief This file contains data structures for the JAUS_SET_CAMERA_CAPABILITIES
///  message belonging to the Environment Sensor Subgroup set of JAUS messages.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 12 August 2008
///  <br>Last Modified: 12 August 2008
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
#include "jaus/messages/command/environment/setcameracapabilities.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/bitvector.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const double SetCameraCapabilities::Limits::MaxFocalLength = 100.0;
const double SetCameraCapabilities::Limits::MinFocalLength = 0.0;
const double SetCameraCapabilities::Limits::MaxApertureWidth = 100.0;
const double SetCameraCapabilities::Limits::MinApertureWidth = 0.0;
const double SetCameraCapabilities::Limits::MaxGain = 100.0;
const double SetCameraCapabilities::Limits::MinGain = 0.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraCapabilities::SetCameraCapabilities() :
    Message(JAUS_SET_CAMERA_CAPABILITIES),
    mPresenceVector(0),
    mCameraID(0),
    mHorizontalFOV(0),
    mVerticalFOV(0),
    mHorizontalResolution(0),
    mVerticalResolution(0),
    mFocus(0),
    mIris(0),
    mGain(0),
    mFrameRate(0),
    mShutter(0),
    mImageryControl(0),
    mAudioControl(0)
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraCapabilities::SetCameraCapabilities(const SetCameraCapabilities& msg) :
    Message(JAUS_SET_CAMERA_CAPABILITIES),
    mPresenceVector(0),
    mCameraID(0),
    mHorizontalFOV(0),
    mVerticalFOV(0),
    mHorizontalResolution(0),
    mVerticalResolution(0),
    mFocus(0),
    mIris(0),
    mGain(0),
    mFrameRate(0),
    mShutter(0),
    mImageryControl(0),
    mAudioControl(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraCapabilities::~SetCameraCapabilities()
{
    //nothing to see here... move along
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the Camera ID for the message.
///
///   \param id Camera ID [1, 255].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetCameraID(const Byte id)
{
    if(id > 0)
    {
        mCameraID = id;
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The horizontal field of view, in radians. [0, PI]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetHorizontalFOV(const double value)
{
    if(value >= 0 && value <= JAUS_PI)
    {
        mHorizontalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::HorizontalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The vertical field of view, in radians. [0, PI]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetVerticalFOV(const double value)
{
    if(value >= 0 && value <= JAUS_PI)
    {
        mVerticalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::VerticalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The horizontal resolution
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetHorizontalResolution(const UShort value)
{
    mHorizontalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::HorizontalResolution, true);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The vertical resolution
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetVerticalResolution(const UShort value)
{
    mVerticalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::VerticalResolution, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The focus value. [0, 100] %
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetFocus(const double value)
{
    if(value >= Limits::MinFocalLength && value <= Limits::MaxFocalLength)
    {
        mFocus = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Focus, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The iris (aperture) value. [0, 100] %
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetIris(const double value)
{
    if(value >= Limits::MinApertureWidth && value <= Limits::MaxApertureWidth)
    {
        mIris = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Iris, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The gain value. [0, 100] %
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetGain(const double value)
{
    if(value >= Limits::MinGain && value <= Limits::MaxGain)
    {
        mGain = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Gain, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The frame rate of the camera
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetFrameRate(const UShort value)
{
    mFrameRate = value;
    BitVector::SetBit(mPresenceVector, VectorBit::FrameRate, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector.
///
///   \param value The inverse of the shutter speed (e.g. 500 for 1/500th of a second)
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetShutter(const UShort value)
{
    if( value >= 1 )
    {
        mShutter = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Shutter, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and image control. For
///          details of what bits can be set, please see the class ImageryControlBit
//           as defined in the header file for SetCameraCapabilities
///
///   \param value The value to set the imagery option to, multiple bits can be |ed
///                together to pass in multiple options.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetImageryControl(const UShort value)
{
    mImageryControl = value;
    BitVector::SetBit(mPresenceVector, VectorBit::ImageryControl, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and imagery control.
///
///   \param flag The image option to set
///   \param value The value to set the image option to [true, false]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetImageryControlBit(const UShort flag, const bool value)
{
    BitVector::SetBit(mImageryControl, flag, value);
    BitVector::SetBit(mPresenceVector, VectorBit::ImageryControl, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and audio control. For
///          details of what bits can be set, please see the class AudioControlBit
//           as defined in the header file for SetCameraCapabilities
///
///   \param value The value to set the audio option to, multiple bits can be |ed
///                together to pass in multiple options.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetAudioControl(const UShort value)
{
    mAudioControl = value;
    BitVector::SetBit(mPresenceVector, VectorBit::AudioControl, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and audio control.
///
///   \param flag The audio option to set
///   \param value The value to set the audio option to [true, false]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::SetAudioControlBit(const UShort flag, const bool value)
{
    BitVector::SetBit(mAudioControl, flag, value);
    BitVector::SetBit(mPresenceVector, VectorBit::AudioControl, true);
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
int SetCameraCapabilities::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;
        if( msg.Write(mPresenceVector) )
        {
            written = expected = JAUS_USHORT_SIZE;

            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mCameraID);

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::HorizontalFOV ) )
            {
                written += msg.Write( mHorizontalFOV, JAUS_PI, 0, ScaledInteger::UShort );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::VerticalFOV ) )
            {
                written += msg.Write( mVerticalFOV, JAUS_PI, 0, ScaledInteger::UShort );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::HorizontalResolution ) )
            {
                written += msg.Write( mHorizontalResolution );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::VerticalResolution ) )
            {
                written += msg.Write( mVerticalResolution );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Focus ) )
            {
                written += msg.Write( mFocus, Limits::MaxFocalLength, Limits::MinFocalLength, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Iris ) )
            {
                written += msg.Write( mIris, Limits::MaxApertureWidth, Limits::MinApertureWidth, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Gain ) )
            {
                written += msg.Write( mGain, Limits::MaxGain, Limits::MinGain, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::FrameRate ) )
            {
                written += msg.Write( mFrameRate );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Shutter ) )
            {
                written += msg.Write( mShutter );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ImageryControl ) )
            {
                written += msg.Write( mImageryControl );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::AudioControl ) )
            {
                written += msg.Write( mAudioControl );
                expected += JAUS_SHORT_SIZE;
            }

            if( expected == written )
            {
                return written;
            }
        }

        SetJausError(ErrorCodes::WriteFailure);
        return -1;
    }
    else
    {
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
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
int SetCameraCapabilities::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;
        if( msg.Read(mPresenceVector) )
        {
            read = expected = JAUS_USHORT_SIZE;

            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mCameraID);

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::HorizontalFOV ) )
            {
                read += msg.Read( mHorizontalFOV, JAUS_PI, 0, ScaledInteger::UShort );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::VerticalFOV ) )
            {
                read += msg.Read( mVerticalFOV, JAUS_PI, 0, ScaledInteger::UShort );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::HorizontalResolution ) )
            {
                read += msg.Read( mHorizontalResolution );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::VerticalResolution ) )
            {
                read += msg.Read( mVerticalResolution );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Focus ) )
            {
                read += msg.Read( mFocus, Limits::MaxFocalLength, Limits::MinFocalLength, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Iris ) )
            {
                read += msg.Read( mIris, Limits::MaxApertureWidth, Limits::MinApertureWidth, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Gain ) )
            {
                read += msg.Read( mGain, Limits::MaxGain, Limits::MinGain, ScaledInteger::Byte );
                expected += JAUS_BYTE_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::FrameRate ) )
            {
                read += msg.Read( mFrameRate );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Shutter ) )
            {
                read += msg.Read( mShutter );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ImageryControl ) )
            {
                read += msg.Read( mImageryControl );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::AudioControl ) )
            {
                read += msg.Read( mAudioControl );
                expected += JAUS_SHORT_SIZE;
            }


            if( expected == read )
            {
                return read;
            }
        }

        SetJausError(ErrorCodes::ReadFailure);
        return -1;
    }
    else
    {
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void SetCameraCapabilities::ClearMessageBody()
{
    mPresenceVector = 0;
    mCameraID = 0;
    mHorizontalFOV = 0;
    mVerticalFOV = 0;
    mHorizontalResolution = 0;
    mVerticalResolution = 0;
    mFocus = 0;
    mIris = 0;
    mGain = 0;
    mFrameRate = 0;
    mShutter = 0;
    mImageryControl = 0;
    mAudioControl = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a new copy of the message
///
////////////////////////////////////////////////////////////////////////////////////
Message* SetCameraCapabilities::Clone() const
{
    return new SetCameraCapabilities(*this);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraCapabilities& SetCameraCapabilities::operator=(const SetCameraCapabilities& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mCameraID = msg.mCameraID;
        mHorizontalFOV = msg.mHorizontalFOV;
        mVerticalFOV = msg.mVerticalFOV;
        mHorizontalResolution = msg.mHorizontalResolution;
        mVerticalResolution = msg.mVerticalResolution;
        mFocus = msg.mFocus;
        mIris = msg.mIris;
        mGain = msg.mGain;
        mFrameRate = msg.mFrameRate;
        mShutter = msg.mShutter;
        mImageryControl = msg.mImageryControl;
        mAudioControl = msg.mAudioControl;
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Test case for message transmission
///
///   \return JAUS_OK for success, JAUS_FAILURE for errors
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraCapabilities::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    SetCameraCapabilities sendingMessage;  // Stores message data natively.
    SetCameraCapabilities receivedMessage; // Stores message data natively.
    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetCameraID(1);
    sendingMessage.SetHorizontalFOV(2.1);
    sendingMessage.SetVerticalFOV(1.0);
    sendingMessage.SetHorizontalResolution(640);
    sendingMessage.SetVerticalResolution(480);
    sendingMessage.SetFocus(20.5);
    sendingMessage.SetIris(90.5);
    sendingMessage.SetGain(45.6);
    sendingMessage.SetFrameRate(0);
    sendingMessage.SetShutter(500);
    sendingMessage.SetImageryControl(ImageryControlMask::AutoGain | ImageryControlMask::AutoFocus);
    sendingMessage.SetImageryControlBit(ImageryControlBit::RedEye, true);
    sendingMessage.SetAudioControlBit(AudioControlBit::Audio, true);


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure.

    SetCameraCapabilities copiedMessage(sendingMessage);
    SetCameraCapabilities *clonedMessage = dynamic_cast<SetCameraCapabilities*>(copiedMessage.Clone());

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.

    if(clonedMessage->Write(packet))
    {
        cout << "Size of serialized data is (including header): " << packet.Length() << endl;
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receivedMessage.Read(packet))
        {
            // Data should match! Check the output to make sure through the debug windows
        }
        else
        {
            cout << "Error: Could not de-serialize message.\n";
            return JAUS_FAILURE;
        }
    }
    else
    {
        cout << "Error: Could not serialize message data.\n";
        return JAUS_FAILURE;
    }

    delete clonedMessage;
    clonedMessage = NULL;

    copiedMessage.ClearMessageBody();
    sendingMessage.ClearMessageBody();

    return JAUS_OK;
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
UShort SetCameraCapabilities::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }
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
UInt SetCameraCapabilities::GetPresenceVectorMask(const UShort version) const { return 0x7FF; }

/* End of File */
