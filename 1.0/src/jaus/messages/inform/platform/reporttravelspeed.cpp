////////////////////////////////////////////////////////////////////////////////////
///
///  \file reporttravelspeed.cpp
///  \brief This file contains the message class for Report Travel Speed.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#include "jaus/messages/inform/platform/reporttravelspeed.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

const double Jaus::ReportTravelSpeed::Limits::MinSpeed = 0.0;
const double Jaus::ReportTravelSpeed::Limits::MaxSpeed = 10000;

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportTravelSpeed::ReportTravelSpeed() : Message(JAUS_REPORT_TRAVEL_SPEED)
{
    mSpeed = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportTravelSpeed::ReportTravelSpeed(const ReportTravelSpeed& msg) : Message(JAUS_REPORT_TRAVEL_SPEED)
{
    mSpeed = 0;
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportTravelSpeed::~ReportTravelSpeed()
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
int ReportTravelSpeed::SetSpeed(const double value)
{
    if(value >= Limits::MinSpeed && value <= Limits::MaxSpeed)
    {
        mSpeed = value;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportTravelSpeed::Print() const
{
    Message::Print();
    cout << "Travel Speed: " << mSpeed << endl;
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
int ReportTravelSpeed::WriteMessageBody(Stream& msg, 
                                      const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected += JAUS_USHORT_SIZE;
        written += msg.Write(mSpeed, Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::UShort);

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
int ReportTravelSpeed::ReadMessageBody(const Stream& msg, 
                                     const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        expected = 0;

        expected += JAUS_USHORT_SIZE;
        read += msg.Read(mSpeed, Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::UShort);
        
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
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportTravelSpeed::ClearMessageBody()
{
    mSpeed = 0;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportTravelSpeed& ReportTravelSpeed::operator =(const ReportTravelSpeed& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mSpeed =           msg.mSpeed;
    }
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
UShort ReportTravelSpeed::GetPresenceVectorSize(const UShort version) const { return 0; }

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
UInt ReportTravelSpeed::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
