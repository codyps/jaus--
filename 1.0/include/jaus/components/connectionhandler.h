////////////////////////////////////////////////////////////////////////////////////
///
///  \file connectionhandler.h
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
#ifndef __JAUS_CONNECTION_HANDLER_BASE__H
#define __JAUS_CONNECTION_HANDLER_BASE__H

#include "jaus/messages/message.h"
#include "jaus/messages/largedataset.h"
#include "jaus/components/cmplibdll.h"
#include "jaus/components/receipt.h"
#include "jaus/components/streamcallback.h"
#include <cxutils/mutex.h>

namespace Jaus
{
    const unsigned int JAUS_RESPONSE_TIMEOUT_TIME   = 750; ///<  How long to wait before timeout on receiving response (milliseconds).
    const unsigned int JAUS_MAX_SEND_COUNT          = 3;   ///<  Maximum number of times to retransmit messages.

    class MessageHandler;   // Forward declaration.

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ConnectionHandler
    ///   \brief Interface class for creating connection handlers for JAUS
    ///          components.
    ///
    ///   There are three pure virtual methods that must be implemented.  The first
    ///   is Initialize.  This method requires the ID of the JAUS component and a 
    ///   pointer to a MessageHandler to receive messages that arrive over the
    ///   transport used by the connection handler.  The second method is 
    ///   the Send method.  This method takes a Stream as an
    ///   argument and then transmits it to all destinations defined in the 
    ///   header of the message over the transport used by connection handler.
    ///   The last is the Shutdown method which handles all connection shutdown
    ///   and cleanup procedures.
    ///
    ///   The Send methods provided make use of the Send method.
    ///   They contain additional functionality such as waiting for a response
    ///   message to arrive to query/command message, etc.  Versions are also
    ///   available for sending a Message based structure which automatically
    ///   handles serialization of the message for the user.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL ConnectionHandler : public StreamCallback, 
                                           public ErrorHistory
    {
    public:
        // Constructor
        ConnectionHandler();
        // Destructor.
        virtual ~ConnectionHandler();
        // Sets the ID of the component.  Used for routing of messages.
        int SetComponentID(const Address& id);
        //  Tells the connection handler where to send message for processing
        virtual int SetMessageHandler(Jaus::MessageHandler* msgHandler);
        // Ignore messages requesting acknowledge/negative acknowledge.
        // This is false by default.
        virtual void BlockAckNackMessages(const bool enable = true);
        // Remove message handler.
        virtual void ClearMessageHandler();
        ///////////////////////////////////////////////////////////////////////
        ///
        ///  \brief Closes any open connection to transport layer.
        ///
        ///  \return JAUS_OK on success, JAUS_FAILURE otherwise.
        ///
        ///////////////////////////////////////////////////////////////////////
        virtual int Shutdown();
        ///////////////////////////////////////////////////////////////////////
        ///
        ///  \brief Transmits serialized JAUS message.
        ///
        ///  \param msg The serialized JAUS message to send.
        ///
        ///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
        ///
        ///////////////////////////////////////////////////////////////////////
        virtual int SendStream(const Stream& msg) = 0;
        // Serializes, then sends the message.
        virtual int Send(const Message* msg);
        // Sends the message and waits for response message.
        virtual int Send(const Stream& msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT);
        // Sends the message and waits for response to arrive, and returns it.
        virtual int Send(const Message* msg,
                         Receipt& receipt,
                         const UShort responseCode = 0,
                         const UShort tms = JAUS_RESPONSE_TIMEOUT_TIME,
                         const UShort tries = JAUS_MAX_SEND_COUNT); 
        // Set a list of accepted priories for messages, block all others.
        // By default, all levels of priority are accepted.
        virtual int SetAcceptedPriorityValues(const std::set<UShort> priorities);
        Address GetID() const;        
    protected:   
        int CheckForReceipt(const Stream& msg, const Header& header);
        // Does quick pre-processing of received messages.
        virtual void ProcessStreamCallback(const Stream& msg,
                                           const Header* info,
                                           const StreamCallback::Transport transport);
        // Processes large data packets.
        virtual int ProcessMultiPacketStream(const Stream& msg, const Header& header, Stream** merged);
        // Clears any pending receipts.
        virtual void ClearPendingReceipts();
        Address mComponentID;                   ///< ID of component connection handler is working for.
        volatile bool mBlockAckNackFlag;        ///< If true, ACK/NACK responses are automatically generated.
        std::set<UShort> mAcceptedPriorities;   ///<  Message priority types allowed.
        Jaus::MessageHandler* mpMessageHandler; ///< Pointer to message handler to give messages for processing.
        Stream* mpMergedStream;                 ///< Temporary stream data.
        CxUtils::Mutex mHandlerMutex;           ///< Mutex for thread protection.
        CxUtils::Mutex mPrioritiesMutex;        ///< Mutex for priorities allowed.
        CxUtils::Mutex mReceiptsMutex;          ///< Thread protection for receipt data.
        std::set<Receipt*> mReceipts;           ///< Messages expecting a response.
        LargeDataSetMap mMultiPacketStreams;    ///< Multi-packet stream sequences for this component.
    };

}

#endif
/*  End of File */
