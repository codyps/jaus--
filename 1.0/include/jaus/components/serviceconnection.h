////////////////////////////////////////////////////////////////////////////////////
///
///  \file serviceconnection.h
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
#ifndef __JAUS_SERVICE_CONNECTION_H
#define __JAUS_SERVICE_CONNECTION_H


#include "jaus/messages/address.h"
#include "jaus/messages/stream.h"
#include "jaus/components/cmplibdll.h"
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \class ServiceConnection
    ///  \brief Class describing different types of service connections.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL ServiceConnection
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \class Key
        ///  \brief Class for sorting service connections in an STL map.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL Key
        {
        public:
            Key();
            Key(const Address& id,
                const UShort code,
                const UInt pv);
            ~Key() {}
            bool operator <(const ServiceConnection::Key& key) const;            
            bool operator ==(const ServiceConnection::Key& key) const;            
            Address mProviderID;    ///<  The provider of the service.
            UShort mMessageCode;    ///<  Message type.
            UInt mPresenceVector;   ///<  Presence vector for message.
        private:
            Stream mBuffer;         ///<  Buffer for comparison.
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \class Inform
        ///  \brief Data structure for storing information related to an inform type
        ///         service connection.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL Inform
        {
        public: 
            Inform();
            Inform(const Inform& inform);
            Inform(const UShort code,
                   const UInt presenceVector,
                   const Address& provider,
                   const Address& requestor);
            ~Inform();            
            int RemoveSubscriber(const Address& id);
            int AddPendingSubscriber(const Address& id);
            int RemovePendingSubscriber(const Address& id);
            int RemoveSubscribersFromNode(const Address& id);
            int SuspendSubscriber(const Address& id);
            int ActivateSubscriber(const Address& id);
            bool IsPending() const;
            bool IsPending(const Address& id) const;
            bool IsSubscriber(const Address& id) const;
            bool IsSuspended(const Address& id) const;
            bool IsServiceSuspended() const;
            unsigned int GetNumSubscribers() const;
            Inform& operator=(const Inform& inform);
            double mPeriodicRate;   ///<  Periodic update rate of SC.
            Byte mInstanceID;       ///<  Instance ID of the SC.
            Address mProviderID;    ///<  Source of the data for the SC (service provider).
            Address mRequestorID;   ///<  ID of creator of the SC.
            UShort mMessageCode;    ///<  Message code for the SC.
            UInt   mPresenceVector; ///<  Presence vector for the SC.
            Address::Set mSubscribers;///<  Set of all active subscribers to SC.
            Address::Set mSuspended;  ///<  Set of all suspended subscribers to SC.
            Address::Set mPending;    ///<  Set of subscribers pending confirmation of SC.
        };
        typedef std::map<ServiceConnection::Key, ServiceConnection::Inform> InformMap;

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \class Command
        ///  \brief Data structure for storing information related to an command type
        ///         service connection.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL Command
        {
        public: 
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///  \class Commander
            ///  \brief Data structure to store information about a commanding component.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            class JAUS_CMP_DLL Commander
            {
            public:
                Commander();
                Commander(const Commander& cmd);
                Commander(const Address& id, const Byte authority);
                ~Commander() {}
                bool operator<(const Commander& cmd) const;
                Commander& operator=(const Commander& cmd);

                Address mID;            ///<  ID of a commander.
                Byte mAuthorityLevel;   ///<  Authority level of a commander.
                UInt mCreationTimeMs;   ///<  Time command SC was created in milliseconds (UTC).
            };
            typedef std::set<Commander> CommanderSet; ///<  Type definition for an STL set of commanders.
            Command();
            Command(const Command& cmd);
            Command(const UShort code,
                    const UInt presenceVector,
                    const Address& providerID);
            ~Command();
            int RemoveCommander(const Address& id);
            int RemoveCommandersFromNode(const Address& id);
            int AddPendingCommander(const Commander& cmd);
            int RemovePendingCommander(const Address& id);
            int SuspendCommander(const Address& id);
            int ActivateCommander(const Address& id);
            bool IsPending() const;
            bool IsPending(const Address& id) const;
            bool IsCommander(const Address& id) const;
            bool IsSuspended(const Address& id) const;
            Address GetCommanderID() const { return mCommander.mID; }
            Commander GetNextCommander() const;
            Command& operator=(const Command& cmd);
            unsigned int GetNumCommanders() const;
            double mPeriodicRate;       ///<  Periodic update rate of SC.
            Byte mProviderAuthorityCode;///<  Authority level of the service provider.
            Byte mInstanceID;           ///<  Instance ID of the SC.
            Address mProviderID;        ///<  ID of the component receiving the commands.
            UShort mMessageCode;        ///<  Message code for the SC.
            UInt   mPresenceVector;     ///<  Presence vector for the SC.
            Commander mCommander;       ///<  Currenting command component of SC.
            CommanderSet mActive;       ///<  Active commanders.
            CommanderSet mSuspended;    ///<  Set of all suspended subscribers to SC.
            CommanderSet mPending;      ///<  Set of subscribers pending confirmation of SC.
        };
        typedef std::map<ServiceConnection::Key, ServiceConnection::Command> CommandMap;
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Status
        ///   \brief Status of service connection.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Status
        {
            Active = 0,
            Terminated = 1,
            Suspended = 2,
            NotUpdating = 3
        };
        ServiceConnection();
        ServiceConnection(const ServiceConnection& sc);
        ~ServiceConnection();
        bool operator<(const ServiceConnection& sc) const;
        bool operator==(const ServiceConnection& sc) const;
        ServiceConnection::Key GetKey() const;
        ServiceConnection& operator=(const ServiceConnection& sc);
        Status mPrimaryStatus;  ///<  SC status.       
        Status mSecondaryStatus;///<  Secondary status.
        double mPeriodicRate;   ///<  Periodic update rate of SC.
        Byte mInstanceID;       ///<  Instance ID of the SC.
        UShort mMessageCode;    ///<  Message code for the SC.
        UShort mSequenceNumber; ///<  Sequence number for the SC.
        UInt   mPresenceVector; ///<  Presence vector for the SC.
        Address mRequestorID;   ///<  Component that requested the SC.
        Address mProviderID;    ///<  Component providing the service (SC).
        UInt mUpdateTimeMs;     ///<  Last time the SC was updated by sending or receiving SC message.
        double mUpdateIntervalMs;   ///<  How long to wait inbetween updates.
    };
    typedef std::set<ServiceConnection*> ServiceConnectionSet;                        ///<  STL set of service connections.
    typedef std::map<ServiceConnection::Key, ServiceConnection> ServiceConnectionMap; ///<  STL Service Connection data map.
}


#endif
/* End of File */
