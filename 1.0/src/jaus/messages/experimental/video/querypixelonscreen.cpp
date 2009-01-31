////////////////////////////////////////////////////////////////////////////////////
///
///  \file querypixelonscreen.cpp
///  \brief This file contains a custom experimental message to query a 
///  SICK LIDAR sensor.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 19 December 2008
///  <br>Copyright (c) 2008
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
#include "jaus/messages/experimental/video/querypixelonscreen.h"
#include "jaus/messages/experimental/experimentalcodes.h"

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryPixelOnScreen::QueryPixelOnScreen() : Message(JAUS_QUERY_PIXEL_ON_SCREEN)
{
    mX = -1;
    mY = -1;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryPixelOnScreen::QueryPixelOnScreen(const QueryPixelOnScreen& query) : Message(JAUS_QUERY_PIXEL_ON_SCREEN)
{
    *this = query;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
QueryPixelOnScreen::~QueryPixelOnScreen()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the X and Y location of the pixel on the video screen to
///   perform the query at.
///
///   \param[in] x X position on video camera screen [0: left, 1.0: right].
///   \param[in] y Y position on video camera screen [0: bottom, 1.0: top].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE;
///
////////////////////////////////////////////////////////////////////////////////////
int QueryPixelOnScreen::SetPixelLocation(const Float x, const Float y)
{
    if(x >= 0.0 && x <= 1.0 && y >= 0.0 && y <= 1.0)
    {
        mX = x;
        mY = y;
        return OK;
    }
    return FAILURE;    
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
int QueryPixelOnScreen::WriteMessageBody(Stream& msg, 
                                         const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int written = 0;
        int expected = 0;

        written += msg.Write(mX);
        expected += Jaus::JAUS_FLOAT_SIZE;

        written += msg.Write(mY);
        expected += Jaus::JAUS_FLOAT_SIZE;

        if(written == expected)
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
int QueryPixelOnScreen::ReadMessageBody(const Stream& msg, 
                                        const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int read = 0;
        int expected = 0;
        read += msg.Read(mX);
        expected += Jaus::JAUS_FLOAT_SIZE;

        read += msg.Read(mY);
        expected += Jaus::JAUS_FLOAT_SIZE;

        if(read == expected)
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
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
QueryPixelOnScreen& QueryPixelOnScreen::operator=(const QueryPixelOnScreen& query)
{
    CopyHeaderData(&query);
    mX = query.mX;
    mY = query.mY;

    return *this;
}

/*  End of File */
