////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportwaterdepth.cpp
///  \brief This message is used to provide the receiver the water depth in meters.
///
///  <br>Author(s): David Adams
///  <br>Created: 4 April 2009
///  <br>Last Modified: 4 April 2009
///  <br>Copyright (c) 2009
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
#include "jaus/messages/inform/environment/reportwaterdepth.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <string.h>

const double Jaus::ReportWaterDepth::Limits::MinWaterDepth = 0.0;
const double Jaus::ReportWaterDepth::Limits::MaxWaterDepth = 20000;

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportWaterDepth::ReportWaterDepth() : Message(JAUS_REPORT_WATER_DEPTH)
{
    mWaterDepth = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportWaterDepth::ReportWaterDepth(const ReportWaterDepth &msg) : Message(JAUS_REPORT_WATER_DEPTH)
{
    mWaterDepth = msg.mWaterDepth;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportWaterDepth::~ReportWaterDepth()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of speed.
///
///   \param value Desired speed [0, 10000] m/s.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportWaterDepth::SetDepth(const double value)
{
    if(value >= Limits::MinWaterDepth && value <= Limits::MaxWaterDepth)
    {
        mWaterDepth = value;
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
int ReportWaterDepth::WriteMessageBody(Stream& msg, 
                                  const UShort version) const
{
    
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected += JAUS_UINT_SIZE;
        written += msg.Write(mWaterDepth, Limits::MaxWaterDepth, Limits::MinWaterDepth, ScaledInteger::UInt);

        if(expected == written)
        {
            return written;
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
int ReportWaterDepth::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        expected = 0;

        expected += JAUS_UINT_SIZE;
        read += msg.Read(mWaterDepth, Limits::MaxWaterDepth, Limits::MinWaterDepth, ScaledInteger::UInt);
        
        if(expected == read)
        {
            return read;
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
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportWaterDepth::ClearMessageBody()
{
    mWaterDepth = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportWaterDepth &ReportWaterDepth::operator=(const ReportWaterDepth &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mWaterDepth =           msg.mWaterDepth;
    }
    return *this;
}

/*  End of File */
