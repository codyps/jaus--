////////////////////////////////////////////////////////////////////////////////////
///
///  \file judpserver.h
///  \brief This file contains software for creating JAUS compatible 
///  Univeral Datagram Protocol connections.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 16 November 2007
///  <br>Created: 16 November 2007
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
#ifndef __JAUS_UDP_SERVER__H
#define __JAUS_UDP_SERVER__H

#include <cxutils/udpserver.h>
#include <cxutils/thread.h>
#include <cxutils/mutex.h>

#include <string>
#include <map>
#include "jaus/messages/stream.h"
#include "jaus/components/streamcallback.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class JUDPServer
    ///   \brief UDP Server for receiving JAUS formatted messages.
    ///
    ///   This implementation of a UDP server automatically handles the removal
    ///   of any UDP transport information from received messages, providing the
    ///   regular serialized JAUS message for processing.
    ///
    ///   If a StreamCallback is used, this method will create a thread which
    ///   will keep trying to receive messages over UDP, and each time one is 
    ///   received it will be given the callback.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL JUDPServer
    {
    public:
        JUDPServer();
        ~JUDPServer();
        int Initialize(StreamCallback* cb, 
                       const std::string& multicastGroup = "224.1.0.1");
        int Shutdown();
        virtual int Recv(Stream& msg,
                         const long int tms = 0,
                         std::string* src = 0) const;
        int GetHostname(const Address& id, std::string& name) const;
        std::map<Address, std::string> GetHostnames() const;
    private:
        static void ServerThread(void *args);   ///<  Thread function for continuous receiving.
        CxUtils::UdpServer mServer;             ///<  Server socket connection.
        CxUtils::Mutex mCallbackMutex;          ///<  Mutex for performing message callback.
        CxUtils::Mutex mTransportMutex;         ///<  Mutex for Transport Packet.
        CxUtils::Thread mServerThread;          ///<  Server thread for continuous receiving.
        StreamCallback* mpCallback;             ///<  Pointer to message callback.
        std::map<Address, std::string> mHostnames; ///<  IP addresses of all machines sending on the JAUS port.
        std::string mTempHostname;                 ///<  Temporary hostname variable.
        Stream mTransport;                         ///<  Transport packet for UDP.
    };
}

#endif
/* End of File */
