////////////////////////////////////////////////////////////////////////////////////
///
///  \file event.h
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
#ifndef __JAUS_EVENT__H
#define __JAUS_EVENT__H

#include <map>
#include "jaus/messages/command/events/createeventrequest.h"
#include "jaus/messages/command/events/updateevent.h"
namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Event
    ///   \brief Data structure for describing events.  Used by the Report Events
    ///          message.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Event
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Type
        ///   \brief Type of event.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Type
        {
            Periodic = 0,
            EveryChange,
            FirstChange,
            FirstChangeBoundaries,
            PeriodicWithoutReplacement,
            OneTime
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Key
        ///   \brief Key structor for storing/organizing groups of event structures.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Key
        {
        public:
            Key();
            Key(const Key& key);
            ~Key();
            bool operator<(const Key& key) const;
            Key& operator=(const Key& key);
            Byte mID;                   ///< ID (unique).
            Event::Type mEventType;     ///< Type of event.
            UShort mMessageCode;        ///< Message code for event.
            Address mSourceID;          ///< Source/Provider of the event.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this data.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                EventBoundary  = 0x01,
                LimitDataField = 0x02,
                LowerLimit     = 0x04,
                UpperLimit     = 0x08,
                State          = 0x10,
                EventID        = 0x20,
                QueryMessageBody    = 0x40
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this data.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                EventBoundary = 0,
                LimitDataField,
                LowerLimit,
                UpperLimit,
                State,
                EventID,
                QueryMessageBody
            };
        };        
        ////////////////////////////////////////////////////////////////////////////////////
        /// 
        ///  \class Limits
        ///  \brief Class defining the limits of values used in class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxPeriodicRate;    ///<  Maximum value for periodic rate 1092.
            const static double MinPeriodicRate;    ///<  Minimum value for periodic rate 0.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Conditions
        ///   \brief Data structure for storing the conditions for when the event should
        ///          be generated.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class Conditions
        {
            friend class Event;
        public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \enum Type
            ///   \brief Different boundary type for event conditions.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            enum Type
            {
                Equal = 0,
                NotEqual,
                InsideInclusive,
                InsideExclusive,
                OutsideInclusive,
                OutsideExclusive,
                GreaterThanOrEqual,
                GreaterThan,
                LessThanOrEqual,
                LessThan                
            };
            Conditions();
            Conditions(const Event::Conditions& c);
            ~Conditions();

            void SetLimitDataField(const Byte df);
            void SetLowerLimit(const VarType& limit);
            void SetUpperLimit(const VarType& limit);
            void SetState(const VarType& state);
            void SetBoundaryType(const Event::Conditions::Type type);

            void Clear();
            void ClearType();
            void ClearLimitDataField();
            void ClearUpperLimit();
            void ClearLowerLimit();
            void ClearState();
            
            Byte* GetLimitDataField() const;
            VarType* GetLowerLimit() const;
            VarType* GetUpperLimit() const;
            VarType* GetState() const;
            Event::Conditions::Type* GetBoundaryType() const;

            Conditions& operator=(const Conditions& conditions);
        protected:
            Conditions::Type* mpBoundaryType;   ///< Type of boundary conditions.
            Byte* mpLimitDataField;             ///< Limit data field number to base conditions on.
            VarType* mpLowerLimit;              ///< Lower limit condition.
            VarType* mpUpperLimit;              ///< Upper limit condition.
            VarType* mpState;                   ///< Value for Equal or Not Equal condition.
        };
        typedef std::vector<Event> List;          ///<  STL vector of events.
        typedef std::set<Event*> Set;             ///<  STL set of event pointers.
        typedef std::map<Event::Key, Event*> Map; ///<  STL map of events.
        Event();
        Event(const Event& e);
        Event(const CreateEventRequest& command);
        ~Event();

        void CopyTo(CreateEventRequest& command);
        void CopyTo(UpdateEvent& command);
        void CopyFrom(const CreateEventRequest& command);
        void CopyFrom(const UpdateEvent& command);
        void Update(const UpdateEvent& command);
        void SetEventID(const Byte id);
        void SetMessageCode(const UShort code);
        void SetSequenceNumber(const Byte sn);
        void SetTimeStampMs(const unsigned int ts);
        void SetEventType(const Event::Type type);
        void SetQueryMessage(const Message* msg, const bool clone = true);
        void SetQueryMessageBody(const Stream& msg);
        void SetConditions(const Conditions& conditions);
        void SetPeriodicRate(const double rate);
        void SetEventProvider(const Address& id);
        void SetEventSubscribers(const Address::Set& subscribers);
        void Clear();
       
        bool IsPeriodic() const;
        bool MatchesCreateRequest(const CreateEventRequest& msg) const;
        Byte GetPresenceVector() const;    
        Byte GetEventID() const;
        Byte GetSequenceNumber() const;
        UShort GetMessageCode() const;
        unsigned int GetTimeStampMs() const;
        double GetPeriodicRate() const;
        Event::Type GetEventType() const;
        Conditions* GetConditions() const;
        Message* GetQueryMessage() const;
        Stream*  GetQueryMessageBody() const;  
        Address GetEventProvider() const;
        Address::Set* GetEventSubscribers() const;
        Event::Key GetKey() const;
        Event& operator=(const Event& e);
    protected:
        Byte mEventID;              ///< Event ID (unique identifier).
        Byte mSequenceNumber;       ///< Event sequence number (how many times it has generated).
        UShort mMessageCode;        ///< Message code for event.
        double mPeriodicRate;       ///< Periodic rate if periodic event.
        unsigned int mTimeStampMs;  ///< Event update time stamp in milliseconds.
        Event::Type mEventType;     ///< Type of event.
        Conditions* mpConditions;   ///< Event conditions.
        Stream* mpQueryMessageBody; ///< Query message body/data.
        Message* mpQueryMessage;    ///< Query message structure.
        Address mEventProvider;     ///< Provider of the event.
        Address::Set mSubscribers;  ///< Event subscribers.
    };    
}


#endif 
/*  End of File */
