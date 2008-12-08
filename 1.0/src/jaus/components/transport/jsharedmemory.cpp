////////////////////////////////////////////////////////////////////////////////////
///
///  \file jsharedmemory.cpp
///  \brief This file contains software for storing written JAUS messages
///  in shared/mapped memory in an array-like structure.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 March 2007
///  <br>Last Modified: 13 March 2008
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
#include "jaus/components/transport/jsharedmemory.h"
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/largedataset.h"

#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>

using namespace Jaus;

#define JAUS_SM_BASE           JAUS_UINT_SIZE*6        ///<  Base address in buffer where messages begin.
#define JAUS_SM_SIZE_POS       0                       ///<  Where size of shared memory is stored.
#define JAUS_SM_E_TIME_POS     JAUS_UINT_SIZE          ///<  Where enqueue time is stored.
#define JAUS_SM_D_TIME_POS     JAUS_UINT_SIZE*2        ///<  Where dequeue time is stored.
#define JAUS_SM_COUNT_POS      JAUS_UINT_SIZE*3        ///<  Where count is stored.
#define JAUS_SM_START_POS      JAUS_UINT_SIZE*4        ///<  Where start position in buff is.
#define JAUS_SM_END_POS        JAUS_UINT_SIZE*5        ///<  Where end position in buff is.

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSharedMemory::Registry::Registry()
{
    mNodeRegistryFlag = false;
    mpRegBuffer = new unsigned char [JAUS_USHORT_SIZE +
                                     255*(2*JAUS_BYTE_SIZE)];
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSharedMemory::Registry::~Registry()
{
    CloseRegistry();
    if( mpRegBuffer )
    {
        delete[] mpRegBuffer;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Opens a view of the registry for components.
///
///   \param sid Subsystem ID number.
///   \param nid Node ID number.
///
///   \return JAUS_OK if opened, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::OpenComponentRegistry(const Byte sid, const Byte nid)
{
    int result = JAUS_FAILURE;
    char sname[256];

    if(sid == 0 || sid == 255 || nid == 0 || nid == 255 )
        return result;

    sprintf(sname, "JComponentRegistry_%03d.%03d", sid, nid);

    mRegistryMutex.Enter();

    if( mRegistry.OpenMappedMemory( sname ) )
    {
        result = JAUS_OK;
        mNodeID(sid, nid, 1, 1);
        mNodeRegistryFlag = false;
    }
    else if( mRegistry.CreateMappedMemory(sname, JAUS_USHORT_SIZE + 255*(2*JAUS_BYTE_SIZE)) )
    {
        mNodeRegistryFlag = false;
        mNodeID(sid, nid, 1, 1);
        mRegistry.Lock();
        memset((void *)mRegistry.GetMemory(), 0, mRegistry.Length());
        mRegistry.Unlock();
        result = JAUS_OK;
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Opens a view of the registry for nodes.
///
///   \param sid Subsystem ID number.
///   \param nid Node ID number.
///
///   \return JAUS_OK if opened, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::OpenNodeRegistry(const Byte sid, const Byte nid)
{
    int result = JAUS_FAILURE;
    char sname[256];

    sprintf(sname, "JNodeRegistry");

    mRegistryMutex.Enter();

    if( mRegistry.OpenMappedMemory( sname ) )
    {
        result = JAUS_OK;
        mNodeID(sid, nid, 1, 1);
        mNodeRegistryFlag = true;
    }
    else if( sid != 0 && sid != 255 &&
             nid != 0 && nid != 255 &&
             mRegistry.CreateMappedMemory(sname, JAUS_USHORT_SIZE + 255*(2*JAUS_BYTE_SIZE)) )
    {
        mNodeRegistryFlag = true;
        mNodeID(sid, nid, 1, 1);
        mRegistry.Lock();
        memset((void *)mRegistry.GetMemory(), 0, mRegistry.Length());
        mRegistry.Unlock();
        result = JAUS_OK;
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Closes the view of the registry.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::CloseRegistry()
{
    mNodeRegistryFlag = false;
    mRegistryMutex.Enter();

    mRegistry.CloseMappedMemory();
    mNodeID(0, 0, 0, 0);

    mRegistryMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Register/Add an ID to the registry.
///
///   The ID must have the same subsystem and node ID components as the
///   Node Manager the registry is for.
///
///   \param id The ID to register.
///
///   \return JAUS_OK if registered, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::Register(const Address& id)
{
    int result = JAUS_FAILURE;

    if( !id.IsValid() ||
        id.IsBroadcast() ||
        id.mNode != mNodeID.mNode ||
        id.mSubsystem != mNodeID.mSubsystem)
    {
            return result;
    }
    mRegistryMutex.Enter();

    if( mRegistry.IsOpen() )
    {
        UShort count = 0;
        unsigned char *mem = NULL;
        bool exists = false;
        mRegistry.Lock();
        mem = (unsigned char *)mRegistry.pGetMemory();
        memcpy(&count, mem, JAUS_USHORT_SIZE);
        mem += JAUS_USHORT_SIZE;
        for(UShort i = 0; i < count*2; i+=2)
        {
            if( mNodeRegistryFlag )
            {
                if( mem[i] == id.mSubsystem && mem[i+1] == id.mNode )
                {
                    exists = true;
                    result = JAUS_OK;
                    break;
                }
            }
            else
            {
                if( mem[i] == id.mComponent && mem[i+1] == id.mInstance )
                {
                    exists = true;
                    result = JAUS_OK;
                    break;
                }
            }
        }
        if( !exists && count < 255 )
        {
            //  Add in the back of the registry.
            mem += count*2;
            if( mNodeRegistryFlag )
            {
                mem[0] = id.mSubsystem;
                mem[1] = id.mNode;
            }
            else
            {
                mem[0] = id.mComponent;
                mem[1] = id.mInstance;
            }
            mem = (unsigned char *)mRegistry.pGetMemory();
            count++;
            memcpy(mem, &count, JAUS_USHORT_SIZE);
            result = JAUS_OK;
        }
        mRegistry.Unlock();
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Unregister an ID from the register.  This is done when a
///   component needs to disconnect from a Node Manager.
///
///   \param id The JAUS ID to unregister.
///
///   \return JAUS_OK if id was removed, JAUS_FAILURE if registry not open or ID
///           not inside.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::UnRegister(const Address& id)
{
    int result = JAUS_FAILURE;

    if(!id.IsValid() ||
        id.IsBroadcast() ||
        id.mNode != mNodeID.mNode ||
        id.mSubsystem != mNodeID.mSubsystem)
    {
            return result;
    }

    mRegistryMutex.Enter();

    if( mRegistry.IsOpen() )
    {
        UShort count = 0;
        unsigned char *mem = NULL;
        bool exists = false;
        mRegistry.Lock();

        mem = (unsigned char *)mRegistry.pGetMemory();
        memcpy(&count, mem, JAUS_USHORT_SIZE);
        mem += JAUS_USHORT_SIZE;
        //  Find the component ID values in the memory block.
        for(UShort i = 0; i < count*2; i+=2)
        {
            if( mNodeRegistryFlag )
            {
                if( mem[i] == id.mSubsystem && mem[i+1] == id.mNode )
                {
                    exists = true;
                    result = JAUS_OK;
                    //  Delete the memory items by overwriting it.
                    memcpy(&mem[i], &mem[i+2], count*2 - i);
                    //  Update the count in memory.
                    count--;
                    mem = (unsigned char *)(mRegistry.pGetMemory());
                    memcpy(mem, &count, JAUS_USHORT_SIZE);
                    break;
                }
            }
            else
            {
                if( mem[i] == id.mComponent && mem[i+1] == id.mInstance )
                {
                    exists = true;
                    result = JAUS_OK;
                    //  Delete the memory items by overwriting it.
                    memcpy(&mem[i], &mem[i+2], count*2 - i);
                    //  Update the count in memory.
                    count--;
                    mem = (unsigned char *)(mRegistry.pGetMemory());
                    memcpy(mem, &count, JAUS_USHORT_SIZE);
                    break;
                }
            }
        }

        mRegistry.Unlock();
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get a copy of all JAUS ID values within the registry.
///
///   \param registry List of all component ID's in registry.
///
///   \return JAUS_OK if copy retrieved, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Registry::GetRegistry(Address::List& registry)
{
    int result = JAUS_FAILURE;

    registry.clear();

    mRegistryMutex.Enter();

    if( mRegistry.IsOpen() )
    {
        result = JAUS_OK;

        UShort count = 0;
        unsigned char *mem = NULL;

        mRegistry.Lock();

        mem = (unsigned char *)mRegistry.pGetMemory();
        memcpy(&count, mem, JAUS_USHORT_SIZE);
        mem += JAUS_USHORT_SIZE;

        if( count > 0 )
        {
            registry.reserve( count );
        }

        for(UShort i = 0; i < count*2; i+=2)
        {
            Address toAdd;
            if( mNodeRegistryFlag )
            {
                toAdd(mem[i], mem[i+1], 1, 1);
                //registry.push_back(Address(mem[i], mem[i+1], 1, 1));
            }
            else
            {
                toAdd(mNodeID.mSubsystem, mNodeID.mNode, mem[i], mem[i+1]);
                //registry.push_back(Address(mNodeID.mSubsystem, mNodeID.mNode, mem[i], mem[i+1]));
            }
            if(toAdd.IsValid() && !toAdd.IsBroadcast())
            {
                registry.push_back(toAdd);
            }
        }

        mRegistry.Unlock();
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks if the ID is in the registry.
///
///   \return True if registered, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool JSharedMemory::Registry::IsRegistered(const Address& id) const
{
    bool result = false;

    if( id.mSubsystem != mNodeID.mSubsystem ||
        id.mNode != mNodeID.mNode )
    {
            return result;
    }

    unsigned char *rcopy = NULL;
    rcopy = mpRegBuffer;

    mRegistryMutex.Enter();

    if( mRegistry.IsOpen() )
    {
        UShort count = 0;
        unsigned char *mem = NULL;

        mRegistry.Lock();
        // Just copy the buffer from memory, and process it later.
        memcpy(rcopy, mRegistry.pGetMemory(), mRegistry.Length());
        mRegistry.Unlock();

        mem = rcopy;
        if( mem )
        {
            memcpy(&count, mem, JAUS_USHORT_SIZE);
            mem += JAUS_USHORT_SIZE;

            for(UShort i = 0; i < count*2; i+=2)
            {
                if( mNodeRegistryFlag )
                {
                    if( id.mSubsystem == mem[i] && id.mNode == mem[i+1] )
                    {
                        result = true;
                        break;
                    }
                }
                else
                {
                    if( id.mComponent == mem[i] && id.mInstance == mem[i+1] )
                    {
                        result = true;
                        break;
                    }
                }
            }
        }
    }

    mRegistryMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the number of components ID's inside the registry.
///
///   \return The number of components ID's inside the registry.
///
////////////////////////////////////////////////////////////////////////////////////
UShort JSharedMemory::Registry::GetNumComponents() const
{
    mRegistryMutex.Enter();
    UShort count = 0;
    if( mRegistry.IsOpen() )
    {
        mRegistry.Read( count , 0 );
    }

    mRegistryMutex.Leave();

    return count;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if registry is open, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool JSharedMemory::Registry::IsOpen() const
{
    bool result = false;
    mRegistryMutex.Enter();
    result = mRegistry.IsOpen();
    mRegistryMutex.Leave();
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all values in the registry.
///
////////////////////////////////////////////////////////////////////////////////////
void JSharedMemory::Registry::ClearRegistry()
{
    mRegistryMutex.Enter();

    if( mRegistry.IsOpen() ) {
        mRegistry.Write( (UShort)(0), 0 );
    }

    mRegistryMutex.Leave();
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSharedMemory::JSharedMemory()
{
    mpMessageCb = NULL;
    mEnableMultiPacketCollectionFlag;
    mpMergedStream = NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JSharedMemory::~JSharedMemory()
{
    Close();
    if(mpMergedStream)
    {
        delete mpMergedStream;
    }
    mpMergedStream = NULL;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Open a message box so that message can be placed inside of it.
///
///  Use this method to create a view of a components shared memory inbox
///  so you can send the component messages.
///
///  \param id The JAUS address of the message box.
///
///  \return JAUS_OK if a mapping view was made, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::OpenInbox(const Address &id)
{
    int result = JAUS_FAILURE;
    //  Generate string name
    char name[JAUS_SHARED_MEMORY_NAME_MAX_SIZE];
    sprintf(name, "%03d.%03d.%03d.%03d_JSM", id.mSubsystem, id.mNode, id.mComponent, id.mInstance);

    Close();

    mSharedMemMutex.Enter();
    // Try and open a view for read/writing.
    if(mBox.OpenMappedMemory(name))
    {
        mBox.Lock();
        // If failed to lookup length, but
        // did open file, pull memory size from
        // header data.
        if(mBox.Length() == 0)
        {
            mBox.SetLength(4);
            unsigned int len = 0;
            mBox.SetReadPos(0);
            mBox.Read(len);
            mBox.SetLength(len);
        }
        mBox.Unlock();
        mComponentID = id;
        result = JAUS_OK;
    }

    mSharedMemMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Create shared memory JAUS message box for receiving JAUS messages.
///
///  Use this method to create a shared memory location where the node manager
///  can send you messages.
///
///  \param id The JAUS address of the message box. (your components ID).
///  \param cb A pointer to your StreamCallback so that when messages are
///            received you can be notified.
///  \param size The desired size in shared memory for your messages.  Depending
///              on the type of messages you expect to reveive, you may
///              want to make this larger than the default size.  The
///              default size holds 10 MAX size JAUS messages.
///
///  \return JAUS_OK if a mapping view was made, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::CreateInbox(const Address& id, StreamCallback *cb, const unsigned int size)
{
    int result = JAUS_FAILURE;
    //  Generate string name
    char name[JAUS_SHARED_MEMORY_NAME_MAX_SIZE];
    sprintf(name, "%03d.%03d.%03d.%03d_JSM", id.mSubsystem, id.mNode, id.mComponent, id.mInstance);

    if(!id.IsValid() || id.IsBroadcast())
    {
        return result;
    }

    Close();

    mSharedMemMutex.Enter();
    //  First check to see if it already exists.
    if(mBox.OpenMappedMemory(name))
    {
        //  Somebody on this machine has already
        //  created a message box for recieving using
        //  this ID. So fail.
        mSharedMemMutex.Leave();
        return result;
    }
    //  Create shared memory location.
    if(mBox.CreateMappedMemory(name, size + JAUS_SM_BASE))
    {
        mBox.SetWritePos(0);
        mBox.Lock();
        mBox.Write((unsigned int)(size + JAUS_SM_BASE)); //  Size of shared memory.
        mBox.Write((unsigned int)Time::GetUtcTimeMs());  //  Current timestamp.
        mBox.Write((unsigned int)Time::GetUtcTimeMs());  //  Current timestamp.
        mBox.Write((unsigned int)0);                     //  0 messsages in box.
        mBox.Write((unsigned int)JAUS_SM_BASE);     //  Start pos in memory.
        mBox.Write((unsigned int)JAUS_SM_BASE);     //  End pos in memory.
        mBox.Unlock();
        mComponentID = id;
        result =  JAUS_OK;
    }
    mSharedMemMutex.Leave();

    RegisterCallback(cb);

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Closes the message box.  Releases view of memory map.
///
///  \return JAUS_OK if shutdown, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Close()
{
    int result  = JAUS_OK;
     //  Generate string name
    std::string sharedMemName;
    ClearCallback();
    mSharedMemMutex.Enter();
    sharedMemName = mBox.GetMappedName();
    result = mBox.CloseMappedMemory();
    if(sharedMemName.empty() == false)
    {
        CxUtils::MappedMemory::DeleteMappedMemory(sharedMemName.c_str());
    }
    mSharedMemMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears out all messages in the box.
///
///  \return JAUS_OK if empty, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::Empty()
{
    int result = JAUS_FAILURE;
    mSharedMemMutex.Enter();
    if(mBox.IsOpen())
    {
        mBox.Lock();
        mBox.SetWritePos(JAUS_SM_COUNT_POS);
        mBox.Write((unsigned int)(0));
        mBox.Write((unsigned int)JAUS_SM_BASE);     //  Start pos in memory to base.
        mBox.Write((unsigned int)JAUS_SM_BASE);     //  End pos in memory to base.
        mBox.Unlock();

        result = JAUS_OK;
    }
    mSharedMemMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Puts a message into the shared memory block, and updates values
///         in shared memory such as count, update time, etc.
///
///  \param msg The serialized JAUS message to queue.
///
///  \return Number of bytes added, 0 on failure to add whole message.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::EnqueueMessage(const Stream &msg)
{
    int result = JAUS_FAILURE;
    Header info;

    msg.SetReadPos( 0 );

    if(!msg.IsValid())
    {
        return SetJausError(ErrorCodes::BadPacket);
    }
    
    if(mEnableMultiPacketCollectionFlag)
    {
        msg.Read(info);
        if(info.mDataFlag != Header::DataControl::Single)
        {
            if(ProcessMultiPacketStream(msg, info, &mpMergedStream))
            {
                result = JAUS_OK;
                if(mpMergedStream)
                {
                    result = EnqueueMessage(*mpMergedStream);
                }
            }    
            return result;
        }        
    }
    // Enter critical section.
    // This will protect the mapped memory object from
    // other threads within our application.
    mSharedMemMutex.Enter();
    // Make sure we have an open view to shared memory.
    if(mBox.IsOpen())
    {
        // Lock the shared memory location critical area.
        // This will block another process from accessing
        // shared memory, because CxUtils::MappedMemory uses
        // a Global Shared Mutex.  The name of the
        // Global Mutex is the same as the mapped memory location but with
        // "_Mutex" appended after.  For example, if our message
        // box uses a shared memory file name of "%001.001.001.001_JSM"
        // then the mutex name will be "%001.001.001.001_JSM_Mutex"
        mBox.Lock();
        unsigned int sharedMemSize = mBox.Size(); // Length of shared memory block in bytes.
        unsigned int messageSize = msg.Length();  // Total size of packet.

        Byte *sharedMemory;
        unsigned int *startPos, *endPos, *enqueueTimeMs, *messageCount;

        // The GetMemory method returns a pointer to shared
        // memory that starts after a 4 byte header.  The 4 byte
        // header in shared memory is a 32 bit unsigned integer which
        // is the size of the shared memory block in bytes.
        sharedMemory = (Byte *)mBox.GetMemory();
        // Get pointers to shared message box header information.
        enqueueTimeMs = (unsigned int *)(sharedMemory + JAUS_SM_E_TIME_POS); //  Last write time to message box.
        messageCount = (unsigned int *)(sharedMemory + JAUS_SM_COUNT_POS);   //  Current number of messages in box.
        startPos = (unsigned int *)(sharedMemory + JAUS_SM_START_POS);       //  Starting byte position of first message in box after 4 byte size header.
        endPos = (unsigned int *)(sharedMemory + JAUS_SM_END_POS);           //  Ending byte position of last message in box after 4 byte size header.

        //  If the message won't fit because we have reached the end, then re-order the
        //  contents of the shared memory buffer by  moving the data back
        //  to the beginning (after header ends).
        if( messageSize + JAUS_UINT_SIZE + *endPos >= sharedMemSize )
        {
            memcpy( &sharedMemory[JAUS_SM_BASE], &sharedMemory[*startPos], (*endPos) - (*startPos) );
            (*endPos) = JAUS_SM_BASE + (*endPos) - (*startPos);
            (*startPos) = JAUS_SM_BASE;
        }

        //  If the message still won't fit, then you have
        //  big problems.
        if( messageSize + JAUS_UINT_SIZE + *endPos < sharedMemSize )
        {
            // Write the size of the data.
            memcpy(&sharedMemory[*endPos], &messageSize, JAUS_UINT_SIZE);
            // Now write the data.
            memcpy(&sharedMemory[*endPos + JAUS_UINT_SIZE], msg.pPtr(), msg.Length());
            // Now add up the results
            (*endPos) += messageSize + JAUS_UINT_SIZE;
            (*messageCount)++;
            (*enqueueTimeMs) = Time::GetUtcTimeMs();
            result = JAUS_OK;
        }
        // Leave mutex so that another
        // process can read the message we just added.
        mBox.Unlock();
    }

    // Unblock access to data within this class so
    // other threads can get to it if needed.
    mSharedMemMutex.Leave();

    // If success, return number of bytes written.
    if( result == JAUS_OK )
    {
        return msg.Length();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes a message from the front of the box (oldest message).
///
///  The oldest message is read first because this is a FIFO queue.
///
///  \param msg The serialized JAUS message removed from the box.
///  \param info Option to copy of the header data for the message.
///
///  \return Number of bytes extracted if message removed, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::DequeueMessage(Stream &msg, Header *info)
{
    int result = JAUS_FAILURE;

    msg.Clear();

    // Enter critical section.
    // This will protect the mapped memory object from
    // other threads within our application.
    mSharedMemMutex.Enter();
    // Make sure we have an open view to shared memory.
    {
        // Lock the shared memory location critical area.
        // This will block another process from accessing
        // shared memory, because CxUtils::MappedMemory uses
        // a Global Shared Mutex.  The name of the
        // Global Mutex is the same as the mapped memory location but with
        // "_Mutex" appended after.  For example, if our message
        // box uses a shared memory file name of "%001.001.001.001_JSM"
        // then the mutex name will be "%001.001.001.001_JSM_Mutex"
        mBox.Lock();
        Byte *sharedMemory;
        unsigned int *startPos, *endPos, *dequeueTimeMs, *messageCount;
        unsigned int messageSize = 0;

        // The GetMemory method returns a pointer to shared
        // memory that starts after a 4 byte header.  The 4 byte
        // header in shared memory is a 32 bit unsigned integer which
        // is the size of the shared memory block in bytes.
        sharedMemory = (Byte *)mBox.pGetMemory();
        //  Get a pointer to the dequeue time in header.
        dequeueTimeMs = (unsigned int *)(sharedMemory + JAUS_SM_D_TIME_POS);

        // Get pointers to shared memory box header information.
        messageCount = (unsigned int *)(sharedMemory + JAUS_SM_COUNT_POS);
        // If there are messages in there..
        if(*messageCount > 0)
        {
            startPos = (unsigned int *)(sharedMemory + JAUS_SM_START_POS);
            endPos = (unsigned int *)(sharedMemory + JAUS_SM_END_POS);
            // Copy the size of the message to be read (value is in bytes).
            memcpy(&messageSize, &sharedMemory[*startPos], JAUS_UINT_SIZE);
            // If the whole message is inside shared memory (not out of bounds).
            if( messageSize > 0 && *startPos + messageSize <= *endPos)
            {
                // Copy the message to the Stream.
                msg.Write( (unsigned char *)&sharedMemory[*startPos + JAUS_UINT_SIZE], messageSize );
                // Advance the startPos position by the number of bytes for the
                // message.
                (*startPos) += messageSize + JAUS_UINT_SIZE;
                // Decrease count
                (*messageCount)--;
                // Only set result to JAUS_OK if the
                // message read is valid.
                if( msg.IsValid() )
                {
                    result = JAUS_OK;
                }
            }
        }
        //  Update the dequeue attempt time.
        //  This is important to do, so that any process wanting to send
        //  messages through shared memory can detect if anyone is
        //  reading from shared memory.
        *dequeueTimeMs = (unsigned int)Time::GetUtcTimeMs();

        mBox.Unlock();
    }
    mSharedMemMutex.Leave();

    // If a pointer to a header was given, then
    // read the header from the stream automatically.
    if( info && result == JAUS_OK )
    {
        msg.Read(*info, 0);
    }
    // On success, return number of bytes read.
    if( result == JAUS_OK )
    {
        return msg.Length();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Adds a callback method that is called whenever a message is
///  in the message box.  This removes the need for polling for messages.
///
///  Any previous callback is removed, only one can be used at a time.
///
///  \param cb The message callback structure to use.
///
///  \return JAUS_OK if callback added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::RegisterCallback(StreamCallback *cb)
{
    ClearCallback();
    if(cb)
    {
        mCallbackMutex.Enter();
        mpMessageCb = cb;
        mCallbackMutex.Leave();
        if(CreateThread()) {
            SetThreadName(mBox.pGetMappedName());
            // Set higher priority for node manager.
            if(mComponentID.mComponent == 1)
            {
                SetThreadPriority(50);
            }
            return JAUS_OK;
        }
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes any callback that is used.
///
////////////////////////////////////////////////////////////////////////////////////
void JSharedMemory::ClearCallback()
{
    // If no callback is registered, then
    // stop the thread since it is not needed.
    StopThread(1000);
    if(IsThreadActive())
        KillThread();

    mCallbackMutex.Enter();
    mpMessageCb = NULL;
    mCallbackStreamData.Destroy();
    mCallbackMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Enables automatic collection of mutli-packet stream data.  Once
///         a message is complete it will be added to shared memory automatically.
///
///   \param[in] enable If true, large data set collection enabled.
///
////////////////////////////////////////////////////////////////////////////////////
void JSharedMemory::EnableLargeDataSets(const bool enable)
{
    mEnableMultiPacketCollectionFlag = enable;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return Returns the number of messages in the box.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int JSharedMemory::Size()
{
    unsigned int s = 0;
    mSharedMemMutex.Enter();
    if(mBox.IsOpen())
    {
        mBox.Lock();
        mBox.SetReadPos(JAUS_SM_COUNT_POS);
        mBox.Read(s);
        mBox.Unlock();
    }
    mSharedMemMutex.Leave();

    return s;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return Size of shared memory buffer in bytes.  This value can be used
///          to determine how many messages you can store.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int JSharedMemory::GetBufferSize() const
{
    unsigned int size = 0;
    mSharedMemMutex.Enter();
    if( mBox.IsOpen() )
    {
        size = mBox.Length() - JAUS_SM_BASE;
    }
    mSharedMemMutex.Leave();
    return size;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if the box is open, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool JSharedMemory::IsOpen() const
{
    bool result = false;

    mSharedMemMutex.Enter();
    if(mBox.IsOpen())
    {
        result = true;
    }
    mSharedMemMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the time stamp of the last message written in the box.
///
///  This information cannot be used to determine if a message box is being
///  used because there may be long periods where no messages are sent.
///
////////////////////////////////////////////////////////////////////////////////////
UInt JSharedMemory::GetEnqueueTime() const
{
    unsigned int ts = 0;

    mSharedMemMutex.Enter();
    if(mBox.IsOpen())
    {
        mBox.Lock();
        mBox.SetReadPos(JAUS_SM_E_TIME_POS);
        mBox.Read(ts);
        mBox.Unlock();
    }
    mSharedMemMutex.Leave();

    return ts;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Gets the time stamp of the time in milliseconds the last
///  attempt to perform a Dequeue occurred.  This can be used to determine if
///  another process is trying to extract data from the message box.
///
////////////////////////////////////////////////////////////////////////////////////
UInt JSharedMemory::GetDequeueTime() const
{
    unsigned int ts = 0;

    mSharedMemMutex.Enter();
    if(mBox.IsOpen())
    {
        mBox.Lock();
        mBox.SetReadPos(JAUS_SM_D_TIME_POS);
        mBox.Read(ts);
        mBox.Unlock();
    }
    mSharedMemMutex.Leave();

    return ts;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Determines if a JSharedMemory is actively being checked for messages.
///
///  This function compares the current time to the time of the last attempt to
///  Dequeue a messages.  If the time difference is less than the parameter
///  passed, then the box is considered active.
///
///  \param thresh The time threshold in ms to check for activity.  If the
///  current time minus the last access time is less than this threshold, then
///  the box is considered active.
///
///  \return True if access to the box is active, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool JSharedMemory::IsActive(const unsigned int thresh) const
{
    bool result = false;

    UInt prev = GetDequeueTime();
    UInt cur = Time::GetUtcTimeMs();

    mSharedMemMutex.Enter();
    if(mBox.IsOpen() && ( (cur - prev) <= thresh || prev > cur ) )
        result = true;
    mSharedMemMutex.Leave();

    if(result == false)
    {
        static int count = 0;
        count++;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief If a callback has been registered, this function is performed in a
///  thread.  It keeps checking to see if the box has been updated, and if so
///  it dequeue's a message and passes it to the callback.
///
////////////////////////////////////////////////////////////////////////////////////
void JSharedMemory::Execute()
{
    Header info;
    bool boxOpen = true, haveCb = true;
    unsigned int stime = Time::GetUtcTimeMs();
    unsigned int count = 0;
    /* Keep looping unless the quit thread function has been called,
       the call back is deleted, or the shared memory is closed. */
    while(!QuitThreadFlag() && haveCb && boxOpen) {

        /*  Try an extract a message from the buffer.  Regardless of
            whether or not a messages is extracted, the timestamp for
            dequeue is updated.  This signals that there is a process
            attempting to read from the buffer (IsActive()).  */
        if(DequeueMessage(mCallbackStreamData, &info))
        {
            //  Use the appropriate callback to process message.
            mCallbackMutex.Enter();
            if(!QuitThreadFlag() && mpMessageCb)
            {
                mpMessageCb->ProcessStreamCallback(mCallbackStreamData,
                                                   &info,
                                                   StreamCallback::SharedMemory,
                                                   NULL);
            }
            else
            {
                haveCb = false;
            }
            mCallbackMutex.Leave();
        }

        #ifdef WIN32
        if( ++count == 50 )
        {
            count = 0;

            CxUtils::SleepMs(1);

        }
        #else
        count = 0;
        usleep(500);
        #endif

        if(!QuitThreadFlag() && Time::GetUtcTimeMs() - stime > 250) {
            mSharedMemMutex.Enter();
            if(!mBox.IsOpen())
                boxOpen = false;
            mSharedMemMutex.Leave();
            CxUtils::SleepMs(1);
            stime = Time::GetUtcTimeMs();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Checks if a shared memory buffer exisits for the given ID.
///
///  \param id Id of the message box to check and see if it already was
///
///  \return True if a message box with the JAUS ID already exists.
///
////////////////////////////////////////////////////////////////////////////////////
bool JSharedMemory::Exists(const Address &id)
{
    char name[JAUS_SHARED_MEMORY_NAME_MAX_SIZE];
    sprintf(name, "%03d.%03d.%03d.%03d_JSM", id.mSubsystem, id.mNode, id.mComponent, id.mInstance);
    return CxUtils::MappedMemory::IsMemOpen(name);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the message is for this component, and it is part of 
///   a multi-packet stream sequence, then this method will collect the
///   pieces.
///
///   The individual packets in the stream are collected by this class in 
///   large data sets.  Once an entire stream has been collected, it is merged
///   and then saved to the 3rd parameter.
///
///   \param msg Message that is part of multi-packet stream sequence.
///   \param header The header for the message.
///   \param merged Pointer to a Pointer to a Stream object for storing 
///                 a merged Stream object if full sequence has been received.
///                 Don't forget to delete you data when done!
///
///   \return JAUS_OK on processing success, JAUS_FAILURE otherwise.  If 
///           JAUS_OK is returned, check the merged parameter for a 
///           merged message stream.
///
////////////////////////////////////////////////////////////////////////////////////
int JSharedMemory::ProcessMultiPacketStream(const Stream& msg, const Header& header, Stream** merged)
{
    bool addedFlag = false;                         // Did we add to a data set?
    UInt pv = 0;                                    // Presence vector.
    MessageCreator::GetPresenceVector(msg, pv);
    LargeDataSetMap::iterator mpstream;             // Iterator for STL map.
    LargeDataSet::Key key(header.mSourceID,
                          header.mCommandCode,
                          pv);                      // Key for indexing in STL map.
    LargeDataSet *toMerge = NULL;

    mMultiPacketStreamsMutex.Enter();

    // Delete any older data.
    if(merged)
    {
        if(*merged)
        {
            delete *merged;
            *merged = NULL;
        }
    }    

    // See if we can add to and exisiting data set, or
    // if we must create a new one.

    if(header.mDataFlag == Header::DataControl::First)
    {
        mpstream = mMultiPacketStreams.find(key);
        // Check for existing data set.
        if(mpstream != mMultiPacketStreams.end())
        {
            // See if we can add to the existing stream.
            if(mpstream->second->AddToDataSet(msg))
            {
                addedFlag = true;
                if(mpstream->second->IsDataSetComplete())
                {
                   toMerge = mpstream->second;
                   mMultiPacketStreams.erase(mpstream);
                }
            }
        }
    }
    else
    {
        // If either a re-transmit, final, or normal packet in
        // multi-packet stream, find a matching stream to add to
        // and add to it.
        for(mpstream = mMultiPacketStreams.begin();
            mpstream != mMultiPacketStreams.end() && !addedFlag;)
        {
            if(mpstream->first.mCommandCode == header.mCommandCode &&
                mpstream->first.mSourceID == header.mSourceID)
            {
                // Try add the stream to the data set.
                if(mpstream->second->AddToDataSet(msg))
                {
                    addedFlag = true;
                    if(mpstream->second->IsDataSetComplete())
                    {
                        toMerge = mpstream->second;
                        mMultiPacketStreams.erase(mpstream);
                    }
                    break;
                }
            }
            // If it has been more than 5 seconds, and we haven't
            // merged the data set yet, get rid of it.
            if(mpstream->second->IsDataSetComplete() == false &&
                Time::GetUtcTimeMs() - mpstream->second->GetUpdateTimeMs() > 1000)
            {
                delete mpstream->second;
#ifdef WIN32     
                mpstream = mMultiPacketStreams.erase(mpstream);
#else                
                mMultiPacketStreams.erase(mpstream);
                mpstream = mMultiPacketStreams.begin();
#endif
            }
            else
            {
                mpstream++;
            }
        }
    }

    // If no match was found, then we need
    // to create a new data set.
    if(addedFlag == false)
    {
        LargeDataSet *lds = new LargeDataSet();

        lds->StartLargeDataSet(msg, &header);
        key.mPresenceVector = 0;

        mpstream = mMultiPacketStreams.find(key);
        while(mpstream != mMultiPacketStreams.end())
        {
            key.mIdentifier++;
            mpstream = mMultiPacketStreams.find(key);
        }

        mMultiPacketStreams[key] = lds;
        mpstream = mMultiPacketStreams.find(key);

        if(lds->IsDataSetComplete())
        {
            toMerge = mpstream->second;
            mMultiPacketStreams.erase(mpstream);
        }
        addedFlag = true;
    }

    // If we have data to merge...
    if(toMerge != NULL)
    {
        (*merged) = new Stream();
        // Merge the data
        toMerge->GetMergedStream(**merged);
        // Make sure ACK/NACK is off.
        Header header;
        (*merged)->Read(header, 0);
        header.mAckNack = Header::AckNack::None;
        (*merged)->Write(header, 0);
        // Delete the data set.
        delete toMerge;
        toMerge = NULL;
    }

    mMultiPacketStreamsMutex.Leave();

    return addedFlag ? JAUS_OK : JAUS_FAILURE;

}

/*  End of File */
