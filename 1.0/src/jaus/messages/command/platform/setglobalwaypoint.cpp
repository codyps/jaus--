////////////////////////////////////////////////////////////////////////////////////
///
///  \file setglobalwaypoint.cpp
///  \brief This file contains the message class for Set Global Waypoint.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 22 August 2008
///  <br>Last Modified: 22 August 2008
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
#include "jaus/messages/command/platform/setglobalwaypoint.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UShort SetGlobalWaypoint::Limits::WaypointNumberLowerLimit = 0;
const UShort SetGlobalWaypoint::Limits::WaypointNumberUpperLimit = 65535;

const double SetGlobalWaypoint::Limits::MinLatitude = -90;
const double SetGlobalWaypoint::Limits::MaxLatitude = 90;
const double SetGlobalWaypoint::Limits::MinLongitude = -180;
const double SetGlobalWaypoint::Limits::MaxLongitude = 180;
const double SetGlobalWaypoint::Limits::MinAltitude = -10000;
const double SetGlobalWaypoint::Limits::MaxAltitude = 35000;
const double SetGlobalWaypoint::Limits::MinAngle = -JAUS_PI;
const double SetGlobalWaypoint::Limits::MaxAngle = JAUS_PI;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalWaypoint::SetGlobalWaypoint() :
    Message(JAUS_SET_GLOBAL_WAYPOINT),
    mPresenceVector(0),
    mWaypointNumber(0),
    mLatitude(0),
    mLongitude(0),
    mAltitude(0),
    mRoll(0),
    mPitch(0),
    mYaw(0)
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalWaypoint::SetGlobalWaypoint(const SetGlobalWaypoint& msg) :
    Message(JAUS_SET_GLOBAL_WAYPOINT),
    mPresenceVector(0),
    mWaypointNumber(0),
    mLatitude(0),
    mLongitude(0),
    mAltitude(0),
    mRoll(0),
    mPitch(0),
    mYaw(0)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalWaypoint::~SetGlobalWaypoint()
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the waypoint number
///
///   \param value Waypoint number, [0, 65535]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetWaypointNumber(const UShort value)
{
    //if(value >= Limits::WaypointNumberLowerLimit && value <= Limits::WaypointNumberUpperLimit)
    //{
        mWaypointNumber = value;
        return JAUS_OK;
    //}
    //return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the latitude
///
///   \param value Latitude coordinate, [-90, 90] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetLatitude(const double value)
{
    if(value >= Limits::MinLatitude && value <= Limits::MaxLatitude)
    {
        mLatitude = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the longitude
///
///   \param value Longitude coordinate, [-90, 90] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetLongitude(const double value)
{
    if(value >= Limits::MinLongitude && value <= Limits::MaxLongitude)
    {
        mLongitude = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the altitude
///
///   \param value Altitude coordinate, [-90, 90] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetAltitude(const double value)
{
    if(value >= Limits::MinAltitude && value <= Limits::MaxAltitude)
    {
        mAltitude = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Altitude);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Roll value
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetRoll(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mRoll = value;
        BitVector::SetBit( mPresenceVector, VectorBit::Roll, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Pitch value
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetPitch(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mPitch = value;
        BitVector::SetBit( mPresenceVector, VectorBit::Pitch, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector for the message.
///
///   \param value Yaw value
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::SetYaw(const double value)
{
    if(value >= Limits::MinAngle && value <= Limits::MaxAngle)
    {
        mYaw = value;
        BitVector::SetBit( mPresenceVector, VectorBit::Yaw, true);
        return JAUS_OK;
    }
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
int SetGlobalWaypoint::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;

        if(msg.Write(mPresenceVector))
        {
            expected = written = JAUS_BYTE_SIZE;

            written += msg.Write(mWaypointNumber);
            expected += JAUS_USHORT_SIZE;

            written += msg.Write(mLatitude, Limits::MaxLatitude, Limits::MinLatitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            written += msg.Write(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Altitude))
            {
                written += msg.Write(mAltitude, Limits::MaxAltitude, Limits::MinAltitude, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Roll))
            {
                written += msg.Write(mRoll, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch))
            {
                written += msg.Write(mPitch, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Yaw))
            {
                written += msg.Write(mYaw, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(expected == written)
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
int SetGlobalWaypoint::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;

        if(msg.Read(mPresenceVector))
        {
            expected = read = JAUS_BYTE_SIZE;

            read += msg.Read(mWaypointNumber);
            expected += JAUS_USHORT_SIZE;

            read += msg.Read(mLatitude, Limits::MaxLatitude, Limits::MinLatitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            read += msg.Read(mLongitude, Limits::MaxLongitude, Limits::MinLongitude, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Altitude))
            {
                read += msg.Read(mAltitude, Limits::MaxAltitude, Limits::MinAltitude, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Roll))
            {
                read += msg.Read(mRoll, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Pitch))
            {
                read += msg.Read(mPitch, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }
            if(BitVector::IsBitSet(mPresenceVector, VectorBit::Yaw))
            {
                read += msg.Read(mYaw, Limits::MaxAngle, Limits::MinAngle, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(expected == read)
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
void SetGlobalWaypoint::ClearMessageBody()
{
    mPresenceVector = 0;
    mWaypointNumber = 0;
    mLatitude = 0;
    mLongitude = 0;
    mAltitude = 0;
    mRoll = 0;
    mPitch = 0;
    mYaw = 0;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SetGlobalWaypoint& SetGlobalWaypoint::operator =(const SetGlobalWaypoint& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mWaypointNumber = msg.mWaypointNumber;
        mLatitude = msg.mLatitude;
        mLongitude = msg.mLongitude;
        mAltitude = msg.mAltitude;
        mRoll = msg.mRoll;
        mPitch = msg.mPitch;
        mYaw = msg.mYaw;
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Unit Testing Code
///
////////////////////////////////////////////////////////////////////////////////////
int SetGlobalWaypoint::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    SetGlobalWaypoint sendingMessage;  // Stores message data natively.
    SetGlobalWaypoint receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetWaypointNumber(65);
    sendingMessage.SetLatitude(28.3);
    sendingMessage.SetLongitude(81.3);
    sendingMessage.SetAltitude(100001.5);
    sendingMessage.SetYaw(-4.5);
    sendingMessage.SetPitch(0.3);
    sendingMessage.SetRoll(1.2);

    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure.

    SetGlobalWaypoint copiedMessage(sendingMessage);
    SetGlobalWaypoint *clonedMessage = dynamic_cast<SetGlobalWaypoint*>(copiedMessage.Clone());

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
UShort SetGlobalWaypoint::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt SetGlobalWaypoint::GetPresenceVectorMask(const UShort version) const { return 0xF; }

/*  End of File */
