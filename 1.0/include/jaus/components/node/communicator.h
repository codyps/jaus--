////////////////////////////////////////////////////////////////////////////////////
///
///  \file communicator.h
///  \brief The Communicator component handles all communication to other
///  subsystems using custom data links.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 5 August 2008
///  <br>Last Modified: 13 August 2008
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
#ifndef __JAUS_COMMUNICATOR_COMPONENT__H
#define __JAUS_COMMUNICATOR_COMPONENT__H

#include "jaus/components/component.h"
#include "jaus/components/streamcallback.h"
#include "jaus/messages/common/configuration/configuration.h"
#include <cxutils/udpclient.h>
#include <cxutils/udpserver.h>
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief The Communicator component is used to send and receive messages 
    ///   between other subsystems.  All JAUS message data that transfered between
    ///   subsystems travels through the Communicator similar to how all data within
    ///   a Subsystem travels through Node Managers.
    ///
    ///   The Communicator component class is designed developers can add new/custom
    ///   Data Link connections to other subsystems to there platform.  By
    ///   Inheriting from the Communicator::DataLink class, new types of data links
    ///   that even add encryption can be easily developed and incoporated into this
    ///   library.  For a simple example of a DataLink which is used by default
    ///   is the DefaultDataLink which uses UDP transport protocal for communication
    ///   between subsystems.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL Communicator : public Component
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief The DataLink class is used to define to Data Links used by the
        ///   Communicator for communication between subsystems.
        ///
        ///   The Communicator component class is designed developers can add new/custom
        ///   Data Link connections to other subsystems to there platform.  By
        ///   Inheriting from the Communicator::DataLink class, new types of data links
        ///   that even add encryption can be easily developed and incoporated into this
        ///   library.  For a simple example of a DataLink which is used by default
        ///   is the DefaultDataLink which uses UDP transport protocal for communication
        ///   between subsystems.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL DataLink
        {
            friend class Communicator;
        public:
            typedef std::map<Byte, DataLink*> Map; ///<  STL Map of Data Links.
            /*! Enumeration of possible data link states. */
            enum State
            {
                Off = 0, // Disconnect link
                On,      // Send and receive messages.
                Standby  // Standby maintain link, do not transmit
            };
            // Constructor.
            DataLink();
            // Destructor.
            virtual ~DataLink();
            // Sets the ID of the data link.
            void SetID(const Byte id);  
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \brief The SetState method is used to turn on/off or put a Data Link into
            ///   Standby operation.
            ///
            ///   \param state Desired state of the data link.
            ///
            ///   \return True if successfully transitioned to the desired state, false on
            ///           failure.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            virtual bool SetState(const State state) = 0;
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \brief The Transmit method sends a serialized JAUS message over the
            ///   data link to other subsystems.  Overload this method to implement how
            ///   data is sent over the link and to add any additional encrypition or
            ///   transport layer data to the JAUS message before transmission.
            ///
            ///   \param data Serialzed JAUS message data to transmit.
            ///
            ///   \return True if the message was sent over the data link, false on failure.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            virtual bool Transmit(const Stream& data) = 0;
            // Method called when a message is received over your data link.
            virtual bool ProcessReceivedMessage(const Stream& data);
            // Gets the ID of the data link.
            Byte GetID() const;
            // Gets the ID of the Subsystem of the Communicator.
            Byte GetSubsystemID() const;
            // Gets the state of the data link.
            virtual State GetState() const = 0;
        private:
            CxUtils::Mutex mCommunicatorMutex; ///<  Mutex for communicator callback interface.
            Byte mID;                          ///<  Data link ID.
            Communicator* mpCommunicator;      ///<  Pointer to communicator.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief The DefaultDataLink is the default communication link used by
        ///   the Communicator class and enables communication over UDP to other 
        ///   subsystems.  It does not use any encryption.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_CMP_DLL DefaultDataLink : public DataLink
        {
        public:
            DefaultDataLink();
            ~DefaultDataLink();
            virtual bool SetState(const State state);
            virtual bool Transmit(const Stream& data);
            virtual State GetState() const { return mLinkState; }
            void ToggleBroadcast(const bool on);
            void SetMulticastAddress(const std::string& multicast = "224.1.0.1",
                                     const unsigned char ttl = 1);
            std::string GetMulticastAddress() const { return mMulticastAddress; }
            bool AddSubsystem(const Byte subsystemID, const std::string& host);
            Byte GetMulticastTTL() const { return mMulticastTTL; }
        protected:
            static void RecvThread(void *arg);      ///<  Method run in thread for receiving UDP traffic.
            State mLinkState;                       ///<  State of the link.
            bool mBroadcastFlag;                    ///<  If true, UDP broadcast is used over Multicast.
            CxUtils::Thread mRecvThread;            ///<  Thread for receiving UDP data.
            CxUtils::Mutex mConnectionsMutex;       ///<  Thread protection for known subsystem connections.
            CxUtils::UdpClient mBroadcastClient;    ///<  UDP client for sending UDP broadcast.
            CxUtils::UdpClient mMulticastClient;    ///<  UDP client for sending UDP multicast.
            CxUtils::UdpServer mServer;             ///<  UDP server for receiving UDP messages.
            std::string mMulticastAddress;          ///<  Multicast address for transmission.
            unsigned char mMulticastTTL;            ///<  Time To Live (TTL) for multicast transmissions.
            std::set<Byte> mFixedConnections;       ///<  Unicast connections that can't be dynamically removed.
            std::map<Byte, CxUtils::UdpClient> mSubsystems;          ///<  Known subsystems and connections to them for sending unicast traffic.
            std::map<Byte, unsigned int> mSubsystemHeartbeatTimesMs; ///<  Time at which a heartbeat message was received from a subsystem.
            Stream mTransportStream;                                 ///<  Stream used for transmitting data over UDP.
            Stream mRecvStream;                                      ///<  Buffer for receiving data on.
        };
        Communicator();
        virtual ~Communicator();
        virtual int SetupService();
        virtual int AddDataLink(DataLink* newLink);
        virtual int DeleteDataLink(const Byte dataLinkID);
        virtual int SetDataLinkSelect(const Byte dataLinkID);
        virtual int SetDataLinkState(const DataLink::State state);
        virtual int SetSelectedDataLinkState(const Byte dataLinkID, const DataLink::State state);
        virtual int Transmit(const Stream& msg) const;
        virtual bool IsDefaultDataLinkSelected(const std::string& multicast = "224.1.0.1") const;
        virtual int ProcessCommandMessage(const Message* msg, const Byte commandAuthority);
        virtual int ProcessQueryMessage(const Message* msg);
        // Method called when a message is received over your data link.
        bool ProcessDataLinkMessage(const Stream& data);
        bool SetNodeConnectionHandlerCallback(StreamCallback* handler);
        void LockDataLinks() const;
        DataLink::Map* GetDataLinks();
        void UnlockDataLinks() const;
        std::vector<Byte> GetDataLinkList() const;
        Configuration::Subsystem GetSubsystemConfiguration() const;
        Identification GetSubsystemIdentification() const;
        int SetSubsystemIdentification(const Identification& identity);
        int SetSubsystemConfiguration(const Configuration::Subsystem& config);
    protected:
        DataLink::Map mDataLinks;       ///<  Map of Data Links.
        CxUtils::Mutex mDataLinksMutex; ///<  Mutex for thread protection of data links.
        Byte mSelectedDataLink;         ///<  The current selected data link.
        CxUtils::Mutex mConfigurationMutex;               ///< Thread protection of configuration data.
        Identification mSubsystemIdentification;          ///< Subsystem identification data.
        Configuration::Subsystem mSubsystemConfiguration; ///< Subsystem configuration.
    private:
        CxUtils::Mutex mNodeConnectionHandlerMutex; ///<  Mutex for thread protection of node connection handler.
        StreamCallback* mpNodeConnectionHandler;    ///<  Routes all messages through the Node Manager.
    };
}

#endif
/*  End of File */
