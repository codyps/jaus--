////////////////////////////////////////////////////////////////////////////////////
///
///  \file setcomponentauthority.cpp
///  \brief This file contains data structures for the JAUS_SET_COMPONENT_AUTHORITY
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
#include "jaus/messages/command/core/setcomponentauthority.h"
#include "jaus/messages/command/commandcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
SetComponentAuthority::SetComponentAuthority() : Message(JAUS_SET_COMPONENT_AUTHORITY)
{
    mAuthorityCode = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetComponentAuthority::SetComponentAuthority(const SetComponentAuthority &msg) : Message(JAUS_SET_COMPONENT_AUTHORITY)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SetComponentAuthority::~SetComponentAuthority()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the authority code being sent in the message.
///
///   \return The authority code set [0,255].
///
////////////////////////////////////////////////////////////////////////////////////
Byte SetComponentAuthority::GetAuthorityCode() const
{
    return mAuthorityCode; 
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the authority code value.
///
///   The default value for all components is 0 for authority code.  The
///   higher the authority code set, components with lesser values will lose
///   control until authority code changed.
///
///   \param code Authority code [0,255].
///
///   \return JAUS_OK if the value was set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetComponentAuthority::SetAuthorityCode(const Byte code) 
{
    mAuthorityCode = code;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs a test case using the message to validate methods.
///
///   \return JAUS_OK on successful test, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int SetComponentAuthority::RunTestCase() const
{
    Stream packet;
    SetComponentAuthority message, messageCopy;

    // Populate a message with data
    message.SetDestinationID( Address(1, 1, 1, 1) );
    message.SetSourceID( Address( 1, 2, 1, 1) );
    message.SetAuthorityCode( 5 );

    // Try to serialize the message
    if( message.Write( packet ) )
    {
        // Now read the serialized message
        if( messageCopy.Read( packet ) )
        {
            if( message.GetDestinationID() == messageCopy.GetDestinationID() &&
                message.GetSourceID() == messageCopy.GetSourceID() &&
                message.GetAuthorityCode() == messageCopy.GetAuthorityCode() )
            {
                // Success!
                return JAUS_OK;
            }
            else
            {
                // Message data read from packet does not
                // match that of the original message structure.  There
                // must be an error in either Read, Write or both!
                return JAUS_FAILURE;
            }
        }
        else
        {
            // Failed to read message!
            return JAUS_FAILURE;
        }
    }
    else
    {
        // Failed to write message
        return JAUS_FAILURE;
    }
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
int SetComponentAuthority::WriteMessageBody(Stream& msg, const UShort version) const
{
    // Check the version of the message to write.
    // This way it is possible to add support for different
    // versions of JAUS
    if(version <= JAUS_VERSION_3_4)
    {
        // Use the Write method of Stream to serialize
        // the data.  This method returns the number of bytes
        // written.
        if(msg.Write( mAuthorityCode ))
        {
            // Return the number of bytes written to 
            // the Stream instance (msg).
            return JAUS_BYTE_SIZE;
        }
    }
    else 
    {
        //  Return JAUS_FAILURE, and set the specific
        //  error that took place when trying to write the
        //  message.  In this case, the version number set
        //  is not currently supported by this library.
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
    }

    // Return failure and signal that the function could not
    // write the data to the Stream by setting an Error flag.
    SetJausError(ErrorCodes::WriteFailure);
    return -1;
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
int SetComponentAuthority::ReadMessageBody(const Stream& msg, const UShort version)
{
    // Check the version of the message to read.
    // This way it is possible to add support for different
    // versions of JAUS
    if(version <= JAUS_VERSION_3_4)
    {
        // Use the Read method of Stream to de-serialize
        // the data.  This method returns the number of bytes
        // read.
        if(msg.Read( mAuthorityCode ))
        {
            // Return the number of bytes read from 
            // the Stream instance (msg).
            return JAUS_BYTE_SIZE;
        }
    }
    else 
    {
        //  Return -1, and set the specific
        //  error that took place when trying to write the
        //  message.  In this case, the version number set
        //  is not currently supported by this library.
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
    }

    // Return -1 and signal that the function could not
    // write the data to the Stream by setting an Error flag.
    SetJausError(ErrorCodes::ReadFailure);
    return -1;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out message data to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void SetComponentAuthority::Print() const
{
    Message::Print();
    cout << "Authority Code: " << (int)mAuthorityCode << endl;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
SetComponentAuthority &SetComponentAuthority::operator=(const SetComponentAuthority &msg)
{
    mAuthorityCode = msg.mAuthorityCode;
    CopyHeaderData(&msg);
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
UShort SetComponentAuthority::GetPresenceVectorSize(const UShort version) const { return 0; }
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
UInt SetComponentAuthority::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
