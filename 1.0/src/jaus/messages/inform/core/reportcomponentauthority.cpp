////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcomponentauthority.cpp
///  \brief This file contains data structures for the JAUS_REPORT_COMPONENT_AUTHORITY
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
#include "jaus/messages/inform/core/reportcomponentauthority.h"
#include "jaus/messages/inform/informcodes.h"

#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentAuthority::ReportComponentAuthority() : Message(JAUS_REPORT_COMPONENT_AUTHORITY)
{
    mAuthorityCode = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentAuthority::ReportComponentAuthority(const ReportComponentAuthority &msg) : Message(JAUS_REPORT_COMPONENT_AUTHORITY)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentAuthority::~ReportComponentAuthority()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the authority code being sent in the message.
///
///   \return The authority code set [0,255].
///
////////////////////////////////////////////////////////////////////////////////////
Byte ReportComponentAuthority::GetAuthorityCode() const
{
    return mAuthorityCode; 
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the authority code value.
///
///   The default value for all components is 0 for authority code.  The
///   higher the authority code set, components with lesser values will lose
///   control until authority code changed.
///
///   \param code Authority code [0,255].
///
///   \return 1 if the value was set, otherwise 0.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportComponentAuthority::SetAuthorityCode(const Byte code) 
{
    mAuthorityCode = code;
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
int ReportComponentAuthority::WriteMessageBody(Stream& msg, 
                                               const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mAuthorityCode) )
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
int ReportComponentAuthority::ReadMessageBody(const Stream& msg, 
                                              const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mAuthorityCode) )
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
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentAuthority &ReportComponentAuthority::operator=(const ReportComponentAuthority &msg)
{
    mAuthorityCode = msg.mAuthorityCode;
    this->CopyHeaderData(&msg);
    return *this;
}

/*  End of File */
