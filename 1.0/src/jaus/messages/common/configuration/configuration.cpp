////////////////////////////////////////////////////////////////////////////////////
///
///  \file configuration.cpp
///  \brief Class for storing system, subsystem, node, component configuration
///         data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 1 March 2008
///  <br>Last Modified: 5 March 2008
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
#include "jaus/messages/common/configuration/configuration.h"
#include <iostream>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets default values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component::Component() : mID(0), mInstance(0), mpServices(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. 
///
///   \param cid Component ID.
///   \param iid Component Instance.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component::Component(const Byte cid,
                                    const Byte iid) : mID(cid), 
                                                      mInstance(iid),
                                                      mpServices(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component::Component(const Component& c) : mID(c.mID), 
                                                          mInstance(c.mInstance),
                                                          mpServices(NULL)
{
    if(c.mpServices)
    {
        mpServices = new Service::Set();
        *mpServices = *c.mpServices;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component::~Component()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return If ID and instance are lower than those in argument, true, otherwise
///          false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Configuration::Component::operator <(const Component& c) const
{
    if( ((((UShort)(mID)) << 8) | mInstance) <
        ((((UShort)(c.mID)) << 8) | c.mInstance) )
    {
        return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component& Configuration::Component::operator =(const Component& c)
{
    if(this != &c)
    {   
        mID = c.mID;
        mInstance = c.mInstance;
        if(mpServices)
        {
            delete mpServices;
            mpServices = NULL;
        }
        if(c.mpServices)
        {
            mpServices = new Service::Set();
            *mpServices = *c.mpServices;
        }
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets default values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node::Node() : mNodeID(0)
{
    // All nodes have a node manager.
    mComponents.insert(Configuration::Component(1, 1));
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets default values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node::Node(const Byte id) : mNodeID(id)
{
    // All nodes have a node manager.
    mComponents.insert(Configuration::Component(1, 1));
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node::Node(const Node& n)
{
    mComponents = n.mComponents;
    mNodeID = n.mNodeID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node::~Node()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a component from the node configuration.
///
///   \param c Component to remove.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Node::AddComponent(const Configuration::Component& c)
{
    Configuration::Component::Set::iterator citr;
    if(c.mID == 0 || c.mInstance == 0 || c.mID == 255 || c.mInstance == 255)
    {
        return;
    }
    citr = mComponents.find(c);
    // Only if not already in there do an insert (avoid overwriting service data).
    if(citr == mComponents.end())
    {
        mComponents.insert(c);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a component from the node configuration.
///
///   \param cid Component ID.
///   \param iid Component instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Node::AddComponent(const Byte cid, const Byte iid)
{
    AddComponent(Configuration::Component(cid, iid));
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a component from the node configuration.
///
///   \param c Component to remove.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Node::RemoveComponent(const Configuration::Component& c)
{
    Configuration::Component::Set::iterator citr;
    citr = mComponents.find(c);
    if(citr != mComponents.end())
    {
        mComponents.erase(citr);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a component from the node configuration.
///
///   \param cid Component ID.
///   \param iid Component instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Node::RemoveComponent(const Byte cid, const Byte iid)
{
    RemoveComponent(Configuration::Component(cid, iid));
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to a component on the node.
///
///   \param cid Component service type.
///   \param iid Component instance type.
///
///   \return Pointer to component if on node, otherwise NULL.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component* Configuration::Node::GetComponent(const Byte cid, const Byte iid) const
{
    Configuration::Component::Set::const_iterator itr;
    for(itr = mComponents.begin();
        itr != mComponents.end();
        itr++)
    {
        if( (*itr).mID == cid && (*itr).mInstance == iid)
        {
            return (Configuration::Component*)&(*itr);
        }
    }

    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list of all components with a specific ID number.
///
///   \param cid Component service type.
///
///   \return List of all components on the node with the matching service ID 
///           number.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component::List Configuration::Node::GetComponentsOfType(const Byte cid) const
{
    Configuration::Component::List list;
    Configuration::Component::Set::const_iterator itr;
    for(itr = mComponents.begin();
        itr != mComponents.end();
        itr++)
    {
        if( (*itr).mID == cid )
        {
            list.push_back( (*itr) );
        }
    }

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node& Configuration::Node::operator =(const Node& n)
{
    if(this != &n)
    {
        mComponents = n.mComponents;
        mNodeID = n.mNodeID;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets default values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem::Subsystem() : mSubsystemID(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets default values to 0.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem::Subsystem(const Byte id) : mSubsystemID(id)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem::Subsystem(const Subsystem& ss)
{
    mSubsystemID = ss.mSubsystemID;
    mNodes = ss.mNodes;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem::~Subsystem()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a component to the subsystem configuration.  This method
///          will also set the subsystem ID to match that of the ID added.
///
///   \param id The ID of the component to add.  
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::AddComponent(const Address& id)
{
    Configuration::Node::Map::iterator node;

    if( id.IsValid() == false || id.IsBroadcast())
    {
        return;
    }

    mSubsystemID = id.mSubsystem;
    node = mNodes.find(id.mNode);
    if( node == mNodes.end() )
    {
        mNodes[id.mNode].mNodeID = id.mNode;
        mNodes[id.mNode].AddComponent(id.mComponent, id.mInstance);
    }
    else
    {
        node->second.AddComponent(id.mComponent, id.mInstance);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a component to the subsystem configuration.
///
///   \param nid Node ID.
///   \param cid Component ID.
///   \param iid Instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::AddComponent(const Byte nid,
                                            const Byte cid,
                                            const Byte iid)
{
    Configuration::Node::Map::iterator node;

    if( nid == 0 || nid == 255 ||
        cid == 0 || cid == 255 ||
        iid == 0 || iid == 255)
    {
        return;
    }

    node = mNodes.find(nid);
    if( node == mNodes.end() )
    {
        mNodes[nid].mNodeID = nid;
        mNodes[nid].AddComponent(cid, iid);
    }
    else
    {
        node->second.AddComponent(cid, iid);
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds a component to the subsystem configuration.
///
///   \param nid Node ID.
///   \param c Component configuration.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::AddComponent(const Byte nid,
                                            const Configuration::Component& c)
{
    Configuration::Node::Map::iterator node;

    if( nid == 0 || nid == 255 ||
        c.mID == 0 || c.mID == 255 ||
        c.mInstance == 0 || c.mInstance == 255)
    {
        return;
    }

    node = mNodes.find(nid);
    if( node == mNodes.end() )
    {
        mNodes[nid].mNodeID = nid;
        mNodes[nid].AddComponent(c);
    }
    else
    {
        node->second.AddComponent(c);
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a node from a subsystem.
///
///   \param nid Node ID.
///   \param cid Component ID.
///   \param iid Instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::RemoveComponent(const Byte nid,
                                               const Byte cid,
                                               const Byte iid)
{
    Node::Map::iterator n;
    n = mNodes.find(nid);
    if(n != mNodes.end())
    {
        // If node ID, remove node.
        if( cid == 1 && iid == 1 )
        {
            //n = mNodes.erase(n);
            mNodes.erase(n);
        }
        else
        {
            n->second.RemoveComponent(cid, iid);
            // If there is nothing on the node, delete it.
            if(n->second.mComponents.size() == 0)
            {
                //n = mNodes.erase(n);
                mNodes.erase(n);
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a node from a subsystem.
///
///   \param nid Node ID.
///   \param c Component configuration data (id and instance).
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::RemoveComponent(const Byte nid,
                                               const Configuration::Component& c)
{
    Node::Map::iterator n;
    n = mNodes.find(nid);
    if(n != mNodes.end())
    {
        // If node ID, remove node.
        if( c.mID == 1 && c.mInstance == 1 )
        {
            //n = mNodes.erase(n);
            mNodes.erase(n);
        }
        else
        {
            n->second.RemoveComponent(c);
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints subsystem configuration data to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::Print() const
{
    std::cout << "========================================\n";
    std::cout << "SubSystem Configuration - " << (int)mSubsystemID << ":\n";
    std::cout << "========================================\n";
    
    Configuration::Node::Map::const_iterator node;
    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        std::cout << "    Node: " << (int)node->first << std::endl;
        Configuration::Component::Set::const_iterator comp;
        for(comp = node->second.mComponents.begin();
            comp != node->second.mComponents.end();
            comp++)
        {
            std::cout << "        [" << (int)comp->mID << "," << (int)comp->mInstance << "]\n";
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all data.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Subsystem::Clear()
{
    mNodes.clear();
    mSubsystemID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks if the component is part of the subsystem configuration.
///
///   \return True if component is within the subsystem configuration, otherwise
///           false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Configuration::Subsystem::HaveComponent(const Address& id) const
{
    Node::Map::const_iterator node;
    if(id.mSubsystem == mSubsystemID)
    {
        node = mNodes.find(id.mNode);
        if(node != mNodes.end())
        {
            if(node->second.mComponents.find(Component(id.mComponent, id.mInstance)) != 
                node->second.mComponents.end())
            {
                return true;
            }
        }
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to a component on the subsystem.
///
///   \param id The component ID to get a pointer to.
///
///   \return Pointer to component if within configuration, otherwise NULL.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Component* Configuration::Subsystem::GetComponent(const Address& id) const
{
    Node::Map::const_iterator node;
    if(id.mSubsystem == mSubsystemID)
    {
        node = mNodes.find(id.mNode);
        if(node != mNodes.end())
        {
            return node->second.GetComponent(id.mComponent, id.mInstance);
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list all ID's on the subsystem.
///
///   \return List of all IDs on the subsystem.
///
////////////////////////////////////////////////////////////////////////////////////
Address::List Configuration::Subsystem::GetAddresses() const
{
    Configuration::Node::Map::const_iterator node;
    Configuration::Component::Set::const_iterator comp;
    Address::List list;
    Address id;
    id.mSubsystem = mSubsystemID;
    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        id.mNode = node->first;
        for(comp = node->second.mComponents.begin();
            comp != node->second.mComponents.end();
            comp++)
        {
            id.mComponent = (*comp).mID;
            id.mInstance = (*comp).mInstance;
            list.push_back(id);
        }
    }

    return list;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list all ID's on the subsystem that are for a specific type
///          of component.
///
///   \param cid Component/Service ID type.
///
///   \return List of all IDs on the subsystem with matching component type.
///
////////////////////////////////////////////////////////////////////////////////////
Address::List Configuration::Subsystem::GetComponentsOfType(const UShort cid) const
{
    Configuration::Node::Map::const_iterator node;
    Configuration::Component::Set::const_iterator comp;
    Address::List list;
    Address id;

    id.mSubsystem = mSubsystemID;
    id.mComponent = (Byte)(cid);

    for(node = mNodes.begin();
        node != mNodes.end();
        node++)
    {
        id.mNode = node->first;
        for(comp = node->second.mComponents.begin();
            comp != node->second.mComponents.end();
            comp++)
        {
            if( (*comp).mID == cid )
            {
                id.mInstance = (*comp).mInstance;
                list.push_back(id);
            }
        }
    }

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem& Configuration::Subsystem::operator =(const Subsystem& ss)
{
    if(this != &ss)
    {
        mSubsystemID = ss.mSubsystemID;
        mNodes = ss.mNodes;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Configuration()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Configuration(const Configuration& cfg)
{
    mSubsystems = cfg.mSubsystems;
    mIdentifications = cfg.mIdentifications;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::~Configuration()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds an ID to the configuration.
///
///   \param id The ID to add to the configuration.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::AddComponent(const Address& id)
{
    Configuration::Subsystem::Map::iterator ss;

    ss = mSubsystems.find(id.mSubsystem);
    
    if(!id.IsValid() || id.IsBroadcast())
    {    
        return;
    }

    if( ss == mSubsystems.end() )
    {
        mSubsystems[id.mSubsystem].AddComponent(id);  
    }
    else
    {
        ss->second.AddComponent(id.mNode, id.mComponent, id.mInstance);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a subsystems identification.
///
///   \param ss Subsystem number.
///   \param id Identification data.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::AddSubsystemIdentification(const Byte ss,
                                               const Identification& id)
{
    mIdentifications[ss] = id;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the data to the configuration.
///
///   \param config Configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::AddConfiguration(const Configuration& config)
{
    Subsystem::Map::const_iterator ss;
    Subsystem::Map::iterator ss2;
    Node::Map::const_iterator node;
    Node::Map::iterator node2;
    Component::Set::const_iterator comp;
    IdentificationMap::const_iterator ident;

    // Add identifications.
    for(ident = config.mIdentifications.begin();
        ident != config.mIdentifications.end();
        ident++)
    {
        if(mIdentifications.find(ident->first) == mIdentifications.end())
        {
            mIdentifications[ident->first] = ident->second;
        }
    }

    for(ss = config.mSubsystems.begin();
        ss != config.mSubsystems.end();
        ss++)
    {
        ss2 = mSubsystems.find(ss->first);
        if(ss2 == mSubsystems.end())
        {
            mSubsystems[ss->first] = ss->second;
        }
        else
        {
            for(node = ss->second.mNodes.begin();
                node != ss->second.mNodes.end();
                node++)
            {
                node2 = ss2->second.mNodes.find(node->first);
                if(node2 == ss2->second.mNodes.end())
                {
                    ss2->second.mNodes[node->first] = node->second;
                }
                else
                {
                    for(comp = node->second.mComponents.begin();
                        comp != node->second.mComponents.end();
                        comp++)
                    {
                        node2->second.mComponents.insert(*comp);
                    }
                }
            }
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the data to the configuration.
///
///   \param sconfig Subsystem configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::AddSubsystemConfiguration(const Configuration::Subsystem& sconfig)
{
    if(sconfig.mSubsystemID != 0 && sconfig.mSubsystemID != 255)
    {
        Node::Map::const_iterator node;
        mSubsystems[sconfig.mSubsystemID].mSubsystemID = sconfig.mSubsystemID;
        
        for(node = sconfig.mNodes.begin();
            node != sconfig.mNodes.end();
            node++)
        {
            mSubsystems[sconfig.mSubsystemID].mNodes[node->first] = node->second;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the data to the configuration.
///
///   \param ss Subsystem number.
///   \param nconfig Node configuration.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::AddNodeConfiguration(const Byte ss, const Configuration::Node& nconfig)
{
    if(ss != 0 && ss != 255 &&
        nconfig.mNodeID != 0 && nconfig.mNodeID != 255)
    {
        mSubsystems[ss].mNodes[nconfig.mNodeID] = nconfig;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief ID of the JAUS component to remove from the system configuration.
///
///   If after the ID is removed, and a subsystem configuration no longer has
///   any data in it, then it and its identification is removed.
///
///   \param id ID of the component to remove.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::RemoveComponent(const Address& id)
{
    Configuration::Subsystem::Map::iterator ss;
    ss = mSubsystems.find(id.mSubsystem);
    if( ss != mSubsystems.end() )
    {
        ss->second.RemoveComponent(id.mNode, id.mComponent, id.mInstance);

        // If no nodes are left for a subsystem, then
        // remove the subsystem.
        if(ss->second.mNodes.size() == 0)
        {            
            // Erase from identifications too.
            if( mIdentifications.find(ss->first) != mIdentifications.end())
            {
                mIdentifications.erase(mIdentifications.find(ss->first));
            }
            //ss = mSubsystems.erase(ss);
            mSubsystems.erase(ss);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Removes a subsystems identification from the configuration. 
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::RemoveSubsystemIdentification(const Byte ss)
{
    Configuration::IdentificationMap::iterator id;
    id = mIdentifications.find(ss);
    if(id != mIdentifications.end())
    {
        mIdentifications.erase(id);
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints system configuration data to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void Configuration::Print() const
{
    Configuration::Subsystem::Map::const_iterator ss;

    std::cout << "========================================\n";
    std::cout << "System Configuration:\n";
    std::cout << "========================================\n";
  
    for(ss = mSubsystems.begin();
        ss != mSubsystems.end();
        ss++)
    {
        std::cout << "Subsystem: " << (int)ss->first << std::endl;
        if(HaveSubsystemIdentification(ss->first))
        {
            std::cout << "Identification: " << GetSubsystemIdentification(ss->first)->GetIdentification() << std::endl;
        }
        Configuration::Node::Map::const_iterator node;
        for(node = ss->second.mNodes.begin();
            node != ss->second.mNodes.end();
            node++)
        {
            Configuration::Component::Set::const_iterator comp;
            for(comp = node->second.mComponents.begin();
                comp != node->second.mComponents.end();
                comp++)
            {
                Address id(ss->first, node->first, comp->mID, comp->mInstance);
                std::cout << "    ";
                id.PrintID();
            }
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Finds out if a subsystems configuration is present.
///
///   \param ss Subsystem ID.
///
///   \return True if subsystem data is present, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Configuration::HaveSubsystem(const Byte ss) const
{
    return mSubsystems.find(ss) != mSubsystems.end() ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Finds out of a subsystems identification data is present.
///
///   \param ss Subsystem ID.
///
///   \return True if present, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Configuration::HaveSubsystemIdentification(const Byte ss) const
{
    return mIdentifications.find(ss) != mIdentifications.end() ? true : false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to a subsystem configuration.
///
///   \param ss Subsystem ID.
///
///   \return Pointer to configuration, NULL if not present.  Warning this
///            value will change if data is removed/added to the configuration.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem* Configuration::GetSubsystem(const Byte ss) const
{
    Configuration::Subsystem::Map::const_iterator s;
    s = mSubsystems.find(ss);
    if(s != mSubsystems.end())
    {
        return (Configuration::Subsystem*)&(s->second);
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to a node configuration.
///
///   \param ss Subsystem ID.
///   \param nid Node ID.
///
///   \return Pointer to configuration, NULL if not present.  Warning this
///            value will change if data is removed/added to the configuration.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Node* Configuration::GetNode(const Byte ss, const Byte nid) const
{
    Configuration::Subsystem::Map::const_iterator s;
    s = mSubsystems.find(ss);
    if(s != mSubsystems.end())
    {
        Configuration::Node::Map::const_iterator n;
        n = s->second.mNodes.find(nid);
        if(n != s->second.mNodes.end())
        {
            return (Configuration::Node*)&(n->second);
        }
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a pointer to a subsystem identifcation.
///
///   \param ss Subsystem ID.
///
///   \return Pointer to identification, NULL if not present.  Warning this
///            value will change if data is removed/added to the configuration.
///
////////////////////////////////////////////////////////////////////////////////////
Identification* Configuration::GetSubsystemIdentification(const Byte ss) const
{
    Configuration::IdentificationMap::const_iterator id;
    id = mIdentifications.find(ss);
    if(id != mIdentifications.end())
    {
        return (Identification*)&(id->second);
    }
    return NULL;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list of all components of a specific type in the
///          system configuration.
///
///   \param cid Component type.
///
///   \return List of all components with matching type.
///
////////////////////////////////////////////////////////////////////////////////////
Address::List Configuration::GetComponentsOfType(const Byte cid) const
{
    Configuration::Subsystem::Map::const_iterator ss;
    Configuration::Node::Map::const_iterator node;
    Configuration::Component::Set::const_iterator comp;
    Address::List list;

    for(ss = mSubsystems.begin();
        ss != mSubsystems.end();
        ss++)
    {
        Address id;

        id.mSubsystem = ss->first;
        id.mComponent = cid;

        for(node = ss->second.mNodes.begin();
            node != ss->second.mNodes.end();
            node++)
        {
            id.mNode = node->first;
            for(comp = node->second.mComponents.begin();
                comp != node->second.mComponents.end();
                comp++)
            {
                if( (*comp).mID == cid )
                {
                    id.mInstance = (*comp).mInstance;
                    list.push_back(id);
                }
            }
        }
    }

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a list of all nodes in the system.
///
///   \return List of all nodes stored in the configuration.
///
////////////////////////////////////////////////////////////////////////////////////
Address::List Configuration::GetNodes() const
{
    Configuration::Subsystem::Map::const_iterator ss;
    Configuration::Node::Map::const_iterator node;
    Configuration::Component::Set::const_iterator comp;
    Address id;
    Address::List list;

    id.mComponent = 1;
    id.mInstance = 1;

    for(ss = mSubsystems.begin();
        ss != mSubsystems.end();
        ss++)
    {
        id.mSubsystem = ss->first;
        
        for(node = ss->second.mNodes.begin();
            node != ss->second.mNodes.end();
            node++)
        {
            id.mNode = node->first;
            for(comp = node->second.mComponents.begin();
                comp != node->second.mComponents.end();
                comp++)
            {
                if( (*comp).mID == 1 && (*comp).mInstance == 1 )
                {
                    list.push_back(id);
                }
            }
        }
    }

    return list;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration& Configuration::operator =(const Configuration& config)
{
    mSubsystems = config.mSubsystems;
    mIdentifications = config.mIdentifications;
    return *this;
}

/*  End of File */
