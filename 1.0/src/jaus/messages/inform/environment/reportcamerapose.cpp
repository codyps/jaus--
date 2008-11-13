////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcamerapose.cpp
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

#include "jaus/messages/inform/environment/reportcamerapose.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <string>

using namespace std;
using namespace Jaus;

const double ReportCameraPose::Limits::MaxCameraOrigin = 32.767;
const double ReportCameraPose::Limits::MinCameraOrigin = -32.767;
const double ReportCameraPose::Limits::MaxCameraAxisCosine = 1.0;
const double ReportCameraPose::Limits::MinCameraAxisCosine = -1.0;
const unsigned int ReportCameraPose::Limits::MaxCameraNameLength = 15;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraPose::ReportCameraPose() :
    Message(JAUS_REPORT_CAMERA_POSE),
    mPresenceVector(0),
    mCameraID(0),
    mCameraName(""),
    mXCameraOrigin(0),
    mYCameraOrigin(0),
    mZCameraOrigin(0),
    mXCameraAxisDirCosineX(0),
    mXCameraAxisDirCosineY(0),
    mXCameraAxisDirCosineZ(0),
    mZCameraAxisDirCosineX(0),
    mZCameraAxisDirCosineY(0),
    mZCameraAxisDirCosineZ(0)
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraPose::ReportCameraPose(const ReportCameraPose &msg) :
    Message(JAUS_REPORT_CAMERA_POSE),
    mPresenceVector(0),
    mCameraID(0),
    mCameraName(""),
    mXCameraOrigin(0),
    mYCameraOrigin(0),
    mZCameraOrigin(0),
    mXCameraAxisDirCosineX(0),
    mXCameraAxisDirCosineY(0),
    mXCameraAxisDirCosineZ(0),
    mZCameraAxisDirCosineX(0),
    mZCameraAxisDirCosineY(0),
    mZCameraAxisDirCosineZ(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraPose::~ReportCameraPose()
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the camera ID
///
///   \param id Camera ID. [1, 255]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetCameraID(const Byte id)
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
///   \brief Set the camera Name
///
///   \param name The name to use, if more than 15 characters it will be truncated
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetCameraName(const std::string name)
{
    if(name.length() <= Limits::MaxCameraNameLength)
    {
        mCameraName = name;
        mCameraName.resize(Limits::MaxCameraNameLength);
    }
    else
    {
        mCameraName = name.substr(0, Limits::MaxCameraNameLength);
    }

    BitVector::SetBit(mPresenceVector, VectorBit::CameraName, true);

    return JAUS_OK;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera's coordinate frame origin
///
///   \param value The X coordinate, in meters
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetXCameraOrigin(const double value)
{
    if(value <= Limits::MaxCameraOrigin && value >= Limits::MinCameraOrigin)
    {
        mXCameraOrigin = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XCameraOrigin, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera's coordinate frame origin
///
///   \param value The Y coordinate, in meters
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetYCameraOrigin(const double value)
{
    if(value <= Limits::MaxCameraOrigin && value >= Limits::MinCameraOrigin)
    {
        mYCameraOrigin = value;
        BitVector::SetBit(mPresenceVector, VectorBit::YCameraOrigin, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera's coordinate frame origin
///
///   \param value The Z coordinate, in meters
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetZCameraOrigin(const double value)
{
    if(value <= Limits::MaxCameraOrigin && value >= Limits::MinCameraOrigin)
    {
        mZCameraOrigin = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZCameraOrigin, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera X axis direction cosine
///
///   \param value The X coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetXCameraAxisDirCosineX(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mXCameraAxisDirCosineX = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XCameraAxisDirCosineX, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera X axis direction cosine
///
///   \param value The Y coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetXCameraAxisDirCosineY(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mXCameraAxisDirCosineY = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XCameraAxisDirCosineY, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera X axis direction cosine
///
///   \param value The Z coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetXCameraAxisDirCosineZ(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mXCameraAxisDirCosineZ = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XCameraAxisDirCosineZ, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera Z axis direction cosine
///
///   \param value The X coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetZCameraAxisDirCosineX(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mZCameraAxisDirCosineX = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZCameraAxisDirCosineX, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera Z axis direction cosine
///
///   \param value The Y coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetZCameraAxisDirCosineY(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mZCameraAxisDirCosineY = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZCameraAxisDirCosineY, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the Camera Z axis direction cosine
///
///   \param value The ZX coordinate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportCameraPose::SetZCameraAxisDirCosineZ(const double value)
{
    if(value <= Limits::MaxCameraAxisCosine && value >= Limits::MinCameraAxisCosine)
    {
        mZCameraAxisDirCosineZ = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZCameraAxisDirCosineZ, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;

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
int ReportCameraPose::WriteMessageBody(Stream& msg, const UShort version) const
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

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::CameraName ) )
            {
                written += msg.Write( mCameraName );
                //written += 15;
                expected += 15;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraOrigin ) )
            {
                written += msg.Write( mXCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::YCameraOrigin ) )
            {
                written += msg.Write( mYCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraOrigin ) )
            {
                written += msg.Write( mZCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineX ) )
            {
                written += msg.Write( mXCameraAxisDirCosineX, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineY ) )
            {
                written += msg.Write( mXCameraAxisDirCosineY, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineZ ) )
            {
                written += msg.Write( mXCameraAxisDirCosineZ, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineX ) )
            {
                written += msg.Write( mZCameraAxisDirCosineX, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineY ) )
            {
                written += msg.Write( mZCameraAxisDirCosineY, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineZ ) )
            {
                written += msg.Write( mZCameraAxisDirCosineZ, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
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
int ReportCameraPose::ReadMessageBody(const Stream& msg, const UShort version)
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

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::CameraName ) )
            {
                read += msg.Read( mCameraName, 15 );
                expected += 15;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraOrigin ) )
            {
                read += msg.Read( mXCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::YCameraOrigin ) )
            {
                read += msg.Read( mYCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraOrigin ) )
            {
                read += msg.Read( mZCameraOrigin, Limits::MaxCameraOrigin, Limits::MinCameraOrigin, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineX ) )
            {
                read += msg.Read( mXCameraAxisDirCosineX, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineY ) )
            {
                read += msg.Read( mXCameraAxisDirCosineY, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::XCameraAxisDirCosineZ ) )
            {
                read += msg.Read( mXCameraAxisDirCosineZ, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineX ) )
            {
                read += msg.Read( mZCameraAxisDirCosineX, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineY ) )
            {
                read += msg.Read( mZCameraAxisDirCosineY, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
                expected += JAUS_SHORT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ZCameraAxisDirCosineZ ) )
            {
                read += msg.Read( mZCameraAxisDirCosineZ, Limits::MaxCameraAxisCosine, Limits::MinCameraAxisCosine, ScaledInteger::Short );
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
void ReportCameraPose::ClearMessageBody()
{
    mPresenceVector = 0;
    mCameraID = 0;
    mCameraName.clear();
    mXCameraOrigin = 0;
    mYCameraOrigin = 0;
    mZCameraOrigin = 0;
    mXCameraAxisDirCosineX = 0;
    mXCameraAxisDirCosineY = 0;
    mXCameraAxisDirCosineZ = 0;
    mZCameraAxisDirCosineX = 0;
    mZCameraAxisDirCosineY = 0;
    mZCameraAxisDirCosineZ = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportCameraPose &ReportCameraPose::operator=(const ReportCameraPose &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mCameraID = msg.mCameraID;
        mCameraName = msg.mCameraName;
        mXCameraOrigin = msg.mXCameraOrigin;
        mYCameraOrigin = msg.mYCameraOrigin;
        mZCameraOrigin = msg.mZCameraOrigin;
        mXCameraAxisDirCosineX = msg.mXCameraAxisDirCosineX;
        mXCameraAxisDirCosineY = msg.mXCameraAxisDirCosineY;
        mXCameraAxisDirCosineZ = msg.mXCameraAxisDirCosineZ;
        mZCameraAxisDirCosineX = msg.mZCameraAxisDirCosineX;
        mZCameraAxisDirCosineY = msg.mZCameraAxisDirCosineY;
        mZCameraAxisDirCosineZ = msg.mZCameraAxisDirCosineZ;
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
int ReportCameraPose::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    ReportCameraPose sendingMessage;  // Stores message data natively.
    ReportCameraPose receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetCameraID(65);
    sendingMessage.SetCameraName("1");
    sendingMessage.SetXCameraOrigin(-32);
    sendingMessage.SetYCameraOrigin(6);
    sendingMessage.SetZCameraOrigin(7);
    sendingMessage.SetXCameraAxisDirCosineX(0.5);
    sendingMessage.SetXCameraAxisDirCosineY(1.0);
    sendingMessage.SetXCameraAxisDirCosineZ(0.7);
    sendingMessage.SetZCameraAxisDirCosineX(0.5);
    sendingMessage.SetZCameraAxisDirCosineY(1.0);
    sendingMessage.SetZCameraAxisDirCosineZ(0.6);


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure.

    ReportCameraPose copiedMessage(sendingMessage);
    ReportCameraPose *clonedMessage = dynamic_cast<ReportCameraPose*>(copiedMessage.Clone());

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
UShort ReportCameraPose::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }
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
UInt ReportCameraPose::GetPresenceVectorMask(const UShort version) const { return 0x3F; }


/*  End of File */

