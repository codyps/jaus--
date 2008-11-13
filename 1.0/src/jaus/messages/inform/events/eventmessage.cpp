////////////////////////////////////////////////////////////////////////////////////
///
///  \file eventmessage.cpp
///  \brief This file contains the Event message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 March 2008
///  <br>Last Modified: 25 March 2008
///  <br>Copyright (c) 2008
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
#include "jaus/messages/inform/events/eventmessage.h"
#include "jaus/messages/inform/informcodes.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/inform/core/reporttime.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
EventMessage::EventMessage() : Message(JAUS_EVENT)
{
    mEventID = 0;
    mEventMessageCode = 0;
    mEventSequenceNumber = 0;
    mpMessage = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
EventMessage::EventMessage(const EventMessage& msg) : Message(JAUS_EVENT)
{
    mEventID = msg.mEventID;
    mEventMessageCode = msg.mEventMessageCode;
    mEventSequenceNumber = msg.mEventSequenceNumber;
    mMessageBody = msg.mMessageBody;
    mpMessage = 0;
    if(msg.mpMessage)
    {
        mpMessage = msg.mpMessage->Clone();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
EventMessage::~EventMessage()
{
    if(mpMessage)
    {
        delete mpMessage;
        mpMessage = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the uniquie identifier associated with the event.
///
///   \param id The event ID.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::SetEventID(const Byte id)
{
    mEventID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the type of message included with the event. This value is
///   overwritten/also set by the SetEventMessage class.
///
///   \param mcode EventMessage message type.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::SetEventMessageCode(const UShort mcode)
{
    mEventMessageCode = mcode;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the event sequence number.
///
///   \param sn EventMessage sequence number.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::SetEventSequenceNumber(const Byte sn)
{
    mEventSequenceNumber = sn;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the event message data to use within the event message.
///
///   If you use this method, the WriteMessageBody function of the
///   message is used to generate the message body data.
///
///   If clone is set to false, then ownership of the msg pointer is transfered
///   to this class.  When the data with this message is cleared or deleted, the
///   pointer will be deleted.
///
///   \param msg Pointer to the message data.
///   \param clone If true a copy of the message is made, otherwise the
///                the EventMessage message class takes ownership of the pointer
///                and will handle deletion.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::SetEventMessage(const Message* msg, const bool clone)
{
    if(msg)
    {
        mEventMessageCode = msg->GetCommandCode();
        mMessageBody.Clear();
        if(msg->WriteMessageBody(mMessageBody, GetVersion()) >= 0)
        {
            if(mpMessage)
            {
                delete mpMessage;
                mpMessage = NULL;
            }
            if(clone)
            {
                mpMessage = msg->Clone();
            }
            else
            {
                mpMessage = (Message*)msg;
            }
            return JAUS_OK;
        }
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the message body data associated with the event.
///
///   \param msg Message body data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::SetEventMessageBody(const Stream& msg)
{
    if(mpMessage)
    {
        delete mpMessage;
        mpMessage = NULL;
    }
    mMessageBody = msg;
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
int EventMessage::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected = JAUS_BYTE_SIZE*2 + JAUS_UINT_SIZE + JAUS_USHORT_SIZE + mMessageBody.Length();
        written += msg.Write(mEventID);
        written += msg.Write(mEventMessageCode);
        written += msg.Write(mEventSequenceNumber);
        written += msg.Write(((UInt)mMessageBody.Length()));
        written += msg.Write(mMessageBody);

        if( expected == written ) 
        {
            return written;
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
int EventMessage::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        UInt bodySize = 0;
        expected = JAUS_BYTE_SIZE*2 + JAUS_UINT_SIZE + JAUS_USHORT_SIZE + mMessageBody.Length();
        
        if(mpMessage)
        {
            delete mpMessage;
            mpMessage = NULL;
        }

        read += msg.Read(mEventID);
        read += msg.Read(mEventMessageCode);
        read += msg.Read(mEventSequenceNumber);
        read += msg.Read(bodySize);

        if(bodySize > 0)
        {   
            read += msg.Read(mMessageBody, bodySize);
            expected += bodySize;
            if( (mpMessage = MessageCreator::CreateMessage(mEventMessageCode)) != NULL)            
            {
                Header header;
                if( msg.Read(header, 0) )
                {
                    mpMessage->SetSourceID(header.mSourceID);
                    mpMessage->SetDestinationID(header.mDestinationID);
                }
                if(mpMessage->ReadMessageBody(mMessageBody, version) < 0)
                {
                    delete mpMessage;
                    mpMessage = NULL;
                }
            }
        }

        if( expected == read )
        {
            return expected;
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
///   \brief Runs a test case to validate the class methods.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE. 
///
////////////////////////////////////////////////////////////////////////////////////
int EventMessage::RunTestCase() const
{
    EventMessage eventMessage;
    ReportTime reportTime;
    Stream stream;
    Time time;
    Date date;
    
    time.SetCurrentTime();
    date.SetCurrentDate();
    reportTime.SetTimeStamp(time);
    reportTime.SetDateStamp(date);

    eventMessage.SetEventMessageCode(reportTime.GetCommandCode());
    eventMessage.SetEventID(22);
    eventMessage.SetEventSequenceNumber(3);
    eventMessage.SetEventMessage(&reportTime);

    if( eventMessage.WriteMessageBody(stream, JAUS_DEFAULT_VERSION) >= 0 )
    {
        eventMessage.ClearMessageBody();
        if(eventMessage.ReadMessageBody(stream, JAUS_DEFAULT_VERSION) >= 0)
        {
            if(eventMessage.GetEventMessageCode() == reportTime.GetCommandCode() &&
                eventMessage.GetSequenceNumber() == 3 &&
                eventMessage.GetEventID() == 22 &&
                eventMessage.GetEventMessage()->GetCommandCode() == reportTime.GetCommandCode() &&
                eventMessage.GetEventMessageBody()->Length() > 0)
            {
                return JAUS_OK;
            }
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clones the message. 
///
////////////////////////////////////////////////////////////////////////////////////
Message* EventMessage::Clone() const
{
    return new Jaus::EventMessage(*this);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window. 
///
////////////////////////////////////////////////////////////////////////////////////
void EventMessage::Print() const
{
    Message::Print();
    cout << "EventMessage Message Type: " << setbase(16) << setfill('0') << setw(4) << mEventMessageCode << endl;
    cout << "EventMessage Sequence Number: " << (int)mEventSequenceNumber << endl;
    cout << "EventMessage Message Size: " << mMessageBody.Length() << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message body data. 
///
////////////////////////////////////////////////////////////////////////////////////
void EventMessage::ClearMessageBody()
{
    mEventID = 0;
    mEventMessageCode = 0;
    mEventSequenceNumber = 0;
    mMessageBody.Clear();
    if(mpMessage)
    {
        delete mpMessage;
        mpMessage = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the message data.
///
////////////////////////////////////////////////////////////////////////////////////
EventMessage& EventMessage::operator =(const EventMessage& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mEventID = msg.mEventID;
        mEventMessageCode = msg.mEventMessageCode;
        mEventSequenceNumber = msg.mEventSequenceNumber;
        mMessageBody = msg.mMessageBody;
        if(mpMessage)
        {
            delete mpMessage;
            mpMessage  = NULL;
        }
        if(msg.mpMessage)
        {
            mpMessage = msg.mpMessage->Clone();
        }
    }
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
UShort EventMessage::GetPresenceVectorSize(const UShort version) const { return 0; }

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
UInt EventMessage::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
