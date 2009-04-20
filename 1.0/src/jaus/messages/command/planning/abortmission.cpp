////////////////////////////////////////////////////////////////////////////////////
///
///  \file abortmission.cpp
///  \brief This file contains data structure for the JAUS_ABORT_MISSION message.
///
///  <br>Author(s): Cathy Yen
///  <br>Created: 13 April 2009
///  <br>Last Modified: 13 April 2009
///  <br>Copyright (c) 2009
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
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
#include "jaus/messages/command/planning/abortmission.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/messagecreator.h"

#include <iostream>
#include <jaus/messages/errorcodes.h> 

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
AbortMission::AbortMission():Message( JAUS_ABORT_MISSION )
{
    mMissionID = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
AbortMission::AbortMission(const AbortMission &msg):Message( JAUS_ABORT_MISSION )
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
AbortMission::~AbortMission()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the mission ID.
///
///  \param missionID Mission ID value.  Mission ID's should be unique.
///
////////////////////////////////////////////////////////////////////////////////////
void AbortMission::SetMissionID(const UShort missionID)
{
    mMissionID = missionID;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the task ID.
///
///  \param taskID Task ID value.  Mission ID's should be unique.
///
////////////////////////////////////////////////////////////////////////////////////
void AbortMission::SetTaskID(const UShort taskID)
{
    mTaskID = taskID;
}
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message body data to the stream.
///
///   \param msg The stream to write to.
///   \param version The desired version of the message to write.
///
///   \return Number of bytes written.  A return of 0 is not
///           an error (some messages have no message body), 
///           return of -1 indicates error.
///
////////////////////////////////////////////////////////////////////////////////////
int AbortMission::WriteMessageBody(Stream &msg, const UShort version) const
{
    if (version <= JAUS_VERSION_3_4)
    {
        unsigned int counter = 0;
        counter += msg.Write( (UShort) mMissionID);
        counter += msg.Write( (UShort) mTaskID);
        return counter;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
    }
       
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message body data from the stream.
///
///   \param msg The stream to read from.
///   \param version The desired version of the message to read.
///
///   \return Number of bytes read.  A return of 0 is not
///           an error (some messages have no message body), 
///           return of -1 indicates error.
///
////////////////////////////////////////////////////////////////////////////////////
int AbortMission::ReadMessageBody(const Stream &msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        UShort missionID;
        UShort taskID;

        unsigned int counter = 0;
        
        counter += msg.Read(missionID);
        mMissionID = missionID;
        counter += msg.Read(taskID);
        mTaskID = taskID;

        return counter;
    }
    else
    {
        SetJausError(ErrorCodes::UnsupportedVersion);
        return -1;
    }

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void AbortMission::ClearMessageBody()
{ 
    mMissionID = 0;
    mTaskID = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Print message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void AbortMission::Print() const 
{
    Message::Print();
    cout << "Mission ID: " << mMissionID << "  Task ID: " << mTaskID <<endl;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Dose not apply to AbortMission message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort AbortMission::GetPresenceVectorSize(const UShort version ) const
{
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Dose not apply to AbortMission message.
///
////////////////////////////////////////////////////////////////////////////////////
UInt AbortMission::GetPresenceVectorMask(const UShort version ) const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Makes copy of.
///
////////////////////////////////////////////////////////////////////////////////////
Message* AbortMission::Clone() const
{
    return new AbortMission(*this);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
AbortMission& AbortMission::operator=(const AbortMission &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mMissionID = msg.mMissionID;
        mTaskID = msg.mTaskID;
    }

    return *this;
}


int AbortMission::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.

    // AbortMission msg for sending
    AbortMission sendAbortMission;               

    // Set mission id to 1
    sendAbortMission.SetMissionID(1);
    // Set task id to 1
    sendAbortMission.SetTaskID(1);

    // AbortMission msg for receiving
    AbortMission receiveAbortMission;

    // setting some header info
    sendAbortMission.SetDestinationID( Address(1, 2, 3, 4) );
    sendAbortMission.SetSourceID( Address(2, 3, 4, 5) );
    sendAbortMission.SetPriority( Header::Priority::High );
    sendAbortMission.SetAckNack( Header::AckNack::Request );

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.
    if(sendAbortMission.Write(packet))
    {
        sendAbortMission.Print();   // the original data
        cout << std::dec <<  "Size of serialized data is (including header): " << packet.Length() << endl;

        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receiveAbortMission.Read(packet))
        {
            receiveAbortMission.Print(); // data after reading from the stream
            // Data should match! Check the output to make sure through the debug windows
        }
        else
        {
            cout << "Error: Could not de-serialize message.\n";
            return JAUS_FAILURE;
        }
    }
    else
    {
        cout << "Error: Could not serialize message data.\n";
        return JAUS_FAILURE;
    }

    sendAbortMission.ClearMessageBody();
    receiveAbortMission.ClearMessageBody();

    return JAUS_OK;
}



/* end of file */
