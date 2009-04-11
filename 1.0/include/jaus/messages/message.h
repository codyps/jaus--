////////////////////////////////////////////////////////////////////////////////////
///
///  \file message.h
///  \brief This file contains software for deriving messages in JAUS message
///  structures.
///
///  All messages are written into Stream structures, however the Message
///  base class is used for creating data structures that store read
///  message data and contain methods for  reading/riting from/to
///  a Stream structure.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 26 February 2007
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
#ifndef _JAUS_MESSAGE_H
#define _JAUS_MESSAGE_H

#ifdef __cplusplus

#include "header.h"
#include "stream.h"
#include "time.h"
#include "errorhistory.h"
#include "bitvector.h"
#include <vector>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Message
    ///   \brief Main class for creating implementations of specific JAUS messages.
    ///   Each Message based class is capable of reading/writing a specific
    ///   message.
    ///
    ///   All message classes inherit from this interface.  It requies the child class
    ///   to implement methods for serializing/writing and de-serializing/reading
    ///   message data from a Stream structure.  It also has methods for
    ///   making message duplicates.  Each message can read/write a whole message or
    ///   just the message body.
    ///
    ///   Depending on the version of the message, data will be written
    ///   to a Stream differently when using the Write method.  By default, the
    ///   most recent version of JAUS is used.  So if you need an older version of
    ///   a message used, make sure to specifiy the version type for the message.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Message : public ErrorHistory
    {
    public: 
        typedef std::vector<Message*> List;
        // Sets message type.
        Message(const UShort ccode);
        // Destructor, does any cleanup.
        virtual ~Message();
        // Sets the message priority.
        int SetPriority(const UShort p = Header::Priority::Default);
        // Sets the service connection flag for the message.
        int SetServiceConnectionFlag(const UShort f = JAUS_NO_SERVICE_CONNECTION);
        // Set the source ID of the message.
        int SetSourceID(const Address &src);
        // Set the destination ID of the message.
        int SetDestinationID(const Address &dest);
        // Set the message version.
        int SetVersion(const UShort v);
        // Swap source and destination ID.
        int SwapSourceAndDestination();
        // Set the Acknowledge/Negative-Acknowledge value.
        int SetAckNack(const UShort ack);
        // Set the message sequence number.
        int SetSequenceNumber(const UShort seq);
        // Copy message header information to header structure.
        void CopyToHeader(Header& h) const;
        // Copy header data to internal members.
        void CopyFromHeader(const Header& h);
        // Copy header data from another message structure.
        void CopyHeaderData(const Message* msg);
        // Sets header values to defaults.
        void ClearMessageHeader();   
        // Clears/Resets all message values to defaults.
        virtual void ClearMessage();
        // Clears only message body information.
        virtual void ClearMessageBody() = 0;
        // Get the message type.
        inline UShort GetCommandCode() const { return mCommandCode; }
        // Get the priority of the message.
        inline UShort GetPriority() const { return mPriority; }
        // Get the service connection flag data for message.
        inline UShort GetServiceConnectionFlag() const { return mServiceConnectionFlag; }
        // Get the message version number.
        inline UShort GetVersion() const { return mVersion; }
        // Get the Acknowledge/Negative-Acknowledge value.
        inline UShort GetAckNack() const { return mAckNack; }      
        // Get the message sequence number.
        inline UShort GetSequenceNumber() const { return mSequenceNumber; }        
        // Get the source ID of the message.
        inline Address GetSourceID() const { return mSourceID; }
        // Get the destination ID of the message.
        inline Address GetDestinationID() const { return mDestinationID; }
        // Prints message data to console window.
        virtual void Print() const;        
        // Runs a test case for the message.
        virtual int RunTestCase() const;
        // Writes/Serializes message to a Stream.
        virtual int Write(Stream &msg) const; 
        // Reads/De-serialize data from Stream, and overwrites internal members.
        virtual int Read(const Stream &msg);  
        // Writes message body data from current write position in stream for specific JAUS version.
        // Sets an error or returns -1 on failure.
        virtual int WriteMessageBody(Stream& msg,
                                     const UShort version) const = 0;
        // Reads message body data from current read position in stream and for specific JAUS version.
        // Sets an error or returns -1 on failure.
        virtual int ReadMessageBody(const Stream& msg, 
                                    const UShort version) = 0;
        // Make a copy of the message and returns pointer to it.
        virtual Message* Clone() const = 0;          
        // Get the size of the message presence vector. 0 value indicates no presence vector.
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const = 0;
        // Get the mask associated with presence vector.  Indicates what bits are used.
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const = 0;
    protected:
        int ReadAndValidateHeader(const Stream& msg, Header& header) const;     
        void CopyHeaderData(const Header &header);
        UShort mVersion;                   ///<  Message version.
        UShort mPriority;                  ///<  Message priority.
        UShort mServiceConnectionFlag;     ///<  Service connection flag.
        UShort mAckNack;                   ///<  AckNack flag.
        UShort mSequenceNumber;            ///<  Sequence number (used in service connections).
        const UShort mCommandCode;         ///<  Message type (command code).
        Address mSourceID;                 ///<  Source ID of the message.
        Address mDestinationID;            ///<  Destination ID of the message.
    };

} //  End of Jaus namespace

#endif
#endif
/* End of file */
