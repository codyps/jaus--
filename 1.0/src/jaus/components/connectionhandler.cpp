////////////////////////////////////////////////////////////////////////////////////
///
///  \file connectionhandler.cpp
///  \brief File containing base interface class for defining connection handlers
///  in JAUS.  A connection handler must have the ability to send and receive
///  JAUS formatted messages over a transport layer.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 19 November 2007
///  <br>Last Modified: 13 March 2008
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
#include "jaus/components/connectionhandler.h"
#include "jaus/components/messagehandler.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/inform/acknackmessage.h"
#include <iostream>
#include <cxutils/cxtypes.h>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ConnectionHandler::ConnectionHandler() : mpMessageHandler(NULL), mpMergedStream(NULL)
{
    mBlockAckNackFlag = false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ConnectionHandler::~ConnectionHandler()
{
    ClearPendingReceipts();
    mHandlerMutex.Enter();
    // Delete any pending data sets.
    LargeDataSetMap::iterator lds;

    for(lds = mMultiPacketStreams.begin();
        lds != mMultiPacketStreams.end();
        lds++)
    {
        if( lds->second )
        {
            delete lds->second;
        }
    }
    mMultiPacketStreams.clear();
    mpMessageHandler = NULL;
    if(mpMergedStream)
    {
        delete mpMergedStream;
        mpMergedStream = NULL;
    }
    mHandlerMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID of the component the handler is working for.
///
///   The component ID is used to determine if a message received is for
///   the component or not.  If it is, then the message will be passed to
///   any pending receipts or the message handler.
///
///   \param id The ID of the component the handler works for.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::SetComponentID(const Address& id)
{
    if(id.IsValid() && !id.IsBroadcast())
    {
        mHandlerMutex.Enter();
        mComponentID = id;
        mHandlerMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the message handler to use for message processing.
///
///   \param msgHandler Pointer to message processing handler.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::SetMessageHandler(Jaus::MessageHandler* msgHandler)
{
    if( msgHandler )
    {
        mHandlerMutex.Enter();
        mpMessageHandler = msgHandler;
        mHandlerMutex.Leave();
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::Shutdown()
{
    ClearPendingReceipts();
    mHandlerMutex.Enter();
    // Delete any pending data sets.
    LargeDataSetMap::iterator lds;

    for(lds = mMultiPacketStreams.begin();
        lds != mMultiPacketStreams.end();
        lds++)
    {
        if( lds->second )
        {
            delete lds->second;
        }
    }
    mMultiPacketStreams.clear();
    mpMessageHandler = NULL;
    if(mpMergedStream)
    {
        delete mpMergedStream;
        mpMergedStream = NULL;
    }
    mHandlerMutex.Leave();
    return 1;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Enables/Disables the blocking of messages that have the Ack/Nack
///   field of the message header not equal to None.  
///
///   By default this is turned off, but to enable blocking, call this function
///   with a value of true.
///
///   \param enable If true, blocking is enabled.
///
////////////////////////////////////////////////////////////////////////////////////
void ConnectionHandler::BlockAckNackMessages(const bool enable)
{
    mBlockAckNackFlag = enable;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the message handler.
///
////////////////////////////////////////////////////////////////////////////////////
void ConnectionHandler::ClearMessageHandler()
{
    mHandlerMutex.Enter();
    mpMessageHandler = NULL;
    mHandlerMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Serializes a message and then sends it.
///
///   \param msg The message to serialize and then send.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE and error code set.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::Send(const Message* msg)
{
    Stream serialized;
    if( msg->Write(serialized) )
    {
        return SendStream(serialized) > 0 ? JAUS_OK : JAUS_FAILURE;
    }
    return SetJausError(msg->GetLastJausError());
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a serialized message, and waits for a desired response message.
///
///   This method is useful when you need your program to wait until a response
///   to your query/command is recieved before continuing with other processing.
///   Just pass it the message you want to send and a receipt structure to 
///   store the response.
///
///   Although setting the expected response message code is a parameter to
///   this method, it is optional.  If the value is set to 0, then this
///   method will try to automatically lookup what the response should be.
///   However, if the message is not part of this library (a custom message
///   you made without updating MessageCreator) this will not work and you
///   will need to manually set the response type.
///
///   This method does not support messages to a broadcast destination since
///   it is impossible to know exactly how many responses to expect.
///
///   \param msg The serialized JAUS message to send.
///   \param receipt Receipt structure to store response information.
///   \param responseCode The message code of the response to the message sent.
///                       Set to 0 (default) to let the Send method automatically
///                       lookup the response code for you.
///   \param tms Total time to wait in milliseconds before timeout waiting for
///              response.
///   \param tries How many attempts to make.  The max allowed is 
///                JAUS_MAX_SEND_COUNT.  Example:  If the value is 2, then at
///                most the your message will be sent 2 times within the
///                total timeout time (tms).
///
///   \return JAUS_OK if response message received successfully, otherwise 
///           JAUS_FAILURE.  You may look at the Error History and the
///           status value of the receipt to identify reason for failure.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::Send(const Stream& msg,
                            Receipt& receipt, 
                            const UShort responseCode,
                            const UShort tms,
                            const UShort tries)
{
    Header header;
    bool done = false;
    double resendTime;
    std::set<Receipt*>::iterator r;
    unsigned int timingError;

#ifdef WIN32
    timingError = 30;  // Why Microsoft give us millisecond accuracy??
#else
    timingError = 0;
#endif

    if(tries > 3 || tms == 0)
    {
        return SetJausError(ErrorCodes::InvalidValue);
    }
    
    resendTime = tms*1.0/tries;

    // Try to read header information for the message being sent.
    msg.SetReadPos(0);
    if(!msg.Read(header, 0))
    {
        return SetJausError(ErrorCodes::BadPacket);
    }

    //  Verify the destination is valid and not broadcast.
    if(header.mDestinationID.IsBroadcast() || !header.mDestinationID.IsValid()) 
    {
        return SetJausError(ErrorCodes::InvalidAddress);
    }

    receipt.Clear();
    receipt.mStatus = Receipt::Pending;
    receipt.mSentAckNack = header.mAckNack;
    receipt.mSentMessageCode = header.mCommandCode;
    receipt.mDestinationID = header.mDestinationID;
    // If response code is 0, then try lookup
    // what the response message should be.
    if(responseCode == 0)
    {
        MessageCreator::GetResponseCodes(header.mCommandCode, &receipt.mResponseCodes);
        if(receipt.mResponseCodes.empty() && header.mAckNack != Header::AckNack::Request)
        {
            // Unsupported message.
            return SetJausError(ErrorCodes::InvalidMessageType);
        }
    }
    // If not 0, use the specified response expected.
    else
    {
        receipt.mResponseCodes.insert(responseCode);
    }
    
    //  Add to the receipts vector so that it can
    //  receive a message as soon as it arrives.
    mReceiptsMutex.Enter();
    receipt.mUpdateTimeMs = Time::GetUtcTimeMs();
    mReceipts.insert(&receipt);
    mReceiptsMutex.Leave();

    //  Now place the message in the outbox for
    //  the node manager to route to destination.
    if(SendStream(msg) == JAUS_FAILURE) 
    {
        mReceiptsMutex.Enter();

        //  Remove from receipt vector if failure to send.
        r = mReceipts.find(&receipt);
        if( r != mReceipts.end() )
        {
#ifdef WIN32            
            r = mReceipts.erase(r);
#else            
            mReceipts.erase(r);
            r = mReceipts.begin();
#endif            
        }

        mReceiptsMutex.Leave();

        return JAUS_FAILURE;
    }
    
    // Mark that we sent the message.
    // Since the receipt is incide of mReceipts, we MUST
    // use a mutex before modifying it.
    mReceiptsMutex.Enter();
    
    receipt.mSendCount = 1;
    receipt.mSendTimeMs = Time::GetUtcTimeMs();
    mReceiptsMutex.Leave();

    //  Now we wait for a responseCode or timeout.
    while(!done) 
    {     
        mReceiptsMutex.Enter(); //  Thread safety!

        if(receipt.mStatus == Receipt::Success)
        {
            done = true;
        }
        else
        {
            // If a pre-defined time has passed, and we have not 
            // received a response, then we may have to resend the message.
            if( !done && (Time::GetUtcTimeMs() - receipt.mUpdateTimeMs) > (UInt)(resendTime) + timingError) 
            {
                //  Re-send the message, but only JAUS_MAX_SEND_COUNT times.
                if( receipt.mSendCount < tries ) 
                {
                    Header retransmitHeader(header);
                    retransmitHeader.mDataFlag = Header::DataControl::Retransmit;
                    receipt.mSendStream.Write(retransmitHeader, 0);
                    SendStream(receipt.mSendStream);                //  Re-transmit.
                    receipt.mSendCount++;                           //  Increase send count.
                    receipt.mUpdateTimeMs = Time::GetUtcTimeMs();   //  Update send time.
                }
                // We have already sent too many times.
                else 
                {
                    receipt.mStatus = Receipt::Timeout;
                    SetJausError(ErrorCodes::TimeOut);
                    done = true;
                }
            }
        }
        mReceiptsMutex.Leave();     //  Don't deadlock!
        CxUtils::SleepMs(1);        //  Use a 1ms sleep to avoid race condition.
    }

    //  Remove the pointer of the set of receipts so it is 
    //  no longer checked by the ProcessMessageCallback function. 
    mReceiptsMutex.Enter();
    r = mReceipts.find( &receipt );
    if( r != mReceipts.end() )
    {
        mReceipts.erase( r );
    }
    mReceiptsMutex.Leave();

    if(receipt.mStatus == Receipt::Success)
    {
        // Try to read the serialized message data if we can.  First, check
        // if the only response we want is an AckNack response.  If this is the
        // the case, then create an AckNack message.
        if( receipt.mResponseCodes.size() == 1 &&
            header.mCommandCode == receipt.mSentMessageCode &&
            header.mDataSize == 0 &&
            (header.mAckNack == Header::AckNack::Ack || header.mAckNack == Header::AckNack::Nack))
        {
            receipt.mpResponseMessage = new AckNackMessage(header);
        }
        else
        {
            receipt.mpResponseMessage = MessageCreator::CreateMessage(*receipt.GetResponseStream(), NULL);
        }
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends a message, and waits for a desired response message.
///
///   This method is useful when you need your program to wait until a response
///   to your query/command is recieved before continuing with other processing.
///   Just pass it the message you want to send and a receipt structure to 
///   store the response.
///
///   Althouth setting the expected response message code is a parameter to
///   this method, it is optional.  If the value is set to 0, then this
///   method will try to automatically lookup what the response should be.
///   However, if the message is not part of this library (a custom message
///   you made without updating MessageCreator) this will not work and you
///   will need to manually set the response type.
///
///   This method does not support messages to a broadcast destination since
///   it is impossible to know exactly how many responses to expect.
///
///   \param msg The message data to send.
///   \param receipt Receipt structure to store response information.
///   \param responseCode The message code of the response to the message sent.
///                       Set to 0 (default) to let the Send method automatically
///                       lookup the response code for you.
///   \param tms Total time to wait in milliseconds before timeout waiting for
///              response.
///   \param tries How many attempts to make.  The max allowed is 
///                JAUS_MAX_SEND_COUNT.  Example:  If the value is 2, then at
///                most the your message will be sent 2 times within the
///                total timeout time (tms).
///
///   \return JAUS_OK if response message received successfully, otherwise 
///           JAUS_FAILURE.  You may look at the Error History and the
///           status value of the receipt to identify reason for failure.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::Send(const Message* msg,
                            Receipt& receipt,
                            const UShort responseCode,                            
                            const UShort tms,
                            const UShort tries)
{
    Stream serialized;
    if( msg->Write(serialized) )
    {
        return Send(serialized, 
                    receipt,
                    responseCode,
                    tms,
                    tries);
    }
    return SetJausError(msg->GetLastJausError());
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \return The ID of the JAUS component using the connection handler.
///
////////////////////////////////////////////////////////////////////////////////////
Address ConnectionHandler::GetID() const
{
    return mComponentID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets what priority values will be allowed for receiving by
///   the connection handler.  If this is not set then all values are accepted 
///   which is the default behavior.
///
///   \param priorities Accepted priority values for receiving.
///
///   \return JAUS_OK.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::SetAcceptedPriorityValues(const std::set<UShort> priorities)
{
    mPrioritiesMutex.Enter();
    mAcceptedPriorities = priorities;
    mPrioritiesMutex.Leave();
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks to see if there is a receipt waiting for the message, and
///          if so, gives it the data.
///
///   \param msg The message to serialize to check for waiting receipt.
///   \param header The de-serialized header for message.
///
///   \return JAUS_OK if receipt match was made, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::CheckForReceipt(const Stream& msg, const Header& header)
{
    int processed = JAUS_FAILURE;

    std::set<Receipt*>::iterator rcpt;
    mReceiptsMutex.Enter();

    for( rcpt = mReceipts.begin();
         rcpt != mReceipts.end();
         rcpt++ )
    {
        //  If a process is waiting on a specific response to
        //  a message sent, see if the latest received message
        //  satisifes this request.
        if( (*rcpt)->mStatus != Receipt::Success &&
            (*rcpt)->mDestinationID == header.mSourceID &&
            header.mServiceConnectionFlag != JAUS_SERVICE_CONNECTION )
        {
            if( (*rcpt)->mResponseCodes.find(header.mCommandCode) != (*rcpt)->mResponseCodes.end() )
            {                
                // Copy the response message.
                (*rcpt)->mpResponseStream = msg.Clone();
                // Set the status.
                (*rcpt)->mStatus = Receipt::Success;                   
                // Mark time of receipt.
                (*rcpt)->mUpdateTimeMs = Time::GetUtcTimeMs();
                // If the response message was received, but not AckNack, then
                // it is obvious that the message was received, otherwise we would
                // not have gotten a response, so set AckNack to true if requested.
                if((*rcpt)->mSentAckNack == Header::AckNack::Request &&
                    (*rcpt)->mAckNackFlag == false)
                {
                    (*rcpt)->mAckNackFlag = true;
                    (*rcpt)->mReceivedAckNack = Header::AckNack::Ack;
                }
                mReceipts.erase(rcpt);
                processed = JAUS_OK;
                break;
            }
            //  Check for ACK/NACK response if we are expecting one, and not received it yet.
            else if( (*rcpt)->mSentMessageCode == header.mCommandCode &&
                     (*rcpt)->mSentAckNack == Header::AckNack::Request &&
                     (*rcpt)->mAckNackFlag == false &&
                     header.mDataSize == 0 &&
                     (header.mAckNack == Header::AckNack::Ack || header.mAckNack == Header::AckNack::Nack ) )
            {
                (*rcpt)->mReceivedAckNack = header.mAckNack;
                (*rcpt)->mAckNackFlag = true;
                // Mark time of receipt.
                (*rcpt)->mUpdateTimeMs = Time::GetUtcTimeMs();
                // If no response is associated with the message, and Ack/Nack was
                // received, then we are done!
                if( (*rcpt)->mResponseCodes.size() == 0 )
                {
                    (*rcpt)->mSendCount = Receipt::Success;
                    mReceipts.erase(rcpt);
                }
                processed = JAUS_OK;
                break;
            }
        }            
    }

    mReceiptsMutex.Leave();

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method which handles messages recieved over a connection.
///
///   This method does not actually process a message for responses.  It either
///   passes it for message routing, or sends it to the message handler for
///   processing.
///
///   \param msg Serialized JAUS message received.
///   \param info Pointer to header data if available (data read from msg).
///   \param transport Transport layer message was received from.
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
void ConnectionHandler::ProcessStreamCallback(const Stream& msg,
                                              const Header* info,
                                              const StreamCallback::Transport transport)
{
    Header header;
    if( info )
    {
        header = *info;
    }
    else
    {
        if( msg.Read(header, 0 ) == JAUS_FAILURE )
        {
            SetJausError( msg.GetLastJausError() ); return;
        }
    }

    // If blocking enabled, all messages that don't have
    // the ACK/NACK flag set to None are blocked.
    if(mBlockAckNackFlag && header.mAckNack != Header::AckNack::None)
    {
        return;
    }

    mPrioritiesMutex.Enter();
    if(mAcceptedPriorities.size() > 0 &&
        mAcceptedPriorities.find(header.mPriority) == mAcceptedPriorities.end())
    {
        return;
    }
    mPrioritiesMutex.Leave();

    // If ack/nack requested, then send
    // and acknowledge message.
    if(header.mAckNack == Header::AckNack::Request)
    {
        Stream ackNackMessage;
        Header ackNackHeader;
        ackNackHeader = header;
        ackNackHeader.mDestinationID = header.mSourceID;
        ackNackHeader.mSourceID = GetID();
        ackNackHeader.mDataSize = 0;
        ackNackHeader.mDataFlag = Header::DataControl::Single;
        ackNackHeader.mAckNack = Header::AckNack::Ack;
        ackNackMessage.Write(ackNackHeader);
        SendStream(ackNackMessage);
    }

    // If the message is destined for this component, otherwise
    // do nothing with it.
    if(header.mDestinationID == mComponentID ||
        Address::DestinationMatch(header.mDestinationID, mComponentID))
    {
        // If the message is part of a multi-packet stream sequence
        // then we must collect the pieces for merging.
        if(header.mDataFlag != Header::DataControl::Single)
        {
            // Process multi-sequence data
            if(ProcessMultiPacketStream(msg, header, &mpMergedStream))
            {
                // If we finally recieved the last message in the
                // stream sequcne (got merged stream) pass it to
                // the receipt handler or message handler.
                if(mpMergedStream)
                {
                    mpMergedStream->Read(header, 0);
                    if(CheckForReceipt(*mpMergedStream, header) == JAUS_FAILURE && mpMessageHandler)
                    {
                        mpMessageHandler->AddToMessageQueue(*mpMergedStream, &header);
                    }
                }
            }
        }
        // If it is just a normal message, then pass to receipt
        // or message handler.
        else if( CheckForReceipt(msg, header) == JAUS_FAILURE && mpMessageHandler)
        {
            mpMessageHandler->AddToMessageQueue(msg, &header);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the message is for this component, and it is part of 
///   a multi-packet stream sequence, then this method will collect the
///   pieces.
///
///   The individual packets in the stream are collected by this class in 
///   large data sets.  Once an entire stream has been collected, it is merged
///   and then saved to the 3rd parameter.
///
///   \param msg Message that is part of multi-packet stream sequence.
///   \param header The header for the message.
///   \param merged Pointer to a Pointer to a Stream object for storing 
///                 a merged Stream object if full sequence has been received.
///                 Don't forget to delete you data when done!
///
///   \return JAUS_OK on processing success, JAUS_FAILURE otherwise.  If 
///           JAUS_OK is returned, check the merged parameter for a 
///           merged message stream.
///
////////////////////////////////////////////////////////////////////////////////////
int ConnectionHandler::ProcessMultiPacketStream(const Stream& msg, const Header& header, Stream** merged)
{
    bool addedFlag = false;                         // Did we add to a data set?
    UInt pv = 0;                                    // Presence vector.
    MessageCreator::GetPresenceVector(msg, pv);
    LargeDataSetMap::iterator mpstream;             // Iterator for STL map.
    LargeDataSet::Key key(header.mSourceID,
                          header.mCommandCode,
                          pv);                      // Key for indexing in STL map.
    LargeDataSet *toMerge = NULL;
    // Delete any older data.
    if(merged)
    {
        if(*merged)
        {
            delete *merged;
            *merged = NULL;
        }
    }    

    // See if we can add to and exisiting data set, or
    // if we must create a new one.

    if(header.mDataFlag == Header::DataControl::First)
    {
        mpstream = mMultiPacketStreams.find(key);
        // Check for existing data set.
        if(mpstream != mMultiPacketStreams.end())
        {
            // See if we can add to the existing stream.
            if(mpstream->second->AddToDataSet(msg))
            {
                addedFlag = true;
                if(mpstream->second->IsDataSetComplete())
                {
                   toMerge = mpstream->second;
                   mMultiPacketStreams.erase(mpstream);
                }
            }
        }
    }
    else
    {
        // If either a re-transmit, final, or normal packet in
        // multi-packet stream, find a matching stream to add to
        // and add to it.
        for(mpstream = mMultiPacketStreams.begin();
            mpstream != mMultiPacketStreams.end() && !addedFlag;)
        {
            if(mpstream->first.mCommandCode == header.mCommandCode &&
                mpstream->first.mSourceID == header.mSourceID)
            {
                // Try add the stream to the data set.
                if(mpstream->second->AddToDataSet(msg))
                {
                    addedFlag = true;
                    if(mpstream->second->IsDataSetComplete())
                    {
                        toMerge = mpstream->second;
                        mMultiPacketStreams.erase(mpstream);
                    }
                    break;
                }
            }
            // If it has been more than 5 seconds, and we haven't
            // merged the data set yet, get rid of it.
            if(mpstream->second->IsDataSetComplete() == false &&
                Time::GetUtcTimeMs() - mpstream->second->GetUpdateTimeMs() > 1000)
            {
                delete mpstream->second;
#ifdef WIN32     
                mpstream = mMultiPacketStreams.erase(mpstream);
#else                
                mMultiPacketStreams.erase(mpstream);
                mpstream = mMultiPacketStreams.begin();
#endif
            }
            else
            {
                mpstream++;
            }
        }
    }

    // If no match was found, then we need
    // to create a new data set.
    if(addedFlag == false)
    {
        LargeDataSet *lds = new LargeDataSet();

        lds->StartLargeDataSet(msg, &header);
        key.mPresenceVector = 0;

        mpstream = mMultiPacketStreams.find(key);
        while(mpstream != mMultiPacketStreams.end())
        {
            key.mIdentifier++;
            mpstream = mMultiPacketStreams.find(key);
        }

        mMultiPacketStreams[key] = lds;
        mpstream = mMultiPacketStreams.find(key);

        if(lds->IsDataSetComplete())
        {
            toMerge = mpstream->second;
            mMultiPacketStreams.erase(mpstream);
        }
        addedFlag = true;
    }

    // If we have data to merge..
    if(toMerge != NULL)
    {
        (*merged) = new Stream();
        // Merge the data
        toMerge->GetMergedStream(**merged);
        // Delete the data set.
        delete toMerge;
        toMerge = NULL;
    }

    return addedFlag ? JAUS_OK : JAUS_FAILURE;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clear all pending receipt/waiting responses.
///
////////////////////////////////////////////////////////////////////////////////////
void ConnectionHandler::ClearPendingReceipts()
{
    std::set<Receipt*>::iterator rcpt;
    mReceiptsMutex.Enter();

    for( rcpt = mReceipts.begin();
         rcpt != mReceipts.end();
         rcpt++ )
    {
        (*rcpt)->mStatus = Receipt::Timeout;
    }
    mReceipts.clear();
    mReceiptsMutex.Leave();
}


/*  End of File */
