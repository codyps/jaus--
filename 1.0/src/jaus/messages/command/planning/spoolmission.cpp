//////////////////////////////////////////////////////////////////////////////////////
/////
/////  \file spoolmission.cpp
/////  \brief This file contains data structure for the JAUS_SPOOL_MISSION message.
/////
/////  <br>Author(s): Daniel Barber, Cathy Yen
/////  <br>Created: 25 March 2009
/////  <br>Last Modified: 12 March 2009
/////  <br>Copyright (c) 2009
/////  <br>Applied Cognition and Training in Immersive Virtual Environments
/////  <br>(ACTIVE) Laboratory
/////  <br>Institute for Simulation and Training (IST)
/////  <br>University of Central Florida (UCF)
/////  <br>Email: dbarber@ist.ucf.edu
/////  <br>Web:  http://active.ist.ucf.edu
/////
/////  Redistribution and use in source and binary forms, with or without
/////  modification, are permitted provided that the following conditions are met:
/////      * Redistributions of source code must retain the above copyright
/////        notice, this list of conditions and the following disclaimer.
/////      * Redistributions in binary form must reproduce the above copyright
/////        notice, this list of conditions and the following disclaimer in the
/////        documentation and/or other materials provided with the distribution.
/////      * Neither the name of the ACTIVE LAB, IST, UCF, nor the
/////        names of its contributors may be used to endorse or promote products
/////        derived from this software without specific prior written permission.
///// 
/////  THIS SOFTWARE IS PROVIDED BY THE ACTIVE LAB''AS IS'' AND ANY
/////  EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
/////  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
/////  DISCLAIMED. IN NO EVENT SHALL UCF BE LIABLE FOR ANY
/////  DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
/////  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
/////  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
/////  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
/////  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
/////  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
/////
//////////////////////////////////////////////////////////////////////////////////////
#include "jaus/messages/command/planning/spoolmission.h"
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
SpoolMission::SpoolMission():Message( JAUS_SPOOL_MISSION )
{
    mAppendFlag = Replace;

}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
SpoolMission::SpoolMission(const SpoolMission &msg):Message( JAUS_SPOOL_MISSION )
{
    mAppendFlag = Replace;
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
SpoolMission::~SpoolMission()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets append flag.
///  \param flag Append flag options.
///         <br>0 = Replace the current mission. 
///         <br>1 = Append to current mission. 
///
////////////////////////////////////////////////////////////////////////////////////
void SpoolMission::SetAppendFlag(const AppendFlag flag)
{
    mAppendFlag = flag;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets append flag.
///
////////////////////////////////////////////////////////////////////////////////////
SpoolMission::AppendFlag SpoolMission::GetAppendFlag() const 
{ 
    return mAppendFlag; 
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets a mission object.
///  \param mission Mission reference passed to set up.
///
////////////////////////////////////////////////////////////////////////////////////
void SpoolMission::SetMission(const Mission& mission)
{
    mMission = mission;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets mission object inside the class.
///
////////////////////////////////////////////////////////////////////////////////////
const Jaus::Mission* SpoolMission::GetMission() const
{
    return &mMission;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets mission object inside the class.
///
////////////////////////////////////////////////////////////////////////////////////
const Jaus::Mission* SpoolMission::GetMission()
{
    return &mMission;
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
int SpoolMission::WriteMessageBody(Stream &msg, const UShort version) const
{
    if (version <= JAUS_VERSION_3_4)
    {
        unsigned int counter = 0;
        counter += msg.Write( (UShort) mMission.GetMissionID());
        counter += msg.Write((Byte)mAppendFlag);
        counter += (UInt)mMission.GetTasks()->WriteTask(msg);

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
int SpoolMission::ReadMessageBody(const Stream &msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        UShort missionID;
        Byte flag;

        unsigned int counter = 0;
        
        counter += msg.Read(missionID);
        counter += msg.Read(flag);
        if (mMission.GetTasks()) {
            delete mMission.GetTasks();
        }
        mMission.CreateRootTask();
        
        counter += mMission.GetTasks()->ReadTask((UShort)msg.GetReadPos(), msg);

        mAppendFlag = (AppendFlag)flag;
        mMission.SetMissionID(missionID);
        mMission.UpdateMissionTaskMap();

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
void SpoolMission::ClearMessageBody()
{  
    mAppendFlag = Replace;
    mMission.ClearMission();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Print message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void SpoolMission::Print() const 
{
    Message::Print();
    cout << "Append Flag: " << (int)mAppendFlag << endl;
    mMission.PrintMission();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Dose not apply to spoolmission message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort SpoolMission::GetPresenceVectorSize(const UShort version ) const
{
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Dose not apply to spoolmission message.
///
////////////////////////////////////////////////////////////////////////////////////
UInt SpoolMission::GetPresenceVectorMask(const UShort version ) const
{
    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Makes copy of.
///
////////////////////////////////////////////////////////////////////////////////////
Message* SpoolMission::Clone() const
{
    return new SpoolMission(*this);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
SpoolMission& SpoolMission::operator=(const SpoolMission &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mMission = msg.mMission;
        mAppendFlag = msg.mAppendFlag;
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs spoolmission test.
///          
////////////////////////////////////////////////////////////////////////////////////
int SpoolMission::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.

    // preparing a task msg tree
    Mission::Task* taskPtr = NULL;               // use for child task pointer

    Mission::Task* root = new Mission::Task(1);  // create a root task
    AddTestTaskMessages(3, root);                // add 3 task msgs to the root task

    taskPtr = new Mission::Task(2);              // create a child task id 2
    AddTestTaskMessages(2, taskPtr);             // add 2 msgs to the child task
    root->AddChild(taskPtr);                     // test AddChild()

    taskPtr = new Mission::Task(3);              // Create a child task id 3
    AddTestTaskMessages(1, taskPtr);             // add 1 msgs to the task
    root->AddChild(taskPtr);                     // add this child to the root as well 

    taskPtr = new Mission::Task(4);              // Create a child task id 4
    AddTestTaskMessages(4, taskPtr);             // add 2 msgs to the task
    root->AddChild(taskPtr);                     // add this child to the root as well 

    taskPtr = new Mission::Task(5);              // Create a child task id 5
    AddTestTaskMessages(2, taskPtr);             // add 2 msgs to the task
    root->GetChild(3)->AddChild(taskPtr);        // make this a child of task 3 


    // preparing a spoolmission the mission is contained in
    SpoolMission sendSpoolmission;               // let this be the sender spoolmission

    // preparing a mission the task msg tree belong to
    sendSpoolmission.mMission.SetMissionID(1);
    sendSpoolmission.mMission.AddTasks(root);                     // adds the task msg tree to mission

    //sendSpoolmission.SetMission(mission);        // set mission in spoolmission

    SpoolMission receiveSpoolmission;            // let this be the reveiver spoolmission

    // header settings can also be done from the calling object first.
    sendSpoolmission.SetDestinationID( Address(1, 2, 3, 4) );
    sendSpoolmission.SetSourceID( Address(2, 3, 4, 5) );

    // If it is a high priority message, indicate that
    sendSpoolmission.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendSpoolmission.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendSpoolmission.SetAppendFlag(SpoolMission::Replace);

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.

    if(sendSpoolmission.Write(packet))
    {
        sendSpoolmission.Print();   // the original data
        cout << std::dec <<  "Size of serialized data is (including header): " << packet.Length() << endl;


         
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receiveSpoolmission.Read(packet))
        {
            receiveSpoolmission.Print(); // data after reading from the stream
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

    sendSpoolmission.ClearMessageBody();
    receiveSpoolmission.ClearMessageBody();
  


    return JAUS_OK;
}

void SpoolMission::AddTestTaskMessages(const int num, Mission::Task* task)const
{   // used for testing mission planning task message examples.
    Mission::Task::Message* taskMessage = NULL;
    for(int i = 0; i < num; i++)
    {
        taskMessage = new Mission::Task::Message();
        taskMessage->mID = (UShort)i;

        switch(i)
        {
        case 0:
            taskMessage->mpMessage = MessageCreator::CreateMessage(JAUS_SET_WRENCH_EFFORT);
            taskMessage->mpMessage->SetSourceID(Address(1, 1, 1, 1));
            taskMessage->mpMessage->SetDestinationID(Address(2, 3, 4, 5));
            break;
        case 1:
            taskMessage->mpMessage = MessageCreator::CreateMessage(JAUS_SET_GLOBAL_VECTOR);
            taskMessage->mpMessage->SetSourceID(Address(1, 1, 1, 1));
            taskMessage->mpMessage->SetDestinationID(Address(2, 3, 4, 5));

            break;
        case 2:
            taskMessage->mpMessage = MessageCreator::CreateMessage(JAUS_SET_GLOBAL_WAYPOINT);
            taskMessage->mpMessage->SetSourceID(Address(1, 1, 1, 1));
            taskMessage->mpMessage->SetDestinationID(Address(2, 3, 4, 5));
            break;
        case 3:
            taskMessage->mpMessage = MessageCreator::CreateMessage(JAUS_SET_LOCAL_WAYPOINT);
            taskMessage->mpMessage->SetSourceID(Address(1, 1, 1, 1));
            taskMessage->mpMessage->SetDestinationID(Address(2, 3, 4, 5));
            break;
        default:
            taskMessage->mpMessage = MessageCreator::CreateMessage(JAUS_SET_WRENCH_EFFORT);
            taskMessage->mpMessage->SetSourceID(Address(1, 1, 1, 1));
            taskMessage->mpMessage->SetDestinationID(Address(2, 3, 4, 5));
            break;
        };
        task->GetMessages()->insert(taskMessage);
    }
}



/* end of file */
