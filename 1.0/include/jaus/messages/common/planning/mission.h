////////////////////////////////////////////////////////////////////////////////////
///
///  \file mission.h
///  \brief This file contains a simple data structure for storing
///  mission information for Mission Planning in JAUS.
///
///  <br>Author(s): Daniel Barber, Cathy Yen
///  <br>Created: 12 March 2009
///  <br>Last Modified: 12 March 2009
///  <br>Copyright (c) 2007
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
#ifndef __JAUS_MISSION_H
#define __JAUS_MISSION_H

#include "jaus/messages/message.h"
#include <cxutils/packet.h>


#include <set>
#include <vector>
#include <map>

using namespace CxUtils;
namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Mission
    ///   \brief Main data structure for storing Mission Planning information in an
    ///   organized way.
    ///
    ///   Main entry point for describing a JAUS Mission using Task objects.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Mission 
    {
    public:

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Status
        ///   \brief statu options of a task or a message.
        ///               <br>0 = spooling. 
        ///               <br>1 = pending. 
        ///               <br>2 = paused. 
        ///               <br>3 = aborted.
        ///               <br>4 = finished.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Status         ///< Indicates status of a mission, task or message.
        {
            Spooling = 0,
            Pending,
            Paused,
            Aborted,
            Finished,
        };        
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Task
        ///   \brief Nested structure for storing mission task data within Mission class.
        ///
        ///   A task is composed of several JAUS messages and may contain child
        ///   tasks.  This structure stores this data in a tree format.  Each Task
        ///   has a set of all the messages in the task, and a pointer to vector of 
        ///   child tasks.  
        ///
        ///   Structure contains get and set methods, along with methods to traverse 
        ///   tasks that are added to the tree of parent and child tasks.
        ///    
        ///   Child task information can be accessed by a using an STL vector, or
        ///   by using pointers from the First Child Task to the Last, etc. like a
        ///   double linked list.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Task
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \class Message
            ///   \brief Nested structure for storing task message data within Task class.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            class JAUS_MSG_DLL Message
            {
            public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \struct Compare
            ///   \brief compares id and allows ordering base on message id in a message set.  
            ///
            ////////////////////////////////////////////////////////////////////////////////////
                struct Compare
                {
                    ////////////////////////////////////////////////////////////////////////////////////
                    ///
                    ///   \brief compares task id.  
                    ///
                    ////////////////////////////////////////////////////////////////////////////////////
                    bool operator () (const Message* left, const Message* right) const
                    {
                        return left->mID < right->mID;
                    }
                };
                ////////////////////////////////////////////////////////////////////////////////////
                ///
                ///   \enum Flag
                ///   \brief blocking flag options of a message.
                ///               <br>0 = spooling. 
                ///               <br>1 = pending. 
                ///
                ////////////////////////////////////////////////////////////////////////////////////
                enum Flag     
                {
                    NonBlocking = 0,
                    Blocking
                };
                typedef std::set<Message*, Compare> Set;  ///<  Set structure for storing messages. 
                Message();
                Message(const Task::Message &message);
                ~Message();
                void PrintMessage() const;
                bool operator<(const Message& message) const; // Sorts by ID
                Message &operator=(const Message &message);   // Sets equal to
                Status mStatus;                  ///<  Status of the message.
                Jaus::Message* mpMessage;        ///<  Pointer to the JAUS message to spool in task.
                UShort mID;                      ///<  Unique identifier for message.
                Flag mBlockingFlag;              ///<  Indicates whether the message is blocking or not.
            };
            typedef std::vector<Task*> List;     ///<  Vector structure for storing tasks.
            typedef std::map<UShort, Task*> Map; ///<  Map structore for storing tasks.

            Task(const UShort id);
            Task(const Task &task);
            virtual ~Task();
            bool IsRootTask() const;
            void SetStatus(const Status status);
            void SetID(const UShort id);
            void SetRoot(const Task* rootTask);
            UShort GetID() const;
            Status GetStatus() const;
            void PrintTask() const;
            void Clear();
            const Message::Set* GetMessages() const;
            Message::Set* GetMessages();
            const Task* GetChild(const UShort id) const;
            Task* GetChild(const UShort id);            
            const Task::List* GetChildTasks() const;
            Task* GetNextSibling();
            const Task* GetNextSibling() const;
            Task* GetPrevSibling();
            const Task* GetPrevSibling() const;
            Task* GetParent();
            const Task* GetParent() const;
            Task* GetRoot();
            const Task* GetRoot() const;
            Task& operator=(const Task& task);
            bool AddChild(Task* childTask);
            bool RemoveChild(const UShort id);
            bool ReplaceChild(Task* replaceTask);
            unsigned int WriteTask(Stream& packet) const;
            unsigned int ReadTask(const UShort pos, 
                            const Stream& packet, 
                            Task* parent = NULL);
            void UpdateTaskMap(Map& mapping);

        protected:
            Status mStatus;                 ///<  Status of the Task.
            UShort mTaskID;                 ///<  The task ID value (must be unique).
            Task::List mChildTasks;         ///<  Sets of pointers to all child tasks to perform.
            Message::Set mMessages;         ///<  All the messages associated with this task.
            Task* mpNextSibling;            ///<  Pointer to sibling task in tasking tree.
            Task* mpPrevSibling;            ///<  Pointer to previous task in tasking tree.
            Task* mpParent;                 ///<  Pointer to parent task.
            Task* mpRoot;                   ///<  Pointer to root/main task.
        };


        Mission();
        Mission(const Mission &mission);
        virtual ~Mission();

        void SetMissionID(const UShort id);
        void SetStatus(const Status status);

        inline Status GetStatus() const { return mStatus; }       ///<   Gets Status of the mission.
        inline UShort GetMissionID() const { return mMissionID; } ///<   Gets the mission id.
        inline void UpdateMissionTaskMap() { mpTask->UpdateTaskMap(mTaskMap);} ///< Updates the protected member mTaskMap 
        void ClearMission();
        void PrintMission() const;
        Task* GetTask(const UShort id);
        const Task* GetTask(const UShort id) const;
        Task* GetTasks();
        const Task* GetTasks() const;
        bool AddTasks(Task* rootTask);
        bool AppendMission(const Mission& mission);
        Mission &operator=(const Mission &mission);
        Task* CreateRootTask(const UShort taskID = 0);
    protected:
        Status mStatus;       ///<  Status of the mission.
        Task* mpTask;         ///<  Pointer to the root/start of tasks in mission.
        UShort mMissionID;    ///<  The mission ID.
        Task::Map mTaskMap;   ///<  Map of all tasks for faster lookup.
    };
}


#endif
/*  End of File */