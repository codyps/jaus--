////////////////////////////////////////////////////////////////////////////////////
///
///  \file address.cpp
///  \brief This file contains a data structure for storing JAUS component
///  ID information.  Each component in JAUS has an ID which is used for
///  addressing (hence Address).
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 30 July 2007
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
#include "jaus/messages/address.h"
#include <iostream>
#include <iomanip>
#include <assert.h>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Address() 
{
    mSubsystem = 0;
    mNode = 0;
    mComponent = 0;
    mInstance = 0;
}


////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Constructor.
///
///  \param rhs Address to be equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Address(const Address &rhs)
{
    mSubsystem = rhs.mSubsystem;
    mNode = rhs.mNode;
    mComponent = rhs.mComponent;
    mInstance = rhs.mInstance;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Converts combined ID value into components.
///
///  \param id Combined source/destination ID.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Address(const UInt id)
{
    ExtractID(id, mSubsystem, mNode, mComponent, mInstance);
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Constructor.  No ID value can be set to 0.
///
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Address(const Byte s,
                 const Byte n,
                 const Byte c,
                 const Byte i)
{
    mSubsystem = s;
    mNode = n;
    mComponent = c;
    mInstance = i;
}


////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Sets the address.  No ID value can be set to 0.
///
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Address::SetAddress(const Byte s,
                         const Byte n,
                         const Byte c,
                         const Byte i)
{
    mSubsystem = s;
    mNode = n;
    mComponent = c;
    mInstance = i;
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Prints ID to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void Address::PrintID() const 
{
    cout << (int)mSubsystem << ":" << 
            (int)mNode << ":" << 
            (int)mComponent << ":" << 
            (int)mInstance << endl;
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Returns the std::string representation of a Address
///
////////////////////////////////////////////////////////////////////////////////////
string Address::ToString() const 
{
    char buffer[16];

    sprintf(buffer, "%d:%d:%d:%d", (int)mSubsystem, (int)mNode, (int)mComponent, (int)mInstance);

    return (string)buffer;
}

////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Sets the address.  No ID value can be set to 0.
///
///  \param id ID containing subsytem, node, component, instance id values.
///
////////////////////////////////////////////////////////////////////////////////////
void Address::SetAddress(const UInt id)
{
    ExtractID(id, mSubsystem, mNode, mComponent, mInstance);
}


////////////////////////////////////////////////////////////////////////////////////
/// 
///  \brief Gets the address.  No ID value can be set to 0.
///
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
////////////////////////////////////////////////////////////////////////////////////
void Address::Get(Byte &s,
                   Byte &n,
                   Byte &c,
                   Byte &i)
{
    s = mSubsystem;
    n = mNode;
    c = mComponent;
    i = mInstance;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Converts individual id values into a single number.
///
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
///  \return A single number representing the address.
///
////////////////////////////////////////////////////////////////////////////////////
UInt Address::GetID(const Byte s,
                          const Byte n,
                          const Byte c,
                          const Byte i)
{
    UInt num = 0;

    num = s;
    num <<= JAUS_BYTE_BITS;
    num += n;
    num <<= JAUS_BYTE_BITS;
    num += c;
    num <<= JAUS_BYTE_BITS;
    num += i;

    return num;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Converts combined ID value into components.
///
///  \param id Combined source/destination ID.
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
///  \return A single number representing the address.
///
////////////////////////////////////////////////////////////////////////////////////
void Address::ExtractID(const UInt id,
                        Byte &s,
                        Byte &n,
                        Byte &c,
                        Byte &i)
{
    s = (Byte)((id & 0xFF000000) >> 24);
    n = (Byte)((id & 0x00FF0000) >> 16);
    c = (Byte)((id & 0x0000FF00) >> 8);
    i = (Byte)((id & 0x000000FF));
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the address.
///
///  \param s Subsytem ID.
///  \param n Node ID.
///  \param c Component ID.
///  \param i Instance ID.
///
///  \return Modified Address structure.
///
////////////////////////////////////////////////////////////////////////////////////
Address & Address::operator()(const Byte s, const Byte n,
                                const Byte c, const Byte i)
{
    mSubsystem = s;
    mNode = n;
    mComponent = c;
    mInstance = i;
    return *this;
}

/* End of file */
