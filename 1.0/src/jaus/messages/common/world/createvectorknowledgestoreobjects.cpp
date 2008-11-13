////////////////////////////////////////////////////////////////////////////////////
///
///  \file createvectorknowledgestoreobjects.cpp
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
#include "jaus/messages/command/world/createvectorknowledgestoreobjects.h"
#include "jaus/messages/command/commandcodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CreateVectorKnowledgeStoreObjects::CreateVectorKnowledgeStoreObjects() : Message(JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS)
{
    mPresenceVector = mMessageProperties = mLocalRequestID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
CreateVectorKnowledgeStoreObjects::CreateVectorKnowledgeStoreObjects(const CreateVectorKnowledgeStoreObjects& msg)  : Message(JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
CreateVectorKnowledgeStoreObjects::~CreateVectorKnowledgeStoreObjects()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the message properties field value.  If bit 0 is set to 1, then
///   the receiving component of this message will send a Report Vector Knowledge 
///   Store Object response message when received.
///
///   \param properties Message properties data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE;
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::SetMessageProperties(const Byte properties)
{
    mMessageProperties = properties;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the local request ID.  This value is used to match up with a
///   correspnding Report Vector Knowledge Store Object creation command that is
///   sent if the 0 bit of the message properties field is set to 1.
///
///   \param id Local request ID.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE;
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::SetLocalRequestID(const Byte id)
{
    mLocalRequestID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds an object to the list of objects.  If the object has
///   a buffer, then the presence vector bit for object buffers is enabled.
///
///   \param object Object data to add to command.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE;
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::AddObject(const Object& object)
{
    mObjects.push_back(object);
    if(object.HaveBuffer())
    {
        mPresenceVector |= VectorMask::ObjectBuffer;
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the objects for the create message.
///
///   \param objects Vector list of objects to place in message.
///   \param buffers If true, then then the objects have buffer data, and the
///          presence vector bit for Object Buffers is set.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE;
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::SetObjects(const Object::List& objects, const bool buffers)
{
    mObjects = objects;
    if(buffers)
    {
        mPresenceVector |= VectorMask::ObjectBuffer;
    }
    else 
    {
        mPresenceVector &= ~VectorMask::ObjectBuffer;
    }
    return JAUS_OK;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message body data to the stream.
///
///   \param msg The stream to write to.
///   \param version The desired version of the message to write.
///
///   \return Number of bytes written on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::WriteMessageBody(Stream& msg, 
                                                        const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected += JAUS_BYTE_SIZE*3 + JAUS_USHORT_SIZE;
        written += msg.Write(mPresenceVector);
        written += msg.Write(mMessageProperties);
        written += msg.Write(mLocalRequestID);
        written += msg.Write((UShort)(mObjects.size()));
        
        Object::List::const_iterator itr;
        for(itr = mObjects.begin(); itr != mObjects.end(); itr++)
        {
            expected += JAUS_BYTE_SIZE;
            written += msg.Write((Byte)itr->GetType());
            if((mPresenceVector & VectorMask::ObjectBuffer) != 0)
            {
                expected += JAUS_FLOAT_SIZE;
                written += msg.Write(itr->GetBuffer());
            }
            expected += JAUS_BYTE_SIZE;
            written += msg.Write((Byte)(itr->GetFeatures()->size()));
            Object::Feature::List::const_iterator feature;
            for(feature = itr->GetFeatures()->begin();
                feature != itr->GetFeatures()->end();
                feature++)
            {
                expected += JAUS_USHORT_SIZE;
                written += msg.Write(feature->mClass);
                expected += JAUS_BYTE_SIZE + feature->mData.Size();
                written += msg.Write(feature->mData);
            }
            expected += JAUS_USHORT_SIZE;
            written += msg.Write((UShort)(itr->GetPoints()->size()));
            Object::Position::List::const_iterator point;

            for(point = itr->GetPoints()->begin();
                point != itr->GetPoints()->end();
                point++)
            {
                expected += JAUS_INT*2;
                written += msg.Write(point->mLatitude, 90.0, -90.0, ScaledInteger::Int);
                written += msg.Write(point->mLongitude, 180.0, -180.0, ScaledInteger::Int);
            }
        }

        if( expected == written ) 
        {
            return written;
        }
        else
        {
            SetJausError(ErrorCodes::WriteFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message body data from the stream.
///
///   \param msg The stream to read from.
///   \param version The desired version of the message to read.
///
///   \return Number of bytes read on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int CreateVectorKnowledgeStoreObjects::ReadMessageBody(const Stream& msg, 
                                                       const UShort version)
{
    ClearMessageBody();
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        
        UShort numObjects = 0;

        expected = JAUS_BYTE_SIZE*3 + JAUS_USHORT_SIZE;
        read += msg.Read(mPresenceVector);
        read += msg.Read(mMessageProperties);
        read += msg.Read(mLocalRequestID);
        read += msg.Read(numObjects);

        for(UShort o = 0; o < numObjects; o++)
        {
            Object obj;
            Byte objectType;
            Byte numFeatures = 0;
            UShort numPoints = 0;

            expected += JAUS_BYTE_SIZE;
            read += msg.Read(objectType);

            obj.SetType((Object::Type)(objectType));
            if((mPresenceVector & VectorMask::ObjectBuffer) != 0)
            {
                expected += JAUS_FLOAT_SIZE;
                Float buffer;
                read += msg.Read(buffer);
                obj.SetBuffer(buffer);
            }
            
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(numFeatures);
            for(Byte f = 0; f < numFeatures; f++)
            {
                Object::Feature feature;                
                read += msg.Read(feature.mClass);
                read += msg.Read(feature.mData);
                expected += JAUS_USHORT_SIZE;
                expected += feature.mData.Size() + JAUS_BYTE_SIZE;
                // Add feature to object
                obj.GetFeatures()->push_back(feature);
            }

            expected += JAUS_USHORT_SIZE;
            read += msg.Read(numPoints);

            for(UShort p = 0; p < numPoints; p++)
            {
                Object::Position point;
                expected += JAUS_INT*2;
                read += msg.Read(point.mLatitude, 90.0, -90.0, ScaledInteger::Int);
                read += msg.Read(point.mLongitude, 180.0, -180.0, ScaledInteger::Int);
                obj.GetPoints()->push_back(point);
            }

            mObjects.push_back(obj);
        }

        if(expected == read)
        {
            return expected;
        }
        
        SetJausError(ErrorCodes::ReadFailure); return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void CreateVectorKnowledgeStoreObjects::ClearMessageBody()
{
    mPresenceVector = mMessageProperties = mLocalRequestID = 0;
    mObjects.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
CreateVectorKnowledgeStoreObjects& CreateVectorKnowledgeStoreObjects::operator =(const CreateVectorKnowledgeStoreObjects& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mPresenceVector = msg.mPresenceVector;
        mMessageProperties = msg.mMessageProperties;
        mLocalRequestID = msg.mLocalRequestID;
        mObjects = msg.mObjects;
    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size in bytes of the presence vector used by
///          the message.
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  size.  Defaults is current version of library.
///
///   \return Size in bytes of presence vector associated with message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort CreateVectorKnowledgeStoreObjects::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the bit mask associated with the message indicating the
///          bits used in the presence vector of this message (if it has one).
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  mask.  Defaults is current version of library.
///
///   \return Presence vector mask associated with message.  This value
///           is used to determine what bits are used in the presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
UInt CreateVectorKnowledgeStoreObjects::GetPresenceVectorMask(const UShort version) const { return 0x1; }

/*  End of File */
