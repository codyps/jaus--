////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_messages.cpp
///  \brief Example programing for testing/demonstrating some of the classes
///         used in the JAUS++ message library.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 May 2007
///  <br>Last Modified: 25 April 2008
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
#include <iostream>
#include <algorithm>
#ifdef WIN32
#include <vld.h>
#endif
#include "jaus/messages/msglib.h"
#include "jaus/messages/common/world/object.h"
#include "jaus/messages/common/planning/mission.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/command/planning/spoolmission.h"

using namespace std;
using namespace Jaus;

int MessageExample();
int MessageCreatorExample();
int LargeDataSetExample();
int MissionPlanningMessageExample();

int main(int argc, char *argv[])
{
    cout << "Program Usage: example_messages.exe <num>\n";
    cout << "Values for <num>:\n";
    cout << "    1 - Run Simple Message Example/Test\n";
    cout << "    2 - Run Large Data Set Example/Test\n";
    cout << "    3 - Run Message Creator Example/Test\n";
    cout << "    5 - Run Mission Planning Message Example/Test\n";

    if(argc > 1)
    {
        switch(atoi(argv[1]))
        {
        case 1:
            MessageExample();
            break;
        case 2:
            LargeDataSetExample();
            break;
        case 3:
            MessageCreatorExample();
            break;
		case 4:
			{
				if(argc > 2)
				{
					return MessageCreator::RunTestCase((UShort)(atoi(argv[2])));
				}
			}
			break;
        case 5:
            MissionPlanningMessageExample();
        default:
            cout << "Invalid Value\n";
            break;
        };
    }
    else
    {
        //MessageExample();
        //LargeDataSetExample();
        //MessageCreatorExample();
        MissionPlanningMessageExample();
        
    }
    return 0;
}

int MessageExample()
{
    Stream packet;          // Used to store serialized form of message.
    ReportTime reportTime;  // Stores message data natively.
    ReportTime receivedTime;// Stores message data natively.
    // Set the destination and source of the message (who to send to, who did the sending).
    reportTime.SetDestinationID( Address( 1, 2, 3, 4) );
    reportTime.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    reportTime.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    reportTime.SetAckNack( Header::AckNack::Request );

    // Now set the time.
    reportTime.SetTimeStamp( Jaus::Time::GetUtcTime() );

    // If we check the presence vector for this message, it will
    // have only time data, not date stamp.
    if( BitVector::IsBitSet( reportTime.GetPresenceVector(), ReportTime::VectorBit::Time) )
    {
        cout << "TimeStamp data is present in message.\n";
    }
    else
    {
        cout << "Error: Presence Vector should indicate that TimeStamp is present.\n";
        return 0;
    }
    
    reportTime.GetTimeStamp()->PrintTime();

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.
    if(reportTime.Write(packet))
    {
        cout << "Size of serialized data is (including header): " << packet.Length() << endl;
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receivedTime.Read(packet))
        {
            // Data should match!
            if( BitVector::IsBitSet( receivedTime.GetPresenceVector(), ReportTime::VectorBit::Time) &&
                *receivedTime.GetTimeStamp() == *reportTime.GetTimeStamp())
            {
                cout << "TimeStamp data is present in message, and matches the original message.\n";
                receivedTime.GetTimeStamp()->PrintTime();
            }
            else
            {
                cout << "Error: Could not de-serialize message.\n";
                return 0;
            }
        }
        else
        {
            cout << "Error: Could not de-serialize message.\n";
            return 0;
        }
    }
    else
    {
        cout << "Error: Could not serialize message data.\n";
        return 0;
    }

    return 1;
}

int MessageCreatorExample()
{
    Message* msg = NULL;
    Stream packet;
    ReportHeartbeatPulse heartbeat;

    MessageCreator::RunTestCase(JAUS_CONFIRM_EVENT_REQUEST);
    msg = MessageCreator::CreateMessage(JAUS_REPORT_HEARTBEAT_PULSE);
    msg->SetDestinationID(Address(1, 255, 1, 1));
    msg->SetSourceID(Address(1, 2, 3, 4));
    msg->Print();
    if(msg->Write(packet))
    {
        if(heartbeat.Read(packet))
        {
            cout << "Created correct message using MessageCreator, and was able to read it!\n";
            heartbeat.Print();
        }
    }
    
    delete msg;
    
    // Lets add a custom message to the Message Creator.  You should
    // do this when you have defined a new message type that is
    // not part of the JAUS++ library.  Failure to do so may make it
    // difficult for you to send and receive your new message.  The
    // alternative to this method is to modify the library to include your
    // custom message.
    
    // Create a custom JAUS Message
    class MyCustomMessage : public Message
    {
    public:
        MyCustomMessage() : Message(0xD800)
        {
            mSomeValue = 0;
        }
        ~MyCustomMessage()
        {
            
        }
        virtual int ReadMessageBody(const Stream& message, const UShort version)
        {
            return message.Read(mSomeValue);
        }
        virtual int WriteMessageBody(Stream& message, const UShort version) const
        {
            return message.Write(mSomeValue);
        }
        virtual void ClearMessageBody() 
        {
            mSomeValue = 0;
        }
        virtual Message* Clone() const 
        {
             MyCustomMessage* newMsg = new MyCustomMessage();
             newMsg->mSomeValue = mSomeValue;
             return newMsg;
        }
        virtual UInt GetPresenceVectorMask(const UShort version) const { return 0; }
        virtual UShort GetPresenceVectorSize(const UShort version) const { return 0; }
        Int mSomeValue;
    };
    
    // Add the custom message to Creator
    MessageCreator::AddCustomMessage(new MyCustomMessage());
    // Now it can be created on the fly dynamically
    msg = MessageCreator::CreateMessage(0xD800);
    if(msg)
    {
        // Success!  Now cleanup memory.
        delete msg;
        msg = NULL;
    }

    return 1;
}

