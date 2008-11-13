////////////////////////////////////////////////////////////////////////////////////
///
///  \file judpserver.cpp
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
#include "jaus/components/transport/judpserver.h"
#include "jaus/messages/largedataset.h"
#include "jaus/components/transport/net.h"

using namespace Jaus;
using namespace CxUtils;

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JUDPServer::JUDPServer()
{
    mpCallback = NULL;
    mTransport.Reserve( gNetworkHeader.size() + JAUS_HEADER_SIZE );
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JUDPServer::~JUDPServer()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes the UDP server socket.
///
///  If the callback parameter is used (not equal to NULL) then a thread is
///  created which continuously attempts to receive data over UDP and uses the
///  callback to process the messages.  Messages received will be stripped of
///  any UDP specific header.
///
///  \param cb Pointer to StreamCallback object for message processing.
///  \param multicastGroup The IP address group to connect to for multicast
///                        data.  The default value is "224.1.0.1".
///
///  \return JAUS_OK if initialized, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPServer::Initialize(StreamCallback* cb,
                           const std::string& multicastGroup)
{
    Shutdown();

    if( mServer.InitializeMulticastSocket( gNetworkPort, multicastGroup ) )
    {
        if( cb )
        {
            mCallbackMutex.Enter();
            mpCallback = cb;
            mCallbackMutex.Leave();
            mServerThread.CreateThread(&JUDPServer::ServerThread, this);
            mServerThread.SetThreadName("JUDP");
        }

        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Shuts down the socket connection and closes any active thread.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPServer::Shutdown()
{
    mCallbackMutex.Enter();
    mpCallback = NULL;
    mCallbackMutex.Leave();
    mServer.Shutdown();
    mServerThread.StopThread();

    mHostnames.clear();
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Receives a single JAUS message over UDP.
///
///  The final contents of the msg parameter will be stripped of any
///  JAUS UDP header so that it is a standard JAUS message.
///
///  \param msg The received message.
///  \param tms How long to wait (ms) before timeout. Use 0 for blocking-receive.
///  \param src Optional parameter.  If a pointer to a string is passed then
///             hostname of the sender is saved to the string.
///
///  \return Number of bytes received.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPServer::Recv(Stream& msg,
                     const long int tms,
                     std::string* src) const
{
    msg.Clear();
    if( mServer.Recv(msg, 2*(gNetworkHeader.size() + JAUS_MAX_PACKET_SIZE), tms, src) )
    {
        if( msg.Length() >= gNetworkHeader.size() &&
            strncmp( (const char *)msg.pPtr(),
                     gNetworkHeader.c_str(),
                     gNetworkHeader.size() ) == 0 )
        {
            msg.Delete(gNetworkHeader.size(), 0);
            return (int)msg.Length();
        }
        else
        {
            return 0;
        }
    }
 
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Get the hostname, IP address, for a JAUS ID if available/known.
///
///  \param id JAUS ID of component sending over UDP.
///  \param name The hostname or IP address in string format of source.
///
///  \return JAUS_OK if success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JUDPServer::GetHostname(const Address& id, std::string& name) const
{
    int result = JAUS_FAILURE;
    std::map<Address, std::string>::const_iterator hn;
    mTransportMutex.Enter();
    for( hn = mHostnames.begin();
         hn != mHostnames.end();
         hn++ )
    {
        if( hn->first == id ||
            (hn->first.mSubsystem == id.mSubsystem &&
             hn->first.mNode == id.mNode ) )
        {
            name = hn->second;
            result = JAUS_OK;
            break;
        }
    }
    mTransportMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return A copy of the known hostnames (ip addresses) of those sending
///  JUDP messages.
///
////////////////////////////////////////////////////////////////////////////////////
std::map<Address, std::string> JUDPServer::GetHostnames() const
{
    std::map<Address, std::string> copy;
    mTransportMutex.Enter();
    copy = mHostnames;
    mTransportMutex.Leave();

    return copy;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function performed in a thread which keeps receiving messages over
///  the UDP connection (if initialized) and passes received message to a
///  callback provided at initialization.  Method also updates the
///  hostnames DB.
///
////////////////////////////////////////////////////////////////////////////////////
void JUDPServer::ServerThread(void *args)
{
    bool haveCb = true;
    JUDPServer *udp = (JUDPServer*)(args);
    udp = dynamic_cast<JUDPServer*>(udp);
    Header header;

    udp->mTransport.Reserve( gNetworkHeader.size() + JAUS_MAX_PACKET_SIZE );

    // Keep looping and receiving
    while( udp &&
           !udp->mServerThread.QuitThreadFlag() &&
           haveCb)
    {
        udp->mTransport.Clear();
        //  Try receive data (but perform blocking receive).
        if( udp->mServer.IsValid() &&
            udp->Recv(udp->mTransport, 100, &udp->mTempHostname) > 0 )
        {
            // See if it is a valid JAUS message
            if( udp->mTransport.Read(header, 0) )
            {
                // Update our list of host names.
                udp->mTransportMutex.Enter();
                udp->mHostnames[header.mSourceID] = udp->mTempHostname;                
                udp->mTransportMutex.Leave();
            }
            udp->mCallbackMutex.Enter();
            if(header.mCommandCode != 0x4202)
            {
                int x;
                x = 3;
            }
            // Process the received message.
            if( udp->mpCallback != NULL )
            {
                udp->mpCallback->ProcessStreamCallback(udp->mTransport,
                                                       NULL,
                                                       StreamCallback::UDP);
            }
            else
            {
                haveCb = false;
            }
            udp->mCallbackMutex.Leave();
        }
        else
        {
            udp->mCallbackMutex.Enter();
            if( udp->mpCallback == NULL )
            {
                haveCb = false;
            }
            udp->mCallbackMutex.Leave();
        }
    }
}

/*  End of File */
