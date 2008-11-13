////////////////////////////////////////////////////////////////////////////////////
///
///  \file jsharedmemory.h
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
#ifndef __JAUS_SHARED_MEMORY_H
#define __JAUS_SHARED_MEMORY_H

#include "jaus/components/streamcallback.h"
#include "jaus/messages/largedataset.h"
#include "jaus/messages/time.h"

#include <cxutils/thread.h>
#include <cxutils/mutex.h>
#include <cxutils/mappedmemory.h>

#ifdef __cplusplus

namespace Jaus
{
    const Byte JAUS_TRANSPORT_SHARED_MEMORY                = 1;                        ///<  Shared memory transport layer.
    const unsigned int JAUS_SHARED_MEMORY_NAME_MAX_SIZE    = 25;                       ///<  Max number of characters in name mapping.
    const unsigned int JAUS_SHARED_MEMORY_DEFAULT_SIZE = 50*JAUS_MAX_PACKET_SIZE;      ///<  Holds a maximum of 50 max size messages.

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class JSharedMemory
    ///   \brief Structure for storing multiple written JAUS messages (Stream) in
    ///   a shared/mapped memory buffer.
    ///
    ///   This structure is used to support transfer of messages between
    ///   processes using shared memory.  It can be used to create an inbox for
    ///   receiving messages, or opening a view to a components inbox to send
    ///   messages.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL JSharedMemory : protected CxUtils::Thread, public ErrorHistory
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Registry
        ///   \brief Registry to keep track of components running on a node.  It also
        ///   is used by components to signal their presence to a node manager.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL Registry
        {
            friend class NodeManager;
        public:
            Registry();
            ~Registry();
            int OpenComponentRegistry(const Byte sid, const Byte nid);
            int OpenNodeRegistry(const Byte sid, const Byte nid);
            int CloseRegistry();
            int Register(const Address& id);
            int UnRegister(const Address& id);
            int GetRegistry(Address::List& registry);
            bool IsRegistered(const Address& id) const;
            bool IsOpen() const;            
            UShort GetNumComponents() const;
            inline Address GetNodeID() const { return mNodeID; }
        protected:
            void ClearRegistry();
            bool mNodeRegistryFlag;             ///<  If true, ID values in registry are for nodes.
            Address mNodeID;                    ///<  ID of the node manager.
            CxUtils::Mutex  mRegistryMutex;     ///<  Just incase a registry object is used in threads.
            CxUtils::MappedMemory mRegistry;    ///<  Shared memory containing registry data.
            unsigned char* mpRegBuffer;         ///<  Buffer for keeping local copies of data.
            bool mComponentRegistryFlag;        ///<  Is this a component or node registry?            
        };
        JSharedMemory();
        ~JSharedMemory();
        int OpenInbox(const Address &id);
        int CreateInbox(const Address& id, 
                        StreamCallback* cb, 
                        const unsigned int size = JAUS_SHARED_MEMORY_DEFAULT_SIZE);
        int Close();
        int Empty();
        int EnqueueMessage(const Stream &msg) const;
        int DequeueMessage(Stream &msg, 
                           Header* header = NULL);      
        int RegisterCallback(StreamCallback* cb);
        void ClearCallback();    
        bool IsOpen() const;        
        bool IsEmpty() { return Size() == 0; }
        bool IsActive(const unsigned int thresh = 2500) const;
        unsigned int Size();
        unsigned int GetBufferSize() const;
        static bool Exists(const Address &id);
        UInt GetEnqueueTime() const;  
        UInt GetDequeueTime() const;
    protected:
        Address mComponentID;           ///<  ID of component's shared memory.
        CxUtils::Mutex mSharedMemMutex; ///<  Thread protection.
        CxUtils::Mutex mCallbackMutex;  ///<  Callback mutex for thread protection.
        CxUtils::MappedMemory mBox;     ///<  Shared memory.
        StreamCallback* mpMessageCb;    ///<  Pointer to your message processing callback.        
        Stream mCallbackStreamData;     ///<  Copy of message for callback.
        virtual void Execute();         ///<  Thread method.
    };
}

#endif

#endif
/*  End of File */
