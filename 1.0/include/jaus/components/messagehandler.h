////////////////////////////////////////////////////////////////////////////////////
///
///  \file messagehandler.h
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
#ifndef __JAUS_MESSAGE_HANDLER__H
#define __JAUS_MESSAGE_HANDLER__H

#include "jaus/messages/types.h"
#include "jaus/messages/stream.h"
#include "jaus/messages/largedataset.h"
#include "jaus/components/streamcallback.h"
#include "jaus/components/messagecallback.h"
#include <cxutils/thread.h>
#include <cxutils/mutex.h>
#include <list>
#include <map>
#include <set>

namespace Jaus
{
    class ConnectionHandler; // Forward declaration.

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class MessageHandler
    ///   \brief Base class for creating interfaces in JAUS that need to 
    ///   perform message processing.
    ///
    ///   The message handler is designed to organize incomming messages for 
    ///   processing, and provide a thread safe means to do so.  This class
    ///   creates a private thread which merges message stream data, de-serializes
    ///   it, and then passes it to one of the many ProcessMessage methods. 
    ///   All message processing is done within this thread, and message 
    ///   interpretation (getting responses, etc.) is done through callbacks or
    ///   by overloading the ProcessMessage virtual functions.
    ///
    ///   The message handler also prioritizes messages based on priority or if
    ///   a service connection flag has been set.
    ///
    ///   This interface IS THREAD-SAFE as is.  Feel free to use it in a 
    ///   multi-threaded environment.  However if you inherit from it, your added
    ///   data may not be thread safe.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL MessageHandler
    {
    public:
        MessageHandler();
        virtual ~MessageHandler();
        // Use this method to make sure some messages are not buffered within internal queue.
        int AddToDoNotBufferList(const UShort messageCode, const Address& source = Address());
        // Add message to internal queue for processing outside of current thread.
        int AddToMessageQueue(const Stream& msg, const Header* info = NULL);
        // Add a callback for serialized messages.
        int RegisterCallback(StreamCallback* cb);
        // Add a callback for serialized messages.
        int RegisterCallback(StreamFunctionCallback cb, void* args);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(MessageCallback* cb);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(MessageFunctionCallback cb, void* args);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(const UShort mcode, MessageCallback* cb);
        // Add a callback to get de-serialized messages.
        int RegisterCallback(const UShort mcode, MessageFunctionCallback cb, void* args);
        // Removes a callback registered for a specific message.
        void RemoveCallback(const UShort mcode);
        // Clear Stream callback.
        void ClearStreamCallback();
        // Clear message callback.
        void ClearMessageCallback();
        // Shuts down the message queue
        void Shutdown();
        // Deletes all unprocessed data.
        void ClearMessageQueue();
        // General method for processing a serialized JAUS message.  Called when message cannot be read.
        virtual int ProcessMessage(const Stream& msg, const Header& info);
        // Method to process de-serialized messages.
        virtual int ProcessMessage(const Message* msg);
        // Get the time of last messages processed in processing thread.
        unsigned int GetLastMessageProcessTimeMs() const { return mLastProcessedMessageTimeMs; }        
        void LockConnectionHandler() const { mConnectionHandlerMutex.Enter(); }
        void UnlockConnectionHandler() const { mConnectionHandlerMutex.Leave(); }
        void ToggleDeserialization();
        bool IsDeserializationEnabled() const { return mAutoSerializationFlag; }
    private:
        typedef std::list<Stream*> StreamQueue;     ///<  Queue of stream data.
        typedef std::list<Header*> HeaderQueue;     ///<  Queue of stream header data.
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class StreamPipeline
        ///   \brief Simple structure for storing lists of message stream data.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL StreamPipeline
        {
        public:
            StreamPipeline();
            virtual ~StreamPipeline();
            StreamQueue mStreams;           ///<  Stream data in queue of pipeline.
            HeaderQueue mHeaders;           ///<  De-serialized headers for streams in pipeline.
        };
        static void MessageHandlerThread(void *args);        
        volatile bool mAutoSerializationFlag;               ///<  Automatic de-serialization flag.
        CxUtils::Thread mMessageHandlerThread;              ///<  Message handler thread.
        CxUtils::Mutex mConnectionHandlerMutex;             ///<  Connection handler mutex.
        CxUtils::Mutex mProcessingMessageMutex;             ///<  Mutex of the current message being processed.
        CxUtils::Mutex mMessageHandlerMutex;                ///<  Mutex for message handler.
        StreamPipeline mPipeline;                           ///<  Default messages priority pipeline.
        StreamPipeline mPriorityPipeline;                   ///<  High priority messages pipline.
        StreamCallback::Data* mpStreamCallback;             ///<  Callback for getting message stream data.
        MessageCallback::Data* mpMessageCallback;           ///<  Callback for getting messages.
        MessageCallbackMap mMessageCallbacks;               ///<  Map of stream callbacks for specific message types.
        Stream *mpCurrentMessage;                           ///<  Pointer to the current message being processed.
        Header *mpCurrentMessageHeader;                     ///<  Pointer to the header of the message being processed.
        volatile unsigned int mLastProcessedMessageTimeMs;  ///<  Time of last processed message in ms.
        std::map<Address, std::set<UShort> > mDoNotBuffer;  ///<  Messages and sources of messages to not buffer within queue.
    };
}

#endif
/*  End of File */
