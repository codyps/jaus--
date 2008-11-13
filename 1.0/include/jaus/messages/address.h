////////////////////////////////////////////////////////////////////////////////////
///
///  \file address.h
///  \brief This file contains a data structure for storing JAUS component
///  ID information.  Each component in JAUS has an ID which is used for
///  addressing (hence Address).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 14 September 2007
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
#ifndef _JAUS_ADDRESS_H
#define _JAUS_ADDRESS_H

#include "jaus/messages/types.h"
#include <string>
#include <set>
#include <vector>

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    /// 
    ///  \class Address
    ///  \brief Simple structure for storing id information for JAUS messages.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Address
    {
    public:     
        typedef std::set<Address> Set;      ///<  STL set of Address structures.
        typedef std::vector<Address> List;  ///<  STL vector of Address structures.
        Address();
        Address(const Address &rhs);
        Address(const UInt id);
        Address(const Byte s, const Byte n,
                 const Byte c, const Byte i);
        void SetAddress(const Byte s, const Byte n,
                        const Byte c, const Byte i);
        void SetAddress(const UInt id);
        void Get(Byte &s, Byte &n, 
                 Byte &c, Byte &i);
        void PrintID() const;
        std::string ToString() const;

        inline operator UInt() const 
        {                                           //  Combines individual
            return (UInt)( ( mSubsystem << 24 ) |   //  byte values into a 
                           ( mNode << 16 ) |        //  single 4 byte number.
                           ( mComponent << 8 ) |   
                           ( mInstance ) );
        }
        
        inline bool IsValid() const 
        {                        
            if ( mSubsystem == 0 ||         //  If any value is 0, then
                 mNode == 0 ||              //  the address is invalid. Made 
                 mComponent == 0 ||         //  inline because of frequent use.
                 mInstance == 0 )
                        return false;
            return true;
        }

        inline bool IsBroadcast() const 
        {
            if( mSubsystem == 255 ||    //  This function checks to see if
                mNode == 255 ||         //  any of the ID values is a 
                mComponent == 255 ||    //  broadcast.  This function is
                mInstance == 255 )      //  made inline since it is called
                    return true;        //  frequently.
            return false;
        }
        inline UInt GetID() const 
        {                                           //  Combines individual
            return (UInt)( ( mSubsystem << 24 ) |   //  byte values into a 
                           ( mNode << 16 ) |        //  single 4 byte number.
                           ( mComponent << 8 ) |   
                           ( mInstance ) );
        }
        //  Check if destination ID matches the second ID.
        static bool DestinationMatch(const Address& dest, const Address id)
        {
            if( (dest.mSubsystem == 255 || dest.mSubsystem == id.mSubsystem) &&
                (dest.mNode == 255 || dest.mNode == id.mNode) &&
                (dest.mComponent == 255 || dest.mComponent == id.mComponent) &&
                (dest.mInstance == 255 || dest.mInstance == id.mInstance) ) {
                    return true;
            }

            return false;
        }
        static UInt GetID(const Byte s, 
                               const Byte n,
                               const Byte c, 
                               const Byte i);  ///<  Returns ID in unsigned integer.
        static void ExtractID(const UInt id, 
                              Byte &s, 
                              Byte &n, 
                              Byte &c, 
                              Byte &i);        ///<  Get ID from an unsigned integer.
        inline bool operator==(const Address &rhs) const
        {
            if( mSubsystem == rhs.mSubsystem &&
                mNode == rhs.mNode &&
                mComponent == rhs.mComponent &&
                mInstance == rhs.mInstance )
                    return true;
            return false;
        }

        inline bool operator==(const UInt id) const 
        { 
            return id == this->GetID(); 
        }

        inline bool operator!=(const Address &rhs) const
        {
            if( mSubsystem != rhs.mSubsystem ||
                mNode != rhs.mNode ||
                mComponent != rhs.mComponent ||
                mInstance != rhs.mInstance )
                    return true;
            return false;
        }
        inline bool operator<(const Address &rhs) const
        {
            return GetID() < rhs.GetID();
        }

        inline bool operator<=(const Address &rhs) const
        {
            return GetID() <= rhs.GetID();
        }

        inline bool operator>(const Address &rhs) const
        {
            return GetID() > rhs.GetID();
        }

        inline bool operator>=(const Address &rhs) const
        {
            return GetID() >= rhs.GetID();
        }

        Address &operator()(const Byte s, const Byte n, 
                             const Byte c, const Byte i);
        inline Address &operator=(const Address &rhs)
        {
            mSubsystem = rhs.mSubsystem;
            mNode = rhs.mNode;
            mComponent = rhs.mComponent;
            mInstance = rhs.mInstance;
            return *this;
        }
        inline Address &operator=(const UInt id) 
        { 
            mSubsystem = (Byte)((id & 0xFF000000) >> 24);
            mNode =      (Byte)((id & 0x00FF0000) >> 16);
            mComponent = (Byte)((id & 0x0000FF00) >> 8);
            mInstance =  (Byte)((id & 0x000000FF));
            return *this;
        }

        Byte mSubsystem;    ///<  Subsystem ID.
        Byte mNode;         ///<  Node ID.
        Byte mComponent;    ///<  Component ID.
        Byte mInstance;     ///<  Instance ID.
    };
    typedef Address ID;
} //  End of Jaus namespace

#endif
#endif
/* End of file */
