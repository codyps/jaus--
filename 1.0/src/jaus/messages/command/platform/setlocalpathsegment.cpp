////////////////////////////////////////////////////////////////////////////////////
///
///  \file setlocalpathsegment.cpp
///  \brief This file contains the message class for Set Global Waypoint.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 25 August 2008
///  <br>Last Modified: 25 August 2008
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
#include "jaus/messages/command/platform/setlocalpathsegment.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UShort SetLocalPathSegment::Limits::PathSegmentNumberLowerLimit = 0;
const UShort SetLocalPathSegment::Limits::PathSegmentNumberUpperLimit = 65535;

const double SetLocalPathSegment::Limits::MinX = -100000;
const double SetLocalPathSegment::Limits::MaxX =  100000;
const double SetLocalPathSegment::Limits::MinY = -100000;
const double SetLocalPathSegment::Limits::MaxY =  100000;
const double SetLocalPathSegment::Limits::MinZ = -10000;
const double SetLocalPathSegment::Limits::MaxZ =  35000;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetLocalPathSegment::SetLocalPathSegment() :
    Message(JAUS_SET_LOCAL_PATH_SEGMENT),
    mPresenceVector(0),
    mPathSegmentNumber(0),
    mP1X(0),
    mP1Y(0),
    mP1Z(0),
    mP2X(0),
    mP2Y(0),
    mP2Z(0),
    mWeightingFactor(0)
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetLocalPathSegment::SetLocalPathSegment(const SetLocalPathSegment& msg) :
    Message(JAUS_SET_LOCAL_PATH_SEGMENT),
    mPresenceVector(0),
    mPathSegmentNumber(0),
    mP1X(0),
    mP1Y(0),
    mP1Z(0),
    mP2X(0),
    mP2Y(0),
    mP2Z(0),
    mWeightingFactor(0)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetLocalPathSegment::~SetLocalPathSegment()
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the path segment number
///
///   \param value Waypoint number, [0, 65535]
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetPathSegmentNumber(const UShort value)
{
    //if(value >= Limits::PathSegmentNumberLowerLimit && value <= Limits::PathSegmentNumberUpperLimit)
    //{
        mPathSegmentNumber = value;
        return JAUS_OK;
    //}
    //return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the X of P1
///
///   \param value X coordinate, [-100000, 100000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP1X(const double value)
{
    if(value >= Limits::MinX && value <= Limits::MaxX)
    {
        mP1X = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the Y of P1
///
///   \param value Y coordinate, [-100000, 100000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP1Y(const double value)
{
    if(value >= Limits::MinY && value <= Limits::MaxY)
    {
        mP1Y = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the Z of P1
///
///   \param value Z coordinate, [-10000, 35000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP1Z(const double value)
{
    if(value >= Limits::MinZ && value <= Limits::MaxZ)
    {
        mP1Z = value;
        BitVector::SetBit(mPresenceVector, VectorBit::P1Z);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the X of P2
///
///   \param value X coordinate, [-100000, 100000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP2X(const double value)
{
    if(value >= Limits::MinX && value <= Limits::MaxX)
    {
        mP2X = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the Y of P2
///
///   \param value Y coordinate, [-100000, 100000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP2Y(const double value)
{
    if(value >= Limits::MinY && value <= Limits::MaxY)
    {
        mP2Y = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the Z of P2
///
///   \param value Z coordinate, [-10000, 35000] degrees
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetP2Z(const double value)
{
    if(value >= Limits::MinZ && value <= Limits::MaxZ)
    {
        mP2Z = value;
        BitVector::SetBit(mPresenceVector, VectorBit::P2Z);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the weighting factor
///
///   \param value Straightness of the segment, [0, 500] with 0 being straight
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::SetWeightingFactor(const double value)
{
    if(value >= 0 && value <= 500)
    {
        mWeightingFactor = value;
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
int SetLocalPathSegment::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;

        if(msg.Write(mPresenceVector))
        {
            expected = written = JAUS_BYTE_SIZE;

            written += msg.Write(mPathSegmentNumber);
            expected += JAUS_USHORT_SIZE;

            written += msg.Write(mP1X, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            written += msg.Write(mP1Y, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::P1Z))
            {
                written += msg.Write(mP1Z, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }

            written += msg.Write(mP2X, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            written += msg.Write(mP2Y, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::P2Z))
            {
                written += msg.Write(mP2Z, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }

            written += msg.Write(mWeightingFactor, 500, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;

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
int SetLocalPathSegment::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;

        if(msg.Read(mPresenceVector))
        {
            expected = read = JAUS_BYTE_SIZE;

            read += msg.Read(mPathSegmentNumber);
            expected += JAUS_USHORT_SIZE;

            read += msg.Read(mP1X, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            read += msg.Read(mP1Y, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::P1Z))
            {
                read += msg.Read(mP1Z, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }

            read += msg.Read(mP2X, Limits::MaxX, Limits::MinX, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            read += msg.Read(mP2Y, Limits::MaxY, Limits::MinY, ScaledInteger::Int);
            expected += JAUS_INT_SIZE;

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::P2Z))
            {
                read += msg.Read(mP2Z, Limits::MaxZ, Limits::MinZ, ScaledInteger::Int);
                expected += JAUS_INT_SIZE;
            }

            read += msg.Read(mWeightingFactor, 500, 0, ScaledInteger::UShort);
            expected += JAUS_USHORT_SIZE;

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
void SetLocalPathSegment::ClearMessageBody()
{
    mPresenceVector = 0;
    mPathSegmentNumber = 0;
    mP1X = 0;
    mP1Y = 0;
    mP1Z = 0;
    mP2X = 0;
    mP2Y = 0;
    mP2Z = 0;
    mWeightingFactor = 0;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SetLocalPathSegment& SetLocalPathSegment::operator =(const SetLocalPathSegment& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mPathSegmentNumber = msg.mPathSegmentNumber;
        mP1X = msg.mP1X;
        mP1Y = msg.mP1Y;
        mP1Z = msg.mP1Z;
        mP2X = msg.mP2X;
        mP2Y = msg.mP2Y;
        mP2Z = msg.mP2Z;
        mWeightingFactor = msg.mWeightingFactor;
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Unit Testing Code
///
////////////////////////////////////////////////////////////////////////////////////
int SetLocalPathSegment::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    SetLocalPathSegment sendingMessage;  // Stores message data natively.
    SetLocalPathSegment receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetPathSegmentNumber(1);
    sendingMessage.SetP1X(50678);
    sendingMessage.SetP1Y(6078);
    sendingMessage.SetP1Z(10000);
    sendingMessage.SetP2X(55000);
    sendingMessage.SetP2Y(6800);
    sendingMessage.SetP2Z(6000);
    sendingMessage.SetWeightingFactor(250);

    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure.

    SetLocalPathSegment copiedMessage(sendingMessage);
    SetLocalPathSegment *clonedMessage = dynamic_cast<SetLocalPathSegment*>(copiedMessage.Clone());

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
UShort SetLocalPathSegment::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt SetLocalPathSegment::GetPresenceVectorMask(const UShort version) const { return 0x3; }


/*  End of File */
