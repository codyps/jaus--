////////////////////////////////////////////////////////////////////////////////////
///
///  \file jtcpclient.cpp
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
#include "jaus/components/transport/jtcpclient.h"
#include "jaus/components/transport/net.h"
#include <string.h>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPClient::JTCPClient() : mpCallback(NULL)
{
    mSendStream.Write((unsigned char *)gNetworkHeader.c_str(), ((unsigned int)gNetworkHeader.size()));
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPClient::~JTCPClient()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the client connection.
///
///   \param host The address of the host being connected to.
///   \param cb The callback to use for messages received on the connection.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPClient::Initialize(const std::string& host,
                           StreamCallback* cb)
{
    Shutdown();

    // Try make a connection.
    if(mTCP.InitializeSocket(host.c_str(), gNetworkPort) > 0)
    {
        mpCallback = cb;

        if(mClientThread.CreateThread(JTCPClient::ClientThreadFunction, this))
        {
            return JAUS_OK;
        }
    }

    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the client connection.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPClient::Shutdown()
{
    // Close the TCP connection
    mTCP.Shutdown();
    // Now stop the thread.
    mClientThread.StopThread();
    mMessage.Clear();
    mStreamBuffer.Clear();
    mRecvBuffer.Clear();
    mpCallback = NULL;
    mDestinationID(0, 0, 0, 0);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds any necessary TCP transport header, and sends the data.
///
////  \param msg The message to send.
///
///   \return Number of bytes sent on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPClient::Send(const Jaus::Stream& msg)
{
    int result = 0;
    mTransportMutex.Enter();
    mSendStream.SetLength((unsigned int)(gNetworkHeader.size()));
    mSendStream.SetWritePos((unsigned int)gNetworkHeader.size());
    mSendStream.Write(msg);
    result = mTCP.Send(mSendStream);
    mTransportMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Thread function.  This method constantly attempts to receive
///          incomming data from the connection.
///
////////////////////////////////////////////////////////////////////////////////////
void JTCPClient::ClientThreadFunction(void *args)
{
    JTCPClient *client = static_cast<JTCPClient*>(args);
    
    if(client)
    {
        Header header;
        // Reserver some memory for receiving
        client->mRecvBuffer.Reserve(JAUS_MAX_PACKET_SIZE*2);
        client->mStreamBuffer.Reserve(JAUS_MAX_PACKET_SIZE*10);
        while(!client->mClientThread.QuitThreadFlag() &&
              client->mTCP.IsValid())
        {
            // Try recieve some data (this method is blocking).
            if(client->mTCP.Recv(client->mRecvBuffer, JAUS_MAX_PACKET_SIZE + gNetworkHeader.size(), 0))
            {
                // Add the data to the total buffer to the end.
                client->mStreamBuffer += client->mRecvBuffer;
                // Now try pull out any message data.
                unsigned int pos = 0;
                unsigned char *ptr;

                ptr = (unsigned char *)client->mStreamBuffer.Ptr();
                //Parse the stream.
                for(pos = 0; pos < client->mStreamBuffer.Length() - gNetworkHeader.size();)
                {
                    //  If able to read transport header from message.
                    if(strncmp( (const char *)(&ptr[pos]), 
                                 gNetworkHeader.c_str(), 
                                 gNetworkHeader.size()) == 0)
                    {
                        pos += (unsigned int)(gNetworkHeader.size());
                        // Try read the JAUS header.
                        if(Stream::ReadHeader(&ptr[pos],
                                              client->mStreamBuffer.Length() - pos,
                                              header) > 0)
                        {
                            // We have a message, YAY!
                            client->mMessage.Clear();
                            if(client->mMessage.ReadMessage(&ptr[pos], header.mDataSize + JAUS_HEADER_SIZE))
                            {
                                pos += client->mMessage.Length();

                                // Enter mutex, and run callback.
                                client->mCallbackMutex.Enter();
                                if(client->mpCallback)
                                {
                                    client->mpCallback->ProcessStreamCallback(client->mMessage,
                                                                              &header,
                                                                              StreamCallback::TCP);
                                }
                                client->mCallbackMutex.Leave();
                            }
                        }
                    }
                    else
                    {
                        pos++;
                    }

                    if(pos > 0)
                    {
                        // Delete old data
                        client->mStreamBuffer.Delete(pos, 0);
                    }
                    // If the buffer is full with bad data, clear it.
                    if( client->mStreamBuffer.Length() >= client->mStreamBuffer.Reserved() - (gNetworkHeader.size() + JAUS_HEADER_SIZE))
                    {
                        client->mStreamBuffer.Clear();
                    }
                }

            }// If data received.
        } // While connected and not told to quit thread.
    } // If able to cast data.
}

/*  End of File */
