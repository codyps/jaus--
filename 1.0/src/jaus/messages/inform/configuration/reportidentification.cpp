////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportidentification.cpp
///  \brief This file contains the message for Report Identification.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 24 July 2007
///  <br>Last Modified: 6 March 2008
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
#include "jaus/messages/inform/configuration/reportidentification.h"
#include "jaus/messages/inform/informcodes.h"

#include <iostream>
#include <stdlib.h>
#include <string.h>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportIdentification::ReportIdentification() : Message( JAUS_REPORT_IDENTIFICATION)
{
    mQueryType = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportIdentification::ReportIdentification(const ReportIdentification& report) : Message( JAUS_REPORT_IDENTIFICATION)
{
    *this = report;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportIdentification::~ReportIdentification()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the type of query message this report is responding to.  This
///         indicates that the identification is for system, subsystem, 
///         node, etc.
///
///  \param query Query type (see ReportIdentification::Type enumeration).
///
////////////////////////////////////////////////////////////////////////////////////
void ReportIdentification::SetQueryType(const Byte query)
{
    mQueryType = query;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets the identification data for the message.
///
///  \param ident Identification data to use for message.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportIdentification::SetIdentification(const Identification& ident)
{
    mIdentification = ident;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return Returns a pointer to the identification data in the message so that
///          you can read its values or change them.
///
////////////////////////////////////////////////////////////////////////////////////
Identification* ReportIdentification::GetIdentification() const
{
    return (Identification*)(&mIdentification);
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
int ReportIdentification::WriteMessageBody(Stream& msg, 
                                           const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        expected = JAUS_BYTE_SIZE*2 + JAUS_USHORT_SIZE + (UInt)mIdentification.GetIdentification().size();
        written += msg.WriteByte(mQueryType);
        written += msg.WriteByte(mIdentification.GetAuthority());
        written += msg.Write(mIdentification.GetType());
        written += msg.Write((unsigned char *)mIdentification.GetIdentification().c_str(), 
                             (unsigned int)mIdentification.GetIdentification().size());

        if( expected == written ) 
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
int ReportIdentification::ReadMessageBody(const Stream& msg, 
                                          const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        Byte qtype, authority;
        UShort type;
        unsigned char *ident = NULL;
        expected += JAUS_BYTE_SIZE*2 + JAUS_USHORT_SIZE;
        read += msg.Read(qtype);
        read += msg.Read(authority);
        read += msg.Read(type);
        
        expected += msg.Length() - msg.GetReadPos();

        ident = new unsigned char[msg.Length() - msg.GetReadPos() + 1];
        memset(ident, 0, msg.Length() - msg.GetReadPos() + 1);
        read += msg.Read(ident, msg.Length() - msg.GetReadPos());

        mQueryType = qtype;
        mIdentification.SetType(type);
        mIdentification.SetAuthority(authority);
        mIdentification.SetIdentification((char *)ident);
        if(ident)
        {
            delete[] ident;
            ident = NULL;
        }

        if( expected == read )
        {
            return expected;
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
///  \brief Performs a test case to validate the message.
///
///  \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportIdentification::RunTestCase() const
{
    Stream packet;
    ReportIdentification msg1, msg2;
    Identification ident;

    ident.SetType(10001);
    ident.SetIdentification("Calculon");
    ident.SetAuthority(2);

    msg1.SetQueryType(ReportIdentification::Subsystem);
    msg1.SetIdentification(ident);

    if(msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) > 0 )
    {
        if( msg1.mQueryType == msg2.mQueryType &&
            msg1.mIdentification.GetIdentification() == msg2.mIdentification.GetIdentification() &&
            msg1.mIdentification.GetType() == msg2.mIdentification.GetType() &&
            msg1.mIdentification.GetAuthority() == msg2.mIdentification.GetAuthority())
        {
            return JAUS_OK;
        }
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportIdentification::ClearMessageBody()
{
    mQueryType = 0;
    mIdentification.ClearIdentification();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Pritns message data to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportIdentification::Print() const
{
    Message::Print();
    mIdentification.PrintIdentification();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportIdentification& ReportIdentification::operator=(const Identification& ident)
{
    mIdentification = ident;
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportIdentification& ReportIdentification::operator=(const ReportIdentification& report)
{
    if( this != &report) {
        CopyHeaderData(&report);
        mQueryType = report.mQueryType;
        mIdentification = report.mIdentification;
    }
    return *this;
}

/* End of File */
