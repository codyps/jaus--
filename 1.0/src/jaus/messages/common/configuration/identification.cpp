////////////////////////////////////////////////////////////////////////////////////
///
///  \file identification.cpp
///  \brief This file contains data structure for JAUS_REPORT_IDENTIFICATION
///  message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 24 July 2007
///  <br>Last Modified: 1 March 2008
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
#include "jaus/messages/common/configuration/identification.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Identification::Identification() : mAuthority(0), mType(0)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Identification::Identification(const Identification& report) : mAuthority(0), mType(0)
{
    *this = report;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Identification::~Identification()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set the lowest level of authority required to gain control of
///  the subsystem node or component capabilities.
///
///  \param auth Authority level required for control.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Identification::SetAuthority(const Byte auth)
{
    mAuthority = auth;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Defines the particular unmanned vehicle type, node type, or 
///  component types.  No values have been set yet in the current
///  JAUS documents (3.3).
///
///  \param type Type value.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Identification::SetType(const UShort type)
{
    mType = type;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set the string representing the identification of the component, etc.
///
///  \param ident NULL terminated ASCII string Human-recognizable name of
///                the subystem, node or component.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Identification::SetIdentification(const std::string& ident)
{
    mIdentification = ident;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void Identification::ClearIdentification()
{
    mAuthority = 0;
    mType = 0;
    mIdentification.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Prints message data to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void Identification::PrintIdentification() const
{
    cout << "Authority Level: " << (int)mAuthority << endl;
    cout << "Type: " << mType << endl;
    cout << "Identification: " << mIdentification << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Checks if the identification data is equal or not.
///
////////////////////////////////////////////////////////////////////////////////////
bool Identification::operator ==(const Identification& id) const
{
    if( mAuthority == id.mAuthority &&
        mType == id.mType &&
        mIdentification == id.mIdentification) 
    {
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Identification& Identification::operator=(const Identification& ident)
{
    if( this != &ident) 
    {
        mType = ident.mType;
        mAuthority = ident.mAuthority;
        mIdentification = ident.mIdentification;
    }
    return *this;
}

/* End of File */
