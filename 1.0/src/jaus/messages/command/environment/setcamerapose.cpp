////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcamerapose.cpp
///  \brief This file contains data structures for the JAUS_SET_CAMERA_POSE
///  message belonging to the Environment Sensor Subgroup set of JAUS messages.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 8 August 2008
///  <br>Last Modified: 11 August 2008
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
#include "jaus/messages/command/environment/setcamerapose.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/bitvector.h"
#include <iostream>
#include <cmath>

using namespace std;
using namespace Jaus;

const double SetCameraPose::Limits::DisplacementOrLinearRateUpperLimit = 100.0;
const double SetCameraPose::Limits::DisplacementOrLinearRateLowerLimit = -100.0;
const double SetCameraPose::Limits::AngleOrAngularRotationRateUpperLimit = 100.0;
const double SetCameraPose::Limits::AngleOrAngularRotationRateLowerLimit = -100.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraPose::SetCameraPose() : 
        Message(JAUS_SET_CAMERA_POSE),
        mPresenceVector(0),
        mCameraID(0),
        mModeIndicator(0),
        mXDisplacementOrLinearRate(0),
        mYDisplacementOrLinearRate(0),
        mZDisplacementOrLinearRate(0),
        mXAngleOrAngularRotationRate(0),
        mYAngleOrAngularRotationRate(0),
        mZAngleOrAngularRotationRate(0)
{
    //do nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraPose::SetCameraPose(const SetCameraPose& msg) :
        Message(JAUS_SET_CAMERA_POSE),
        mPresenceVector(0),
        mCameraID(0),
        mModeIndicator(0),
        mXDisplacementOrLinearRate(0),
        mYDisplacementOrLinearRate(0),
        mZDisplacementOrLinearRate(0),
        mXAngleOrAngularRotationRate(0),
        mYAngleOrAngularRotationRate(0),
        mZAngleOrAngularRotationRate(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraPose::~SetCameraPose()
{
    //do nothing
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
int SetCameraPose::SetCameraID(const Byte id)
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
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param displacementRate Boolean flag, true if the value is displacement rate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetXDisplacementOrLinearRate(const double value, const bool displacementRate)
{
    if(fabs(value) <= Limits::DisplacementOrLinearRateUpperLimit)
    {
        mXDisplacementOrLinearRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XDisplacementOrLinearRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::XDisplacementOrLinearRate, displacementRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param displacementRate Boolean flag, true if the value is displacement rate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetYDisplacementOrLinearRate(const double value, const bool displacementRate)
{
    if(fabs(value) <= Limits::DisplacementOrLinearRateUpperLimit)
    {
        mYDisplacementOrLinearRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::YDisplacementOrLinearRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::YDisplacementOrLinearRate, displacementRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param displacementRate Boolean flag, true if the value is displacement rate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetZDisplacementOrLinearRate(const double value, const bool displacementRate)
{
    if(fabs(value) <= Limits::DisplacementOrLinearRateUpperLimit)
    {
        mZDisplacementOrLinearRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZDisplacementOrLinearRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::ZDisplacementOrLinearRate, displacementRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param angleRate Boolean flag, true if the value is angular rate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetXAngleOrAngularRotationRate(const double value, const bool angleRate)
{
    if(fabs(value) <= Limits::AngleOrAngularRotationRateUpperLimit)
    {
        mXAngleOrAngularRotationRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::XAngleOrAngularRotationRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::XAngleOrAngularRotationRate, angleRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param angleRate Boolean flag, true if the value is angular rate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetYAngleOrAngularRotationRate(const double value, const bool angleRate)
{
    if(fabs(value) <= Limits::AngleOrAngularRotationRateUpperLimit)
    {
        mYAngleOrAngularRotationRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::YAngleOrAngularRotationRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::YAngleOrAngularRotationRate, angleRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value and updates the presence vector and mode indicator.
///
///   \param value Desired rate [-100, 100] %.
///   \param angleRate Boolean flag, true if the value is angular rate
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetCameraPose::SetZAngleOrAngularRotationRate(const double value, const bool angleRate)
{
    if(fabs(value) <= Limits::AngleOrAngularRotationRateUpperLimit)
    {
        mZAngleOrAngularRotationRate = value;
        BitVector::SetBit(mPresenceVector, VectorBit::ZAngleOrAngularRotationRate, true);
        BitVector::SetBit(mModeIndicator, VectorBit::ZAngleOrAngularRotationRate, angleRate);
        return JAUS_OK;
    }
    else
        return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of X Displacement or Linear Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetXDisplacementOrLinearRate() const
{
    return mXDisplacementOrLinearRate;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of Y Displacement or Linear Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetYDisplacementOrLinearRate() const
{
    return mYDisplacementOrLinearRate;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of Z Displacement or Linear Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetZDisplacementOrLinearRate() const
{
    return mZDisplacementOrLinearRate;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of X Angle or Angular Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetXAngleOrAngularRotationRate() const
{
    return mXAngleOrAngularRotationRate;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of Y Angle or Angular Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetYAngleOrAngularRotationRate() const
{
    return mYAngleOrAngularRotationRate;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return Value of Z Angle or Angular Rate for the message
///
////////////////////////////////////////////////////////////////////////////////////
inline double SetCameraPose::GetZAngleOrAngularRotationRate() const
{
    return mZAngleOrAngularRotationRate;
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
int SetCameraPose::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        if( msg.Write(mPresenceVector) )
        {
            written = expected = 1;

            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mCameraID);

            expected += JAUS_BYTE_SIZE;
            written += msg.Write(mModeIndicator);

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::XDisplacementOrLinearRate))
            {
                written += msg.Write(mXDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::YDisplacementOrLinearRate))
            {
                written += msg.Write(mYDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZDisplacementOrLinearRate))
            {
                written += msg.Write(mZDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::XAngleOrAngularRotationRate))
            {
                written += msg.Write(mXAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::YAngleOrAngularRotationRate))
            {
                written += msg.Write(mYAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZAngleOrAngularRotationRate))
            {
                written += msg.Write(mZAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
                expected += JAUS_SHORT_SIZE;
            }

            if( expected == written )
            {
                return written;
            }
        }
        
        SetJausError(ErrorCodes::WriteFailure); return -1;
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
int SetCameraPose::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        if( msg.Read(mPresenceVector) )
        {
            read = expected = 1;

            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mCameraID);

            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mModeIndicator);

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::XDisplacementOrLinearRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mXDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::YDisplacementOrLinearRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mYDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZDisplacementOrLinearRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mZDisplacementOrLinearRate, 100.0, -100, ScaledInteger::Short);
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::XAngleOrAngularRotationRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mXAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::YAngleOrAngularRotationRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mYAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
            }

            if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZAngleOrAngularRotationRate))
            {
                expected += JAUS_SHORT_SIZE;
                read += msg.Read(mZAngleOrAngularRotationRate, 100.0, -100, ScaledInteger::Short);
            }

            if( expected == read )
            {
                return read;
            }
        }
        
        SetJausError(ErrorCodes::ReadFailure); return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void SetCameraPose::ClearMessageBody()
{
    mPresenceVector = 0;
    mCameraID = 0;
    mModeIndicator = 0;
    mXDisplacementOrLinearRate = 0;
    mYDisplacementOrLinearRate = 0;
    mZDisplacementOrLinearRate = 0;
    mXAngleOrAngularRotationRate = 0;
    mYAngleOrAngularRotationRate = 0;
    mZAngleOrAngularRotationRate = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints Camera Pose values to console.
///
////////////////////////////////////////////////////////////////////////////////////
void SetCameraPose::PrintCameraPose() const
{
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::XDisplacementOrLinearRate))
    {
        cout << "X Displacement or Linear Rate: " << mXDisplacementOrLinearRate << endl;
    }
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::YDisplacementOrLinearRate))
    {
        cout << "Y Displacement or Linear Rate: " << mYDisplacementOrLinearRate << endl;
    }
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZDisplacementOrLinearRate))
    {
        cout << "Z Displacement or Linear Rate: " << mZDisplacementOrLinearRate << endl;
    }
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::XAngleOrAngularRotationRate))
    {
        cout << "X Angle or Angular Rotation Rate: " << mXAngleOrAngularRotationRate << endl;
    }
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::YAngleOrAngularRotationRate))
    {
        cout << "Y Angle or Angular Rotation Rate: " << mYAngleOrAngularRotationRate << endl;
    }
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ZAngleOrAngularRotationRate))
    {
        cout << "Z Angle or Angular Rotation Rate: " << mZAngleOrAngularRotationRate << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a new copy of the message
///
////////////////////////////////////////////////////////////////////////////////////
Message* SetCameraPose::Clone() const
{
    return new SetCameraPose(*this);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SetCameraPose& SetCameraPose::operator=(const SetCameraPose& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mPresenceVector = msg.mPresenceVector;
        mCameraID = msg.mCameraID;
        mModeIndicator = msg.mModeIndicator;
        mXDisplacementOrLinearRate = msg.mXDisplacementOrLinearRate;
        mYDisplacementOrLinearRate = msg.mYDisplacementOrLinearRate;
        mZDisplacementOrLinearRate = msg.mZDisplacementOrLinearRate;
        mXAngleOrAngularRotationRate = msg.mXAngleOrAngularRotationRate;
        mYAngleOrAngularRotationRate = msg.mYAngleOrAngularRotationRate;
        mZAngleOrAngularRotationRate = msg.mZAngleOrAngularRotationRate;
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
int SetCameraPose::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    SetCameraPose sendingMessage;  // Stores message data natively.
    SetCameraPose receivedMessage;// Stores message data natively.
    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetCameraID(1);
    sendingMessage.SetXDisplacementOrLinearRate(75, false);
    sendingMessage.SetYDisplacementOrLinearRate(10, false);
    //sendingMessage.SetZDisplacementOrLinearRate(0, true);
    sendingMessage.SetXAngleOrAngularRotationRate(15, true);
    sendingMessage.SetYAngleOrAngularRotationRate(45, false);
    sendingMessage.SetZAngleOrAngularRotationRate(35, true);

    // If we check the presence vector for this message, it should have
    // everything byt the Z Displacement
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::XDisplacementOrLinearRate) )
    {
        cout << "X Displacement is set to " << sendingMessage.GetXDisplacementOrLinearRate() << "\n";
    }
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::YDisplacementOrLinearRate) )
    {
        cout << "Y Displacement is set to " << sendingMessage.GetYDisplacementOrLinearRate() << "\n";
    }
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::ZDisplacementOrLinearRate) )
    {
        cout << "Z Displacement is set to " << sendingMessage.GetZDisplacementOrLinearRate() << "\n";
    }
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::XAngleOrAngularRotationRate) )
    {
        cout << "X Angular is set to " << sendingMessage.GetXAngleOrAngularRotationRate() << "\n";
    }
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::YAngleOrAngularRotationRate) )
    {
        cout << "X Angular is set to " << sendingMessage.GetYAngleOrAngularRotationRate() << "\n";
    }
    if( BitVector::IsBitSet( sendingMessage.GetPresenceVector(), SetCameraPose::VectorBit::ZAngleOrAngularRotationRate) )
    {
        cout << "X Angular is set to " << sendingMessage.GetZAngleOrAngularRotationRate() << "\n";
    }
    cout << static_cast<int>(sendingMessage.GetPresenceVector()) << endl;
    cout << static_cast<int>(sendingMessage.GetModeIndicator()) << endl;


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure. 

    SetCameraPose copiedMessage(sendingMessage);
    SetCameraPose *clonedMessage = dynamic_cast<SetCameraPose*>(copiedMessage.Clone());

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.
    if(clonedMessage->Write(packet))
    {
        cout << "Size of serialized data is (including header): " << packet.Length() << endl;
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receivedMessage.Read(packet))
        {
            // Data should match! Check the output to make sure
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::XDisplacementOrLinearRate) )
            {
                cout << "X Displacement is set to " << receivedMessage.GetXDisplacementOrLinearRate() << "\n";
            }
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::YDisplacementOrLinearRate) )
            {
                cout << "Y Displacement is set to " << receivedMessage.GetYDisplacementOrLinearRate() << "\n";
            }
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::ZDisplacementOrLinearRate) )
            {
                cout << "Z Displacement is set to " << receivedMessage.GetZDisplacementOrLinearRate() << "\n";
            }
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::XAngleOrAngularRotationRate) )
            {
                cout << "X Angular is set to " << receivedMessage.GetXAngleOrAngularRotationRate() << "\n";
            }
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::YAngleOrAngularRotationRate) )
            {
                cout << "X Angular is set to " << receivedMessage.GetYAngleOrAngularRotationRate() << "\n";
            }
            if( BitVector::IsBitSet( receivedMessage.GetPresenceVector(), SetCameraPose::VectorBit::ZAngleOrAngularRotationRate) )
            {
                cout << "X Angular is set to " << receivedMessage.GetZAngleOrAngularRotationRate() << "\n";
            }

            cout << static_cast<int>(receivedMessage.GetPresenceVector()) << endl;
            cout << static_cast<int>(receivedMessage.GetModeIndicator()) << endl;
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

    cin.get();
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
UShort SetCameraPose::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt SetCameraPose::GetPresenceVectorMask(const UShort version) const { return 0x3F; }
/*  End of File */
