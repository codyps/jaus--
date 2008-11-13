////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportrelativeobjectposition.h
///  \brief This file contains the message structure for Report Camera Pose.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 18 August 2008
///  <br>Last Modified: 18 August 2008
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

#include "jaus/messages/inform/environment/reportrelativeobjectposition.h"
#include "jaus/messages/inform/informcodes.h"
#include "jaus/messages/time.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const double ReportRelativeObjectPosition::Limits::MaxRange = 10000.0;
const double ReportRelativeObjectPosition::Limits::MinRange = -10000.0;
const double ReportRelativeObjectPosition::Limits::MaxRangeError = 1000.0;
const double ReportRelativeObjectPosition::Limits::MinRangeError = 0;
const double ReportRelativeObjectPosition::Limits::MaxBearing = JAUS_PI;
const double ReportRelativeObjectPosition::Limits::MinBearing = 0 - JAUS_PI;
const double ReportRelativeObjectPosition::Limits::MaxBearingError = JAUS_PI;
const double ReportRelativeObjectPosition::Limits::MinBearingError = 0;
const double ReportRelativeObjectPosition::Limits::MaxInclination = JAUS_PI;
const double ReportRelativeObjectPosition::Limits::MinInclination = 0;
const double ReportRelativeObjectPosition::Limits::MaxInclinationError = JAUS_PI;
const double ReportRelativeObjectPosition::Limits::MinInclinationError = 0;
const Byte ReportRelativeObjectPosition::Limits::MinConfidence = 0;
const Byte ReportRelativeObjectPosition::Limits::MaxConfidence = 255;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportRelativeObjectPosition::ReportRelativeObjectPosition() :
    Message(JAUS_REPORT_RELATIVE_OBJECT_POSITION),
    mPresenceVector(0),
    mTimeStamp(),
    mRange(0),
    mRangeError(0),
    mBearing(0),
    mBearingError(0),
    mInclination(0),
    mInclinationError(0),
    mConfidence(0),
    mObjectID(0)
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportRelativeObjectPosition::ReportRelativeObjectPosition(const ReportRelativeObjectPosition &msg) :
    Message(JAUS_REPORT_RELATIVE_OBJECT_POSITION),
    mPresenceVector(0),
    mTimeStamp(),
    mRange(0),
    mRangeError(0),
    mBearing(0),
    mBearingError(0),
    mInclination(0),
    mInclinationError(0),
    mConfidence(0),
    mObjectID(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportRelativeObjectPosition::~ReportRelativeObjectPosition()
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the time stamp for the data in the message
///
///   \param value The time
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetTimeStamp(const Time value)
{
    mTimeStamp = value;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the range to the relative object
///
///   \param value The range of the object
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetRange(const double value)
{
    if(value <= Limits::MaxRange && value >= Limits::MinRange)
    {
        mRange = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Range, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the error of the range measurement
///
///   \param value The range range
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetRangeError(const double value)
{
    if(value <= Limits::MaxRangeError && value >= Limits::MinRangeError)
    {
        mRangeError = value;
        BitVector::SetBit(mPresenceVector, VectorBit::RangeError, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the bearing to the relative object
///
///   \param value The bearing of the object
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetBearing(const double value)
{
    if(value <= Limits::MaxBearing && value >= Limits::MinBearing)
    {
        mBearing = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Bearing, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the error of the bearing measurement
///
///   \param value The bearing error
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetBearingError(const double value)
{
    if(value <= Limits::MaxBearingError && value >= Limits::MinBearingError)
    {
        mBearingError = value;
        BitVector::SetBit(mPresenceVector, VectorBit::BearingError, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the inclination to the relative object
///
///   \param value The inclination of the object
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetInclination(const double value)
{
    if(value <= Limits::MaxInclination && value >= Limits::MinInclination)
    {
        mInclination = value;
        BitVector::SetBit(mPresenceVector, VectorBit::Inclination, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the error of the inclination measurement
///
///   \param value The inclination error
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetInclinationError(const double value)
{
    if(value <= Limits::MaxInclinationError && value >= Limits::MinInclinationError)
    {
        mInclinationError = value;
        BitVector::SetBit(mPresenceVector, VectorBit::InclinationError, true);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the confidence of the measurements
///
///   \param value The confidence: 0 - Low, 255 - Highest
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetConfidence(const Byte value)
{
    mConfidence = value;
    BitVector::SetBit(mPresenceVector, VectorBit::Confidence, true);
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the object identifier value
///
///   \param value Object identifier
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportRelativeObjectPosition::SetObjectID(const UShort value)
{
    mObjectID = value;
    BitVector::SetBit( mPresenceVector, VectorBit::ObjectID, true );
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
int ReportRelativeObjectPosition::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        if( msg.Write(mPresenceVector) )
        {
            written = expected = JAUS_BYTE_SIZE;

            written += msg.Write( mTimeStamp.ToUInt() );
            expected += JAUS_UINT_SIZE;

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Range ) )
            {
                written += msg.Write( mRange, Limits::MaxRange, Limits::MinRange, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::RangeError ) )
            {
                written += msg.Write( mRangeError, Limits::MaxRangeError, Limits::MinRangeError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Bearing ) )
            {
                written += msg.Write( mBearing, Limits::MaxBearing, Limits::MinBearing, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::RangeError ) )
            {
                written += msg.Write( mBearingError, Limits::MaxBearingError, Limits::MinBearingError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Inclination ) )
            {
                written += msg.Write( mInclination, Limits::MaxInclination, Limits::MinInclination, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::InclinationError ) )
            {
                written += msg.Write( mInclinationError, Limits::MaxInclinationError, Limits::MinInclinationError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Confidence ) )
            {
                written += msg.Write( mConfidence );
                expected += JAUS_BYTE_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ObjectID ) )
            {
                written += msg.Write( mObjectID );
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
int ReportRelativeObjectPosition::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        if( msg.Read(mPresenceVector) )
        {
            read = expected = JAUS_BYTE_SIZE;

            //Read can not read data directly into a Time object
            UInt timeCode;
            read += msg.Read( timeCode );        
            mTimeStamp.SetTime(timeCode);
            expected += JAUS_UINT_SIZE;

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Range ) )
            {
                read += msg.Read( mRange, Limits::MaxRange, Limits::MinRange, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::RangeError ) )
            {
                read += msg.Read( mRangeError, Limits::MaxRangeError, Limits::MinRangeError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Bearing ) )
            {
                read += msg.Read( mBearing, Limits::MaxBearing, Limits::MinBearing, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::RangeError ) )
            {
                read += msg.Read( mBearingError, Limits::MaxBearingError, Limits::MinBearingError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Inclination ) )
            {
                read += msg.Read( mInclination, Limits::MaxInclination, Limits::MinInclination, ScaledInteger::Int );
                expected += JAUS_INT_SIZE;
            }
            if( BitVector::IsBitSet( mPresenceVector, VectorBit::InclinationError ) )
            {
                read += msg.Read( mInclinationError, Limits::MaxInclinationError, Limits::MinInclinationError, ScaledInteger::UInt );
                expected += JAUS_UINT_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::Confidence ) )
            {
                read += msg.Read( mConfidence );
                expected += JAUS_BYTE_SIZE;
            }

            if( BitVector::IsBitSet( mPresenceVector, VectorBit::ObjectID ) )
            {
                read += msg.Read( mObjectID );
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
void ReportRelativeObjectPosition::ClearMessageBody()
{
    mPresenceVector = 0;
    mTimeStamp.SetTime(0);
    mRange = 0;
    mRangeError = 0;
    mBearing = 0;
    mBearingError = 0;
    mInclination = 0;
    mInclinationError = 0;
    mConfidence = 0;
    mObjectID = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the fields based on the presence vector mask passed.
///
///   \param mask Any bit position set in the mask will clear a field
///               in the message.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportRelativeObjectPosition::ClearFields(const UShort mask)
{
    mPresenceVector &= ~mask;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportRelativeObjectPosition &ReportRelativeObjectPosition::operator=(const ReportRelativeObjectPosition &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mTimeStamp = msg.mTimeStamp;
        mRange = msg.mRange;
        mRangeError = msg.mRangeError;
        mBearing = msg.mBearing;
        mBearingError = msg.mBearingError;
        mInclination = msg.mInclination;
        mInclinationError = msg.mInclinationError;
        mConfidence = msg.mConfidence;
        mObjectID = msg.mObjectID;
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
int ReportRelativeObjectPosition::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    ReportRelativeObjectPosition sendingMessage;  // Stores message data natively.
    ReportRelativeObjectPosition receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    Time t;
    t.SetCurrentTime();
    sendingMessage.SetTimeStamp(t);
    sendingMessage.SetRange(90000);
    sendingMessage.SetRangeError(500);
    sendingMessage.SetBearing(3.1);
    sendingMessage.SetBearingError(0.6);
    sendingMessage.SetInclination(2.2);
    sendingMessage.SetInclinationError(0.8);
    sendingMessage.SetConfidence(100);
    sendingMessage.SetObjectID(25);


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure. 

    ReportRelativeObjectPosition copiedMessage(sendingMessage);
    ReportRelativeObjectPosition *clonedMessage = dynamic_cast<ReportRelativeObjectPosition*>(copiedMessage.Clone());

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
UShort ReportRelativeObjectPosition::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt ReportRelativeObjectPosition::GetPresenceVectorMask(const UShort version) const { return 0xFF; }

/*  End of File */

