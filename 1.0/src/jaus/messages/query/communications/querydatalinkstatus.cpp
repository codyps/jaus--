////////////////////////////////////////////////////////////////////////////////////
///
///  \file querydatalinkstatus.cpp
///  \brief Implementation of the Query Data Link Status message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 6 August 2008
///  <br>Last Modified: 6 August 2008
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
#include "jaus/messages/query/communications/querydatalinkstatus.h"
#include "jaus/messages/query/querycodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryDataLinkStatus::QueryDataLinkStatus() : Message(JAUS_QUERY_DATA_LINK_STATUS)
{
    mDataLinkID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryDataLinkStatus::QueryDataLinkStatus(const QueryDataLinkStatus& msg) : Message(JAUS_QUERY_DATA_LINK_STATUS)
{
    mDataLinkID = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryDataLinkStatus::~QueryDataLinkStatus()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID of the data link to select.
///
///   \param id ID of data link to select [0,255].
///
////////////////////////////////////////////////////////////////////////////////////
void QueryDataLinkStatus::SetDataLinkID(const Byte id)
{
    mDataLinkID = id;
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
int QueryDataLinkStatus::WriteMessageBody(Stream& msg, 
                                        const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if(msg.Write(mDataLinkID))
        {
            return JAUS_BYTE_SIZE;
        }

        SetJausError(ErrorCodes::WriteFailure); return -1;
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
int QueryDataLinkStatus::ReadMessageBody(const Stream& msg, 
                                       const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if(msg.Read(mDataLinkID))
        {
            return JAUS_BYTE_SIZE;
        }

        SetJausError(ErrorCodes::ReadFailure); return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body (sets state to 0).
///
////////////////////////////////////////////////////////////////////////////////////
void QueryDataLinkStatus::ClearMessageBody()
{
    mDataLinkID = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QueryDataLinkStatus& QueryDataLinkStatus::operator =(const QueryDataLinkStatus& msg)
{
    CopyHeaderData(&msg);
    mDataLinkID = msg.mDataLinkID;
    return *this;
}

/*  End of File */
