////////////////////////////////////////////////////////////////////////////////////
///
///  \file clearemergency.cpp
///  \brief This file contains data structures for the JAUS_CLEAR_EMERGENCY
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 20 July 2007
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
#include "jaus/messages/command/core/clearemergency.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ClearEmergency::ClearEmergency() : Message(JAUS_CLEAR_EMERGENCY)
{
    mEmergencyCode = 1;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ClearEmergency::ClearEmergency(const ClearEmergency &msg) : Message(JAUS_CLEAR_EMERGENCY)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ClearEmergency::~ClearEmergency()
{
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
int ClearEmergency::WriteMessageBody(Stream &msg, 
                                     const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mEmergencyCode) ) 
        {
            return JAUS_USHORT_SIZE;
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
int ClearEmergency::ReadMessageBody(const Stream& msg, 
                                    const UShort version)
{
    UShort ecode;

    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(ecode))
        {
            mEmergencyCode = ecode;
            return JAUS_USHORT_SIZE;
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
///   \brief Gets the value of the emergency code.
///
///   \return Emergency code for message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort ClearEmergency::GetEmergencyCode() const { return mEmergencyCode; }

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the emergency code.  Possible values are 0 for or 1.
///
///   To clear an emergency state use the ClearEmergency message.
///
///   \param code Emergency code (currently version of message only accepts a
///               value of 1 which is also the default value).
///
///   \return JAUS_OK if the value is set, otherwise JAUS_FAILURE and an error code is set.
///
////////////////////////////////////////////////////////////////////////////////////
int ClearEmergency::SetEmergencyCode(const UShort code)
{
    if(code == 1)
    {
        mEmergencyCode = code;
        return JAUS_OK;
    }

    this->SetJausError(ErrorCodes::InvalidValue);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears contents of message.
///
////////////////////////////////////////////////////////////////////////////////////
void ClearEmergency::ClearMessageBody()
{
    mEmergencyCode = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ClearEmergency &ClearEmergency::operator =(const ClearEmergency &msg)
{
    CopyHeaderData(&msg);
    mEmergencyCode = msg.mEmergencyCode;
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
UShort ClearEmergency::GetPresenceVectorSize(const UShort version) const { return 0; }
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
UInt ClearEmergency::GetPresenceVectorMask(const UShort version) const { return 0; }
/*  End of File */
