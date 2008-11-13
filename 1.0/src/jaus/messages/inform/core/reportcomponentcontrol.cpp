////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcomponentcontrol.cpp
///  \brief This file contains data structures for the JAUS_REPORT_COMPONENT_CONTROL
///  message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 9 July 2007
///  <br>Last Modified: 26 February 2008
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
#include "jaus/messages/inform/core/reportcomponentcontrol.h"
#include "jaus/messages/inform/informcodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentControl::ReportComponentControl() : Message(JAUS_REPORT_COMPONENT_CONTROL)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentControl::ReportComponentControl(const ReportComponentControl& report) : Message(JAUS_REPORT_COMPONENT_CONTROL)
{
    *this = report;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentControl::~ReportComponentControl()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the component in control.  Use ID fields of 0 if
///   there is no component in control.
///
///   \param id JAUS ID of controlling component.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportComponentControl::SetControllerID(const Address& id)
{
    if( !id.IsBroadcast() ) {
        mAuthorityAddress = id;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the component in controls authority code.
///
///   \param code Authority code value [0,255].
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportComponentControl::SetControllerAuthorityCode(const Byte code)
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
int ReportComponentControl::WriteMessageBody(Stream& msg, 
                                             const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Write(mAuthorityAddress.mSubsystem) &&
            msg.Write(mAuthorityAddress.mNode) &&
            msg.Write(mAuthorityAddress.mComponent) &&
            msg.Write(mAuthorityAddress.mInstance) &&
            msg.Write(mAuthorityCode) )
        {
            return JAUS_BYTE_SIZE*5;
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
int ReportComponentControl::ReadMessageBody(const Stream& msg, 
                                            const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( msg.Read(mAuthorityAddress.mSubsystem) &&
            msg.Read(mAuthorityAddress.mNode) &&
            msg.Read(mAuthorityAddress.mComponent) &&
            msg.Read(mAuthorityAddress.mInstance) &&
            msg.Read(mAuthorityCode) )
        {
            return JAUS_BYTE_SIZE*5;
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
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportComponentControl& ReportComponentControl::operator =(const ReportComponentControl& report)
{
    if( this != &report ) 
    {
        CopyHeaderData( &report );
        mAuthorityAddress = report.mAuthorityAddress;
        mAuthorityCode = report.mAuthorityCode;
    }
    return *this;
}

/*  End of File */