// This is an example/test of the Large Data Set class for
// creating/merging multi-packet streams.  Developers using the
// JAUS++ components library should never need to use
// this class.  However, just in case you do, here is an example.
int LargeDataSetExample()
{
    Stream packet;
    Header header;
    const unsigned int dsize = 20000;
    unsigned char data[dsize];
    LargeDataSet mpstream; // Mulit-Packet Stream
   
    //  Create a message with a large amount of data.
    memset(data, 0, dsize);
    char letter = 'a';
    //  Fill it with junk data that can be
    //  recognized.
    for(unsigned int i = 0; i < dsize; i++)
    {
        data[i] = letter;
        letter++;
        if( letter == 'z' || letter == 'Z' )
            letter = 'a';
    }

    //  Use valid source and destination ID values.
    header.mDestinationID(1, 2, 3, 4);
    header.mSourceID(1, 2, 3, 5);
    header.mCommandCode = 0x4807; //  Large data set message.
    header.mDataSize = dsize;
    //header.mAckNack = Header::AckNack::Ack;
    header.mServiceConnectionFlag = JAUS_SERVICE_CONNECTION;
    header.mSequenceNumber = 10;
    //  Write data.  This will be our original packet.
    packet.Write( header );
    packet.Write( data, dsize );

    cout << "Creating a Multi-Packet Stream Sequence from stream with " << packet.Length() << " Bytes.\n";

    //  Generate the mpstream.  This will split the
    //  large packet up into a mutli-packet sequence.
    if( mpstream.CreateLargeDataSet( packet, NULL, 8 ) == JAUS_FAILURE ) 
    {
        
        cout << "Couldn't create large data set, what's up with that?\n";
        return 0;
    }

    cout << "==========================================\n";
    cout << "Multi-Packet Stream Sequence Includes:\n";
    cout << "==========================================\n";
    
    //  Print all the multi-packet stream information.
    mpstream.Print();

    // Test re-assembly of the multi-packet stream to a 
    // single large message again.
    packet.Destroy();
    if( mpstream.GetMergedStream( packet ) == JAUS_FAILURE ) 
    {
        cout << "Something is wrong with the mpstream, must be mal-formed\n";
        return 0;
    }

    // Verify the contents match up with the original.
    if ( memcmp(data, packet.pPtr() + JAUS_HEADER_SIZE, dsize) != 0 ||
         packet.Length() != JAUS_HEADER_SIZE + dsize ) 
    {        
        cout << "Could not re-assemble Johny 5!\n";
        return 0;
    }

    //  Make sure all data is erased.
    packet.Destroy();

    cout << "\nDeleting original message stream, and mixing up the Multi-Packet stream data.\n";
    cout << "This will test capability to assembple multi-packet stream data that arrives out of order.\n";
    
    //  Make a copy of the multi-packet stream, and re-arrange the data
    //  so we can test assembly with out-of-order data.
    Stream::List streamCopy;
    streamCopy = *mpstream.GetDataSet();
    
    random_shuffle( streamCopy.begin(), streamCopy.end() );

    cout << "Assembling Multi-Packet Stream data that is out of order...";
    mpstream.StartLargeDataSet( streamCopy[0] );
    for( unsigned int i = 1; i <(unsigned int)streamCopy.size(); i++ ) 
    {
        mpstream.AddToDataSet( streamCopy[i] );
        if( mpstream.IsDataSetComplete() )
        {
            break;
        }
    }
    cout << "Done!\n";

    if( !mpstream.IsDataSetComplete() ) 
    {
        cout << "\nFailed to Re-Assemble data!!\n";
        return 0;
    }

    cout << "Merging sequence into single stream, and comparing with original data...";

    mpstream.GetMergedStream( packet );

    // Verify the contents match up from the beginning
    if ( memcmp(data, packet.Ptr() + JAUS_HEADER_SIZE, dsize) == 0 &&
        packet.Length() == dsize + JAUS_HEADER_SIZE) 
    {
        cout << "Success!\nMerged sequence matches original data!\n";
        return 1;
    }

    cout << "Failure, I don't know what went wrong!\n";
    if(mpstream.HaveFirstPacket() == false)
    {
        cout << "Don't have first packet in data set.\n";
    }
    if(mpstream.HaveLastPacket() == false)
    {
        cout << "Don't have last packet in data set.\n";
    }
    std::set<UShort> missing = mpstream.GetMissingPackets();
    std::set<UShort>::iterator mitr;
    for(mitr = missing.begin();
        mitr != missing.end();
        mitr++)
    {
        cout << "Missing Packet: " << *mitr << endl;
    }

    return 0;
}


///  This is to show that you can create a spool mission message using MessageCreator
///  RunTestCase is called to show the match data from before and after the msg read and write.
///  each of the methods insided spoolmission and mission classes are tested and working as well.

int MissionPlanningMessageExample()
{   // use for testing Mission Planning Task Messages functionalities.
    SpoolMission* msg = NULL;
    msg = (SpoolMission*)MessageCreator::CreateMessage(JAUS_SPOOL_MISSION);

    msg->SetSourceID(Address(1, 1, 1, 1));
    msg->SetDestinationID(Address(2, 3, 4, 5));

    msg->Print();
    
    msg->RunTestCase();
    system("Pause");
    delete msg;

   return 0;
#if 0

#endif
    return 0;
}
    
