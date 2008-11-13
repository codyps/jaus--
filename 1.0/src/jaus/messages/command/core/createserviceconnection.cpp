////////////////////////////////////////////////////////////////////////////////////
///
///  \file createserviceconnection.cpp
///  \brief This file contains data structures for the JAUS_CREATE_SERVICE_CONNECTION
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 30 July 2007
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
#include "jaus/messages/command/core/createserviceconnection.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;

const double Jaus::CreateServiceConnection::Limits::MaxPeriodicRate = 1092.0;
const double Jaus::CreateServiceConnection::Limits::MinPeriodicRate = 0.0;

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
///   Although a presence vector is written into this message, the message
///   itself is not variable in length, so the presence vector size parameter
///   for the Message constructor is 0.
///
////////////////////////////////////////////////////////////////////////////////////
CreateServiceConnection::CreateServiceConnection() : Message(JAUS_CREATE_SERVICE_CONNECTION)
{
    mScCommandCode = 0;
    mPeriodicRate = 0;
    mPresenceVector = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
///   Although a presence vector is written into this message, the message
///   itself is not variable in length, so the presence vector size parameter
///   for the Message constructor is 0.
///
////////////////////////////////////////////////////////////////////////////////////
CreateServiceConnection::CreateServiceConnection(const CreateServiceConnection &msg) : Message(JAUS_CREATE_SERVICE_CONNECTION)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CreateServiceConnection::~CreateServiceConnection()
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
int CreateServiceConnection::SetMessageCode(const UShort code)
{
    mScCommandCode = code;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Desired periodic update rate for SC.
///
///   \param rate Periodic update rate desired for SC in Hz.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int CreateServiceConnection::SetPeriodicUpdateRate(const double rate)
{
    if(rate >= Limits::MinPeriodicRate && rate <= Limits::MaxPeriodicRate)
    {
        mPeriodicRate = ScaledInteger::ToScaledUShort(rate, 
                                                      Limits::MaxPeriodicRate,
                                                      Limits::MinPeriodicRate);
        return JAUS_OK;
    }
    return SetJausError(ErrorCodes::InvalidValue);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The periodic update rate for the desired service connection.
///
////////////////////////////////////////////////////////////////////////////////////
double CreateServiceConnection::GetPeriodicUpdateRate() const
{
    return ScaledInteger::ToDouble(mPeriodicRate, 
                                   Limits::MaxPeriodicRate, 
                                   Limits::MinPeriodicRate);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief The presence vector based on the requested command code.  It must of
///   size JAUS_UINT_SIZE (4 bytes), or it will not be accepted even if the class
///   code doesn't have on this size.
///
///   \param vect Presence vector to associate with the SC.
///
///   \return 1 if the value was set, otherwise 0 and the setting of an
///   error code.
///
////////////////////////////////////////////////////////////////////////////////////
int CreateServiceConnection::SetPresenceVector(const UInt &vect)
{
    mPresenceVector = vect;
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
int CreateServiceConnection::WriteMessageBody(Stream& msg, 
                                              const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mScCommandCode) &&
            msg.Write(mPeriodicRate) &&
            msg.Write(mPresenceVector)) 
        {
            return JAUS_USHORT_SIZE*2 + JAUS_UINT_SIZE;
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
int CreateServiceConnection::ReadMessageBody(const Stream& msg, 
                                             const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mScCommandCode) &&
            msg.Read(mPeriodicRate) &&
            msg.Read(mPresenceVector))
        {
            return JAUS_USHORT_SIZE*2 + JAUS_UINT_SIZE;
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
void CreateServiceConnection::ClearMessageBody()
{
    mScCommandCode = 0;
    mPeriodicRate = 0;
    mPresenceVector = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
CreateServiceConnection &CreateServiceConnection::operator =(const CreateServiceConnection &msg)
{
    CopyHeaderData(&msg);
    mScCommandCode = msg.mScCommandCode;
    mPeriodicRate = msg.mPeriodicRate;
    mPresenceVector = msg.mPresenceVector;
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
UShort CreateServiceConnection::GetPresenceVectorSize(const UShort version) const { return 0; }
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
UInt CreateServiceConnection::GetPresenceVectorMask(const UShort version) const { return 1; }

/*  End of File */
