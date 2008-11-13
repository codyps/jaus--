////////////////////////////////////////////////////////////////////////////////////
///
///  \file message.cpp
///  \brief This file contains software for deriving messages in JAUS message
///  structures.
///
///  All messages are written into Stream structures, however the Message
///  base class is used for creating data structures that store read
///  message data and contain methods for  reading/riting from/to
///  a Stream structure.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 26 February 2007
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
#include "jaus/messages/message.h"

#include <iostream>
#include <iomanip>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   \param ccode The command code associated with the message.
///
////////////////////////////////////////////////////////////////////////////////////
Message::Message(const unsigned short ccode)  : mCommandCode(ccode)

{
    mPriority = Header::Priority::Default;
    mVersion = JAUS_DEFAULT_VERSION;
    mAckNack = JAUS_NO_ACK_NACK;
    mServiceConnectionFlag = JAUS_NO_SERVICE_CONNECTION;
    mSequenceNumber = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Message::~Message() {}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the message priority.
///
///   \param p Priority value (defined in header.h).
///
///   \return JAUS_OK if the value was set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetPriority(const unsigned short p)
{
    if(p <= Header::Priority::MaxValue )
    {
        mPriority = p;
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the message JAUS version.
///
///   \param v Version of JAUS, usuall set to JAUS_DEFAULT_VERSION, but can range
///   from 0 - 63, depending on the version of the message.
///
///   \return JAUS_OK if the value is set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetVersion(const UShort v)
{
    mVersion = v;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the status of the message as a service connection message.
///
///   \param f Connection flag for message (JAUS_SERVICE_CON or
///            JAUS_SERVICE_NO_CON).
///
///   \return JAUS_OK if the value was set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetServiceConnectionFlag(const unsigned short f)
{
    if(f == JAUS_SERVICE_CONNECTION || f == JAUS_NO_SERVICE_CONNECTION)
    {
        mServiceConnectionFlag = f;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Swaps the source and destination address/ID.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SwapSourceAndDestination()
{
    Address temp = mSourceID;
    mSourceID = mDestinationID;
    mDestinationID = temp;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the source address the of the message (sender).
///
///   \param src Source address of message.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetSourceID(const Address &src)
{
    mSourceID = src;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the destination address the of the message (receiver).
///
///   \param dest Destination address of message.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetDestinationID(const Address &dest)
{
    mDestinationID = dest;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the destination address the of the message (receiver).
///
///   \param seq Sequence number of message [0, 65535].  Used primarily for
///   Service Connections (SC)'s or multi-sequence stream messages.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetSequenceNumber(const UShort seq)
{
    mSequenceNumber = seq;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the value of the Acknowlege/Negative Acknowlege bits.
///
///   \param ack Value to set for message (JAUS_NO_ACK_NACK, JAUS_REQ_ACK_NACK,
///   JAUS_ACK, or JAUS_NACK).
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::SetAckNack(const UShort ack)
{
    if( ack == JAUS_NO_ACK_NACK ||
        ack == JAUS_REQ_ACK_NACK ||
        ack == JAUS_ACK ||
        ack == JAUS_NACK)
    {
        mAckNack = ack;
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies the header data in the message to internal data members.
///
///   \param msg The message to copy header parameters from.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::CopyHeaderData(const Message *msg)
{
    if(msg)
    {
        mVersion = msg->mVersion;
        mPriority = msg->mPriority;
        mAckNack = msg->mAckNack;
        mServiceConnectionFlag = msg->mServiceConnectionFlag;
        mSourceID = msg->mSourceID;
        mDestinationID = msg->mDestinationID;
        mSequenceNumber = msg->mSequenceNumber;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies the header data in the header to internal data members.
///
///   \param header The message to copy header parameters from.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::CopyHeaderData(const Header &header)
{
    if(mCommandCode == header.mCommandCode)
    {
        mPriority = header.mPriority;
        mAckNack = header.mAckNack;
        mServiceConnectionFlag = header.mServiceConnectionFlag;
        mVersion = header.mVersion;
        mDestinationID = header.mDestinationID;
        mSourceID = header.mSourceID;
        mSequenceNumber = header.mSequenceNumber;
        SetJausError(GetLastJausError());
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Displays information about the message to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::Print() const
{
    cout << "JAUS Message Version ";
    switch(mVersion)
    {
    case JAUS_VERSION_2:
        cout << "2\n";
        break;
    case JAUS_VERSION_3:
        cout << "3 or 3.1\n";
        break;
    case JAUS_VERSION_3_4:
        cout << "3.3\n";
        break;
    default:
        cout << "3.3\n";
        break;
    }
    cout << "Command Code: 0x" << setw(4) << setfill('0') << setbase (16) << mCommandCode << setbase(10) << endl;
    cout << "Priority: " << mPriority << endl;
    cout << "Acknowlege/Negative Acknowledge: " << mAckNack << endl;
    if(mServiceConnectionFlag == JAUS_SERVICE_CONNECTION)
        cout << "Service Connection Message" << endl;
    if(Header::IsExperimentalMessage(mCommandCode))
        cout << "User Defined Message" << endl;
    cout << "Sequence Number: " << mSequenceNumber << endl;
    cout << "Source ID: " << (UInt)mSourceID.mSubsystem << ":"
                          << (UInt)mSourceID.mNode << ":"
                          << (UInt)mSourceID.mComponent << ":"
                          << (UInt)mSourceID.mInstance << endl;
    cout << "Destination ID: " << (UInt)mDestinationID.mSubsystem << ":"
                               << (UInt)mDestinationID.mNode << ":"
                               << (UInt)mDestinationID.mComponent << ":"
                               << (UInt)mDestinationID.mInstance << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message header and body data to default values.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::ClearMessage()
{
    ClearMessageHeader();
    ClearMessageBody();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This is an option method that can be implemented by developers
///          creating message.  The method should run a test case using
///          sample data to valid the Encoding and Decoding of JAUS
///          message data.
///
///   It is important to create test cases to validate that a message has
///   been implemented correctly.  The purpose of this optional method is
///   to create an instance of this class, populate it with some sample
///   data, then serialize (Write) and de-serialize (Read) the
///   message.  This method saves a test case for your message which
///   can be run for all messages and validate the entire message
///   library.
///
///   <b>Make sure at a minimum to validate your WriteMessageBody and
///   ReadMessageBody implementations.</b>
///
///   However, keep in mind that this Test Case does not mean
///   compliance with the JAUS specifications, just that the methods
///   perform correctly for your Message class.
///
///   \return JAUS_OK if the test case was a success, JAUS_FAILURE if
///           there was an error.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::RunTestCase() const
{
    cout << "Test Case Not Implemented for Message: "
         << setw(4) << setfill('0') << setbase(16) << mCommandCode;

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method serializes the contents of the message
///          into a Stream structure, including JAUS Header and
///          message body contents.
///
///   Based on the version of the message set using SetVersion, the
///   appropriate version of the message will be serialized.
///
///   \param msg The message stream to write to.
///
///   \return Number of bytes written/written, 0 on error.  See
///           GetLastJausError to get error code on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::Write(Stream& msg) const
{
    Header header;
    //  Build JAUS header data.
    CopyToHeader(header);

    //  Clear out any previous message data
    msg.Clear();

    //  The first thing we must do is advance
    //  the write position to after the JAUS_HEADER
    //  data.  The header is not written first because
    //  we do not know how large the message body will
    //  be yet.
    msg.Reserve(JAUS_HEADER_SIZE + 1);
    msg.SetLength(JAUS_HEADER_SIZE);
    msg.SetWritePos(JAUS_HEADER_SIZE);

    if(WriteMessageBody(msg, header.mVersion) >= 0)
    {
        header.mDataSize = msg.Length() - JAUS_HEADER_SIZE;
        //  Write the JAUS_HEADER now
        if(msg.Write(header, 0))
        {
            //  Return the number of bytes written.
            return msg.Length();
        }
    }

    //  If we got here, then there was an error,
    //  so clear message data and exit.
    msg.Clear();
    //  We don't need to set any new error codes
    //  because this should be done by the
    //  WriteMessageBody method.
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method takes serialized JAUS message data in a Stream and
///          extracts it, saving to internal data members.  Message extracts
///          header and message body information.
///
///   Based on the version desired, the message will read the data
///   appropriately if supported.
///
///   \param msg The message stream to read from.
///
///   \return Number of bytes read/read, 0 on failure, check
///           GetLastJausError() for reason.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::Read(const Stream& msg)
{
    Header header;
    int read = 0;
    //  First set read position in stream to
    //  the beginning.
    msg.SetReadPos(0);

    //  Now read the header
    if(msg.Read(header))
    {
        if( header.mDataFlag == Header::DataControl::Single )
        {
            //  Now check the header command code
            //  to see if it matches  this message
            if( header.mCommandCode == mCommandCode )
            {
                read = ReadMessageBody(msg, header.mVersion);
                if( read >= 0 && read == (int)header.mDataSize )
                {
                    // Save header data
                    CopyFromHeader(header);
                    //  Success, read with no errors!
                    return JAUS_HEADER_SIZE + header.mDataSize;
                }
                else
                {
                    // Failure, just exit.
                    return JAUS_FAILURE;
                }
            }
            else
            {
                //  Class/Message code doesn't match.
                return SetJausError(ErrorCodes::InvalidMessageType);
            }
        }
        else
        {
            //  Malformed Stream data.
            return SetJausError(ErrorCodes::BadPacket);
        }
    }
    else
    {
        //  Failed to read header data.
        return SetJausError(ErrorCodes::ReadFailure);
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the header from the written JAUS message and checks
///   the header to make sure it has a matching command code as the Message
///   and that the Data Flag is equal to Header::DataControl::Single.
///
///   \param msg The written JAUS message to extract and validate header
///              for.  On success, the read position in the stream will
///              be after the JAUS header.
///   \param header The read header.
///
///   \return JAUS_FAILURE if could not validate, otherwise JAUS_HEADER_SIZE.
///
////////////////////////////////////////////////////////////////////////////////////
int Message::ReadAndValidateHeader(const Stream& msg, Header& header) const
{
    msg.SetReadPos(0);

    if( !msg.Read(header))
    {
        msg.SetReadPos( 0 );
        return Message::SetJausError(ErrorCodes::ReadFailure);;
    }
    if( header.mCommandCode != mCommandCode )
    {
        msg.SetReadPos(0);
        return Message::SetJausError(ErrorCodes::InvalidMessageType);;
    }

    if( header.mDataFlag != Header::DataControl::Single )
    {
        msg.SetReadPos(0);
        return Message::SetJausError(ErrorCodes::BadPacket);
    }

    return JAUS_HEADER_SIZE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies header data from the message to the header structure.  This
///   includes ack/nack, service connection, version, command clode, source and
///   destination ID, and priority.  It does not include message size.
///
///   \param h Header to copy header data to.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::CopyToHeader(Header &h) const
{
    h.mCommandCode = this->mCommandCode;
    h.mVersion = this->mVersion;
    h.mPriority = this->mPriority;
    h.mSourceID = this->mSourceID;
    h.mDestinationID = this->mDestinationID;
    h.mAckNack = this->mAckNack;
    h.mServiceConnectionFlag = this->mServiceConnectionFlag;
    h.mSequenceNumber = mSequenceNumber;

    /*  Set Default Values for items. */
    if(mCommandCode >= 0x3000 && mCommandCode <= 0x3FFF)
        h.mExperimentalFlag = Header::MessageType::Experimental;
    else
        h.mExperimentalFlag = Header::MessageType::Normal;
    h.mDataSize = 0;
    h.mDataFlag = Header::DataControl::Single;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies specific header data from the header structure to internal
///   data members.
///
///   The following data is copied:
///        Version
///        Priority
///        Source ID
///        Destination ID
///        Ack/Nack
///        Service Connection Flag
///
///   \param h Header to copy header from.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::CopyFromHeader(const Header &h)
{
    this->mVersion = h.mVersion;
    this->mPriority = h.mPriority;
    this->mSourceID = h.mSourceID;
    this->mDestinationID = h.mDestinationID;
    this->mAckNack = h.mAckNack;
    this->mServiceConnectionFlag = h.mServiceConnectionFlag;
    this->mSequenceNumber = h.mSequenceNumber;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Resets values to defaults.
///
///   The following values are cleared:
///     Source ID.
///     Destination ID.
///     Ack/Nack.
///     Service Connection Flag.
///     Priority.
///     Version = JAUS_DEFAULT_VERSION.
///
////////////////////////////////////////////////////////////////////////////////////
void Message::ClearMessageHeader()
{
    mSourceID(0, 0, 0, 0);
    mDestinationID(0, 0, 0, 0);
    mAckNack = JAUS_NO_ACK_NACK;
    mVersion = JAUS_DEFAULT_VERSION;
    mPriority = Header::Priority::Default;
    mSequenceNumber = 0;
    mServiceConnectionFlag = JAUS_NO_SERVICE_CONNECTION;
}

/*
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If a presence vector is used in the message, this function will return
///   the size of it in bytes.  If one is not used, then 0 is returned.
///
///   \return Size of presence vector for the message if used, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Message::GetPresenceVectorSize() const
{
    return mPresenceVectorSize;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If a presence vector is used, then this will return a mask indicating
///          what bits are used.  For example, for Report Global Pose you'd get
///          a mask of 0x1FF (JAUS RA 3.3).
///
///   This method will check the version of the message set to determine what
///   mask to return in case it changes based on JAUS version.
///
///   \return Bit vector mask for message presence vector.  0 if no presence
///           vector used.
///
////////////////////////////////////////////////////////////////////////////////////
UInt Message::GetPresenceVectorMask() const
{
    return 0;
}
*/

/* End of File */
