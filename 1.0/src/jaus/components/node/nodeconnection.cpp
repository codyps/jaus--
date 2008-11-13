////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodeconnection.cpp
///  \brief File contains the NodeConnection class which is used to create
///         connections to other Node Managers.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 March 2008
///  <br>Last Modified: 5 March 2008
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
#include "jaus/components/node/nodeconnection.h"
#include "jaus/messages/time.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeConnection::NodeConnection() : mDiscoveryFlag(false),
                                   mpCallback(0),
                                   mpSharedMemory(0),
                                   mpUDP(0),
                                   mpTCP(0),
                                   mpSerial(0),
                                   mSendTimeMs(0),
                                   mRecvTimeMs(0)

{

}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeConnection::~NodeConnection()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a shared memory or UDP/TCP connection.
///
///   \param id The ID of the node being connected to.
///   \param cb The callback to use if any messages are received over the
///             transport.
///   \param host If UDP/TCP is available, this is the host ID/name.
///   \param tcp If true, make a TCP connection, otherwise UDP.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnection::CreateConnection(const Address& id,
                                     StreamCallback* cb,
                                     const std::string* host,
                                     const bool tcp)
{
    // Close any previously open connections.
    Shutdown();
    mID = id;
    mpCallback = cb;
    // First, try to connect through shared memory.
    mpSharedMemory = new JSharedMemory();
    if(mpSharedMemory->OpenInbox(id) > 0 && mpSharedMemory->IsActive(200))
    {
        mConnectionType = StreamCallback::SharedMemory;
        return JAUS_OK;
    }
    else
    {
        delete mpSharedMemory;
        mpSharedMemory = NULL;
    }
    if(host)
    {
        // If udp is true, then try UDP connection first.
        if(tcp == false)
        {
            mpUDP = new JUDPClient();
            if(mpUDP->Initialize(*host))
            {
                mConnectionType = StreamCallback::UDP;
                return JAUS_OK;
            }
            else
            {
                delete mpUDP;
                mpUDP = false;
            }
        }
        else
        {
            // Finally, try TCP
            mpTCP = new JTCPClient();
            if(mpTCP->Initialize(*host, cb))
            {
                mConnectionType = StreamCallback::TCP;
                return JAUS_OK;
            }
            else
            {
                delete mpTCP;
                mpTCP = NULL;
            }
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a serial connection.
///
///   \param id The ID of the node being connected to.
///   \param cb The callback to use if any messages are received over the
///             transport.
///   \param port Serial port to use.
///   \param baud Bits per second rate.
///   \param bits Nubmer of bits per byte.
///   \param parity Parity (see CxUtils::Serial).
///   \param stop Number of stop bits.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnection::CreateConnection(const Address& id,
                                     StreamCallback* cb,
                                     const std::string& port,
                                     const unsigned int baud,
                                     const unsigned int bits,
                                     const unsigned int parity,
                                     const unsigned int stop)
{
    // Close any previously open connections.
    Shutdown();
    mID = id;
    mpCallback = cb;

    // Finally, try TCP
    mpSerial = new JSerial();
    if(mpSerial->Initialize(port.c_str(), cb, baud, bits, parity, stop))
    {
        mConnectionType = StreamCallback::TCP;
        return JAUS_OK;
    }
    else
    {
        delete mpSerial;
        mpSerial = NULL;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Closes the connection and deletes any allocated memory.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnection::Shutdown()
{
    if(mpSharedMemory)
    {
        mpSharedMemory->Close();
        delete mpSharedMemory;
        mpSharedMemory = NULL;
    }
    else if(mpUDP)
    {
        mpUDP->Shutdown();
        delete mpUDP;
        mpUDP = NULL;
    }
    else if(mpTCP)
    {
        mpTCP->Shutdown();
        delete mpTCP;
        mpTCP = NULL;
    }
    else if(mpSerial)
    {
        mpSerial->Shutdown();
        delete mpSerial;
        mpSerial = NULL;
    }
    mpCallback = NULL;
    mID(0, 0, 0, 0);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sends the message stream data if a connection is present.
///
///   \param stream The stream data to send (must be <= JAUS_MAX_PACKET_SIZE).
///
///   \return JAUS_OK on success, otherwise JAUS_FAIURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeConnection::SendStream(const Stream& stream)
{
    int result = JAUS_FAILURE;
    switch(mConnectionType)
    {
    case StreamCallback::SharedMemory:
        result = (mpSharedMemory && mpSharedMemory->EnqueueMessage(stream) > 0) ? JAUS_OK : JAUS_FAILURE;
        break;
    case StreamCallback::UDP:
        result = (mpUDP && mpUDP->Send(stream) > 0) ? JAUS_OK : JAUS_FAILURE;
        break;
    case StreamCallback::TCP:
        result = (mpTCP && mpTCP->Send(stream) > 0) ? JAUS_OK : JAUS_FAILURE;
        break;
    case StreamCallback::Serial:
        result = (mpSerial && mpSerial->Send(stream) > 0) ? JAUS_OK : JAUS_FAILURE;
        break;
    default:
        result = JAUS_FAILURE;
        break;
    }

    if(result == JAUS_OK)
    {
        mSendTimeMs = Time::GetUtcTimeMs();
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Receives any messages from connections first, updates the
///          data receive time, then passes it to main StreamCallback.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeConnection::ProcessStreamCallback(const Stream& msg,
                                           const Header* info,
                                           const StreamCallback::Transport transport)
{
    mRecvTimeMs = Time::GetUtcTimeMs();
    if(mpCallback)
    {
        mpCallback->ProcessStreamCallback(msg, info, transport);
    }
}


/*  End of File */
