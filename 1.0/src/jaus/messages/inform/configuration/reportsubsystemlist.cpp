////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportsubsystemlist.cpp
///  \brief This file contains message for Report Subsystem List.
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
#include "jaus/messages/inform/configuration/reportsubsystemlist.h"
#include "jaus/messages/inform/informcodes.h"

#include <iostream>
#include <string.h>

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSubsystemList::ReportSubsystemList() : Message( JAUS_REPORT_SUBSYSTEM_LIST)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSubsystemList::ReportSubsystemList(const ReportSubsystemList& report) : Message( JAUS_REPORT_SUBSYSTEM_LIST)
{
    *this = report;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSubsystemList::~ReportSubsystemList()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set subsystem list.
///
///  \param list List of subystems.  Max size is 255.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportSubsystemList::SetSubsystemList(const Address::Set& list)
{
    Address::Set::const_iterator id1;
    Address::Set::iterator id2;
    
    mSubsystemList.clear();

    // This message can only send 255 ID's.
    if(list.size() > 255)
    {
        // Only add one ID for each subsystem.
        for(id1 = list.begin(); id1 != list.end(); id1++)
        {
            bool match = false;
            for(id2 = mSubsystemList.begin(); id2 != mSubsystemList.end(); id2++)
            {
                if(id2->mSubsystem == id1->mSubsystem)
                {
                    match = true;
                    break;
                }
            }
            if(!match)
            {
                mSubsystemList.insert(*id1);
            }
        }
    }
    else
    {
        mSubsystemList = list;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return A pointer to the subsystem list data for reading or adding to.
///
////////////////////////////////////////////////////////////////////////////////////
Address::Set* ReportSubsystemList::GetSubsystemList() const
{
    return (Address::Set*)(&mSubsystemList);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportSubsystemList::ClearMessageBody()
{
    mSubsystemList.clear();
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
int ReportSubsystemList::WriteMessageBody(Stream& msg, 
                                          const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        Address::Set::const_iterator id;

        expected = JAUS_BYTE_SIZE;
        written += msg.WriteByte( (Byte)mSubsystemList.size() );
        for(id = mSubsystemList.begin();
            id != mSubsystemList.end();
            id++)
        {
            expected += JAUS_BYTE_SIZE*4;
            written += msg.WriteByte(id->mSubsystem);
            written += msg.WriteByte(id->mNode);
            written += msg.WriteByte(id->mComponent);
            written += msg.WriteByte(id->mInstance);
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
int ReportSubsystemList::ReadMessageBody(const Stream& msg, 
                                         const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        Address id;
        Byte num = 0;
        mSubsystemList.clear();

        expected = JAUS_BYTE_SIZE;        
        read += msg.Read(num);
        
        for(Byte i = 0; i < num; i++)
        {
            expected += JAUS_BYTE_SIZE*4;
            read += msg.Read(id.mSubsystem);
            read += msg.Read(id.mNode);
            read += msg.Read(id.mComponent);
            read += msg.Read(id.mInstance);
            mSubsystemList.insert(id);
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
///  \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSubsystemList& ReportSubsystemList::operator=(const ReportSubsystemList& report)
{
    if( this != &report) 
    {
        CopyHeaderData(&report);
        mSubsystemList = report.mSubsystemList;
    }
    return *this;
}

/* End of File */
