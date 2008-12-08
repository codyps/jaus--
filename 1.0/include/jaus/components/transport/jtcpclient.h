////////////////////////////////////////////////////////////////////////////////////
///
///  \file jtcpclient.h
///  \brief This file contains software for creating JAUS compatible 
///  TCP client connections.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 April 2008
///  <br>Created: 5 April 2008
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
#ifndef __JTCP_CLIENT__H
#define __JTCP_CLIENT__H

#include <cxutils/tcpclient.h>
#include <cxutils/thread.h>
#include "jaus/components/streamcallback.h"
#include "jaus/messages/stream.h"

namespace Jaus
{    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class JTCPClient
    ///   \brief TCP Client socket for sending serialized JAUS messages.
    ///
    ///   This implementation of a TCP client uses the underlying software written
    ///   in CxUtils, but handles the addtion/removal of transport header information
    ///   added to TCP streams in JAUS.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL JTCPClient
    {
    public:
        JTCPClient();
        ~JTCPClient();
        int Initialize(const std::string& host,
                       StreamCallback* cb);
        int Shutdown();
        int Send(const Stream& msg);
        int SetNetworkInterface(const int num) { return mTCP.SetNetworkInterface(num); }
    protected:
        static void ClientThreadFunction(void *args);
        Address mDestinationID;         ///<  ID of destination connection.
        StreamCallback* mpCallback;     ///<  Callback for received messages.
        CxUtils::Thread mClientThread;  ///<  Thread for receiving incomming data.
        CxUtils::TcpClient mTCP;        ///<  TCP connection.
        CxUtils::Mutex mCallbackMutex;  ///<  Mutex for the callback.
        CxUtils::Mutex mTransportMutex; ///<  Mutex for transport packet.
        CxUtils::Packet mRecvBuffer;    ///<  Message receiving buffer.
        CxUtils::Packet mStreamBuffer;  ///<  Buffer of received data.
        Stream mMessage;                ///<  Message received.
        Stream mSendStream;             ///<  Send message stream.
    };
};

#endif
/*  End of File */
