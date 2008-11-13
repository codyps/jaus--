////////////////////////////////////////////////////////////////////////////////////
///
///  \file QueryGlobalPathSegment.cpp
///  \brief This file contains the message structure for Query Global Path Segment
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
#include "jaus/messages/query/platform/queryglobalpathsegment.h"
#include "jaus/messages/query/querycodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const UShort QueryGlobalPathSegment::Limits::MinPathSegmentNumber =  0;    
const UShort QueryGlobalPathSegment::Limits::MaxPathSegmentNumber =  65535;    
const double QueryGlobalPathSegment::Limits::MinP1Latitude =        -90;            
const double QueryGlobalPathSegment::Limits::MaxP1Latitude =        90;            
const double QueryGlobalPathSegment::Limits::MinP1Longitude =        -180;            
const double QueryGlobalPathSegment::Limits::MaxP1Longitude =        180;            
const double QueryGlobalPathSegment::Limits::MinP1Altitude =        -10000;            
const double QueryGlobalPathSegment::Limits::MaxP1Altitude =        35000;            
const double QueryGlobalPathSegment::Limits::MinP2Latitude =        -90;            
const double QueryGlobalPathSegment::Limits::MaxP2Latitude =        90;            
const double QueryGlobalPathSegment::Limits::MinP2Longitude =        -180;            
const double QueryGlobalPathSegment::Limits::MaxP2Longitude =        180;            
const double QueryGlobalPathSegment::Limits::MinP2Altitude =        -10000;            
const double QueryGlobalPathSegment::Limits::MaxP2Altitude =        35000;            
const double QueryGlobalPathSegment::Limits::MinWeighingFactor =    0;        
const double QueryGlobalPathSegment::Limits::MaxWeighingFactor =    500;        

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalPathSegment::QueryGlobalPathSegment() : 
                        Message(JAUS_QUERY_GLOBAL_PATH_SEGMENT)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalPathSegment::QueryGlobalPathSegment(const QueryGlobalPathSegment &msg) : 
                        Message(JAUS_QUERY_GLOBAL_PATH_SEGMENT)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalPathSegment::~QueryGlobalPathSegment()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the presence for the query.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryGlobalPathSegment::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
    return JAUS_OK;
}

int QueryGlobalPathSegment::SetPathSegmentNumber(const UInt val)
{
    if(val >= Limits::MinPathSegmentNumber && val <= Limits::MaxPathSegmentNumber)
    {
        mPathSegmentNumber = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
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
int QueryGlobalPathSegment::WriteMessageBody(Stream& msg, 
                                                   const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;
        written += msg.Write(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        written +=msg.Write(mPathSegmentNumber);
        expected += JAUS_UINT_SIZE;

        if(expected == written)
        {
            return written;
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
int QueryGlobalPathSegment::ReadMessageBody(const Stream& msg, 
                                                  const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        read += msg.Read(mPresenceVector);
        expected += JAUS_BYTE_SIZE;

        read +=msg.Read(mPathSegmentNumber);
        expected += JAUS_UINT_SIZE;

        if(expected == read)
        {
            return read;
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
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryGlobalPathSegment::ClearMessageBody()
{
    mPresenceVector = 0;
    mPathSegmentNumber = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
QueryGlobalPathSegment &QueryGlobalPathSegment::operator=(const QueryGlobalPathSegment &msg)
{
    CopyHeaderData(&msg);
    mPresenceVector = msg.mPresenceVector;    
    mPathSegmentNumber = msg.mPathSegmentNumber;
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size in bytes of the presence vector used by
///          the message.
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  size.  Defaults is current version of library.
///
///   \return Size in bytes of presence vector associated with message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort QueryGlobalPathSegment::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the bit mask associated with the message indicating the
///          bits used in the presence vector of this message (if it has one).
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  mask.  Defaults is current version of library.
///
///   \return Presence vector mask associated with message.  This value
///           is used to determine what bits are used in the presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
UInt QueryGlobalPathSegment::GetPresenceVectorMask(const UShort version) const { return 0x3; }

/*  End of File */