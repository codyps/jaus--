////////////////////////////////////////////////////////////////////////////////////
///
///  \file setwrencheffort.cpp
///  \brief This file contains the message class for Set Wrench Effort.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#include "jaus/messages/command/platform/setwrencheffort.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace Jaus;
using namespace std;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetWrenchEffort::SetWrenchEffort() : Message(JAUS_SET_WRENCH_EFFORT)
{
    mPresenceVector = 0;
    mPropulsiveLinearEffortX = 0;             
    mPropulsiveLinearEffortY = 0;             
    mPropulsiveLinearEffortZ = 0;             
    mPropulsiveRotationalEffortX = 0;             
    mPropulsiveRotationalEffortY = 0;             
    mPropulsiveRotationalEffortZ = 0;             
    mResistiveLinearEffortX = 0;             
    mResistiveLinearEffortY = 0;             
    mResistiveLinearEffortZ = 0;             
    mResistiveRotationalEffortX = 0;             
    mResistiveRotationalEffortY = 0;             
    mResistiveRotationalEffortZ = 0;   
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetWrenchEffort::SetWrenchEffort(const SetWrenchEffort &msg) : Message(JAUS_SET_WRENCH_EFFORT)
{
    mPresenceVector = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetWrenchEffort::~SetWrenchEffort()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveLinearEffortX(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveLinearEffortX = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveLinearEffortX);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveLinearEffortY(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveLinearEffortY = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveLinearEffortY);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveLinearEffortZ(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveLinearEffortZ = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveLinearEffortZ);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveRotationalEffortX(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveRotationalEffortX = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveRotationalEffortX);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveRotationalEffortY(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveRotationalEffortY = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveRotationalEffortY);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of effort [-100,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetPropulsiveRotationalEffortZ(const double percent)
{
    if(percent >= -100.0 && percent <= 100.0)
    {
        mPropulsiveRotationalEffortZ = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::PropulsiveRotationalEffortZ);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveLinearEffortX(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveLinearEffortX = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveLinearEffortX);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveLinearEffortY(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveLinearEffortY = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveLinearEffortY);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveLinearEffortZ(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveLinearEffortZ = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveLinearEffortZ);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveRotationalEffortX(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveRotationalEffortX = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveRotationalEffortX);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveRotationalEffortY(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveRotationalEffortY = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveRotationalEffortY);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the wrench effort value along an axis and sets the bit for
///   the field in the presence vector.
///
///   \param percent Percentage of resistive effort [0,100].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetWrenchEffort::SetResistiveRotationalEffortZ(const double percent)
{
    if(percent >= 0 && percent <= 100.0)
    {
        mResistiveRotationalEffortZ = percent;
        BitVector::SetBit(mPresenceVector, VectorBit::ResistiveRotationalEffortZ);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveLinearEffortX() const { return mPropulsiveLinearEffortX; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveLinearEffortY() const { return mPropulsiveLinearEffortY; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveLinearEffortZ() const { return mPropulsiveLinearEffortZ; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveRotationalEffortX() const { return mPropulsiveRotationalEffortX; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveRotationalEffortY() const { return mPropulsiveRotationalEffortY; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [-100,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetPropulsiveRotationalEffortZ() const { return mPropulsiveRotationalEffortZ; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveLinearEffortX() const { return mResistiveLinearEffortX; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveLinearEffortY() const { return mResistiveLinearEffortY; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveLinearEffortZ() const { return mResistiveLinearEffortZ; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveRotationalEffortX() const { return mResistiveRotationalEffortX; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveRotationalEffortY() const { return mResistiveRotationalEffortY; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return The wrench effort value along an axis.  Check the presence vector
///           to verify the data is valid before using. Range of values [0,100].
///
////////////////////////////////////////////////////////////////////////////////////
double SetWrenchEffort::GetResistiveRotationalEffortZ() const { return mResistiveRotationalEffortZ; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void SetWrenchEffort::Print() const
{
    Message::Print();
    PrintWrenchEffort();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void SetWrenchEffort::PrintWrenchEffort() const
{
    cout << "Wrench Effort:\n";
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortX))
        cout << "Propulsive Linear Effort X: " << mPropulsiveLinearEffortX << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortY))
        cout << "Propulsive Linear Effort Y: " << mPropulsiveLinearEffortY << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortZ))
        cout << "Propulsive Linear Effort Z: " << mPropulsiveLinearEffortZ << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortX))
        cout << "Propulsive Rotational Effort X: " << mPropulsiveRotationalEffortX << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortY))
        cout << "Propulsive Rotational Effort Y: " << mPropulsiveRotationalEffortY << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortZ))
        cout << "Propulsive Rotational Effort Z: " << mPropulsiveRotationalEffortZ << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortX))
        cout << "Resistive Linear Effort X: " << mResistiveLinearEffortX << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortY))
        cout << "Resistive Linear Effort Y: " << mResistiveLinearEffortY << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortZ))
        cout << "Resistive Linear Effort Z: " << mResistiveLinearEffortZ << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortX))
        cout << "Resistive Rotational Effort X: " << mResistiveRotationalEffortX << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortY))
        cout << "Resistive Rotational Effort Y: " << mResistiveRotationalEffortY << endl;
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortZ))
        cout << "Resistive Rotational Effort Z: " << mResistiveRotationalEffortZ << endl;
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
int SetWrenchEffort::WriteMessageBody(Stream& msg, 
                                      const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        expected = JAUS_USHORT_SIZE;
        written += msg.Write(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortX))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveLinearEffortX, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortY))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveLinearEffortY, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortZ))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveLinearEffortZ, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortX))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveRotationalEffortX, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortY))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveRotationalEffortY, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortZ))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mPropulsiveRotationalEffortZ, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortX))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveLinearEffortX, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortY))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveLinearEffortY, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortZ))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveLinearEffortZ, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortX))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveRotationalEffortX, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortY))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveRotationalEffortY, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortZ))
        {
            expected += JAUS_SHORT_SIZE;
            written += msg.Write(mResistiveRotationalEffortZ, 100.0, 0.0, ScaledInteger::Short);
        }
        if(expected == written)
        {
            return written;
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
int SetWrenchEffort::ReadMessageBody(const Stream& msg, 
                                     const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        expected = JAUS_USHORT_SIZE;
        read += msg.Read(mPresenceVector);
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortX))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveLinearEffortX, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortY))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveLinearEffortY, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveLinearEffortZ))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveLinearEffortZ, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortX))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveRotationalEffortX, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortY))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveRotationalEffortY, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::PropulsiveRotationalEffortZ))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mPropulsiveRotationalEffortZ, 100.0, -100.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortX))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveLinearEffortX, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortY))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveLinearEffortY, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveLinearEffortZ))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveLinearEffortZ, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortX))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveRotationalEffortX, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortY))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveRotationalEffortY, 100.0, 0.0, ScaledInteger::Short);
        }
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::ResistiveRotationalEffortZ))
        {
            expected +=  JAUS_SHORT_SIZE;
            read += msg.Read(mResistiveRotationalEffortZ, 100.0, 0.0, ScaledInteger::Short);
        }
        if(expected == read)
        {
            return read;
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
///   \brief Clears all message contents.
///
////////////////////////////////////////////////////////////////////////////////////
void SetWrenchEffort::ClearMessageBody()
{
    mPresenceVector = 0;
    mPropulsiveLinearEffortX = 0;             
    mPropulsiveLinearEffortY = 0;             
    mPropulsiveLinearEffortZ = 0;             
    mPropulsiveRotationalEffortX = 0;             
    mPropulsiveRotationalEffortY = 0;             
    mPropulsiveRotationalEffortZ = 0;             
    mResistiveLinearEffortX = 0;             
    mResistiveLinearEffortY = 0;             
    mResistiveLinearEffortZ = 0;             
    mResistiveRotationalEffortX = 0;             
    mResistiveRotationalEffortY = 0;             
    mResistiveRotationalEffortZ = 0; 
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
SetWrenchEffort &SetWrenchEffort::operator =(const SetWrenchEffort &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector = msg.mPresenceVector;
    mPropulsiveLinearEffortX = msg.mPropulsiveLinearEffortX;             
    mPropulsiveLinearEffortY = msg.mPropulsiveLinearEffortY;             
    mPropulsiveLinearEffortZ = msg.mPropulsiveLinearEffortZ;             
    mPropulsiveRotationalEffortX = msg.mPropulsiveRotationalEffortX;             
    mPropulsiveRotationalEffortY = msg.mPropulsiveRotationalEffortY;             
    mPropulsiveRotationalEffortZ = msg.mPropulsiveRotationalEffortZ;             
    mResistiveLinearEffortX = msg.mResistiveLinearEffortX;             
    mResistiveLinearEffortY = msg.mResistiveLinearEffortY;             
    mResistiveLinearEffortZ = msg.mResistiveLinearEffortZ;             
    mResistiveRotationalEffortX = msg.mResistiveRotationalEffortX;             
    mResistiveRotationalEffortY = msg.mResistiveRotationalEffortY;             
    mResistiveRotationalEffortZ = msg.mResistiveRotationalEffortZ; 
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
UShort SetWrenchEffort::GetPresenceVectorSize(const UShort version) const { return JAUS_USHORT_SIZE; }
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
UInt SetWrenchEffort::GetPresenceVectorMask(const UShort version) const { return 0xFFF; }


/*  End of File */
