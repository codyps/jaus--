////////////////////////////////////////////////////////////////////////////////////
///
///  \file serviceconnectionmanager.h
///  \brief The service connection manager is used to route and manage all 
///  service connection messages for the Node Manager.
///
///  Author(s): Daniel Barber
///  Created: 20 November 2007
///  Last Modified: 9 March 2008
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
#ifndef __SERVICE_CONNECTION_MANAGER__H
#define __SERVICE_CONNECTION_MANAGER__H

#include "jaus/components/serviceconnection.h"
#include "jaus/components/connectionhandler.h"
#include <cxutils/mutex.h>
#include <cxutils/thread.h>
#include <list>

namespace Jaus
{
    class NodeConnectionHandler;
    class CreateServiceConnection;
    class ConfirmServiceConnection;
    class SuspendServiceConnection;
    class TerminateServiceConnection;
    class ActivateServiceConnection;

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ServiceConnectionManager
    ///   \brief Node Manager Service Connection handler and router.
    ///
    ///   This manager creates a thread that takes care of processing messages that
    ///   are part of a service connection for service connection creation, suspension,
    ///   activation, or termination.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL ServiceConnectionManager
    {
    public:
        ServiceConnectionManager();
        ~ServiceConnectionManager();
        int Initialize(const Byte sid, 
                       const Byte nid,
                       NodeConnectionHandler* handler);
        int Shutdown();
        int RouteServiceConnectionMessage(const Stream& msg, const Header& header);
        int ProcessConnectionEvent(const Address& id, const Byte type);
        bool IsInitialized() const;        
    protected:
        typedef std::list<Stream*> StreamQueue;     ///<  Queue of stream data.
        typedef std::list<Header*> HeaderQueue;     ///<  Queue of stream header data.
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class StreamPipeline
        ///   \brief Simple structure for storing lists of message stream data.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class StreamPipeline
        {
        public:
            StreamPipeline();
            virtual ~StreamPipeline();
            void Destroy();
            StreamQueue mStreams;           ///<  Stream data in queue of pipeline.
            HeaderQueue mHeaders;           ///<  De-serialized headers for streams in pipeline.
        };
        bool QueryComponentAuthority(const Address& id, Byte& authority);
        void CreateInformServiceConnection(const Stream& msg, 
                                           const Header& header,
                                           Jaus::CreateServiceConnection& command);
        void CreateCommandServiceConnection(const Stream& msg, 
                                            const Header& header,
                                            Jaus::CreateServiceConnection& command);
        void ConfirmInformServiceConnection(const Stream& msg, 
                                            const Header& header,
                                            Jaus::ConfirmServiceConnection& command);
        void ConfirmCommandServiceConnection(const Stream& msg, 
                                             const Header& header,
                                             Jaus::ConfirmServiceConnection& command);
        void TerminateInformServiceConnection(const Stream& msg, 
                                              const Header& header,
                                              Jaus::TerminateServiceConnection& command);
        void TerminateCommandServiceConnection(const Stream& msg, 
                                               const Header& header,
                                               Jaus::TerminateServiceConnection& command);
        void SuspendInformServiceConnection(const Stream& msg, 
                                            const Header& header,
                                            Jaus::SuspendServiceConnection& command);
        void SuspendCommandServiceConnection(const Stream& msg, 
                                             const Header& header,
                                             Jaus::SuspendServiceConnection& command);
        void ActivateInformServiceConnection(const Stream& msg, 
                                             const Header& header,
                                             Jaus::ActivateServiceConnection& command);
        void ActivateCommandServiceConnection(const Stream& msg, 
                                              const Header& header,
                                              Jaus::ActivateServiceConnection& command);
        void RouteInformServiceConnectionMessage(Stream& msg, Header& header);
        void RouteCommandServiceConnectionMessage(Stream& msg, Header& header);
        void StopServiceConnections();
        static void ServiceConnectionManagerThread(void *args);
        CxUtils::Mutex mServiceConnectionsMutex;        ///<  Mutex for service connection data.
        CxUtils::Mutex mPipelineMutex;                  ///<  Mutex for message pipeline.
        CxUtils::Thread mServiceConnectionManagerThread;///<  Thread where SC messages are handled.
        Address mNodeID;                                ///<  Node ID.
        bool mInitializedFlag;                          ///<  Flag to signal initialization.
        NodeConnectionHandler* mpConnectionHandler;     ///<  Manages connections.
        StreamPipeline mStreamPipeline;                 ///<  Pipeline of messages to process.
        Stream *mpCurrentMessage;                       ///<  Pointer to the current message being processed.
        Header *mpCurrentMessageHeader;                 ///<  Pointer to the header of the message being processed.
        ServiceConnection::InformMap   mInformSC;       ///<  Inform service connections.
        ServiceConnection::CommandMap  mCommandSC;      ///<  Command service connections.
    };
}


#endif
/*  End of File */
