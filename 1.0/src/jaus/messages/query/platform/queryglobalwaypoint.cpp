////////////////////////////////////////////////////////////////////////////////////
///
///  \file queryglobalwaypoint.cpp
///  \brief This file contains the message structure for Query Global Waypoint
///
///  <br>Author(s): David Adams
///  <br>Created: 11 June 2008
///  <br>Last Modified: 11 June 2008
///  <br>Copyright (c) 2007
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#include "jaus/messages/query/platform/queryglobalwaypoint.h"
#include "jaus/messages/query/querycodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UInt QueryGlobalWaypoint::Limits::MinWaypointNumber = 0;
const UInt QueryGlobalWaypoint::Limits::MaxWaypointNumber = 65535;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalWaypoint::QueryGlobalWaypoint() : 
            Message(JAUS_QUERY_GLOBAL_WAYPOINT)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalWaypoint::QueryGlobalWaypoint(const QueryGlobalWaypoint &msg) : 
            Message(JAUS_QUERY_GLOBAL_WAYPOINT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalWaypoint::~QueryGlobalWaypoint()
{
}

void QueryGlobalWaypoint::ClearMessageBody()
{
    mWaypointNumber = 0;
}

int QueryGlobalWaypoint::SetWaypointNumber(const UInt val)
{
    if(val >= Limits::MinWaypointNumber && val <= Limits::MaxWaypointNumber)
    {
        mWaypointNumber = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

UInt QueryGlobalWaypoint::GetWaypointNumber()const
{
    return mWaypointNumber;
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
int QueryGlobalWaypoint::WriteMessageBody(Stream& msg, 
                                       const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mWaypointNumber)) 
        {
            return JAUS_UINT_SIZE;
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
int QueryGlobalWaypoint::ReadMessageBody(const Stream& msg, 
                                                  const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mWaypointNumber))
        {
            return JAUS_UINT_SIZE;
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
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalWaypoint &QueryGlobalWaypoint::operator=(const QueryGlobalWaypoint &msg)
{
    CopyHeaderData(&msg);   
    mWaypointNumber = msg.mWaypointNumber;
    return *this;
}


/*  End of File */
