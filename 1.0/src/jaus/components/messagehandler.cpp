////////////////////////////////////////////////////////////////////////////////////
///
///  \file messagehandler.cpp
///  \brief This file contains the base software for creating classes that need
///  to perform processing of JAUS messages.
///
///  Author(s): Daniel Barber
///  Created: 13 November 2007
///  Last Modified: 11 March 2008
///  Copyright (c) 2007
///  Applied Cognition and Training in Immersive Virtual Environments
///  (ACTIVE) Laboratory
///  Institute for Simulation and Training (IST)
///  University of Central Florida (UCF)
///  Email: dbarber@ist.ucf.edu
///  Web:  http://active.ist.ucf.edu
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
#include "jaus/components/messagehandler.h"
#include "jaus/components/connectionhandler.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/inform/acknackmessage.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
MessageHandler::StreamPipeline::StreamPipeline()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, deletes any messages in pipeline.
///
////////////////////////////////////////////////////////////////////////////////////
MessageHandler::StreamPipeline::~StreamPipeline()
{
    MessageHandler::StreamQueue::iterator str;
    MessageHandler::HeaderQueue::iterator hdr;

    for(str = mStreams.begin();
        str != mStreams.end();
        str++)
    {
        delete (*str);
    }
    for(hdr = mHeaders.begin();
        hdr != mHeaders.end();
        hdr++)
    {
        delete (*hdr);
    }

    mStreams.clear();
    mHeaders.clear();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   This method starts a thread that can only be stopped by the
///   destructor.
///
////////////////////////////////////////////////////////////////////////////////////
MessageHandler::MessageHandler() : mAutoSerializationFlag(true),
                                   mpStreamCallback(0),
                                   mpMessageCallback(0),
                                   mpCurrentMessage(0),
                                   mpCurrentMessageHeader(0),
                                   mLastProcessedMessageTimeMs(0)

{
    mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
    mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
///   This method is the only way to stop the message handler thread.
///
////////////////////////////////////////////////////////////////////////////////////
MessageHandler::~MessageHandler()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receive stream data when it is ready
///   for processing.
///
///   This registered callback will be called when a complete serialized
///   message is collected and ready for processing.  However, if a
///   Message Callback has been registered, it will supercede this one.
///
///   If a callback is registered for a specific message type, then that
///   callback will override other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param cb Pointer to callback object to use for receiving message stream
///             data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(StreamCallback* cb)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();

        if( mpStreamCallback )
        {
            delete mpStreamCallback;
            mpStreamCallback = NULL;
        }

        mpStreamCallback = new StreamCallback::Data(cb);
        result = 1;

        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receive stream data when it is ready
///   for processing.
///
///   This registered callback will be called when a complete serialized
///   message is collected and ready for processing.  However, if a
///   Message Callback has been registered, it will supercede this one.
///
///   If a callback is registered for a specific message type, then that
///   callback will override other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param cb Pointer to a function to use for a callback.
///   \param args Additional arguments to function callback.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(StreamFunctionCallback cb, void *args)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();

        if( mpStreamCallback )
        {
            delete mpStreamCallback;
            mpStreamCallback = NULL;
        }

        mpStreamCallback = new StreamCallback::Data(cb, args);
        result = 1;

        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receive a de-serialized message when it is
///   ready for processing.
///
///   This registered callback will be called when a complete
///   message is collected, and can automatically be de-serialized.  If the
///   Message type is unkown (cannot be created using MessageCreator), then
///   the serialized data in the Stream will be processed by any
///   registered Stream Callbacks.
///
///   If a callback is registered for a specific message type, then that
///   callback will override this callback.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param cb Pointer to callback object.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(MessageCallback* cb)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();

        if( mpMessageCallback )
        {
            delete mpMessageCallback;
            mpMessageCallback = NULL;
        }

        mpMessageCallback = new MessageCallback::Data(cb);
        result = 1;

        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receive a de-serialized message when it is
///   ready for processing.
///
///   This registered callback will be called when a complete
///   message is collected, and can automatically be de-serialized.  If the
///   Message type is unkown (cannot be created using MessageCreator), then
///   the serialized data in the Stream will be processed by any
///   registered Stream Callbacks.
///
///   If a callback is registered for a specific message type, then that
///   callback will override this callback.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param cb Pointer to function callback.
///   \param args Additional arguments for function callback.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(MessageFunctionCallback cb, void *args)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();

        if( mpMessageCallback )
        {
            delete mpMessageCallback;
            mpMessageCallback = NULL;
        }

        mpMessageCallback = new MessageCallback::Data(cb, args);
        result = 1;

        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receives a specific message when it arrives.
///
///   If the specific message specified by mcode is received, then this callback
///   is performed, overriding any other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param mcode The message code to register callback to.
///   \param cb Pointer to callback object.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(const UShort mcode, MessageCallback* cb)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();
        mMessageCallbacks[mcode] = MessageCallback::Data(cb);
        result = 1;
        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register a callback to receives a specific message when it arrives.
///
///   If the specific message specified by mcode is received, then this callback
///   is performed, overriding any other callbacks.
///
///   If you register another callback using this method, the previous
///   one will be replaced.
///
///   \param mcode The message code to register callback to.
///   \param cb Pointer to function callback.
///   \param args Additional arguments for function callback.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::RegisterCallback(const UShort mcode, MessageFunctionCallback cb, void *args)
{
    int result = 0;

    if( cb )
    {
        mProcessingMessageMutex.Enter();
        mMessageCallbacks[mcode] = MessageCallback::Data(cb, args);
        result = 1;
        mProcessingMessageMutex.Leave();
        mMessageHandlerMutex.Enter();
        if(mMessageHandlerThread.IsThreadActive() == false)
        {
            mMessageHandlerThread.CreateThread( &MessageHandler::MessageHandlerThread, this );
            mMessageHandlerThread.SetThreadName( "JausMessageHandler" );
        }
        mMessageHandlerMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Remove a callback registered for a specific type of message.
///
///   \param mcode Message code to remove callback for.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::RemoveCallback(const UShort mcode)
{
    mProcessingMessageMutex.Enter();

    if( mMessageCallbacks.find(mcode) != mMessageCallbacks.end() )
    {
        mMessageCallbacks.erase(mMessageCallbacks.find(mcode));
    }

    mProcessingMessageMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the callback.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::ClearStreamCallback()
{
    mProcessingMessageMutex.Enter();

    if( mpStreamCallback )
    {
        delete mpStreamCallback;
        mpStreamCallback = NULL;
    }

    mProcessingMessageMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the callback.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::ClearMessageCallback()
{
    mProcessingMessageMutex.Enter();

    if( mpMessageCallback )
    {
        delete mpMessageCallback;
        mpMessageCallback = NULL;
    }

    mProcessingMessageMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the message handler, killing off any active running
///          thread.  Thread will be re-created whenever a callback is registered.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::Shutdown()
{
    mMessageHandlerThread.StopThread(1000);

    ClearMessageQueue();

    mProcessingMessageMutex.Enter();
    if( mpCurrentMessage )
    {
        delete mpCurrentMessage;
    }
    if( mpCurrentMessageHeader )
    {
        delete mpCurrentMessageHeader;
    }

    mpCurrentMessage = NULL;
    mpCurrentMessageHeader = NULL;
    if(mpStreamCallback)
    {
        delete mpStreamCallback;
    }
    if(mpMessageCallback)
    {
        delete mpMessageCallback;
    }
    mpStreamCallback = NULL;
    mpMessageCallback = NULL;
    mMessageCallbacks.clear();
    mProcessingMessageMutex.Leave();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Call this method to place a serialized message into the
///   message handler queue so that it can be processed in another thread
///   in order of priority.
///
///   If you do not wish to process a message within the current program
///   scope, use this method.  The message packet is saved to internal
///   data members based on message priority.  It is then processed in
///   another thread which uses the ProcessMessage method.
///
///   Processing a message in another thread has the advantage of not have
///   to block on a communication line receiving incomming messages.  This
///   receiving interface only has to recieve data and pass it to the
///   handler without being blocked while the data is processed.
///
///   \param msg Packet structure with entire serialized JAUS message.
///   \param info De-serialized header information for the serialized message.
///
///   \return JAUS_OK if message being processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::AddToMessageQueue(const Stream& msg, const Header* info)
{
    int result = JAUS_FAILURE;
    Header *header = new Header();

    //  Reset read position by default.
    msg.SetReadPos(0);
    //  Get a copy of the header data
    //  that we can keep
    if( info )
    {
        *header = *info;
    }
    else
    {
        if( msg.Read(*header,0) == 0 )
        {
            delete header;
            return result;
        }
    }
    msg.SetReadPos(0);
    // Enter mutex for thread protection
    mMessageHandlerMutex.Enter();

    // If this is a normal packet, (low priority and not
    // part of a service connection), add to the standard
    // message pipeline.
    if( header->mPriority <= Header::Priority::Default &&
        header->mServiceConnectionFlag != JAUS_SERVICE_CONNECTION)
    {
        mPipeline.mStreams.push_back( new Stream(msg) );
        mPipeline.mHeaders.push_back( header );
        header = NULL;
        result = JAUS_OK;
    }
    // If service connection message, add to the
    // priority message queue
    if( header && header->mServiceConnectionFlag == JAUS_SERVICE_CONNECTION )
    {
        // Check and see if there is already a message in the
        // queue that has not been processed yet.
        HeaderQueue::iterator h; // header
        StreamQueue::iterator p; // packet
        for(h = mPriorityPipeline.mHeaders.begin(),
            p = mPriorityPipeline.mStreams.begin();
            h != mPriorityPipeline.mHeaders.end() &&
            p != mPriorityPipeline.mStreams.end();
            h++, p++)
        {
            if( (*h)->mCommandCode == header->mCommandCode &&
                (*h)->mSourceID == header->mSourceID )
            {
                UInt pv1, pv2;
                MessageCreator::GetPresenceVector( msg, pv1 );
                MessageCreator::GetPresenceVector( *(*p), pv2 );
                if( pv1 == pv2 )
                {
                    result = JAUS_OK;
                    if( (*h)->mSequenceNumber < header->mSequenceNumber )
                    {
                        *(*p) = msg;
                        *(*h) = *header;
                        delete header;
                        header = NULL;
                    }
                    break;
                }  // If presence vector data matches.
            } // If message types and source are equal.
        }//  For loop through existing messages.
        if( header )
        {
            // If no match was found, add it!
            mPriorityPipeline.mHeaders.push_back(header);
            mPriorityPipeline.mStreams.push_back( new Stream(msg) );
            header = NULL;
        }
    } //  If service connection message.

    // If high priority
    if( header && header->mPriority > Header::Priority::Default )
    {
        // Insert data before a message of lower priority.
        HeaderQueue::iterator h;
        StreamQueue::iterator p;
        for(h = mPriorityPipeline.mHeaders.begin(),
            p = mPriorityPipeline.mStreams.begin();
            h != mPriorityPipeline.mHeaders.end() &&
            p != mPriorityPipeline.mStreams.end();
            h++, p++)
        {
            if( (*h)->mPriority < header->mPriority )
            {
                mPriorityPipeline.mHeaders.insert( h, header );
                mPriorityPipeline.mStreams.insert(p, new Stream(msg) );
                header = NULL;
                result = JAUS_OK;
                break;
            }
        }
        //  If it could not be inserted before
        //  another message, add to the end of the priority list
        if( header != NULL )
        {
            mPriorityPipeline.mHeaders.push_back(header );
            mPriorityPipeline.mStreams.push_back(new Stream(msg) );
            header = NULL;
            result = JAUS_OK;
        }
    } // If high priority/critical

    // Leave mutex.
    mMessageHandlerMutex.Leave();

    //  If the header was never used, delete it.
    if( header )
    {
        delete header;
        header = NULL;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes all unprocessed data.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::ClearMessageQueue()
{
    StreamQueue::iterator p;
    HeaderQueue::iterator h;

    //  Delete all memory allocated.
    mMessageHandlerMutex.Enter();
    for(p = mPipeline.mStreams.begin();
        p != mPipeline.mStreams.end();
        p++)
    {
        if( *p )
        {
            delete *p;
        }
    }
    for(p = mPriorityPipeline.mStreams.begin();
        p != mPriorityPipeline.mStreams.end();
        p++)
    {
        if( *p )
        {
            delete *p;
        }
    }

    for(h = mPipeline.mHeaders.begin();
        h != mPipeline.mHeaders.end();
        h++)
    {
        if( *h )
        {
            delete *h;
        }
    }
    for(h = mPriorityPipeline.mHeaders.begin();
        h != mPriorityPipeline.mHeaders.end();
        h++)
    {
        if( *h )
        {
            delete *h;
        }
    }

    // Now clear out the structures.
    mPipeline.mStreams.clear();
    mPipeline.mHeaders.clear();
    mPriorityPipeline.mStreams.clear();
    mPriorityPipeline.mHeaders.clear();

    mMessageHandlerMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method called to process a serialized JAUS message.
///
///   If a message cannot be de-serialized, or is part of a Large Data Set and
///   automatic multi-packet stream handling is disabled, then this method
///   is called.
///
///   \param msg Packet structure with entire serialized JAUS message.
///   \param info De-serialized header information for the serialized message.
///
///   \return JAUS_OK if message was processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::ProcessMessage(const Stream& msg, const Header& info)
{
    int processed = JAUS_FAILURE;

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method called to process a de-serialized JAUS message.
///
///   Processing includes implementing the command or responding to it, etc.
///
///   \param msg JAUS Message to process.
///
///   \return JAUS_OK if message was processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageHandler::ProcessMessage(const Message* msg)
{
    int processed = JAUS_FAILURE;

    return processed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Toggles on or off the automatic de-serialization of stream data
///   to Message structures.  This feature is on by default.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::ToggleDeserialization()
{
    if(mAutoSerializationFlag)
    {
        mAutoSerializationFlag = false;
    }
    else
    {
        mAutoSerializationFlag = true;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Thread function which actually processes messages passed to the
///   message handler using the AddToMessageQueue
///   method.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageHandler::MessageHandlerThread(void *args)
{
    unsigned int loop = 0;
    unsigned int priorityCount = 0;
    bool haveMessage = false;
    Message* message = NULL;
    MessageCallbackMap::iterator mcb;
    MessageHandler* handler = (MessageHandler*)args;

    while( handler && !handler->mMessageHandlerThread.QuitThreadFlag() )
    {
        // Delete any old data if exists
        handler->mProcessingMessageMutex.Enter();
        if( handler->mpCurrentMessage )
        {
            delete handler->mpCurrentMessage;
            handler->mpCurrentMessage = NULL;
        }
        if( handler->mpCurrentMessageHeader )
        {
            delete handler->mpCurrentMessageHeader;
            handler->mpCurrentMessageHeader = NULL;
        }

        handler->mProcessingMessageMutex.Leave();

        haveMessage = false;
        //  Get the next message to process if available.
        handler->mMessageHandlerMutex.Enter();

        // The priorityCount variable is used to give
        // priority message a larger weight than normal
        // messages by a factor of 2.  This means if I'm
        // receiving a lot of priority messages, I'll process
        // 2 priority messages  before processing the next
        // default or low priority message.
        if( priorityCount == 0 )
        {
            if( handler->mPipeline.mStreams.size() > 0 )
            {
                // Copy pointer and remove from pipeline.
                handler->mProcessingMessageMutex.Enter();
                handler->mpCurrentMessage = handler->mPipeline.mStreams.front();
                handler->mpCurrentMessageHeader = handler->mPipeline.mHeaders.front();
                handler->mProcessingMessageMutex.Leave();

                handler->mPipeline.mStreams.pop_front();
                handler->mPipeline.mHeaders.pop_front();
                haveMessage = true;
            }
        }
        if( haveMessage == false )
        {
            if( handler->mPriorityPipeline.mStreams.size() > 0 )
            {
                // Copy pointer and remove from pipeline.
                handler->mProcessingMessageMutex.Enter();
                handler->mpCurrentMessage = handler->mPriorityPipeline.mStreams.front();
                handler->mpCurrentMessageHeader = handler->mPriorityPipeline.mHeaders.front();
                handler->mProcessingMessageMutex.Leave();

                handler->mPriorityPipeline.mStreams.pop_front();
                handler->mPriorityPipeline.mHeaders.pop_front();
                haveMessage = true;
                priorityCount++;
                if( priorityCount > 2 )
                {
                    // Reset priority counter
                    priorityCount = 0;
                }
            }
            else
            {
                priorityCount = 0;
            }
        }

        handler->mMessageHandlerMutex.Leave();

        // If there is a message in need of processing
        // do so.
        if( haveMessage && !handler->mMessageHandlerThread.QuitThreadFlag())
        {
            handler->mProcessingMessageMutex.Enter();
            bool usedCB = false;

            if(handler->mAutoSerializationFlag)
            {
                // Try to de-serialize the message data.  However, if this is
                // just an Ack/Nack message, treat it as such.
                if((handler->mpCurrentMessageHeader->mAckNack == Header::AckNack::Ack ||
                    handler->mpCurrentMessageHeader->mAckNack == Header::AckNack::Nack) &&
                    handler->mpCurrentMessageHeader->mDataSize == 0)
                {
                    message = new AckNackMessage(*handler->mpCurrentMessageHeader);
                }
                else
                {
                    message = MessageCreator::CreateMessage(handler->mpCurrentMessageHeader->mCommandCode);
                }

                if(message)
                {
                    if( message->Read( *handler->mpCurrentMessage ) == JAUS_FAILURE )
                    {
                        delete message;
                        message = NULL;
                    }
                }
            }
            //  Check and see if there are any registered callbacks
            //  for this type of data.
            if( !handler->mMessageHandlerThread.QuitThreadFlag() &&
                    (handler->mpStreamCallback ||
                     handler->mpMessageCallback ||
                     handler->mMessageCallbacks.size() > 0))
            {
                if(message && handler->mMessageCallbacks.size() > 0)
                {
                    mcb = handler->mMessageCallbacks.find(handler->mpCurrentMessageHeader->mCommandCode);
                    if(mcb != handler->mMessageCallbacks.end())
                    {
                        mcb->second.RunCallback(message);
                        usedCB = true;
                    }
                    else if(handler->mpMessageCallback)
                    {
                        handler->mpMessageCallback->RunCallback(message);
                        usedCB = true;
                    }
                }
                else if(message && handler->mpMessageCallback)
                {
                    handler->mpMessageCallback->RunCallback(message);
                    usedCB = true;
                }
                else if(handler->mpStreamCallback)
                {
                    handler->mpStreamCallback->RunCallback(*handler->mpCurrentMessage,
                                                           handler->mpCurrentMessageHeader,
                                                           StreamCallback::MessageHandler);
                    usedCB = true;
                }
            }
            if( !usedCB )
            {
                if( message )
                {
                    handler->ProcessMessage(message);
                }
                else
                {
                    handler->ProcessMessage( *handler->mpCurrentMessage,
                                             *handler->mpCurrentMessageHeader);
                }
            }
            if(message)
            {
                delete message;
                message = NULL;
            }
            handler->mLastProcessedMessageTimeMs = Time::GetUtcTimeMs();
            delete handler->mpCurrentMessage;
            delete handler->mpCurrentMessageHeader;
            handler->mpCurrentMessage = NULL;
            handler->mpCurrentMessageHeader = NULL;
            handler->mProcessingMessageMutex.Leave();
        }

        // Use a small sleep statement so that we don't
        // max out the CPU, but don't do it all the time.
        #ifdef WIN32
        if( ++loop == 50 )
        {
            loop = 0;
            CxUtils::SleepMs(1);

        }
        #else
        loop = 0;
        usleep(500);
        #endif
    }

    if(message)
    {
        delete message;
        message = NULL;
    }
}

/*  End of File */
