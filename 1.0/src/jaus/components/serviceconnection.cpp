////////////////////////////////////////////////////////////////////////////////////
///
///  \file serviceconnection.cpp
///  \brief Used to store information about a Service Connection (SC).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 29 June 2007
///  <br>Last Modified: 21 March 2008
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
#include "jaus/components/serviceconnection.h"
#include "jaus/messages/time.h"
#include <string.h>

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Key::Key() : mMessageCode(0), mPresenceVector(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes values to parameters passed.
///
///  \param id ID of the source or destination of the SC.
///  \param code The message code associagted with the SC.
///  \param pv Presence vector for the message code.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Key::Key(const Address& id,
                            const UShort code,
                            const UInt pv) : mProviderID(id),
                                             mMessageCode(code),
                                             mPresenceVector(pv)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Use for sorting data in ascending order.
///
///  \param key The key data to compare to.
///
///  \return True if less than, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Key::operator<(const ServiceConnection::Key& key) const
{
    Stream* p1 = (Stream*)&mBuffer;
    Stream* p2 = (Stream*)&key.mBuffer;
    p1->SetWritePos(0);
    p1->Write(mProviderID);
    p1->Write(mMessageCode);
    p1->Write(mPresenceVector);

    p2->SetWritePos(0);
    p2->Write(key.mProviderID);
    p2->Write(key.mMessageCode);
    p2->Write(key.mPresenceVector);

    if( memcmp(mBuffer.pPtr(), key.mBuffer.pPtr(), mBuffer.Length()) < 0 )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if the data is equal, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Key::operator==(const ServiceConnection::Key& key) const
{
    if( mMessageCode == key.mMessageCode &&
        mPresenceVector == key.mPresenceVector &&
        mProviderID == key.mProviderID )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Inform::Inform() : mPeriodicRate(0.0),
                                      mInstanceID(0),
                                      mMessageCode(0),
                                      mPresenceVector(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Inform::Inform(const Inform& inform)
{
    *this = inform;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes some of the values that are common when creating a
///         service connection.
///
///  \param code The message code for the SC.
///  \param presenceVector Presence vector to use for the message in the SC.
///  \param provider The ID of the component that will provide the service.
///  \param requestor The ID of the component requesting the service.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Inform::Inform(const UShort code,
                                  const UInt presenceVector,
                                  const Address& provider,
                                  const Address& requestor) : mPeriodicRate(0.0),
                                                              mInstanceID(0),
                                                              mProviderID(provider),
                                                              mRequestorID(requestor),
                                                              mMessageCode(code),
                                                              mPresenceVector(presenceVector)

{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Inform::~Inform()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Inform& ServiceConnection::Inform::operator=(const ServiceConnection::Inform& inform)
{
    if(this != &inform)
    {
        mPeriodicRate = inform.mPeriodicRate;
        mInstanceID = inform.mInstanceID;
        mProviderID = inform.mProviderID;
        mRequestorID = inform.mRequestorID;
        mMessageCode = inform.mMessageCode;
        mPresenceVector = inform.mPresenceVector;
        mSubscribers = inform.mSubscribers;
        mSuspended = inform.mSuspended;
        mPending = inform.mPending;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if there are subscribers pending confirmation of service
///          connection creation, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Inform::IsPending() const
{
    return mPending.size() > 0 ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if the ID is for a pending subscriber for the service
///         connection.
///
///  \param id ID to check for pending.
///
///  \return True if the ID is pending confirmation, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Inform::IsPending(const Address& id) const
{
    if( mPending.find(id) != mPending.end())
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if the ID is a subscriber to the service connection.
///
///  \param id ID of the component to check against subscriber list.
///
///  \return True if the ID is of a subscriber, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Inform::IsSubscriber(const Address& id) const
{
    Address::Set::const_iterator sub;

    sub = mSubscribers.find(id);
    if(sub != mSubscribers.end())
    {
        return true;
    }
    sub = mPending.find(id);
    if(sub != mPending.end())
    {
        return true;
    }
    sub = mSuspended.find(id);
    if(sub != mSuspended.end())
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Remove the ID from the subscriber list.
///
///  \param id ID of subscriber to remove.
///
///  \return JAUS_OK if removed, JAUS_FAILURE if ID is not a subscriber.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::RemoveSubscriber(const Address& id)
{
    Address::Set::iterator rm;
    if( (rm = mSubscribers.find(id)) != mSubscribers.end())
    {
        mSubscribers.erase(rm);
        return JAUS_OK;
    }
    if( (rm = mPending.find(id)) != mPending.end())
    {
        mPending.erase(rm);
        return JAUS_OK;
    }
    if( (rm = mSuspended.find(id)) != mSuspended.end())
    {
        mSuspended.erase(rm);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Add the ID to the pending subscribers list.
///
///  \param id ID to add to pending list.
///
///  \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::AddPendingSubscriber(const Address& id)
{
    mPending.insert(id);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes the ID from the pending subscribers list.
///
///  \param id ID of component to remove from pending list.
///
///  \return JAUS_OK if removed, JAUS_FAILURE if not a pending ID.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::RemovePendingSubscriber(const Address& id)
{
    Address::Set::iterator rm;
    if( (rm = mPending.find(id)) != mPending.end())
    {
        mPending.erase(rm);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes all subscribers that are on the node in question from the
///  SC.
///
///  \param id ID of the node to remove components for.
///
///  \return JAUS_OK if components removed, JAUS_FAILURE if none removed.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::RemoveSubscribersFromNode(const Address& id)
{
    Address::Set::iterator rm;
    int result = JAUS_FAILURE;
    for(rm = mSubscribers.begin();
        rm != mSubscribers.end();)
    {
        if(rm->mSubsystem == id.mSubsystem && rm->mNode == id.mNode)
        {
#ifdef WIN32
            rm = mSubscribers.erase(rm);
#else
            mSubscribers.erase(rm);
            rm = mSubscribers.begin();
#endif
            result = JAUS_OK;
        }
        else
        {
            rm++;
        }
    }
    for(rm = mSuspended.begin();
        rm != mSuspended.end();)
    {
        if(rm->mSubsystem == id.mSubsystem && rm->mNode == id.mNode)
        {
#ifdef WIN32
            rm = mSuspended.erase(rm);
#else
            mSuspended.erase(rm);
            rm = mSuspended.begin();
#endif
            result = JAUS_OK;
        }
        else
        {
            rm++;
        }
    }
    for(rm = mPending.begin();
        rm != mPending.end();)
    {
        if(rm->mSubsystem == id.mSubsystem && rm->mNode == id.mNode)
        {
#ifdef WIN32
            rm = mPending.erase(rm);
#else
            mPending.erase(rm);
            rm = mPending.begin();
#endif
            result = JAUS_OK;
        }
        else
        {
            rm++;
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if the component has suspended there subscription.
///
///  \param id The ID of the component to check.
///
///  \return True if the component has suspended there connection, otherwise
///          false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Inform::IsSuspended(const Address& id) const
{
    if( mSuspended.find(id) != mSuspended.end() )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if all subscribers have suspended, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Inform::IsServiceSuspended() const
{
    if(mSubscribers.size() == 0 && mSuspended.size() > 0 )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes the subscriber ID from the subscriber list, and adds them to
///         the suspended list.
///
///  \param id The ID of the component suspending.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::SuspendSubscriber(const Address& id)
{
    Address::Set::iterator suspend;
    suspend = mSubscribers.find(id);
    if( suspend != mSubscribers.end() )
    {
        mSuspended.insert(id);
        mSubscribers.erase(suspend);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Re-activates the suspended ID.
///
///  \param id ID that is suspended, it will be added to the active list.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Inform::ActivateSubscriber(const Address& id)
{
    Address::Set::iterator suspend;
    suspend = mSuspended.find(id);
    if( suspend != mSuspended.end() )
    {
        mSubscribers.insert(id);
        mSuspended.erase(suspend);
        return JAUS_OK;
    }
    suspend = mPending.find(id);
    if( suspend != mPending.end() )
    {
        mSubscribers.insert(id);
        mPending.erase(suspend);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return The total number of subscribers.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int ServiceConnection::Inform::GetNumSubscribers() const
{
    return (unsigned int)(mSubscribers.size() + mSuspended.size() + mPending.size());
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Commander::Commander() : mAuthorityLevel(0), mCreationTimeMs(0)
{
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Commander::Commander(const ServiceConnection::Command::Commander& cmd)
{
    mAuthorityLevel = cmd.mAuthorityLevel;
    mID = cmd.mID;
    mCreationTimeMs = cmd.mCreationTimeMs;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.  The creation time is also generated in this
///         constructor and is set to current UTC time in ms.
///
///  \param id The ID of the commander.
///  \param authority Authority level of the commander.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Commander::Commander(const Address& id,
                                                 const Byte authority) : mID(id), mAuthorityLevel(authority)
{
    mCreationTimeMs = Time::GetUtcTimeMs();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Compares authority, than ID for sorting in ascending order.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Command::Commander::operator<(const ServiceConnection::Command::Commander& cmd) const
{
    ULong a, b;

    // Sort based on authority level than ID.
    a = mAuthorityLevel;
    a <<= JAUS_UINT_BITS;
    a |= (UInt)(mID);

    b = cmd.mAuthorityLevel;
    b <<= JAUS_UINT_BITS;
    b |= (UInt)(cmd.mID);

    return a < b;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copies the data.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Commander& ServiceConnection::Command::Commander::operator=(const ServiceConnection::Command::Commander& cmd)
{
    mAuthorityLevel = cmd.mAuthorityLevel;
    mID = cmd.mID;
    mCreationTimeMs = cmd.mCreationTimeMs;
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Command() : mPeriodicRate(0),
                                        mProviderAuthorityCode(0),
                                        mInstanceID(0),
                                        mMessageCode(0),
                                        mPresenceVector(0)

{

}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
///  \param code Message type for SC.
///  \param presenceVector Presence Vector for message on SC.
///  \param providerID The service provider.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Command(const UShort code,
                                    const UInt presenceVector,
                                    const Address& providerID) : mPeriodicRate(0),
                                                                 mInstanceID(0),
                                                                 mProviderID(providerID),
                                                                 mMessageCode(code),
                                                                 mPresenceVector(presenceVector)

{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Command(const ServiceConnection::Command& cmd)
{
    *this = cmd;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::~Command() {}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes the ID from the list of all commanders.
///
///  \param id The ID of the commanding SC source to remove.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::RemoveCommander(const Address& id)
{
    CommanderSet::iterator cmd;

    for(cmd = mSuspended.begin();
        cmd != mSuspended.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mSuspended.erase(cmd);
            return JAUS_OK;
        }
    }
    for(cmd = mActive.begin();
        cmd != mActive.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mActive.erase(cmd);
            return JAUS_OK;
        }
    }
    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mPending.erase(cmd);
            return JAUS_OK;
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes all commanders from the node provided.
///
///  \param id The ID of the node to remove commanders from.
///
///  \return JAUS_OK if commanders removed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::RemoveCommandersFromNode(const Address& id)
{
    CommanderSet::iterator cmd;
    int result = JAUS_FAILURE;
    for(cmd = mSuspended.begin();
        cmd != mSuspended.end();
        cmd++)
    {
        if( cmd->mID.mSubsystem == id.mSubsystem &&
            cmd->mID.mNode == id.mNode)
        {
            mSuspended.erase(cmd);
            result = JAUS_OK;
        }
    }
    for(cmd = mActive.begin();
        cmd != mActive.end();
        cmd++)
    {
        if( cmd->mID.mSubsystem == id.mSubsystem &&
            cmd->mID.mNode == id.mNode)
        {
            mActive.erase(cmd);
            result = JAUS_OK;
        }
    }
    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID.mSubsystem == id.mSubsystem &&
            cmd->mID.mNode == id.mNode)
        {
            mPending.erase(cmd);
            result = JAUS_OK;
        }
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Adds a commander to those waiting confirmation.
///
///  \param cmd Commander ID and authority level for SC.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::AddPendingCommander(const ServiceConnection::Command::Commander& cmd)
{
    mPending.insert(cmd);
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes a commander to those waiting confirmation.
///
///  \param id Commander ID.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::RemovePendingCommander(const Address& id)
{
    CommanderSet::iterator cmd;
    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mPending.erase(cmd);
            return JAUS_OK;
        }
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Removes the commander from the active/pending lists and puts it
///         in the suspended list.
///
///  \param id Commander ID.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::SuspendCommander(const Address& id)
{
    CommanderSet::iterator cmd;

    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mSuspended.insert(*cmd);
            mPending.erase(cmd);
            return JAUS_OK;
        }
    }

    for(cmd = mActive.begin();
        cmd != mActive.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mSuspended.insert(*cmd);
            mActive.erase(cmd);
            return JAUS_OK;
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Activates the commander with a matching ID.  Puts the commander
///         component in the active commanders list.
///
///  \param id Commander ID.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ServiceConnection::Command::ActivateCommander(const Address& id)
{
    CommanderSet::iterator cmd;

    for(cmd = mSuspended.begin();
        cmd != mSuspended.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mActive.insert(*cmd);
            mSuspended.erase( cmd );
            return JAUS_OK;
        }
    }

    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            mActive.insert(*cmd);
            mPending.erase( cmd );
            return JAUS_OK;
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if there are components waiting confirmation of SC creation.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Command::IsPending() const
{
    return mPending.size() > 0 ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if a component is pending confirmation.
///
///  \param id Commander ID.
///
///  \return True if pending, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Command::IsPending(const Address& id) const
{
    CommanderSet::const_iterator cmd;

    for(cmd = mPending.begin();
        cmd != mPending.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            return true;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if the ID is the current commander.
///
///  \param id Commander ID.
///
///  \return True if ID is current commander, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Command::IsCommander(const Address& id) const
{
    return id == mCommander.mID ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Check if an ID is suspended.
///
///  \param id Commander ID.
///
///  \return True if suspended, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::Command::IsSuspended(const Address& id) const
{
    CommanderSet::const_iterator cmd;

    for(cmd = mSuspended.begin();
        cmd != mSuspended.end();
        cmd++)
    {
        if( cmd->mID == id )
        {
            return true;
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Figures out who the next active commander should be.  This also
///         Saves the commander information to the mCommander data member.
///
///  \return The commander information for the next active commander.  The ID
///          will be invalid if there are no other possible candidates (need to
///          terminate the SC).
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command::Commander ServiceConnection::Command::GetNextCommander() const
{
    if(mActive.size() > 0)
    {
        for(CommanderSet::const_iterator cmdr = mActive.begin();
            cmdr != mActive.end();
            cmdr++)
        {
            if(cmdr->mAuthorityLevel >= mProviderAuthorityCode)
            {
                return *cmdr;
            }
        }
    }

    return Commander();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return Number of possible commanders on the SC.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int ServiceConnection::Command::GetNumCommanders() const
{
    return (unsigned int)(mSuspended.size() + mPending.size() + mActive.size());
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Command& ServiceConnection::Command::operator=(const ServiceConnection::Command& cmd)
{
    if(this != &cmd)
    {
        mPeriodicRate = cmd.mPeriodicRate;
        mInstanceID = cmd.mInstanceID;
        mProviderID = cmd.mProviderID;
        mMessageCode = cmd.mMessageCode;
        mPresenceVector = cmd.mPresenceVector;
        mCommander = cmd.mCommander;
        mSuspended = cmd.mSuspended;
        mPending = cmd.mPending;
        mActive = cmd.mActive;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::ServiceConnection() : mPrimaryStatus(ServiceConnection::Active),
                                         mSecondaryStatus(ServiceConnection::Active),
                                         mPeriodicRate(0),
                                         mInstanceID(0),
                                         mMessageCode(0),
                                         mSequenceNumber(0),
                                         mPresenceVector(0),
                                         mUpdateTimeMs(0),
                                         mUpdateIntervalMs(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::ServiceConnection(const ServiceConnection& sc) :  mPrimaryStatus(ServiceConnection::Active),
                                                                     mSecondaryStatus(ServiceConnection::Active),
                                                                     mPeriodicRate(0),
                                                                     mInstanceID(0),
                                                                     mMessageCode(0),
                                                                     mSequenceNumber(0),
                                                                     mPresenceVector(0),
                                                                     mUpdateTimeMs(0),
                                                                     mUpdateIntervalMs(0)
{
    *this = sc;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::~ServiceConnection()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Compare SC data by destination ID, message type, and presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::operator<(const ServiceConnection& sc) const
{
    Key a(mProviderID, mMessageCode, mPresenceVector);
    Key b(sc.mProviderID, sc.mMessageCode, sc.mPresenceVector);

    return a < b;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if the SC information matches.
///
////////////////////////////////////////////////////////////////////////////////////
bool ServiceConnection::operator==(const ServiceConnection& sc) const
{
    if(mInstanceID == sc.mInstanceID &&
        mMessageCode == sc.mMessageCode &&
        mPresenceVector == sc.mPresenceVector &&
        mProviderID == sc.mProviderID &&
        mRequestorID == sc.mRequestorID )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Returns a Key for indexing the SC information.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection::Key ServiceConnection::GetKey() const
{
    return Key(mProviderID, mMessageCode, mPresenceVector);
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ServiceConnection& ServiceConnection::operator=(const ServiceConnection& sc)
{
    mPeriodicRate = sc.mPeriodicRate;
    mInstanceID = sc.mInstanceID;
    mMessageCode = sc.mMessageCode;
    mPresenceVector = sc.mPresenceVector;
    mProviderID = sc.mProviderID;
    mRequestorID = sc.mRequestorID;
    mSequenceNumber = sc.mSequenceNumber;
    mUpdateTimeMs = sc.mUpdateTimeMs;
    mPrimaryStatus = sc.mPrimaryStatus;
    mSecondaryStatus = sc.mSecondaryStatus;
    mUpdateIntervalMs = sc.mUpdateIntervalMs;
    return *this;
}

/*  End of File */
