////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryrelativeobjectposition.cpp
///  \brief This file contains the message structure for Query Relative Object Position.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 14 August 2008
///  <br>Last Modified: 14 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cdhollan@ist.ucf.edu
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
#include "jaus/messages/query/environment/queryrelativeobjectposition.h"
#include "jaus/messages/query/querycodes.h"
#include <iostream>

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryRelativeObjectPosition::QueryRelativeObjectPosition() : 
    Message(JAUS_QUERY_RELATIVE_OBJECT_POSITION),
    mPresenceVector(0)
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryRelativeObjectPosition::QueryRelativeObjectPosition(const QueryRelativeObjectPosition& msg) : 
    Message(JAUS_QUERY_RELATIVE_OBJECT_POSITION),
    mPresenceVector(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deconstructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryRelativeObjectPosition::~QueryRelativeObjectPosition()
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the presence vector indicating the fields being requested in
///   the Report Relative Object Position message.
///
///   \param pv See message presence vector fields for values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryRelativeObjectPosition::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
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
int QueryRelativeObjectPosition::WriteMessageBody(Jaus::Stream &msg, const Jaus::UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int written = 0;

        written += msg.Write(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        if(written == expected)
        {
            return written;
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
int QueryRelativeObjectPosition::ReadMessageBody(const Jaus::Stream &msg, const Jaus::UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = 0;
        int read = 0;

        read += msg.Read(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        if(read == expected)
        {
            return read;
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
///   \brief Clears message body values.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryRelativeObjectPosition::ClearMessageBody()
{
    mPresenceVector = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QueryRelativeObjectPosition& QueryRelativeObjectPosition::operator =(const QueryRelativeObjectPosition& msg)
{
    CopyHeaderData(&msg);

    mPresenceVector = msg.mPresenceVector;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Test case for message transmission
///
///   \return JAUS_OK for success, JAUS_FAILURE for errors
///
////////////////////////////////////////////////////////////////////////////////////
int QueryRelativeObjectPosition::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    QueryRelativeObjectPosition sendingMessage;  // Stores message data natively.
    QueryRelativeObjectPosition receivedMessage; // Stores message data natively.
    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetPresenceVector(67);

    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure. 

    QueryRelativeObjectPosition copiedMessage(sendingMessage);
    QueryRelativeObjectPosition *clonedMessage = dynamic_cast<QueryRelativeObjectPosition*>(copiedMessage.Clone());

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.

    if(clonedMessage->Write(packet))
    {
        std::cout << "Size of serialized data is (including header): " << packet.Length() << std::endl;
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receivedMessage.Read(packet))
        {
            // Data should match! Check the output to make sure through the debug windows
        }
        else
        {
            std::cout << "Error: Could not de-serialize message.\n";
            return JAUS_FAILURE;
        }
    }
    else
    {
        std::cout << "Error: Could not serialize message data.\n";
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
UShort QueryRelativeObjectPosition::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt QueryRelativeObjectPosition::GetPresenceVectorMask(const UShort version) const { return 0xFF; }


/*  End of File */
