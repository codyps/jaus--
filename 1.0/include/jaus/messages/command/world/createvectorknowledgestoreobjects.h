////////////////////////////////////////////////////////////////////////////////////
///
///  \file createvectorknowledgestoreobjects.h
///  \brief Implementation of message for Create Vector Knowledge Store
///         Objects.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 July 2008
///  <br>Last Modified: 28 July 2008
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
#ifndef __JAUS_CREATE_WORLD_VECTOR_KNOWLEDGE_STORE_OBJECTS__H
#define __JAUS_CREATE_WORLD_VECTOR_KNOWLEDGE_STORE_OBJECTS__H

#include "jaus/messages/message.h"
#include "jaus/messages/common/world/object.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class CreateVectorKnowledgeStoreObjects
    ///   \brief This message is used to add objects to the Vector Knowledge Store. 
    ///   This message allows multiple vector objects to be created using a single
    ///   message.
    ///  
    ///   The presence vector for this message applies to all objects in the
    ///   message (as of this writing only indicates if a buffer is present).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL CreateVectorKnowledgeStoreObjects : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                ObjectBuffer  = 0x01,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                ObjectBuffer = 0,
            };
        };
        CreateVectorKnowledgeStoreObjects();
        CreateVectorKnowledgeStoreObjects(const CreateVectorKnowledgeStoreObjects& msg);
        virtual ~CreateVectorKnowledgeStoreObjects();
        int SetMessageProperties(const Byte properties);
        int SetLocalRequestID(const Byte id);
        int AddObject(const Object& object);
        int SetObjects(const Object::List& objects, const bool buffers = false);
        Byte GetPresenceVector() const { return mPresenceVector; }
        Byte GetMessageProperties() const { return mMessageProperties; }
        Byte GetLocalRequestID() const { return mLocalRequestID; }
        Object::List* GetObjects() const { return (Object::List*)(&mObjects); }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new CreateVectorKnowledgeStoreObjects(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        CreateVectorKnowledgeStoreObjects& operator=(const CreateVectorKnowledgeStoreObjects& msg);
    protected:
        Byte mPresenceVector;           ///<  Presence vector.
        Byte mMessageProperties;        ///<  Message properties (0: Request confirmation of creation, 1-7 reserved).
        Byte mLocalRequestID;           ///<  Local request id used when return confirmation to requesting component.
        Object::List mObjects;          ///<  Objects to create.
    };
}

#endif
/*  End of File */
