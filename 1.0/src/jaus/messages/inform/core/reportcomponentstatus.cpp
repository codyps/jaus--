////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcomponentstatus.cpp
///  \brief This file contains data structures for the JAUS_REPORT_COMPONENT_STATUS
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 24 July 2007
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
#include "jaus/messages/inform/core/reportcomponentstatus.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentStatus::ReportComponentStatus() : Message(JAUS_REPORT_COMPONENT_STATUS)
{
    mPrimaryStatusCode = 0;
    mSecondaryStatusCode = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentStatus::ReportComponentStatus(const ReportComponentStatus &msg) : Message(JAUS_REPORT_COMPONENT_STATUS)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentStatus::~ReportComponentStatus()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the priamry status code.
///
///   \param code The status code to set.  Bits 0-3:
///                                        0 = Initialize,
///                                        1 = Ready,
///                                        2 = Standby,
///                                        3 = Shutdown,
///                                        4 = Failure,
///                                        5 = Emergency.
///                                        Bits: 4-7 Available.
///
///   \return 1 if the value was set, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportComponentStatus::SetPrimaryStatusCode(const Byte code) 
{
    mPrimaryStatusCode = code;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the secondary status code.
///
///   \param code The status code to set.  Bits 0-15 Reserved.  Each bit corresponds
///   to a specific condition for each primary code.  Bits 16-31 Available.
///
///   \return 1 if the value was set, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportComponentStatus::SetSecondaryStatusCode(const UInt code) 
{
    mSecondaryStatusCode = code;
    return JAUS_OK;
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
int ReportComponentStatus::WriteMessageBody(Stream& msg, 
                                            const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mPrimaryStatusCode) &&
            msg.Write(mSecondaryStatusCode))
        {
            return JAUS_BYTE_SIZE + JAUS_UINT_SIZE;
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
int ReportComponentStatus::ReadMessageBody(const Stream& msg, 
                                           const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mPrimaryStatusCode) &&
            msg.Read(mSecondaryStatusCode) )
        {
            return JAUS_BYTE_SIZE + JAUS_UINT_SIZE;
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
///   \brief  Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportComponentStatus::ClearMessageBody()
{
    mPrimaryStatusCode = 0;
    mSecondaryStatusCode = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentStatus &ReportComponentStatus::operator=(const ReportComponentStatus &msg)
{
    mPrimaryStatusCode = msg.mPrimaryStatusCode;
    mSecondaryStatusCode = msg.mSecondaryStatusCode;
    CopyHeaderData(&msg);
    return *this;
}

/*  End of File */
