////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportservices.cpp
///  \brief This file  contains a message structure for Report Services.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 July 2007
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
#include "jaus/messages/inform/configuration/reportservices.h"
#include "jaus/messages/inform/informcodes.h"
#include <string.h>
#include <stdlib.h>
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportServices::ReportServices() : Message(JAUS_REPORT_SERVICES)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportServices::ReportServices(const ReportServices& msg) : Message(JAUS_REPORT_SERVICES)
{
    CopyHeaderData(&msg);
    mServices = msg.mServices;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportServices::~ReportServices()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the services supported by your component you are sending.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportServices::SetServices(const Service::Set& services)
{
    mServices = services;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get a pointer to the services data so you can check it out or
///          add to it, etc.
///
///   \return Pointer to services set.
///
////////////////////////////////////////////////////////////////////////////////////
Service::Set* ReportServices::GetServices() const
{
    return (Service::Set*)(&mServices);
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
int ReportServices::WriteMessageBody(Stream& msg, 
                                     const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        Service::Set::const_iterator srvc;
        Service::Message::Map::const_iterator imsg;

        expected = JAUS_BYTE_SIZE;
        written += msg.WriteByte( (Byte)mServices.size() );
        
        for(srvc = mServices.begin();
            srvc != mServices.end();
            srvc++)
        {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write( srvc->GetType() );
            expected += JAUS_BYTE_SIZE;
            written += msg.WriteByte( (Byte)srvc->GetInputMessages()->size() );
            // Now write the input message data.
            for(imsg = srvc->GetInputMessages()->begin();
                imsg != srvc->GetInputMessages()->end();
                imsg++)
            {
                expected += JAUS_UINT_SIZE + JAUS_USHORT_SIZE;
                written += msg.Write(imsg->second.mMessageCode);
                written += msg.Write(imsg->second.mPresenceVector);
            }
            // Now write the output messages for service.
            expected += JAUS_BYTE_SIZE;
            written += msg.WriteByte( (Byte)srvc->GetOutputMessages()->size() );
            // Now write the input message data.
            for(imsg = srvc->GetOutputMessages()->begin();
                imsg != srvc->GetOutputMessages()->end();
                imsg++)
            {
                expected += JAUS_UINT_SIZE + JAUS_USHORT_SIZE;
                written += msg.Write(imsg->second.mMessageCode);
                written += msg.Write(imsg->second.mPresenceVector);
            }
        }

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
int ReportServices::ReadMessageBody(const Stream& msg, 
                                    const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        UShort stype = 0;
        Byte scount = 0, mcount = 0;
        Service::Message smsg;
    
        mServices.clear();

        expected += JAUS_BYTE_SIZE;
        read += msg.Read(scount);
        for(Byte s = 0; s < scount; s++)
        {
            Service srvc;
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(stype);
            srvc.SetType(stype);
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mcount); // Number of input messages.
            for(Byte i = 0; i < mcount; i++)
            {
                expected += JAUS_USHORT_SIZE + JAUS_UINT_SIZE;
                read += msg.Read(smsg.mMessageCode);
                read += msg.Read(smsg.mPresenceVector);
                srvc.AddInputMessage(smsg);
            }
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(mcount); // Number of output messages.
            for(Byte i = 0; i < mcount; i++)
            {
                expected += JAUS_USHORT_SIZE + JAUS_UINT_SIZE;
                read += msg.Read(smsg.mMessageCode);
                read += msg.Read(smsg.mPresenceVector);
                srvc.AddOutputMessage(smsg);
            }

            // Add service to the service set
            mServices.insert(srvc);
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
///   \brief Runs a test case to validate the message works.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportServices::RunTestCase() const
{
    Stream packet;
    int result = 0;
    ReportServices msg1, msg2;
    Service srvc;

    srvc.LoadCoreMessageSupport();
    msg1.GetServices()->insert(srvc);
    if( (result = msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION)) > 0 &&
        msg2.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) == result )
    {
        Stream packet2;
        if(msg2.WriteMessageBody(packet2, JAUS_DEFAULT_VERSION) &&
            packet.Length() == packet2.Length() &&
           memcmp(packet.Ptr(), packet2.Ptr(), packet.Length()) == 0)
        {
            return 1;
        }
    }
    return 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportServices::Print() const
{
    Message::Print();
    Service::Set::const_iterator srvc;
    for(srvc = mServices.begin();
        srvc != mServices.end();
        srvc++)
    {
        srvc->PrintService();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportServices::ClearMessageBody()
{
    mServices.clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to (copies the data).
///
////////////////////////////////////////////////////////////////////////////////////
ReportServices& ReportServices::operator=(const ReportServices& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mServices = msg.mServices;
    }
    return *this;
}

/*  End of File */
