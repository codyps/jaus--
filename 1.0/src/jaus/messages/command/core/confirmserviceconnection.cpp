////////////////////////////////////////////////////////////////////////////////////
///
///  \file confirmserviceconnection.cpp
///  \brief This file contains data structures for the JAUS_CONFIRM_SERVICE_CONNECTION
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 11 October 2007
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
#include "jaus/messages/command/core/confirmserviceconnection.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const double ConfirmServiceConnection::Limits::MaxPeriodicRate = 1092.0;
const double ConfirmServiceConnection::Limits::MinPeriodicRate = 0.0;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ConfirmServiceConnection::ConfirmServiceConnection() : Message(JAUS_CONFIRM_SERVICE_CONNECTION)
{
    mScCommandCode = 0;
    mConfirmedPeriodicRate = 0;
    mInstanceID = 0;
    mResponseCode = 0;
    this->mServiceConnectionFlag = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ConfirmServiceConnection::ConfirmServiceConnection(const ConfirmServiceConnection &msg) : Message(JAUS_CONFIRM_SERVICE_CONNECTION)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ConfirmServiceConnection::~ConfirmServiceConnection()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the command code for the desired service connection type.
///
///   \param code Command code for service connection.  SC type.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int ConfirmServiceConnection::SetMessageCode(const UShort code)
{
    mScCommandCode = code;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Desired periodic update rate for SC.
///
///   \param rate Periodic update rate desired for SC.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int ConfirmServiceConnection::SetConfirmedPeriodicUpdateRate(const double rate)
{
    if(rate >= Limits::MinPeriodicRate && rate <= Limits::MaxPeriodicRate)
    {
        mConfirmedPeriodicRate = ScaledInteger::ToScaledUShort(rate,
                                                      Limits::MaxPeriodicRate,
                                                      Limits::MinPeriodicRate);
        return JAUS_OK;
    }
    return SetJausError(ErrorCodes::InvalidValue);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return The confirmed periodic update rate.
///
////////////////////////////////////////////////////////////////////////////////////
double ConfirmServiceConnection::GetConfirmedPeriodicUpdateRate() const
{
    return ScaledInteger::ToDouble(mConfirmedPeriodicRate,
                                   Limits::MaxPeriodicRate,
                                   Limits::MinPeriodicRate);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the response code for the confirm SC message.
///
///   \param rcode Response code for the SC.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int ConfirmServiceConnection::SetResponseCode(const Byte rcode)
{
    if(rcode <= 6)
    {
        mResponseCode = rcode;
        return JAUS_OK;
    }
    return SetJausError(ErrorCodes::InvalidValue);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the instace ID for the confirm SC message.
///
///   For inform type SCs, which use the same command code and
///   presence vector, the same id is used.  Command type SCs
///   shall be always unique to a specific commander/provider
///   pair of components.
///
///   \param id Confirmed instance ID.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int ConfirmServiceConnection::SetInstanceID(const Byte id)
{
    mInstanceID = id;
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
int ConfirmServiceConnection::WriteMessageBody(Stream& msg,
                                               const UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        if( msg.Write(mScCommandCode) &&
            msg.Write(mInstanceID) &&
            msg.Write(mConfirmedPeriodicRate) &&
            msg.Write(mResponseCode))
        {
            return JAUS_USHORT_SIZE*2 + JAUS_BYTE_SIZE*2;
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
int ConfirmServiceConnection::ReadMessageBody(const Stream& msg,
                                              const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        if( msg.Read(mScCommandCode) &&
            msg.Read(mInstanceID) &&
            msg.Read(mConfirmedPeriodicRate) &&
            msg.Read(mResponseCode))
        {
            return JAUS_USHORT_SIZE*2 + JAUS_BYTE_SIZE*2;
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
///   \brief Clears all message contents.
///
////////////////////////////////////////////////////////////////////////////////////
void ConfirmServiceConnection::ClearMessageBody()
{
    mScCommandCode = 0;
    mConfirmedPeriodicRate = 0;
    mInstanceID = 0;
    mResponseCode = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ConfirmServiceConnection &ConfirmServiceConnection::operator =(const ConfirmServiceConnection &msg)
{
    CopyHeaderData(&msg);
    mScCommandCode = msg.mScCommandCode;
    mConfirmedPeriodicRate = msg.mConfirmedPeriodicRate;
    mInstanceID = msg.mInstanceID;
    mResponseCode = msg.mResponseCode;
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
UShort ConfirmServiceConnection::GetPresenceVectorSize(const UShort version) const { return 0; }
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
UInt ConfirmServiceConnection::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
