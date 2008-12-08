////////////////////////////////////////////////////////////////////////////////////
///
///  \file jtcpserver.cpp
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
#include "jaus/components/transport/jtcpserver.h"
#include <string.h>
using namespace Jaus;

const char JAUS_TCP_HEADER[]           = "JAUS01.0";      ///<  Header attached to UDP messages.
const UInt JAUS_TCP_HEADER_SIZE        = 8;               ///<  Length of TCP header in bytes.
const UInt JAUS_MAX_TCP_PACKET_SIZE    = JAUS_MAX_PACKET_SIZE + JAUS_TCP_HEADER_SIZE;  ///<  Maximum size of a JAUS message with a UDP header attached.
const unsigned short JAUS_TCP_PORT     = 3794;            ///<  Port number for UDP communication.

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPServer::ServerThread::ServerThread()
{
    mpCallback = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPServer::ServerThread::~ServerThread()
{
    mServer.Shutdown();
    Shutdown();
    mpCallback = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the thread using a TCP listen socket.  If an 
///   incomming connection is made, a new thread is created which keeps
///   receiving data from the connection.  
///
///   \param socket Listen socket to await connections on.
///   \param cb Callback to use when a JAUS message is receieved.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPServer::ServerThread::Initialize(const CxUtils::TcpListenSocket& socket, StreamCallback* cb)
{
    if(mServer.InitializeSocket(socket))
    {
        mpCallback = cb;
        if(CreateThread()) 
        {
            return JAUS_OK;
        }
    }
    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the TCP socket and thread.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPServer::ServerThread::Shutdown()
{
    mServer.Shutdown();
    StopThread();
    mpCallback = NULL;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method performed in a thread which keeps receiving incomming
///   stream data and parsing it for JAUS messages.
///
////////////////////////////////////////////////////////////////////////////////////
void JTCPServer::ServerThread::Execute()
{
    Header header;
    unsigned int failCount = 0;
    // Reserve some memory for receiving
    mRecvBuffer.Reserve(JAUS_MAX_PACKET_SIZE*2);
    mStreamBuffer.Reserve(JAUS_MAX_PACKET_SIZE*10);
    std::string sourceAddress;
    while(!QuitThreadFlag() &&
          mServer.IsValid())
    {
        // Try recieve some data (this method is blocking).
        if(mServer.Recv(mRecvBuffer, JAUS_MAX_TCP_PACKET_SIZE, 0))
        {
            failCount = 0;
            // Add the data to the total buffer to the end.
            mStreamBuffer += mRecvBuffer;
            // Now try pull out any message data.
            unsigned int pos = 0;
            unsigned char *ptr;

            ptr = (unsigned char *)mStreamBuffer.Ptr();
            //Parse the stream.
            for(pos = 0; 
                mStreamBuffer.Length() >= JAUS_TCP_HEADER_SIZE && pos < mStreamBuffer.Length() - JAUS_TCP_HEADER_SIZE;)
            {
                //  If able to read transport header from message.
                if(strncmp( (const char *)(&ptr[pos]), 
                             JAUS_TCP_HEADER, 
                             JAUS_TCP_HEADER_SIZE) == 0)
                {
                    pos += JAUS_TCP_HEADER_SIZE;
                    // Try read the JAUS header.
                    if(Stream::ReadHeader(&ptr[pos],
                                          mStreamBuffer.Length() - pos,
                                          header) > 0)
                    {
                        // We have a message, YAY!
                        mMessage.Clear();
                        if(mMessage.ReadMessage(&ptr[pos], header.mDataSize + JAUS_HEADER_SIZE))
                        {
                            pos += mMessage.Length();

                            // Enter mutex, and run callback.
                            if(mpCallback)
                            {
                                sourceAddress = mServer.GetClientAddress();
                                mpCallback->ProcessStreamCallback(mMessage,
                                                                  &header,
                                                                  StreamCallback::TCP,
                                                                  &sourceAddress);
                            }
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
                    mStreamBuffer.Delete(pos, 0);
                }
                // If the buffer is full with bad data, clear it.
                if( mStreamBuffer.Length() >= mStreamBuffer.Reserved() - (JAUS_TCP_HEADER_SIZE + JAUS_HEADER_SIZE))
                {
                    mStreamBuffer.Clear();
                }
            } // Search entire buffer for data.
        }// If data received.
        else
        {
            failCount++;
            if(failCount > 10)
                break;
        }
    } // While connected and not told to quit thread.
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPServer::JTCPServer() : mpCallback(0),
                           mpCurrentServer(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JTCPServer::~JTCPServer()
{
    Shutdown();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a TCP server which awaits incomming client connections
///   and creates thread to process the incomming data streams.
///
///   \param cb Callback to use when a JAUS message is receieved.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPServer::Initialize(StreamCallback* cb)
{
    Shutdown();

    if(cb && mListenSocket.InitializeSocket(JAUS_TCP_PORT, 255))
    {
        mpCallback = cb;
        if(mListenThread.CreateThread(JTCPServer::ListenThread, this))
        {
            return JAUS_OK;
        }
    }
    
    Shutdown();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the TCP server.
///
////////////////////////////////////////////////////////////////////////////////////
int JTCPServer::Shutdown()
{
    // Close the listen socket which will
    // stop any waiting for incomming connections.
    mListenSocket.Shutdown();
    // Stop the thread.
    mListenThread.StopThread();
    // Now go through each server and kill them.
    std::vector<ServerThread*>::iterator server;
    for(server = mServers.begin(); server != mServers.end(); server++)
    {
        (*server)->Shutdown();
        delete (*server);
        *server = NULL;
    }
    mServers.clear();
    if(mpCurrentServer)
    {
        delete mpCurrentServer;
        mpCurrentServer = NULL;
    }
    mpCallback = NULL;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the network interface to use for receiving, but does not
///  restart the server.
///
///  \param netInterface Network interface for receiving data.
///
///  \return True on success, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool JTCPServer::SetNetworkInterface(const int netInterface)
{
    mListenSocket.SetNetworkInterface(netInterface);
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function performed in a thread which keeps creating new client
///   connections and removing old ones.
///
////////////////////////////////////////////////////////////////////////////////////
void JTCPServer::ListenThread(void *args)
{
    JTCPServer* server = (JTCPServer*)args;

    while(server->mListenThread.QuitThreadFlag() == false &&
          server->mListenSocket.IsValid())
    {
        // Create a thread to receive incomming data.
        if(server->mpCurrentServer == NULL)
        {
            server->mpCurrentServer = new JTCPServer::ServerThread();          
        }
        // Wait for a new incomming connection.
        if(server->mpCurrentServer->Initialize(server->mListenSocket, server->mpCallback) == JAUS_OK)
        {
            server->mServers.push_back(server->mpCurrentServer);
            server->mpCurrentServer = NULL;
        } 

        // Go through and delete servers no longer running
        std::vector<JTCPServer::ServerThread*>::iterator sthread;
        sthread = server->mServers.begin();
        while(sthread != server->mServers.end())
        {
            if( (*sthread)->IsThreadActive() == false ||
                (*sthread)->mServer.IsValid() == false )
            {
                (*sthread)->Shutdown();
                delete (*sthread);
                (*sthread) = NULL;

                server->mServers.erase(sthread);
                sthread = server->mServers.begin();
#ifdef WIN32
#else
#endif
            }
            else
            {
                sthread++;
            }
        }
        CxUtils::SleepMs(1);
    }
}

/*  End of File */

