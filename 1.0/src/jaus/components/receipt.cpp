////////////////////////////////////////////////////////////////////////////////////
///
///  \file receipt.cpp
///  \brief Used to store information about a message that has been sent
///  and what the expect response to the message should be.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 16 March 2007
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
#include "jaus/components/receipt.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Receipt::Receipt()
{
    mSendTimeMs = mUpdateTimeMs = mTimeoutTimeMs = mSendCount = 0;
    mAckNackFlag = false;
    mSentAckNack = Header::AckNack::None;
    mReceivedAckNack = Header::AckNack::None;
    mStatus = Receipt::Pending;
    mpResponseMessage = NULL;
    mpResponseStream = NULL;
    mSentMessageCode = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.  Deletes any message data within Receipt.
///
////////////////////////////////////////////////////////////////////////////////////
Receipt::~Receipt()
{
    if(mpResponseMessage)
    {
        delete mpResponseMessage;
        mpResponseMessage = NULL;
    }
    if(mpResponseStream)
    {
        delete mpResponseStream;
        mpResponseStream = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all values and delets any allocated memory.
///
////////////////////////////////////////////////////////////////////////////////////
void Receipt::Clear(const bool del)
{
    if(del)
    {
        if(mpResponseMessage)
        {
            delete mpResponseMessage;
            mpResponseMessage = NULL;
        }
        if(mpResponseStream)
        {
            delete mpResponseStream;
            mpResponseStream = NULL;
        }
    }
    mResponseCodes.clear();
    mSendTimeMs = mUpdateTimeMs = mTimeoutTimeMs = mSendCount = 0;
    mAckNackFlag = false;
    mStatus = Receipt::Pending;
    mpResponseMessage = NULL;
    mSentMessageCode = 0;
    mSentAckNack = Header::AckNack::None;
    mReceivedAckNack = Header::AckNack::None;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the total time between sending of the message to receiving
///          a response in milliseconds.
///
///   \return Total time from send to receive in milliseconds.
///
////////////////////////////////////////////////////////////////////////////////////
UInt Receipt::GetResponseTime() const { return mUpdateTimeMs - mSendTimeMs; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Total number of times the message was sent while attempting to
///           get a response.  Value will be no larger than 3.
///
////////////////////////////////////////////////////////////////////////////////////
UInt Receipt::GetSendCount() const { return mSendCount; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if an acknowledgment of your sent message was recieved.
///
////////////////////////////////////////////////////////////////////////////////////
bool Receipt::ReceivedAcknowledge() const
{
    if(mAckNackFlag && mReceivedAckNack == Header::AckNack::Ack)
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The status of the receipt.  Success indicates that a response
///           was received to the messag sent.  Pending means that it is
///           still pending response, Nack means a negative acknowledge was
///           received, Timeout means that we timed out waiting for response.
///
////////////////////////////////////////////////////////////////////////////////////
Receipt::Status Receipt::GetStatus() const { return mStatus; }

////////////////////////////////////////////////////////////////////////////////////
///
///   \return The message code of the message sent which is waiting for a 
///           response.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Receipt::GetSentMessageCode() const
{
    return mSentMessageCode;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The message code of the response message received/expected to the
///           message sent.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Receipt::GetResponseMessageCode() const
{
    if( mpResponseMessage )
    {
        return mpResponseMessage->GetCommandCode(); 
    }
    else if( mpResponseStream && mpResponseStream->Length() > 0 )
    {
        Header header;
        if( mpResponseStream->Read(header, 0) )
        {
            return header.mCommandCode;
        }
    }
    
    if(mResponseCodes.size() > 0)
    {
        return *mResponseCodes.begin();
    }
    
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the serialized JAUS message response received, NULL if
///           data doesn't exist.
///
////////////////////////////////////////////////////////////////////////////////////
const Stream* Receipt::GetResponseStream() const { return mpResponseStream; }


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the Message recieved (de-serialized message). NULL if
///           data doesn't exist.
///
////////////////////////////////////////////////////////////////////////////////////
const Message* Receipt::GetResponseMessage() const { return mpResponseMessage; }


/*  End of File */
