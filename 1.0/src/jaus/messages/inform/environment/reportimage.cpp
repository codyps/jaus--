////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportimage.cpp
///  \brief This file contains a JAUS message for the Report Image message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 7 May 2008
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
#include "jaus/messages/inform/environment/reportimage.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportImage::ReportImage() : Message(JAUS_REPORT_IMAGE)
{
    mpImageData = 0;
    mDataSize = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportImage::ReportImage(const ReportImage &msg) : Message(JAUS_REPORT_IMAGE)
{
    mpImageData = 0;
    mDataSize = 0;
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportImage::~ReportImage()
{
    if(mpImageData)
    {
        delete[] mpImageData;
    }
    mpImageData = NULL;
    mDataSize = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Set the compressed image/video/audio data.  
///
///  \param img Pointer to image data.
///  \param length Length of the image data in bytes.
///  \param copy If true, a copy of the data is made, if false then 
///              class takes ownership of the pointer.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportImage::SetImageData(const Byte* img, const UInt length, const bool copy)
{
    if( img && length > 0 ) 
    {
        if( copy )
        {
            if ( mpImageData && length != mDataSize ) 
            {
                delete[] mpImageData;
                mpImageData = NULL;
            }

            if(!mpImageData ) 
            {
                mpImageData = new Byte[length];
                mDataSize = length;
            }

            memcpy( mpImageData, img, mDataSize );
        }
        else
        {
            if( mpImageData )
            {
                delete[] mpImageData;
                mpImageData = NULL;
            }
            mpImageData = (Byte*)(img);
            mDataSize = length;
        }
        return JAUS_OK;
    }

    SetJausError(ErrorCodes::InvalidValue);

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
int ReportImage::WriteMessageBody(Stream& msg, 
                                  const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        if( mpImageData && msg.Write(mpImageData, mDataSize)) 
        {
            return mDataSize;
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
int ReportImage::ReadMessageBody(const Stream& msg, 
                                 const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        unsigned int length = msg.Length() - msg.GetReadPos();

        if( SetImageData(msg.Ptr() + msg.GetReadPos(), length, true))
        {
            return length;
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
///   \brief Clears all message data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportImage::ClearMessageBody()
{
    if(mpImageData)
    {
        delete[] mpImageData;
    }
    mpImageData = NULL;
    mDataSize = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
ReportImage &ReportImage::operator=(const ReportImage &msg)
{
    CopyHeaderData(&msg);
    if(msg.mpImageData)
    {
        SetImageData(msg.mpImageData, msg.mDataSize, true);
    }
    else
    {
        ClearMessageBody();
    }
    return *this;
}

/*  End of File */
