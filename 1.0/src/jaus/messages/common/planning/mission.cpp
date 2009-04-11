////////////////////////////////////////////////////////////////////////////////////
///
///  \file mission.cpp
///  \brief This file contains a simple data structure for storing
///  mission information for Mission Planning in JAUS.
///
///  <br>Author(s): Daniel Barber, Cathy Yen
///  <br>Created: 12 March 2009
///  <br>Last Modified: 12 March 2009
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
#include "jaus/messages/common/planning/mission.h"
#include "jaus/messages/messagecreator.h"
#include <cxutils/packet.h>

#include <iostream>
#include <assert.h>
//#include <tinyxml/tinyxml.h>
#include <iomanip>


using namespace std;
using namespace Jaus;
using namespace CxUtils;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Message::Message()
{
    mStatus = Mission::Pending;
    mpMessage = NULL;
    mID = 0;
    mBlockingFlag = NonBlocking;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Message::Message(const Message& msg)
{
    mStatus = Mission::Pending;
    mpMessage = NULL;
    mID = 0;
    mBlockingFlag = NonBlocking;
    *this = msg;   
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor, deletes any allocated memory.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Message::~Message()
{
    if( mpMessage )
        delete mpMessage;
    mpMessage = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out the task message information to the command line.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::Message::PrintMessage() const
{
    cout << "Task Message ID (" << mID << ") Blocking (" << (int)mBlockingFlag << ") ";
    if( mpMessage ) 
    {
        cout << "Message Type (0x" << setw(4) << setfill('0') << setbase(16) << mpMessage->GetCommandCode() << ")" << endl;
    }
    else 
    {
        cout << "No Message\n";
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Operator<() overload, compares task message using message id.
///   \param msg Reference to message to compare
///  
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::Task::Message::operator <(const Message &msg) const
{
    if(this->mID < msg.mID) 
    {
        return true;
    } 
    else 
    {
        return false;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Operator=() overload, sets equal to (makes copy).
///   \param msg Message reference used for copy
///   
///   It makes a clone of message passed.  
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Message& Mission::Task::Message::operator=(const Message& msg)
{
    if(this != &msg)
    {
        mStatus = msg.mStatus;
        mID = msg.mID;
        mBlockingFlag = msg.mBlockingFlag;       
        
        if (mpMessage)
        {
            delete mpMessage;
        }
        mpMessage = NULL;
        
        if (msg.mpMessage)
        {
            mpMessage = msg.mpMessage->Clone();
        }
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///  \param id of the task.  This should be a unique identifier.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Task(const UShort id)
{
    mStatus = Mission::Pending;
    mTaskID = id;
    mpNextSibling = NULL;
    mpPrevSibling = NULL;
    mpParent = NULL;
    mpRoot = this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///  \param task The task reference to be copied from
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Task(const Task& task)
{
    mTaskID = 0;
    mStatus = Mission::Pending;
    mpNextSibling = NULL;
    mpPrevSibling = NULL;
    mpParent = NULL;
    mpRoot = this;
    *this = task;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::~Task()
{
    Clear();
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Checks if this is root task.
///
///  \return True if there is no parent task to this task, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::Task::IsRootTask() const
{
    if( mpParent == NULL || mpRoot == this ) {
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the status of the task.
///   \param status Status of the task. 
///               <br>0 = spooling. 
///               <br>1 = pending. 
///               <br>2 = paused. 
///               <br>3 = aborted.
///               <br>4 = finished.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::SetStatus(const Status status)
{
    mStatus = status;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the task id.
///  \param id task id to be set. 
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::SetID(const UShort id)
{
    mTaskID = id;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the pointer to the root node for the task and it's children.
///  \param root pointer to root node.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::SetRoot(const Task* root)
{
    if( root ) 
    {
        if( root == this )
            mpParent = NULL;

        mpRoot = (Task*)root;
        for(unsigned int i = 0; i < (unsigned int)mChildTasks.size(); i++)
            mChildTasks[i]->SetRoot( root );
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the task id.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Mission::Task::GetID() const
{
    return mTaskID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the task status.
///  \return one of the following.
///               <br>0 = spooling. 
///               <br>1 = pending. 
///               <br>2 = paused. 
///               <br>3 = aborted.
///               <br>4 = finished.
///
////////////////////////////////////////////////////////////////////////////////////

Mission::Status Mission::Task::GetStatus() const
{
    return mStatus;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Prints task information out to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::PrintTask() const
{
    if( mpParent == NULL )
    {
        cout << "Task " << (int)mTaskID << " Status (" << (Mission::Status) mStatus << ")" << endl;
    }
    else
    {
        cout << "Child Task " << (int)mTaskID << ", Parent Task: " << mpParent->GetID() << endl;
    }

    cout << "Number of Task Messages: " << mMessages.size() << endl;
    Message::Set::const_iterator msg;
    for(msg = mMessages.begin(); msg != mMessages.end(); msg++)
    {
        (*msg)->PrintMessage();
    }
    if( mChildTasks.size() > 0 ) 
    {
        cout << "Number of Child Tasks: " << mChildTasks.size() << endl;
        for( unsigned int i = 0; i < (unsigned int)mChildTasks.size(); i++) 
        {
            mChildTasks[i]->PrintTask();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Delets all task information.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::Clear()
{
    for(unsigned int i = 0; i < (unsigned int)mChildTasks.size(); i++)
    {
        delete mChildTasks[i];
        mChildTasks[i] = NULL;
    }

    Task::Message::Set::iterator msg;
    for(msg = mMessages.begin(); msg != mMessages.end(); msg++)
    {
        delete *msg;
        //msg = NULL;
    }
    mMessages.clear();
    

    mChildTasks.clear();
    mMessages.clear();
    mpNextSibling = mpPrevSibling = NULL;
    mpParent = NULL;
    mpRoot = this;
    mTaskID = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the set of messages in the task
///  \return Address of the message set.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task::Message::Set* Mission::Task::GetMessages()
{
    return (Message::Set*) &mMessages;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the set of messages in the task, message set cannot be altered.
///  \return Address of the message set.
///
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task::Message::Set* Mission::Task::GetMessages()const
{
    return (const Message::Set*) &mMessages;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets child task with matching id.
///  \param id
///  \return Pointer to the childtask or NULL if not found.
///  
///  This function is recursive and checks the ID value against all child
///  tasks, and each childs children, etc.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::Task::GetChild(const UShort id)
{
    Task* child = NULL;
    Task::List::const_iterator c;
    for(c = mChildTasks.begin();
        c != mChildTasks.end();
        c++)
    {
        if( (*c)->mTaskID == id )
        {
            child = (*c);
            break;
        }
        else if ( (child = (*c)->GetChild(id) ) != NULL)
        {
            break;
        }
    }

    return child;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets child task with matching id.
///  \param id Unique id to be used to find the child task.
///  \return Pointer to the childtask or NULL if not found.
///  
///  This function is recursive and checks the ID value against all child
///  tasks, and each childs children, etc.
///
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::Task::GetChild(const UShort id) const
{
    Task* child = NULL;
    Task::List::const_iterator c;
    for(c = mChildTasks.begin();
        c != mChildTasks.end();
        c++)
    {
        if( (*c)->mTaskID == id )
        {
            child = (*c);
            break;
        }
        else if ( (child = (*c)->GetChild(id) ) != NULL)
        {
            break;
        }
    }

    return child;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the vector of child tasks of the current task.
///  \return Pointer to the vector of childtasks.
///  
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task::List* Mission::Task::GetChildTasks() const
{
    return (const Task::List*)&mChildTasks;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to next task in the vector.
///  \return mpNextSibling in the vector of childtasks.
///  
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::Task::GetNextSibling()
{
    return mpNextSibling;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to next task in the vector.
///  \return mpNextSibling in the vector of childtasks.
///  
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::Task::GetNextSibling() const
{
    return mpNextSibling;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to previous task in the vector.
///  \return mpPrevSibling in the vector of childtasks.
///  
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::Task::GetPrevSibling()
{
    return mpPrevSibling;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to previous task in the vector.
///  \return mpPrevSibling in the vector of childtasks.
///  
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::Task::GetPrevSibling() const
{    
    return mpPrevSibling;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to parent task in the task tree.
///  \return mpParent in the tree of tasks.
///  
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::Task::GetParent()
{
    return mpParent;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to parent task in the task tree.
///  \return mpParent in the tree of tasks.
///  
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::Task::GetParent() const
{
    return mpParent;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to the root task in the task tree.
///  \return mpRoot in the tree of tasks.
///  
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::Task::GetRoot()
{
    return mpRoot;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the pointer to the root task in the task tree.
///  \return mpRoot in the tree of tasks.
///  
////////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::Task::GetRoot() const
{
    return mpRoot;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to (makes copy).
///   
///   It checks to make sure the task passed is not the same as the calling task
///   before the assignment.
///
///////////////////////////////////////////////////////////////////////////////////
Mission::Task& Mission::Task::operator=(const Task& task)
{
    if( this != &task)
    {
        Clear();
        Task::Message::Set::const_iterator msg;

        for(msg = task.mMessages.begin();
            msg != task.mMessages.end();
            msg++)
        {
            mMessages.insert( new Task::Message(**msg) );
        }

        for(unsigned int i = 0; i < (unsigned int)task.mChildTasks.size(); i++)
        {
            AddChild(new Task(*task.mChildTasks[i]));
        }

        mTaskID = task.mTaskID;
        mpParent = task.mpParent;
        mpRoot = task.mpRoot;
        mStatus = task.mStatus;
        //  If we are being set equal to a task
        //  which is the root (top of the tree)
        //  then we need to use different pointers
        //  to parent and root nodes.
        if( task.mpRoot == &task )
        {
            mpRoot = this;
            mpParent = NULL;
        }
    }
    return *this;

}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add a child task to current task.
///
///  If the ID of the task being added as a child matches the ID of the current
///  task its child tasks, etc. then the task will not be added.  Each Task ID
///  MUST be unique.
///
///  \param childTask The child task to be added this task.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::Task::AddChild(Task* childTask)
{
    bool result = JAUS_FAILURE;
    if( childTask ) 
    {
        Task* ptr = NULL;
        ptr = (Task*)childTask;

        // Check to see if child already exists 
        // internally
        Task::List::iterator itr;
        for(itr = mChildTasks.begin(); itr != mChildTasks.end(); itr++)
        {
            if( (*itr) == ptr || (*itr)->GetID() == ptr->GetID() )
            {
                return result;
            }
        }

        ptr->SetRoot(mpRoot);
        ptr->mpParent = this;        
        ptr->mpNextSibling = NULL;
        ptr->mpPrevSibling = NULL;

        mChildTasks.push_back( ptr );
        if( mChildTasks.size() > 1 ) 
        {
            ptr->mpPrevSibling = mChildTasks[mChildTasks.size() - 2];
            ptr->mpPrevSibling->mpNextSibling = ptr;
        }

        return (result = JAUS_OK);
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes the child task based on the unique identifier associated
///         with it.  
///
///  This function is recursive and checks the ID value against all child
///  tasks, and each childs children, etc.
///
///  \param id Unique ID of the child task to remove.
///
///  \return JAUS_OK if removed, otherwise JAUS_FAILURE if not present to remove.
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::Task::RemoveChild(const UShort id)
{
    bool result = JAUS_FAILURE; 

    List::iterator childTask = mChildTasks.begin();

    while( childTask != mChildTasks.end() ) 
    {
        if( (*childTask)->mTaskID == id ) 
        {
            // Update pointers to previous and next siblings.
            if ( (*childTask)->mpNextSibling )
            {
                (*childTask)->mpNextSibling->mpPrevSibling = (*childTask)->mpPrevSibling;
            }
            if ( (*childTask)->mpPrevSibling)
            {
                (*childTask)->mpPrevSibling->mpNextSibling = (*childTask)->mpNextSibling;
            }

            // Delete and remove the task.
            delete (*childTask);
            childTask = mChildTasks.erase(childTask);
            result = JAUS_OK;
            break;
        }
        childTask++;
    }

    //  Check recursively.
    if( result == JAUS_FAILURE )
    {
        childTask = mChildTasks.begin();
        while( childTask != mChildTasks.end() )
        {
            if( (*childTask)->RemoveChild(id) )
            {
                result = JAUS_OK;
                break;
            }
            childTask++;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Replaces the child task with a new one of the same task id
///
///  This function is recursive and checks the ID value against all child
///  tasks, and each childs children, etc.
///
///  \param replaceTask The task to replace the existing child task.
///
///  \return JAUS_OK if removed, otherwise JAUS_FAILURE if not present to remove.
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::Task::ReplaceChild(Task* replaceTask)
{
    bool result = JAUS_FAILURE; 
    if (replaceTask)
    {
        List::iterator childTask = mChildTasks.begin();

        while( childTask != mChildTasks.end() ) 
        {
            if( (*childTask)->mTaskID == replaceTask->GetID()) 
            {
                replaceTask->SetRoot(mpRoot);
                replaceTask->mpParent = (*childTask)->mpParent;        
                if ( (*childTask)->mpNextSibling )
                {
                    (*childTask)->mpNextSibling->mpPrevSibling = replaceTask;
                    replaceTask->mpNextSibling = (*childTask)->mpNextSibling;
                }
                if( (*childTask)->mpPrevSibling)
                {
                    (*childTask)->mpPrevSibling->mpNextSibling = replaceTask;
                    replaceTask->mpPrevSibling = (*childTask)->mpPrevSibling;
                }
                
                // Delete the memory
                delete (*childTask);
                // Replace what the iterator points to.
                childTask = mChildTasks.erase(childTask);
                mChildTasks.insert(childTask, replaceTask);                
                result = JAUS_OK;
                break;
            }
            childTask++;
        }

        //  Check recursively.
        if( result == JAUS_FAILURE )
        {
            childTask = mChildTasks.begin();
            while( childTask != mChildTasks.end() )
            {
                if( (*childTask)->ReplaceChild(replaceTask) )
                {
                    result = JAUS_OK;
                    break;
                }
                childTask++;
            }
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Writes the current task info into a Mission Planning Message Stream packet.
///
///  This function writes the current task and all of its jaus messages and 
///  children tasks into a Mission Planning Message Stream packet. 
///  It does this recursively.
///
///  \param packet packet stream the task(s)info is being written to
///  \return counter - length (in bytes)of the data written into the packet.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int Mission::Task::WriteTask(Stream& packet) const
{
    Stream p;
    unsigned int counter = 0;
    unsigned int childTaskIndex = 0;
    const List* childTasks = this->GetChildTasks();
    const Message::Set* messages = this->GetMessages();

    counter += packet.Write((UShort)this->GetID() );         // writes task id
    counter += packet.Write( (UShort) childTasks->size());    // writes number of children tasks
    childTaskIndex = packet.Size();

    for(UShort i = 0; i < childTasks->size(); i++)
    {
        counter += packet.Write( (UInt)(0) );               // space fill for child index ?
    }

    counter += packet.Write((UShort) messages->size());     // writes number of messages for the task

    Message::Set::const_iterator msg = messages->begin();
    while( msg != messages->end() )
    {
        p.Clear();
        counter += packet.Write( (unsigned short)(*msg)->mID );
        (*msg)->mpMessage->Write(p);

        counter += packet.Write(p);
        counter += packet.Write( (Byte)(*msg)->mBlockingFlag);
        msg++;
    }

    if( childTasks->size() > 0 )
    {
        List::const_iterator t = childTasks->begin();
        while( t != childTasks->end() )
        {
            packet.Write( ((UInt)packet.Length()), childTaskIndex);
            counter += (*t)->WriteTask(packet);
            childTaskIndex += JAUS_UINT_SIZE;
            t++;
        }

    }   

    return counter;

}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Reads out the task info from a Mission Planning Message Stream packet.
///
///  This function read a Mission Planning Stream Message Stream packet, 
///  extracts the task info (including all of its messages and children tasks),
///  and place them into a Task object with its original tree structure.
///  This is doen recursively.
///
///  \param pos starting position to read from the packet.
///  \param packet Mission Planning Message packet stream being read from
///  \param parent parent task of the current task
///
///  \return counter - length (in bytes)of the data read from the packet.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int Mission::Task::ReadTask(const UShort pos, const Stream& packet, Task* parent)
{
    unsigned int counter = 0;
    UShort taskID = 0;
    UShort numChildren = 0;
    UShort numMessages = 0;
    UInt* childTaskIndex = NULL;

    counter += packet.Read( taskID, pos + counter );

    this->SetID(taskID);

    if(parent) 
    {
        if (!(parent->AddChild(this)))
            return JAUS_FAILURE;
    }

    counter += packet.Read( numChildren, pos + counter );

    if( numChildren > 0 )
    {
        childTaskIndex = new UInt[numChildren];
        assert(childTaskIndex);
    }
    //read children tasks positions from the packet into array childTaskIndex
    for(unsigned int i = 0; i < numChildren; i++)
    {
        counter += packet.Read( childTaskIndex[i], pos + counter );
    }
    counter += packet.Read( numMessages, pos + counter );

    for( unsigned int i = 0; i < numMessages; i++ )   //read all task messages
    {
        UShort messageID = 0;                      
        Byte blocking;  
        Stream p;
        Header header;
        Message* taskMessage = NULL;

        counter += packet.Read( messageID, pos + counter );
        counter += packet.Read(p, header, pos + counter);   //start to read Jaus message
        counter += packet.Read(blocking, pos + counter );
        if (p.Length() > 0)
        {
            taskMessage = new Task::Message();
            taskMessage->mpMessage = MessageCreator::CreateMessage(p, &header);
            taskMessage->mID = messageID;
            taskMessage->mBlockingFlag = (Mission::Task::Message::Flag)blocking;
            this->GetMessages()->insert(taskMessage);
        }
    }

    //read children tasks recursively
    for(unsigned int i = 0; i < numChildren; i++)
    {
        Mission::Task* childtaskRead = NULL;
        childtaskRead = new Mission::Task(0);
        counter += childtaskRead->ReadTask( (UShort)childTaskIndex[i], packet, this);
    }

    // delete array memory
    if( childTaskIndex )
        delete[] childTaskIndex;

    return counter;
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Has task and all its children add their address to an STL map
///  structure organized by task ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::Task::UpdateTaskMap(Map&  mapping)
{
    if(this == mpRoot)
    {
        mapping.clear();
    }
    mapping[mTaskID] = this;
    if( mChildTasks.size() > 0 )
    {
        List::iterator child = mChildTasks.begin();
        while(child != mChildTasks.end())
        {
            (*child)->UpdateTaskMap(mapping);
            child++;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Mission()
{
    mpTask = NULL;
    mMissionID = 0;
    mStatus = Mission::Pending;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Mission(const Mission& mission)
{
    mpTask = NULL;
    mMissionID = 0;
    mStatus = Mission::Pending;
    *this = mission;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::~Mission()
{
    if( mpTask )
    {
        delete mpTask;
        mpTask = NULL;
    }
    mMissionID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the mission ID.
///
///  \param id Mission ID value.  Mission ID's should be unique.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::SetMissionID(const UShort id)
{
    mMissionID = id;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the status of the mission.
///
///   \param status Status of the task. 
///               <br>0 = spooling. 
///               <br>1 = pending. 
///               <br>2 = paused. 
///               <br>3 = aborted.
///               <br>4 = finished.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::SetStatus(const Status status)
{
    mStatus = status;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all mission data.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::ClearMission()
{
    mMissionID = 0;
    mTaskMap.clear();
    if( mpTask != NULL )
    {
        delete mpTask;
        mpTask = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Prints mission data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void Mission::PrintMission() const
{
    cout << "Mission ID: " << mMissionID << " Number of Tasks: " << mTaskMap.size() << " Status of the Mission: " << mStatus <<endl;
    if( mpTask )
    {
        mpTask->PrintTask();
    }
}


///////////////////////////////////////////////////////////////////////////////////
///
/// \brief Gets a specific task of the mission base on id.
/// \param id task id used to find the task in the task map.
/// \return Pointer to the task matching the id. 
///
///////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::GetTask(const UShort id) const 
{ 
    Task::Map::const_iterator task;
    task = mTaskMap.find( id );
    if( task != mTaskMap.end() )
        return task->second;

    return NULL;
} 

///////////////////////////////////////////////////////////////////////////////////
///
/// \brief Gets a specific task of the mission base on id.
/// \param id task id used to find the task in the task map.
/// \return Pointer to the task matching the id. 
///
///////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::GetTask(const UShort id)
{ 
    Task::Map::iterator task;
    task = mTaskMap.find( id );
    if( task != mTaskMap.end() )
        return task->second;

    return NULL;
} 


///////////////////////////////////////////////////////////////////////////////////
///
/// \brief Gets the root task of the mission.
/// \return Pointer to the root/start of tasks in mission. 
///
///////////////////////////////////////////////////////////////////////////////////
const Mission::Task* Mission::GetTasks() const 
{ 
    return mpTask; 
} 

///////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the root task of the mission.
/// \return Pointer to the root/start of tasks in mission. 
///
///////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::GetTasks()
{ 
    return mpTask; 
} 

///////////////////////////////////////////////////////////////////////////////////
///
///  \brief Adds root task of the mission.
///  \return JAUS_OK is sucessful, otherwise JAUS_FAILURE.
///
///////////////////////////////////////////////////////////////////////////////////
bool Mission::AddTasks(Task* rootTask)
{
    if (rootTask)
    {
        mpTask = rootTask;
        mpTask->UpdateTaskMap(mTaskMap);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Appends the mission onto the end of the current mission.
///  \return JAUS_OK is sucessful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
bool Mission::AppendMission(const Mission& mission)
{
    bool result = JAUS_FAILURE;

    if( mpTask == NULL )
    {
        *this = mission;
        mpTask->UpdateTaskMap(mTaskMap);
        return JAUS_OK;
    }
    else
    {
        if( mpTask->AddChild(new Task(*mission.mpTask)) )
        {
            mpTask->UpdateTaskMap(mTaskMap);
            return JAUS_OK;
        }
        else 
        {
            return result;
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Creates a duplicate of the mission. (Sets equal to).
///
////////////////////////////////////////////////////////////////////////////////////
Mission& Mission::operator=(const Mission& mission)
{
    if( this != &mission)
    {
        if( mpTask )
        {
            delete mpTask;
            mpTask = NULL;
        }
        mTaskMap.clear();

        if( mission.mpTask )
        {
            Task* clone = new Task(*mission.mpTask);
            mpTask = clone;
            mpTask->SetRoot(mpTask);
            mpTask->UpdateTaskMap( mTaskMap );
        }
        
        mMissionID = mission.mMissionID;
    }
    return *this;

}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Creates a new mission with a single root/main task.
///
///  Any previous mission data will be deleted.
///
///  \param taskID The ID of the root task default to 0
///
///  \return Pointer to the new Task created so that task messages can be
///          added to it, etc. NULL is returned on failure.
///
////////////////////////////////////////////////////////////////////////////////////
Mission::Task* Mission::CreateRootTask(const UShort taskID)
{

    if( mpTask )
    {
        delete mpTask;
        mpTask = NULL;
    }
    mTaskMap.clear();

    mpTask = new Task(taskID);
    assert( mpTask );

    mTaskMap[taskID] = mpTask;

    return mpTask;
}

/*  End of File */
