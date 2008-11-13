////////////////////////////////////////////////////////////////////////////////////
///
///  \file querysubsystemlist.cpp
///  \brief This file contains data structure for JAUS_QUERY_SUBSYSTEM_LIST
///  message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 24 July 2007
///  <br>Last Modified: 25 July 2007
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
#include "jaus/messages/query/configuration/querysubsystemlist.h"
#include "jaus/messages/query/querycodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QuerySubsystemList::QuerySubsystemList() : Message( JAUS_QUERY_SUBSYSTEM_LIST)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QuerySubsystemList::QuerySubsystemList(const QuerySubsystemList& query) : Message( JAUS_QUERY_SUBSYSTEM_LIST)
{
    *this = query;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QuerySubsystemList::~QuerySubsystemList()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QuerySubsystemList& QuerySubsystemList::operator=(const QuerySubsystemList& query)
{
    if( this != &query) 
    {
        CopyHeaderData(&query);
    }
    return *this;
}

/* End of File */
