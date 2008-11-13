////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryconfiguration.cpp
///  \brief This file contains data structure for JAUS_QUERY_CONFIGURATION
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
#include "jaus/messages/query/configuration/queryconfiguration.h"
#include "jaus/messages/query/querycodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryConfiguration::QueryConfiguration() : Message( JAUS_QUERY_CONFIGURATION)
{
    mQueryField = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryConfiguration::QueryConfiguration(const QueryConfiguration& query) : Message( JAUS_QUERY_CONFIGURATION )
{
    mQueryField = 0;
    *this = query;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryConfiguration::~QueryConfiguration()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set the field of configuration query.
///
///  \param type The query type, 0 = reserved, 1 = reserved, 2 = subsystem 
///              identification, 3 = node configuration, 4-255 are reserved.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryConfiguration::SetQueryField(const Byte type)
{
    if( type > 0 ) {
        mQueryField = type;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryConfiguration::ClearMessageBody()
{
    mQueryField = 0;
}

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
int QueryConfiguration::WriteMessageBody(Stream& msg, 
                                         const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mQueryField)) 
        {
            return JAUS_BYTE_SIZE;
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
int QueryConfiguration::ReadMessageBody(const Stream& msg, 
                                        const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mQueryField))
        {
            return JAUS_BYTE_SIZE;
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
///  \brief Pritns message data to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryConfiguration::Print() const
{
    Message::Print();
    cout << "Query Field: " << (int)mQueryField << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QueryConfiguration& QueryConfiguration::operator=(const QueryConfiguration& query)
{
    if( this != &query )
    {
        CopyHeaderData(&query);
        mQueryField = query.mQueryField;
    }
    return *this;
}

/* End of File */
