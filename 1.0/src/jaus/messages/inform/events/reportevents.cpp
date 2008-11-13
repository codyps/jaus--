////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportevents.cpp
///  \brief This file contains the message for Report Events.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 March 2008
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
#include "jaus/messages/inform/events/reportevents.h"
#include "jaus/messages/inform/informcodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportEvents::ReportEvents() : Message(JAUS_REPORT_EVENTS)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportEvents::ReportEvents(const ReportEvents& msg) : Message(JAUS_REPORT_EVENTS)
{
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportEvents::~ReportEvents()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the events to report.
///
///   \param events Event list (must be less than or equal to 255 in size).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportEvents::SetEvents(const Event::List& events)
{
    if(events.size() <= 255)
    {
        mEvents = events;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to the event list data so you can add/remove
///          from it.
///
////////////////////////////////////////////////////////////////////////////////////
Event::List* ReportEvents::GetEvents() const { return (Event::List*)&mEvents; }

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
int ReportEvents::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        Event::List::const_iterator events;

        expected += JAUS_BYTE_SIZE;
        written += msg.WriteByte( (Byte)(mEvents.size()) );
        
        for(events = mEvents.begin();
            events != mEvents.end();
            events++)
        {
            Byte pv;
            pv = events->GetPresenceVector();
            expected += JAUS_BYTE_SIZE*2 + JAUS_USHORT_SIZE;
            written += msg.Write(pv);
            written += msg.Write(events->GetMessageCode());
            written += msg.Write((Byte)events->GetEventType()); 

            Event::Conditions* conditions;
            conditions = events->GetConditions();
            if(conditions)
            {
                if(BitVector::IsBitSet(pv, VectorBit::EventBoundary))
                {
                    expected += JAUS_BYTE_SIZE;
                    written += msg.Write((Byte)(*conditions->GetBoundaryType()));
                }
                if(BitVector::IsBitSet(pv, VectorBit::LimitDataField))
                {
                    expected += JAUS_BYTE_SIZE;
                    written += msg.Write( *conditions->GetLimitDataField() );
                }
                if(BitVector::IsBitSet(pv, VectorBit::LowerLimit))
                {
                    expected += JAUS_BYTE_SIZE + conditions->GetLowerLimit()->Size();
                    written += msg.Write(*conditions->GetLowerLimit());
                }
                if(BitVector::IsBitSet(pv, VectorBit::UpperLimit))
                {
                    expected += JAUS_BYTE_SIZE + conditions->GetUpperLimit()->Size();
                    written += msg.Write(*conditions->GetUpperLimit());
                }
                if(BitVector::IsBitSet(pv, VectorBit::State))
                {
                    expected += JAUS_BYTE_SIZE + conditions->GetState()->Size();
                    written += msg.Write(*conditions->GetState());
                }
            }
            expected += JAUS_BYTE_SIZE;
            written += msg.Write(events->GetEventID());

            if(BitVector::IsBitSet(pv, VectorBit::QueryMessageBody))
            {
                expected += JAUS_UINT_SIZE + events->GetQueryMessageBody()->Length();
                written += msg.Write((UInt)(events->GetQueryMessageBody()->Length()));
                written += msg.Write(*events->GetQueryMessageBody());
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
int ReportEvents::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        Event e;
        Byte numEvents = 0;
        Event::Conditions conditions;

        mEvents.clear();

        expected += JAUS_BYTE_SIZE;
        read += msg.Read( numEvents );
        
        for(Byte i = 0; i < numEvents; i++)
        {
            Byte pv = 0;
            Byte eventType;
            Byte temp;
            VarType varType;
            UShort msgCode;
            bool setConditions = false;

            // Clear previous event data
            e.Clear();
            conditions.Clear();

            expected += JAUS_BYTE_SIZE*2 + JAUS_USHORT_SIZE;
            read += msg.Read(pv);
            read += msg.Read(msgCode);
            read += msg.Read(eventType); 
            
            e.SetMessageCode(msgCode);
            e.SetEventType((Event::Type)(eventType));

            if(BitVector::IsBitSet(pv, VectorBit::EventBoundary))
            {                
                expected += JAUS_BYTE_SIZE;
                read += msg.Read(temp);
                conditions.SetBoundaryType((Event::Conditions::Type)(temp));
                setConditions = true;
                
            }
            if(BitVector::IsBitSet(pv, VectorBit::LimitDataField))
            {
                read += msg.Read( temp );
                expected += JAUS_BYTE_SIZE;
                conditions.SetLimitDataField(temp);
                setConditions = true;
            }
            if(BitVector::IsBitSet(pv, VectorBit::LowerLimit))
            {
                read += msg.Read(varType);
                expected += JAUS_BYTE_SIZE + varType.Size();
                conditions.SetLowerLimit(varType);
                setConditions = true;                
            }
            if(BitVector::IsBitSet(pv, VectorBit::UpperLimit))
            {
                read += msg.Read(varType);
                expected += JAUS_BYTE_SIZE + varType.Size();
                conditions.SetUpperLimit(varType);
                setConditions = true;
            }
            if(BitVector::IsBitSet(pv, VectorBit::State))
            {
                read += msg.Read(varType);
                expected += JAUS_BYTE_SIZE + varType.Size();
                conditions.SetState(varType);
                setConditions = true;
            }
            if(setConditions)
            {
                e.SetConditions(conditions);
            }
            if(BitVector::IsBitSet(pv, VectorBit::EventID))
            {
                expected += JAUS_BYTE_SIZE;
                read += msg.Read(temp);
                e.SetEventID(temp);
            }

            if(BitVector::IsBitSet(pv, VectorBit::QueryMessageBody))
            {
                Stream stream;
                UInt len = 0;
                expected += JAUS_UINT_SIZE;
                read += msg.Read(len);
                if(len > 0)
                {
                    expected += len;
                    read += msg.Read(stream, len);
                    e.SetQueryMessageBody(stream);
                }
            }

            mEvents.push_back(e);
        }

        if( expected == read )
        {
            return expected;
        }
        else
        {
            SetJausError(ErrorCodes::ReadFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs a test case for the message.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportEvents::RunTestCase() const
{
    ReportEvents msg1;
    ReportEvents msg2;
    Event::List* eventList;
    Stream stream;

    Event event1, event2;
    Event::Conditions conditions;

    event1.SetEventID(3);
    event1.SetMessageCode(JAUS_REPORT_TIME);
    event1.SetEventType(Event::EveryChange);
    conditions.SetBoundaryType(Event::Conditions::InsideInclusive);
    conditions.SetUpperLimit(VarType(50000));
    event1.SetConditions(conditions);

    event2.SetEventID(3);
    event2.SetEventType(Event::EveryChange);
    event2.SetMessageCode(JAUS_REPORT_GLOBAL_POSE);

    eventList = msg1.GetEvents();
    eventList->push_back(event1);
    eventList->push_back(event2);

    if(msg1.WriteMessageBody(stream, JAUS_DEFAULT_VERSION) &&
        msg2.ReadMessageBody(stream, JAUS_DEFAULT_VERSION))
    {
        eventList = msg2.GetEvents();
        if( eventList->size() == 2 &&
            (*eventList)[0].GetPresenceVector() == event1.GetPresenceVector() &&
            (*eventList)[0].GetEventID() == event1.GetEventID() &&
            (*eventList)[0].GetMessageCode() == event1.GetMessageCode() &&
            (*eventList)[0].GetConditions() && event1.GetConditions() &&
            *(*eventList)[0].GetConditions()->GetBoundaryType() == *event1.GetConditions()->GetBoundaryType() &&
            *(*eventList)[0].GetConditions()->GetUpperLimit() == *event1.GetConditions()->GetUpperLimit() &&
            (*eventList)[1].GetPresenceVector() == event2.GetPresenceVector() &&
            (*eventList)[1].GetEventID() == event2.GetEventID() &&
            (*eventList)[1].GetMessageCode() == event2.GetMessageCode())
        {
            return JAUS_OK;
        }
    }

    

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportEvents& ReportEvents::operator =(const ReportEvents& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mEvents = msg.mEvents;
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
UShort ReportEvents::GetPresenceVectorSize(const UShort version) const { return 0; }
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
UInt ReportEvents::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
