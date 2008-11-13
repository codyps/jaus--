////////////////////////////////////////////////////////////////////////////////////
///
///  \file jtcpserver.h
///  \brief This file contains software for creating JAUS compatible 
///  TCP server for incomming TCP connections.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 9 April 2008
///  <br>Created: 9 April 2008
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
#ifndef __JAUS_TCP_SERVER__H
#define __JAUS_TCP_SERVER__H

#include <cxutils/tcplistensocket.h>
#include <cxutils/tcpserver.h>
#include <cxutils/thread.h>
#include "jaus/messages/stream.h"
#include "jaus/components/streamcallback.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class JTCPServer
    ///   \brief TCP Server for receiving JAUS formatted messages over incomming
    ///   TCP connections.
    ///
    ///   This class creates a thread which handles the creation of incomming
    ///   TCP client connections.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL JTCPServer
    {
    public:
        JTCPServer();
        ~JTCPServer();
        int Initialize(StreamCallback* cb);
        int Shutdown();
    private:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ServerThread
        ///   \brief Thread for receiving incomming TCP message streams.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL ServerThread : public CxUtils::Thread
        {
        public:
            ServerThread();
            ~ServerThread();
            int Initialize(const CxUtils::TcpListenSocket& socket, StreamCallback* cb);
            int Shutdown();
            virtual void Execute();
            CxUtils::Packet mRecvBuffer;    ///<  Message receiving buffer.
            CxUtils::Packet mStreamBuffer;  ///<  Buffer of received data.
            Stream mMessage;                ///<  Message received.
            CxUtils::TcpServer mServer;     ///<  TCP Server for receiving.
            StreamCallback* mpCallback;     ///<  Callback for received messages.
        };
        static void ListenThread(void *args);   
        CxUtils::TcpListenSocket mListenSocket;
        CxUtils::Thread mListenThread;              ///<  Runs main TCP server thread.
        StreamCallback* mpCallback;                 ///<  Pointer to callback for received messages.
        ServerThread* mpCurrentServer;              ///<  Pointer to most recent un-connected server.
        std::vector<ServerThread*> mServers;        ///<  Servers receiving data.
    };  
}


#endif
/*  End of File */
