////////////////////////////////////////////////////////////////////////////////////
///
///  \file configuration.h
///  \brief Class for storing system, subsystem, node, component configuration
///         data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 1 March 2008
///  <br>Last Modified: 1 March 2008
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
#ifndef __JAUS_CONFIGURATION_INFORMATION_CLASS__H
#define __JAUS_CONFIGURATION_INFORMATION_CLASS__H

#include "jaus/messages/address.h"
#include "jaus/messages/common/configuration/service.h"
#include "jaus/messages/common/configuration/identification.h"
#include <vector>
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Configuration
    ///   \brief System configuration data.
    ///   
    ///   Configuration contains classes for describing component, node, and subsystem
    ///   configurations.  A Configuration structure also includes information about
    ///   the entire system.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Configuration
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Component
        ///   \brief Component ID information.
        ///   
        ///   Component is a simple structure for storing a components ID and 
        ///   instance number within a nodes configuration.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Component
        {
        public:
            typedef std::set<Component>     Set;       ///< STL set of component configurations.
            typedef std::vector<Component>  List;   ///< STL vector of component configurations.     
            Component();
            Component(const Byte cid, const Byte iid);
            Component(const Component& c);
            virtual ~Component();
            bool operator <(const Component& c) const;
            Component& operator=(const Component& c);
            Byte mID;                   ///<  ID of the component (service type, etc.)
            Byte mInstance;             ///<  Instance ID of component.
            Service::Set* mpServices;   ///<  Pointer to services for the component (optional).
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Node
        ///   \brief Node configuration data.
        ///   
        ///   Node is a simple structure for storing the ID of a node, and the
        ///   components connected to it.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Node
        {
        public:
            typedef std::map<Byte,Node> Map;///<  STL set of node configurations.
            typedef std::vector<Node>   List; ///<  STL vector of node configurations.        
            Node();
            Node(const Byte id);
            Node(const Node& node);
            virtual ~Node();
            void AddComponent(const Configuration::Component& c);
            void AddComponent(const Byte cid, const Byte iid);
            void RemoveComponent(const Configuration::Component& c);
            void RemoveComponent(const Byte cid, const Byte iid);
            Component* GetComponent(const Byte cide, const Byte iid) const;
            Configuration::Component::List GetComponentsOfType(const Byte cid) const;
            Node& operator=(const Node& ncfg);
            Component::Set mComponents;     ///<  Components on the node.
            Byte mNodeID;                   ///<  Node ID.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Subsystem
        ///   \brief Subsystem configuration data.
        ///   
        ///   Subsystem is a structure for storing subystem configuration data including
        ///   the subsystem number, and a set of all nodes within it.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Subsystem
        {
        public:
            typedef std::vector<Subsystem>      List;  ///< STL vector of subsystem configurations.
            typedef std::map<Byte,Subsystem>    Map;   ///< STL map of subsystem configurations.
        
            Subsystem();
            Subsystem(const Byte id);
            Subsystem(const Subsystem& ss);
            virtual ~Subsystem();
            void AddComponent(const Address& id);
            void AddComponent(const Byte nid, const Byte cid, const Byte iid);
            void AddComponent(const Byte nid, const Configuration::Component& c);
            void RemoveComponent(const Byte nid, const Byte cid, const Byte iid);
            void RemoveComponent(const Byte nid, const Configuration::Component& c);
            void Print() const;
            void Clear();
            bool HaveComponent(const Address& id) const;
            Component* GetComponent(const Address& id) const;
            Address::List GetAddresses() const;
            Address::List GetComponentsOfType(const UShort cid) const;
            Subsystem& operator=(const Subsystem& ss);
            Byte mSubsystemID;  ///<  Subsystem ID number.
            Node::Map mNodes;   ///<  Nodes within subsystem (configurations).
        };
        typedef std::map<Byte, Identification> IdentificationMap;   ///<  STL map of identifications.
        // Constructor.
        Configuration();
        // Copy constructor.
        Configuration(const Configuration& cfg);
        // Destructor.
        virtual ~Configuration();
        // Adds and id to the configuration.
        void AddComponent(const Address& id);
        // Adds identification for a subsystem
        void AddSubsystemIdentification(const Byte ss,
                                        const Identification& id);
        // Add the configuration data.
        void AddConfiguration(const Configuration& config);
        // Adds subsystem configuration.
        void AddSubsystemConfiguration(const Configuration::Subsystem& sconfig);
        // Adds node configuration.
        void AddNodeConfiguration(const Byte ss, const Configuration::Node& nconfig);
        // Removes a id from the configuration
        void RemoveComponent(const Address& id);
        // Removes identification for a subsystem
        void RemoveSubsystemIdentification(const Byte ss);
        // Prints Configuration to the console.
        void Print() const;
        // Clears configuration.
        void Clear() { mSubsystems.clear(); mIdentifications.clear(); }
        // Checks if subsystem information is available.
        bool HaveSubsystem(const Byte ss) const;
        // Checks if subsystem identification is available.
        bool HaveSubsystemIdentification(const Byte ss) const;
        // Returns pointer to subsystem configuration, NULL if not available.
        Configuration::Subsystem* GetSubsystem(const Byte ss) const;
        // Returns pointer to node configuration, NULL if not available.
        Configuration::Node* GetNode(const Byte ss, const Byte nid) const;
        // Returns pointer to subsystem identification, NULL if not available.
        Identification* GetSubsystemIdentification(const Byte ss) const;
        // Gets the address of all components of a specific type.
        Address::List GetComponentsOfType(const Byte cid) const;
        // Gets the address of all nodes.
        Address::List GetNodes() const;
        // Sets equal to another.
        Configuration& operator=(const Configuration& cfg);
        Subsystem::Map mSubsystems;            ///<  All subsystems within the system.
        IdentificationMap mIdentifications;    ///<  Map of subsystem identification data.
    };
}

#endif
/*  End of File */
