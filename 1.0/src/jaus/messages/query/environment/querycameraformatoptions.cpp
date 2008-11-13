////////////////////////////////////////////////////////////////////////////////////
///
///  \file querycameraformatoptions.cpp
///  \brief This file contains the message structure for Query Camera Formate
///  Options.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 6 June 2008
///  <br>Last Modified: 6 June 2008
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
#include "jaus/messages/query/environment/querycameraformatoptions.h"
#include "jaus/messages/query/querycodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryCameraFormatOptions::QueryCameraFormatOptions() : Message(JAUS_QUERY_CAMERA_FORMAT_OPTIONS)
{
    mCameraID = 0;
    mPresenceVector = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
QueryCameraFormatOptions::QueryCameraFormatOptions(const QueryCameraFormatOptions& msg) : Message(JAUS_QUERY_CAMERA_FORMAT_OPTIONS)
{
    mCameraID = 0;
    mPresenceVector = 0;
    *this = msg;
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID of the camera to get the formation options for.
///
///   \param id Camera ID number [0,255].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryCameraFormatOptions::SetCameraID(const Byte id)
{
    mCameraID = id;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the presence vector indicating the fields being requested in
///   the Report Camera Formation Options message.
///
///   \param pv See message presence vector fields for values.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int QueryCameraFormatOptions::SetPresenceVector(const Byte pv)
{
    mPresenceVector = pv;
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
int QueryCameraFormatOptions::WriteMessageBody(Jaus::Stream &msg, const Jaus::UShort version) const
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = JAUS_BYTE_SIZE*2;
        int written = 0;

        written += msg.Write(mPresenceVector);
        written += msg.Write(mCameraID);

        if(written == expected)
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
int QueryCameraFormatOptions::ReadMessageBody(const Jaus::Stream &msg, const Jaus::UShort version)
{
    if(version <= JAUS_VERSION_3_4)
    {
        int expected = JAUS_BYTE_SIZE*2;
        int read = 0;

        read += msg.Read(mPresenceVector);
        read += msg.Read(mCameraID);

        if(read == expected)
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
///   \brief Clears message body values.
///
////////////////////////////////////////////////////////////////////////////////////
void QueryCameraFormatOptions::ClearMessageBody()
{
    mCameraID = 0;
    mPresenceVector = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QueryCameraFormatOptions& QueryCameraFormatOptions::operator =(const QueryCameraFormatOptions& msg)
{
    CopyHeaderData(&msg);
    mCameraID = msg.mCameraID;
    mPresenceVector = msg.mPresenceVector;
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
UShort QueryCameraFormatOptions::GetPresenceVectorSize(const UShort version) const { return JAUS_BYTE_SIZE; }
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
UInt QueryCameraFormatOptions::GetPresenceVectorMask(const UShort version) const { return 0x7F; }

/*  End of File */
