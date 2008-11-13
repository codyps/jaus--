////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportlocalvector.cpp
///  \brief This file contains the message structure for Report Local Vector
///
///  <br>Author(s): David Adams
///  <br>Created: 12 June 2008
///  <br>Last Modified: 16 June 2008
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
#include "jaus/messages/inform/platform/reportlocalvector.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

const double ReportLocalVector::Limits::MinSpeed =        0;
const double ReportLocalVector::Limits::MaxSpeed =        10000;
const double ReportLocalVector::Limits::MinHeading =    -JAUS_PI;
const double ReportLocalVector::Limits::MaxHeading =    JAUS_PI;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalVector::ReportLocalVector() : 
            Message(JAUS_REPORT_LOCAL_VECTOR)
{
    ClearMessageBody();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalVector::ReportLocalVector(const ReportLocalVector &msg) : 
            Message(JAUS_REPORT_LOCAL_VECTOR)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalVector::~ReportLocalVector()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body information.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportLocalVector::ClearMessageBody()
{
    mSpeed = 0;
    mHeading = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets speed
///
///   \param val Meters per Second, Lower Limit = 0, Upper Limit = 10,000.
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalVector::SetSpeed(const double val)
{
    if(val>=Limits::MinSpeed && val <=Limits::MaxSpeed)
    {
        mSpeed = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a heading
///
///   \param val Integer, Radians Lower Limit = -JAUS_PI, Upper Limit = JAUS_PI
///
///   \return JAUS_OK if value set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportLocalVector::SetHeading(const double val)
{
    if(val>=Limits::MinHeading && val <=Limits::MaxHeading)
    {
        mHeading = val;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The current speed in m/s.
///
////////////////////////////////////////////////////////////////////////////////////
double ReportLocalVector::GetSpeed() const
{
    return mSpeed;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return The current heading in radians.
///
////////////////////////////////////////////////////////////////////////////////////
double ReportLocalVector::GetHeading() const
{
    return mHeading;
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
int ReportLocalVector::WriteMessageBody(Stream& msg, 
                                        const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        
        written += msg.Write(mSpeed,Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        written += msg.Write(mHeading, Limits::MaxHeading, Limits::MinHeading, ScaledInteger::Short);
        expected +=JAUS_SHORT_SIZE;

        if(expected==written)
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
int ReportLocalVector::ReadMessageBody(const Stream& msg, 
                                       const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;
        
        read += msg.Read(mSpeed,Limits::MaxSpeed, Limits::MinSpeed, ScaledInteger::Int);
        expected += JAUS_INT_SIZE;

        read += msg.Read(mHeading, Limits::MaxHeading, Limits::MinHeading, ScaledInteger::Short);
        expected +=JAUS_SHORT_SIZE;

        if(expected==read)
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
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportLocalVector &ReportLocalVector::operator=(const ReportLocalVector &msg)
{
    CopyHeaderData(&msg);   
    mSpeed = msg.mSpeed;
    mHeading = msg.mHeading;
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
UShort ReportLocalVector::GetPresenceVectorSize(const UShort version) const { return 0; }

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
UInt ReportLocalVector::GetPresenceVectorMask(const UShort version) const { return 0; }

/*  End of File */
