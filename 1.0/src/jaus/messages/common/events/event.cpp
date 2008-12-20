////////////////////////////////////////////////////////////////////////////////////
///
///  \file event.cpp
///  \brief This file contains the parent class for creating types of events.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 March 2008
///  <br>Last Modified: 28 March 2008
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
#include "jaus/messages/common/events/event.h"
#include "jaus/messages/messagecreator.h"
#include <math.h>
#include <string.h>

using namespace Jaus;

const double Event::Limits::MaxPeriodicRate = 1092.0;
const double Event::Limits::MinPeriodicRate = 0;

Event::Key::Key() : mID(0),
                    mMessageCode(0)
{
}

Event::Key::Key(const Key& key)
{
    mID = key.mID;
    mEventType = key.mEventType;
    mMessageCode = key.mMessageCode;
    mSourceID = key.mSourceID;
}

Event::Key::~Key()
{
}

bool Event::Key::operator <(const Event::Key &key) const
{
    Stream left, right;
    left.Write(mID);
    left.Write((Byte)mEventType);
    left.Write(mMessageCode);
    left.Write((UInt)mSourceID);
    right.Write(key.mID);
    right.Write((Byte)key.mEventType);
    right.Write(key.mMessageCode);
    right.Write((UInt)key.mSourceID);
    if( memcmp(left.Ptr(), right.Ptr(), left.Length()) < 0)
    {
        return true;
    }
    return false;
}

