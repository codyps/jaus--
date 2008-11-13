////////////////////////////////////////////////////////////////////////////////////
///
///  \file jserial.cpp
///  \brief This file contains software for creating JAUS compatible 
///  Serial Port connections.
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
#include "jaus/components/transport/jserial.h"
#include <string.h>

using namespace Jaus;

const char JAUS_SERIAL_HEADER[]           = "JAUS01.0";      ///<  Header attached to UDP messages.
const UInt JAUS_SERIAL_HEADER_SIZE        = 8;               ///<  Length of TCP header in bytes.
const UInt JAUS_MAX_SERIAL_PACKET_SIZE    = JAUS_MAX_PACKET_SIZE + JAUS_SERIAL_HEADER_SIZE;  ///<  Maximum size of a JAUS message with a UDP header attached.


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSerial::JSerial() : mpCallback(NULL)
{
    mSendStream.Write((unsigned char *)JAUS_SERIAL_HEADER, JAUS_SERIAL_HEADER_SIZE);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSerial::~JSerial()
{
    Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the connection.
///
///   \param port Serial port name.
///   \param cb The callback to use for messages received on the connection.
///   \param baud Bits per second rate.
///   \param bits Number of bits for every byte.
///   \param parity Parity (see CxUtils::Serial for values).
///   \param stop Number of stop bits.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSerial::Initialize(const std::string& port,
                        StreamCallback* cb,
                        const unsigned int baud,
                        const unsigned int bits,
                        const unsigned int parity,
                        const unsigned int stop)
{
    Shutdown();

    // Try make a connection.
    if(mSerial.Connect(port.c_str(), baud, bits, parity, stop) > 0)
    {
        mpCallback = cb;

        if(mClientThread.CreateThread(JSerial::ClientThreadFunction, this))
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
int JSerial::Shutdown()
{
    // Close the connection
    mSerial.Flush();
    mSerial.Disconnect();
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
///   \brief Adds any necessary transport header, and sends the data.
///
////  \param msg The message to send.
///
///   \return Number of bytes sent on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSerial::Send(const Jaus::Stream& msg)
{
    int result = 0;
    mTransportMutex.Enter();
    mSendStream.SetLength(JAUS_SERIAL_HEADER_SIZE);
    mSendStream.SetWritePos(JAUS_SERIAL_HEADER_SIZE);
    mSendStream.Write(msg);
    result = mSerial.Send(mSendStream);
    mTransportMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Thread function.  This method constantly attempts to receive
///          incomming data from the connection.
///
////////////////////////////////////////////////////////////////////////////////////
void JSerial::ClientThreadFunction(void *args)
{
    JSerial *client = static_cast<JSerial*>(args);
    
    if(client)
    {
        Header header;
        // Reserver some memory for receiving
        client->mRecvBuffer.Reserve(JAUS_MAX_PACKET_SIZE*2);
        client->mStreamBuffer.Reserve(JAUS_MAX_PACKET_SIZE*10);
        while(!client->mClientThread.QuitThreadFlag() &&
              client->mSerial.IsConnected())
        {
            // Try recieve some data (this method is blocking).
            if(client->mSerial.ReadBytesAvailable() > 0)
            {
                if(client->mSerial.Recv(client->mRecvBuffer, client->mSerial.ReadBytesAvailable()))
                {
                    // Add the data to the total buffer to the end.
                    client->mStreamBuffer += client->mRecvBuffer;
                    // Now try pull out any message data.
                    unsigned int pos = 0;
                    unsigned char *ptr;

                    ptr = (unsigned char *)client->mStreamBuffer.Ptr();
                    //Parse the stream.
                    for(pos = 0; 
                        client->mStreamBuffer.Length() >= JAUS_SERIAL_HEADER_SIZE && pos < client->mStreamBuffer.Length() - JAUS_SERIAL_HEADER_SIZE;)
                    {
                        //  If able to read transport header from message.
                        if(strncmp( (const char *)(&ptr[pos]), 
                                     JAUS_SERIAL_HEADER, 
                                     JAUS_SERIAL_HEADER_SIZE) == 0)
                        {
                            pos += JAUS_SERIAL_HEADER_SIZE;
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
                                                                                  StreamCallback::Serial);
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
                        if( client->mStreamBuffer.Length() >= client->mStreamBuffer.Reserved() - (JAUS_SERIAL_HEADER_SIZE + JAUS_HEADER_SIZE))
                        {
                            client->mStreamBuffer.Clear();
                        }
                    }

                }// If data received.
            }// If data in the receive buffer.
            CxUtils::SleepMs(1);
        } // While connected and not told to quit thread.
    } // If able to cast data.
}

/*  End of File */
