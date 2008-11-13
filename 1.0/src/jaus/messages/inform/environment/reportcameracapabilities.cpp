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

#include "jaus/messages/inform/environment/reportcameracapabilities.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <string>

using namespace std;
using namespace Jaus;

const double ReportCameraCapabilities::Limits::HorizontalFOVUpperLimit = JAUS_PI;
const double ReportCameraCapabilities::Limits::HorizontalFOVLowerLimit = 0;
const double ReportCameraCapabilities::Limits::VerticalFOVUpperLimit = JAUS_PI;
const double ReportCameraCapabilities::Limits::VerticalFOVLowerLimit = 0;
const unsigned int    ReportCameraCapabilities::Limits::MaxDescriptionLength = 50;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraCapabilities::ReportCameraCapabilities() :
    Message(JAUS_REPORT_CAMERA_CAPABILITIES),
    mPresenceVector(0),
    mCameraID(0),
    mDescription(""),
    mMaxHorizontalFOV(0),
    mMinHorizontalFOV(0),
    mMaxVerticalFOV(0),
    mMinVerticalFOV(0),
    mMaxHorizontalResolution(0),
    mMinHorizontalResolution(0),
    mMaxVerticalResolution(0),
    mMinVerticalResolution(0),
    mMinFrameRate(0),
    mMaxFrameRate(0),
    mMinShutter(0),
    mMaxShutter(0),
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
ReportCameraCapabilities::ReportCameraCapabilities(const ReportCameraCapabilities &msg) :
    Message(JAUS_REPORT_CAMERA_CAPABILITIES),
    mPresenceVector(0),
    mCameraID(0),
    mDescription(""),
    mMaxHorizontalFOV(0),
    mMinHorizontalFOV(0),
    mMaxVerticalFOV(0),
    mMinVerticalFOV(0),
    mMaxHorizontalResolution(0),
    mMinHorizontalResolution(0),
    mMaxVerticalResolution(0),
    mMinVerticalResolution(0),
    mMinFrameRate(0),
    mMaxFrameRate(0),
    mMinShutter(0),
    mMaxShutter(0),
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
ReportCameraCapabilities::~ReportCameraCapabilities()
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the camera ID
///
///   \param value Camera ID. [1, 255]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetCameraID(const Byte value)
{
    if(value > 0)
    {
        mCameraID = value;
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the description of the camera
///
///   \param value The description, limited to 50 characters
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetDescription(const std::string value)
{
    if(value.length() <= Limits::MaxDescriptionLength)
    {
        mDescription = value;
        mDescription.resize(Limits::MaxDescriptionLength);
    }
    else
    {
        mDescription = value.substr(0, Limits::MaxDescriptionLength);
    }

    BitVector::SetBit(mPresenceVector, VectorBit::Description, true);

    return JAUS_OK;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum horizontal FOV
///
///   \param value The FOV in radians
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxHorizontalFOV(const double value)
{
    if(value <= Limits::HorizontalFOVUpperLimit && value >= Limits::HorizontalFOVLowerLimit)
    {
        mMaxHorizontalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::MaxHorizontalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum horizontal FOV
///
///   \param value The FOV in radians
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinHorizontalFOV(const double value)
{
    if(value <= Limits::HorizontalFOVUpperLimit && value >= Limits::HorizontalFOVLowerLimit)
    {
        mMinHorizontalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::MinHorizontalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum vertical FOV
///
///   \param value The FOV in radians
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxVerticalFOV(const double value)
{
    if(value <= Limits::VerticalFOVUpperLimit && value >= Limits::VerticalFOVLowerLimit)
    {
        mMaxVerticalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::MaxVerticalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum vertical field of view
///
///   \param value The FOV in radians
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinVerticalFOV(const double value)
{
    if(value <= Limits::VerticalFOVUpperLimit && value >= Limits::VerticalFOVLowerLimit)
    {
        mMinVerticalFOV = value;
        BitVector::SetBit(mPresenceVector, VectorBit::MinVerticalFOV, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum horizontal resolution
///
///   \param value The maximum number of possible horizontal scan lines produced
///                by the camera
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxHorizontalResolution(const UShort value)
{
    mMaxHorizontalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MaxHorizontalResolution, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum horizontal resolution
///
///   \param value The minimum number of possible horizontal scan lines produced
///                by the camera
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinHorizontalResolution(const UShort value)
{
    mMinHorizontalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MinHorizontalResolution, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum veritcal resolution
///
///   \param value The maximum number of possible vertical scan lines produced
///                by the camera
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxVerticalResolution(const UShort value)
{
    mMaxVerticalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MaxVerticalResolution, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum veritcal resolution
///
///   \param value The maximum number of possible vertical scan lines produced
///                by the camera
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinVerticalResolution(const UShort value)
{
    mMinVerticalResolution = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MinVerticalResolution, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum frame rate
///
///   \param value The frame rate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxFrameRate(const UShort value)
{
    mMaxFrameRate = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MaxFrameRate, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum frame rate
///
///   \param value The frame rate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinFrameRate(const UShort value)
{
    mMinFrameRate = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MinFrameRate, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the maximum shutter speed
///
///   \param value The shutter speed
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMaxShutter(const UShort value)
{
    mMaxShutter = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MaxShutter, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the minimum shutter speed
///
///   \param value The shutter speed
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetMinShutter(const UShort value)
{
    mMinShutter = value;
    BitVector::SetBit(mPresenceVector, VectorBit::MinShutter, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the imagery control bits
///
///   \param value The set of supported features (see the ImageryControlFlags class)
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetImageryControl(const UShort value)
{
    mImageryControl = value;
    BitVector::SetBit(mPresenceVector, VectorBit::ImageryControl, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the audio control bits
///
///   \param value The set of supported features (see the AudioControlFlags class)
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraCapabilities::SetAudioControl(const UShort value)
{
    mAudioControl = value;
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
int ReportCameraCapabilities::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;

        if( msg.Write(mPresenceVector) )
        {
            written = expected = JAUS_USHORT_SIZE;


            written += msg.Write(mCameraID);
            expected += JAUS_BYTE_SIZE;

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Description ) )
            {
                written += msg.Write( mDescription );
                expected += Limits::MaxDescriptionLength;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxHorizontalFOV ) )
            {
                written += msg.Write( mMaxHorizontalFOV, Limits::HorizontalFOVUpperLimit, Limits::HorizontalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinHorizontalFOV ) )
            {
                written += msg.Write( mMinHorizontalFOV, Limits::HorizontalFOVUpperLimit, Limits::HorizontalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxVerticalFOV ) )
            {
                written += msg.Write( mMaxVerticalFOV, Limits::VerticalFOVUpperLimit, Limits::VerticalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinVerticalFOV ) )
            {
                written += msg.Write( mMinHorizontalFOV, Limits::VerticalFOVUpperLimit, Limits::VerticalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxHorizontalResolution ) )
            {
                written += msg.Write( mMaxHorizontalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinHorizontalResolution ) )
            {
                written += msg.Write( mMinHorizontalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxVerticalResolution ) )
            {
                written += msg.Write( mMaxVerticalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinVerticalResolution ) )
            {
                written += msg.Write( mMinVerticalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinFrameRate ) )
            {
                written += msg.Write( mMinFrameRate );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxFrameRate ) )
            {
                written += msg.Write( mMaxFrameRate );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinShutter ) )
            {
                written += msg.Write( mMinShutter );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxShutter ) )
            {
                written += msg.Write( mMaxShutter );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ImageryControl ) )
            {
                written += msg.Write( mImageryControl );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::AudioControl ) )
            {
                written += msg.Write( mAudioControl );
                expected += JAUS_USHORT_SIZE;
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
int ReportCameraCapabilities::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;

        if( msg.Read(mPresenceVector) )
        {
            read = expected = JAUS_USHORT_SIZE;


            read += msg.Read(mCameraID);
            expected += JAUS_BYTE_SIZE;

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Description ) )
            {
                read += msg.Read( mDescription, 50 );
                expected += Limits::MaxDescriptionLength;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxHorizontalFOV ) )
            {
                read += msg.Read( mMaxHorizontalFOV, Limits::HorizontalFOVUpperLimit, Limits::HorizontalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinHorizontalFOV ) )
            {
                read += msg.Read( mMinHorizontalFOV, Limits::HorizontalFOVUpperLimit, Limits::HorizontalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxVerticalFOV ) )
            {
                read += msg.Read( mMaxVerticalFOV, Limits::VerticalFOVUpperLimit, Limits::VerticalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinVerticalFOV ) )
            {
                read += msg.Read( mMinHorizontalFOV, Limits::VerticalFOVUpperLimit, Limits::VerticalFOVLowerLimit, ScaledInteger::UShort );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxHorizontalResolution ) )
            {
                read += msg.Read( mMaxHorizontalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinHorizontalResolution ) )
            {
                read += msg.Read( mMinHorizontalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxVerticalResolution ) )
            {
                read += msg.Read( mMaxVerticalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinVerticalResolution ) )
            {
                read += msg.Read( mMinVerticalResolution );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinFrameRate ) )
            {
                read += msg.Read( mMinFrameRate );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxFrameRate ) )
            {
                read += msg.Read( mMaxFrameRate );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MinShutter ) )
            {
                read += msg.Read( mMinShutter );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::MaxShutter ) )
            {
                read += msg.Read( mMaxShutter );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ImageryControl ) )
            {
                read += msg.Read( mImageryControl );
                expected += JAUS_USHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::AudioControl ) )
            {
                read += msg.Read( mAudioControl );
                expected += JAUS_USHORT_SIZE;
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
void ReportCameraCapabilities::ClearMessageBody()
{
    mPresenceVector = 0;
    mCameraID = 0;
    mDescription = "";
    mMaxHorizontalFOV = 0;
    mMinHorizontalFOV = 0;
    mMaxVerticalFOV = 0;
    mMinVerticalFOV = 0;
    mMaxHorizontalResolution = 0;
    mMinHorizontalResolution = 0;
    mMaxVerticalResolution = 0;
    mMinVerticalResolution = 0;
    mMinFrameRate = 0;
    mMaxFrameRate = 0;
    mMinShutter = 0;
    mMaxShutter = 0;
    mImageryControl = 0;
    mAudioControl = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraCapabilities &ReportCameraCapabilities::operator=(const ReportCameraCapabilities &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mCameraID = msg.mCameraID;
        mDescription = msg.mDescription;
        mMaxHorizontalFOV = msg.mMaxHorizontalFOV;
        mMinHorizontalFOV = msg.mMinHorizontalFOV;
        mMaxVerticalFOV = msg.mMaxVerticalFOV;
        mMinVerticalFOV = msg.mMinVerticalFOV;
        mMaxHorizontalResolution = msg.mMaxHorizontalResolution;
        mMinHorizontalResolution = msg.mMinHorizontalResolution;
        mMaxVerticalResolution = msg.mMaxVerticalResolution;
        mMinVerticalResolution = msg.mMinVerticalResolution;
        mMinFrameRate = msg.mMinFrameRate;
        mMaxFrameRate = msg.mMaxFrameRate;
        mMinShutter = msg.mMinShutter;
        mMaxShutter = msg.mMaxShutter;
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
int ReportCameraCapabilities::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    ReportCameraCapabilities sendingMessage;  // Stores message data natively.
    ReportCameraCapabilities receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetCameraID(65);
    sendingMessage.SetMaxHorizontalFOV(JAUS_PI);
    sendingMessage.SetMinHorizontalFOV(0);
    sendingMessage.SetMaxVerticalFOV(JAUS_PI);
    sendingMessage.SetMinVerticalFOV(0);
    sendingMessage.SetMaxHorizontalResolution(800);
    sendingMessage.SetMinHorizontalResolution(640);
    sendingMessage.SetMaxVerticalResolution(600);
    sendingMessage.SetMinVerticalResolution(480);
    sendingMessage.SetMaxFrameRate(0);
    sendingMessage.SetMinFrameRate(0);
    sendingMessage.SetMaxShutter(500);
    sendingMessage.SetMinShutter(100);
    sendingMessage.SetImageryControl(ImageryControlFlags::AutoFocus | ImageryControlFlags::ImageStabilization);
    sendingMessage.SetAudioControl(AudioControlFlags::Audio | AudioControlFlags::Stero | AudioControlFlags::AutoGain | AudioControlFlags::Directional
        | AudioControlFlags::FrontMicrophone | AudioControlFlags::LeftMicrophone | AudioControlFlags::RearMicrophone |AudioControlFlags::RightMicrophone);


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure.

    ReportCameraCapabilities copiedMessage(sendingMessage);
    ReportCameraCapabilities *clonedMessage = dynamic_cast<ReportCameraCapabilities*>(copiedMessage.Clone());

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
UShort ReportCameraCapabilities::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }
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
UInt ReportCameraCapabilities::GetPresenceVectorMask(const UShort version) const { return 0x7FFF; }

/*  End of File */

