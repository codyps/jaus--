////////////////////////////////////////////////////////////////////////////////////
///
///  \file judpclient.cpp
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
#include "jaus/components/transport/judpclient.h"
#include "jaus/messages/largedataset.h"
#include "jaus/components/transport/net.h"

using namespace Jaus;
using namespace CxUtils;

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JUDPClient::JUDPClient()
{
    mTransport.Reserve( JAUS_MAX_PACKET_SIZE + gNetworkHeader.size() + 1 );
    mTransport.Write( (unsigned char *)(gNetworkHeader.c_str()), gNetworkHeader.size() );
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JUDPClient::~JUDPClient()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Intialize the client connection to an IP address.
///
///  \param host The host name or host IP address. Ex: "localhost", "127.0.0.1", 
///              or "255.255.255.255" for broadcast, etc.
///
///  \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPClient::Initialize(const std::string& host)
{
    return mClient.InitializeSocket(host.c_str(), gNetworkPort);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Intialize the client connection to an IP address.
///
///  \param host The host name or host IP address. Ex: "localhost", "127.0.0.1", 
///              or "255.255.255.255" for broadcast, etc.
///  \param ttl Time To Live for multicast transmissions.
///
///  \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPClient::InitializeMulticast(const std::string& host, const unsigned char ttl)
{
    return mClient.InitializeMulticastSocket(host.c_str(), gNetworkPort, ttl);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Closes the UDP client connection.
///
////////////////////////////////////////////////////////////////////////////////////
void JUDPClient::Shutdown()
{
    mClient.Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sends the serialized JAUS message.
///
///  This method adds the necessary JAUS UDP transport header and then
///  transmits.
///
///  \param msg The message stream to send.
///
///  \return Number of bytes transmitted (includes UDP header size).
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPClient::Send(const Stream& msg)
{
    int result;
    mTransportMutex.Enter();
    mTransport.SetLength( gNetworkHeader.size() );
    mTransport.SetWritePos( gNetworkHeader.size() );
    mTransport.Write(msg);
    result = mClient.Send(mTransport);
    mTransportMutex.Leave();

    return result;
}

/*  End of File */