Event::Key& Event::Key::operator =(const Event::Key &key)
{
    mID = key.mID;
    mEventType = key.mEventType;
    mMessageCode = key.mMessageCode;
    mSourceID = key.mSourceID;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions::Conditions() : mpBoundaryType(0),
                                  mpLimitDataField(0),
                                  mpLowerLimit(0),
                                  mpUpperLimit(0),
                                  mpState(0)

{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions::Conditions(const Event::Conditions& conditions) :  mpBoundaryType(0),
                                                                      mpLimitDataField(0),
                                                                      mpLowerLimit(0),
                                                                      mpUpperLimit(0),
                                                                      mpState(0)
{
    *this = conditions;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions::~Conditions()
{
    Clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the value of the limit data field from report message to base
///          trigger limit on.
///
///   \param df Data field number.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::SetLimitDataField(const Byte df)
{
    if(mpLimitDataField)
    {
        delete mpLimitDataField;
        mpLimitDataField = NULL;
    }
    mpLimitDataField = new Byte;
    *mpLimitDataField = df;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the lower limit for trigger condition.  Used for inside,
///          outside, and low event boundary types.
///
///   \param limit Limit value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::SetLowerLimit(const VarType& limit)
{
    if(mpLowerLimit)
    {
        delete mpLowerLimit;
        mpLowerLimit = NULL;
    }

    mpLowerLimit = new VarType(limit);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the peridic rate for the event.
///
///   \param rate Periodic rate must within Limits ranges.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetPeriodicRate(const double rate)
{
    if(rate >= Limits::MinPeriodicRate && rate <= Limits::MaxPeriodicRate)
    {
        mPeriodicRate = rate;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the upper limit for trigger condition.  Used for inside,
///          outside, and upper event boundary types.
///
///   \param limit Limit value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::SetUpperLimit(const VarType& limit)
{
    if(mpUpperLimit)
    {
        delete mpUpperLimit;
        mpUpperLimit = NULL;
    }

    mpUpperLimit = new VarType(limit);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the trigger condition value for on equal.
///
///   \param state On equal state value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::SetState(const VarType& state)
{
    if(mpState)
    {
        delete mpState;
        mpState = NULL;
    }

    mpState = new VarType(state);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the type of boundary conditions to use.
///
///   \param type Boundary type.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::SetBoundaryType(const Event::Conditions::Type type)
{
    ClearType();
    mpBoundaryType = new Conditions::Type(type);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all values.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::Clear()
{
    if(mpLowerLimit)
    {
        delete mpLowerLimit;
    }
    if(mpUpperLimit)
    {
        delete mpUpperLimit;
    }
    if(mpState)
    {
        delete mpState;
    }

    if(mpLimitDataField)
    {
        delete mpLimitDataField;
    }
    if(mpBoundaryType)
    {
        delete mpBoundaryType;
    }
    mpLowerLimit = mpUpperLimit = mpState = 0;
    mpBoundaryType = 0;
    mpLimitDataField = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the specific value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::ClearType()
{
    if(mpBoundaryType)
    {
        delete mpBoundaryType;
        mpBoundaryType = 0;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the specific value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::ClearLimitDataField()
{
    if(mpLimitDataField)
    {
        delete mpLimitDataField;
    }
    mpLimitDataField = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the specific value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::ClearLowerLimit()
{
    if(mpLowerLimit)
    {
        delete mpLowerLimit;
    }
    mpLowerLimit = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the specific value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::ClearUpperLimit()
{
    if(mpUpperLimit)
    {
        delete mpUpperLimit;
    }
    mpUpperLimit = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the specific value.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Conditions::ClearState()
{
    if(mpState)
    {
        delete mpState;
    }

    mpState = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the data requested, NULL if the data does not exist.
///
////////////////////////////////////////////////////////////////////////////////////
Byte* Event::Conditions::GetLimitDataField() const { return mpLimitDataField; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the data requested, NULL if the data does not exist.
///
////////////////////////////////////////////////////////////////////////////////////
VarType* Event::Conditions::GetLowerLimit() const { return mpLowerLimit; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the data requested, NULL if the data does not exist.
///
////////////////////////////////////////////////////////////////////////////////////
VarType* Event::Conditions::GetUpperLimit() const { return mpUpperLimit; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the data requested, NULL if the data does not exist.
///
////////////////////////////////////////////////////////////////////////////////////
VarType* Event::Conditions::GetState() const { return mpState; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Type of boundary conditions.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions::Type* Event::Conditions::GetBoundaryType() const { return mpBoundaryType; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions& Event::Conditions::operator=(const Event::Conditions& conditions)
{
    if(this != &conditions)
    {
        Clear();
        if(conditions.mpBoundaryType)
        {
            mpBoundaryType = new Conditions::Type(*conditions.mpBoundaryType);
        }
        if(conditions.mpLimitDataField)
        {
            mpLimitDataField = new Byte(*conditions.mpLimitDataField);
        }
        if(conditions.mpLowerLimit)
        {
            mpLowerLimit = new VarType(*conditions.mpLowerLimit);
        }
        if(conditions.mpUpperLimit)
        {
            mpUpperLimit = new VarType(*conditions.mpUpperLimit);
        }
        if(conditions.mpState)
        {
            mpState = new VarType(*conditions.mpState);
        }
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Event() : mEventID(0),
                 mMessageCode(0),
                 mPeriodicRate(0),
                 mTimeStampMs(0),
                 mEventType(Event::OneTime),
                 mpConditions(0),
                 mpQueryMessageBody(0),
                 mpQueryMessage(0)

{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Event(const Event& e) : mEventID(0),
                               mMessageCode(0),
                               mPeriodicRate(0),
                               mTimeStampMs(0),
                               mEventType(Event::OneTime),
                               mpConditions(0),
                               mpQueryMessageBody(0),
                               mpQueryMessage(0)

{
    *this = e;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.  Copies values from the Create Message except for
///   periodic rates and provider/subscriber data.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Event(const CreateEventRequest& command) : mEventID(0),
                                                  mMessageCode(0),
                                                  mPeriodicRate(0),
                                                  mEventType(Event::OneTime),
                                                  mpConditions(0),
                                                  mpQueryMessageBody(0),
                                                  mpQueryMessage(0)

{
    Byte pv = command.GetPresenceVector();

    mEventType = (Event::Type)command.GetEventType();
    mMessageCode = command.GetMessageCode();

    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetBoundaryType((Event::Conditions::Type)command.GetEventBoundary());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLimitDataField(command.GetLimitDataField());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLowerLimit(command.GetLowerLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetUpperLimit(command.GetUpperLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetState(command.GetState());
    }

    if(command.GetQueryMessage())
    {
        SetQueryMessage(command.GetQueryMessage(), true);
    }
    else if(command.GetQueryMessageBody())
    {
        SetQueryMessageBody(*command.GetQueryMessageBody());
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Event::~Event()
{
    Clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies event information to event request message.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::CopyTo(CreateEventRequest& command)
{
    command.ClearMessageBody();
    command.SetEventType((CreateEventRequest::EventType)mEventType);
    command.SetMessageCode(mMessageCode);

    if(mpConditions)
    {
        if(mpConditions->mpBoundaryType)
        {
            command.SetEventBoundary((CreateEventRequest::BoundaryType)*mpConditions->mpBoundaryType);
        }
        if(mpConditions->mpLimitDataField)
        {
            command.SetLimitDataField(*mpConditions->mpLimitDataField);
        }
        if(mpConditions->mpLowerLimit)
        {
            command.SetLowerLimit(*mpConditions->mpLowerLimit);
        }
        if(mpConditions->mpUpperLimit)
        {
            command.SetUpperLimit(*mpConditions->mpUpperLimit);
        }
        if(mpConditions->mpState)
        {
            command.SetState(*mpConditions->mpState);
        }
    }

    if(IsPeriodic())
    {
        command.SetRequestedPeriodicUpdateRate(mPeriodicRate);
    }

    if(mpQueryMessage)
    {
        command.SetQueryMessage(mpQueryMessage);
    }
    else if(mpQueryMessageBody)
    {
        command.SetQueryMessageBody(*mpQueryMessageBody);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies event information to update message.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::CopyTo(UpdateEvent& command)
{
    command.ClearMessageBody();
    command.SetEventType((UpdateEvent::EventType)mEventType);
    command.SetMessageCode(mMessageCode);
    command.SetEventID(mEventID);

    if(mpConditions)
    {
        if(mpConditions->mpBoundaryType)
        {
            command.SetEventBoundary((UpdateEvent::BoundaryType)*mpConditions->mpBoundaryType);
        }
        if(mpConditions->mpLimitDataField)
        {
            command.SetLimitDataField(*mpConditions->mpLimitDataField);
        }
        if(mpConditions->mpLowerLimit)
        {
            command.SetLowerLimit(*mpConditions->mpLowerLimit);
        }
        if(mpConditions->mpUpperLimit)
        {
            command.SetUpperLimit(*mpConditions->mpUpperLimit);
        }
        if(mpConditions->mpState)
        {
            command.SetState(*mpConditions->mpState);
        }
    }

    if(IsPeriodic())
    {
        command.SetRequestedPeriodicUpdateRate(mPeriodicRate);
    }

    if(mpQueryMessage)
    {
        command.SetQueryMessage(mpQueryMessage);
    }
    else if(mpQueryMessageBody)
    {
        command.SetQueryMessageBody(*mpQueryMessageBody);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies values from the Create Event Message except for
///   provider/subscriber and minimum periodic rate information.
///
///   \param command Create Event message to copy Event configuration from.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::CopyFrom(const CreateEventRequest& command)
{
    Byte pv = command.GetPresenceVector();

    Clear();

    mEventType = (Event::Type)command.GetEventType();
    mMessageCode = command.GetMessageCode();

    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetBoundaryType((Event::Conditions::Type)command.GetEventBoundary());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLimitDataField(command.GetLimitDataField());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLowerLimit(command.GetLowerLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetUpperLimit(command.GetUpperLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetState(command.GetState());
    }

    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::RequestedMinimumPeriodicRate))
    {
        mPeriodicRate = command.GetRequestedPeriodicUpdateRate();
    }

    if(command.GetQueryMessage())
    {
        SetQueryMessage(command.GetQueryMessage(), true);
    }
    else if(command.GetQueryMessageBody())
    {
        SetQueryMessageBody(*command.GetQueryMessageBody());
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies values from the Update Event Message except for
///   periodic rates and provider/subscriber data.
///
///   \param command Update Event message to copy Event configuration from.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::CopyFrom(const UpdateEvent& command)
{
    Byte pv = command.GetPresenceVector();

    Clear();

    mEventType = (Event::Type)command.GetEventType();
    mMessageCode = command.GetMessageCode();
    mEventID = command.GetEventID();

    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetBoundaryType((Event::Conditions::Type)command.GetEventBoundary());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLimitDataField(command.GetLimitDataField());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLowerLimit(command.GetLowerLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetUpperLimit(command.GetUpperLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetState(command.GetState());
    }

    if(BitVector::IsBitSet(pv, UpdateEvent::VectorBit::RequestedMinimumPeriodicRate))
    {
        mPeriodicRate = command.GetRequestedPeriodicUpdateRate();
    }

    if(command.GetQueryMessage())
    {
        SetQueryMessage(command.GetQueryMessage(), true);
    }
    else if(command.GetQueryMessageBody())
    {
        SetQueryMessageBody(*command.GetQueryMessageBody());
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Updates event configuration based on Update Event message.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Update(const UpdateEvent& command)
{
    Byte pv = command.GetPresenceVector();

    mEventType = (Event::Type)command.GetEventType();
    mMessageCode = command.GetMessageCode();

    if(mpConditions)
    {
        delete mpConditions;
        mpConditions = NULL;
    }

    if(mpQueryMessageBody)
    {
        delete mpQueryMessageBody;
        mpQueryMessageBody = NULL;
    }
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
        mpQueryMessage = NULL;
    }

    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::EventBoundary))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetBoundaryType((Event::Conditions::Type)command.GetEventBoundary());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LimitDataField))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLimitDataField(command.GetLimitDataField());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::LowerLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetLowerLimit(command.GetLowerLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::UpperLimit))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetUpperLimit(command.GetUpperLimit());
    }
    if(BitVector::IsBitSet(pv, CreateEventRequest::VectorBit::State))
    {
        if(mpConditions == NULL) { mpConditions = new Event::Conditions(); }
        mpConditions->SetState(command.GetState());
    }

    if(command.GetQueryMessage())
    {
        SetQueryMessage(command.GetQueryMessage(), true);
    }
    else if(command.GetQueryMessageBody())
    {
        SetQueryMessageBody(*command.GetQueryMessageBody());
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the data.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::Clear()
{
    if(mpConditions)
    {
        mpConditions->Clear();
        delete mpConditions;
        mpConditions = NULL;
    }
    if(mpQueryMessageBody)
    {
        delete mpQueryMessageBody;
        mpQueryMessageBody = NULL;
    }
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
        mpQueryMessage = NULL;
    }
    mEventID = 0;
    mMessageCode = 0;
    mPeriodicRate = 0;
    mSubscribers.clear();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID value of the event.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetEventID(const Byte id)
{
    mEventID = id;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the event message code.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetMessageCode(const UShort code)
{
    mMessageCode = code;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the event sequence number.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetSequenceNumber(const Byte sn)
{
    mSequenceNumber = sn;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the event update time in milliseconds (UTC).
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetTimeStampMs(const unsigned int ts)
{
    mTimeStampMs = ts;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the type of event.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetEventType(const Event::Type type)
{
    mEventType = type;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the query message body data and message type.
///
///   \param msg Pointer to query message data.
///   \param clone If true a copy of the message is made, otherwise the pointer
///                address is copied.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetQueryMessage(const Message* msg, const bool clone)
{
    if(msg && IsQueryMessage(msg->GetCommandCode()))
    {
        if(mpQueryMessage)
        {
            delete mpQueryMessage;
            mpQueryMessage = NULL;
        }
        if(mpQueryMessageBody)
        {
            mpQueryMessageBody->Clear();
        }
        else
        {
            mpQueryMessageBody = new Stream();
        }
        // Either copy pointer to clone data.
        if(clone)
        {
            mpQueryMessage = msg->Clone();
        }
        else
        {
            mpQueryMessage = (Message*)(msg);
        }
        UShort code = MessageCreator::GetResponseCodes(mpQueryMessage->GetCommandCode());
        if(code != 0)
        {
            mMessageCode = code;
        }
        mpQueryMessage->WriteMessageBody(*mpQueryMessageBody, mpQueryMessage->GetVersion());
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the query message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetQueryMessageBody(const Stream& msg)
{
    // Delete old data.
    if(mpQueryMessage)
    {
        delete mpQueryMessage;
        mpQueryMessage = NULL;
    }
    if(mpQueryMessageBody)
    {
        *mpQueryMessageBody = msg;
    }
    else
    {
        mpQueryMessageBody = new Stream(msg);
    }
    // Try to de-serialize the data.
    mpQueryMessage = MessageCreator::CreateMessage(MessageCreator::GetInformQueryType(mMessageCode));
    if(mpQueryMessage)
    {
        mpQueryMessage->ReadMessageBody(*mpQueryMessageBody, mpQueryMessage->GetVersion());
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the ID of the event provider.
///
///   \param provider Event provider.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetEventProvider(const Address& provider)
{
    if(provider.IsValid() && !provider.IsBroadcast())
    {
        mEventProvider = provider;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the set of components subscribing to the event.
///
///   An alternative to this method is use the GetEventSubscribers method and
///   add directly to the STL set.
///
///   \param subscribers Subscribers of the event.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetEventSubscribers(const Address::Set& subscribers)
{
    mSubscribers = subscribers;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets any boundary conditions associated with the event.
///
///   \param conditions Boundary conditions.
///
////////////////////////////////////////////////////////////////////////////////////
void Event::SetConditions(const Event::Conditions& conditions)
{
    if(mpConditions)
    {
        *mpConditions = conditions;
    }
    else
    {
        mpConditions = new Event::Conditions(conditions);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the event is periodic, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Event::IsPeriodic() const
{
    if(mEventType == Periodic || mEventType == PeriodicWithoutReplacement)
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compares the data in the Create Event message to the data stored
///          within the Event to see if they are a match. The fields comapred
///          are Event Type, Message Code, Event Boundary, Limit Data Field,
///          Lower Limit, Upper Limit, State, and Query Message Body.
///
///   Periodic rate is not checked within this message because Event does not
///   know what the minimum or maximum rates the component can support are.  It
///   is up to the developer to check those values.
///
////////////////////////////////////////////////////////////////////////////////////
bool Event::MatchesCreateRequest(const Jaus::CreateEventRequest& msg) const
{
    bool result = false;

    if(msg.GetMessageCode() == mMessageCode &&
        msg.GetEventType() == (Byte)mEventType)
    {
        Byte createPv = msg.GetPresenceVector();
        Byte eventPv = GetPresenceVector();
        // Compare the presence vector data.
        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::EventBoundary) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::EventBoundary))
        {
            if(mpConditions && mpConditions->mpBoundaryType)
            {
                if(*mpConditions->mpBoundaryType != msg.GetEventBoundary())
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }
        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::LimitDataField) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::LimitDataField))
        {
            if(mpConditions && mpConditions->mpLimitDataField)
            {
                if(*mpConditions->mpLimitDataField != msg.GetLimitDataField())
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }
        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::LowerLimit) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::LowerLimit))
        {
            if(mpConditions && mpConditions->mpLowerLimit)
            {
                if(mpConditions->mpLowerLimit->Equals(msg.GetLowerLimit(), .1) == false)
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }
        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::UpperLimit) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::UpperLimit))
        {
            if(mpConditions && mpConditions->mpUpperLimit)
            {
                if(mpConditions->mpUpperLimit->Equals(msg.GetUpperLimit(), .1) == false)
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }
        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::State) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::State))
        {
            if(mpConditions && mpConditions->mpState)
            {
                if(mpConditions->mpState->Equals(msg.GetState(), .1) == false)
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }

        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::RequestedMinimumPeriodicRate) ==
            IsPeriodic())
        {
            if(mPeriodicRate < msg.GetMinimumPeriodicRate())
            {
                return result;
            }
        }
        else
        {
            return result;
        }

        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::RequestedPeriodicRate) ==
            IsPeriodic())
        {
            if(fabs(mPeriodicRate - msg.GetRequestedPeriodicUpdateRate()) > .1)
            {
                return result;
            }
        }
        else
        {
            return result;
        }

        if(BitVector::IsBitSet(createPv, CreateEventRequest::VectorBit::QueryMessageBody) ==
            BitVector::IsBitSet(eventPv, Event::VectorBit::QueryMessageBody))
        {
            if(mpQueryMessageBody)
            {
                if( mpQueryMessageBody->Length() != msg.GetQueryMessageBody()->Length() ||
                    memcmp(mpQueryMessageBody->Ptr(),
                          msg.GetQueryMessageBody()->Ptr(),
                          mpQueryMessageBody->Length()) != 0)
                {
                    return result;
                }
            }
        }
        else
        {
            return result;
        }

        result = true;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the presence vector associated with the data structure.
///
////////////////////////////////////////////////////////////////////////////////////
Byte Event::GetPresenceVector() const
{
    Byte pv = 0;
    if(mpConditions)
    {
        if(mpConditions->GetBoundaryType())
        {
            BitVector::SetBit(pv, VectorBit::EventBoundary);
        }
        if(mpConditions->mpLimitDataField)
        {
            BitVector::SetBit(pv, VectorBit::LimitDataField);
        }
        if(mpConditions->mpLowerLimit)
        {
            BitVector::SetBit(pv, VectorBit::LowerLimit);
        }
        if(mpConditions->mpUpperLimit)
        {
            BitVector::SetBit(pv, VectorBit::UpperLimit);
        }
        if(mpConditions->mpState)
        {
            BitVector::SetBit(pv, VectorBit::State);
        }
    }
    BitVector::SetBit(pv, VectorBit::EventID);
    if(mpQueryMessageBody || mpQueryMessage)
    {
        BitVector::SetBit(pv, VectorBit::QueryMessageBody);
    }

    return pv;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
Byte Event::GetEventID() const { return mEventID; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Event::GetMessageCode() const { return mMessageCode; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
Byte Event::GetSequenceNumber() const { return mSequenceNumber; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value of the update time for the event.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int Event::GetTimeStampMs() const { return mTimeStampMs; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
double Event::GetPeriodicRate() const { return mPeriodicRate; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Type Event::GetEventType() const { return mEventType; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Conditions* Event::GetConditions() const { return mpConditions; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.  NULL returned if data not present.
///
////////////////////////////////////////////////////////////////////////////////////
Message* Event::GetQueryMessage() const { return mpQueryMessage; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Gets the value.  NULL returned if data not present.
///
////////////////////////////////////////////////////////////////////////////////////
Stream* Event::GetQueryMessageBody() const { return mpQueryMessageBody; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return Address of the component providing the event.
///
////////////////////////////////////////////////////////////////////////////////////
Address Event::GetEventProvider() const { return mEventProvider; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return A pointer to the address of components subscribing to this event.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set* Event::GetEventSubscribers() const { return (Address::Set*)&mSubscribers; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \return A key for sorting the event.
///
////////////////////////////////////////////////////////////////////////////////////
Event::Key Event::GetKey() const
{
    Event::Key key;
    key.mID = mEventID;
    key.mEventType = mEventType;
    key.mMessageCode = mMessageCode;
    key.mSourceID = mEventProvider;
    return key;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Event& Event::operator =(const Event& e)
{
    if(this != &e)
    {
        mEventID = e.mEventID;
        mSequenceNumber = e.mSequenceNumber;
        mMessageCode = e.mMessageCode;
        mPeriodicRate = e.mPeriodicRate;
        mTimeStampMs = e.mTimeStampMs;
        mEventType = e.mEventType;
        mEventProvider = e.mEventProvider;
        mSubscribers = e.mSubscribers;

        if(mpConditions)
        {
            delete mpConditions;
            mpConditions = NULL;
        }
        if(mpQueryMessageBody)
        {
            delete mpQueryMessageBody;
            mpQueryMessageBody = NULL;
        }
        if(mpQueryMessage)
        {
            delete mpQueryMessage;
            mpQueryMessage = NULL;
        }
        if(e.mpConditions)
        {
            mpConditions = new Event::Conditions(*e.mpConditions);
        }
        if(e.mpQueryMessageBody)
        {
            mpQueryMessageBody = e.mpQueryMessageBody->Clone();
        }
        if(e.mpQueryMessage)
        {
            mpQueryMessage = e.mpQueryMessage->Clone();
        }
    }
    return *this;
}


/*  End of File */
