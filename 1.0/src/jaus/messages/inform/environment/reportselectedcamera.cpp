////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcameracount.h
///  \brief This file contains the message structure for Report Selected Camera.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 15 August 2008
///  <br>Last Modified: 15 August 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cholland@ist.ucf.edu
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
#include "jaus/messages/inform/environment/reportselectedcamera.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor. Sets the class code.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSelectedCamera::ReportSelectedCamera() : 
    Message(JAUS_REPORT_SELECTED_CAMERA),
    mCameraID(0)
{
    //nothing
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSelectedCamera::ReportSelectedCamera(const ReportSelectedCamera &msg) :
    Message(JAUS_REPORT_SELECTED_CAMERA),
    mCameraID(0)
{
    *this = msg;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSelectedCamera::~ReportSelectedCamera()
{
    //nothing
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the number of cameras
///
///   \param value The number of cameras.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportSelectedCamera::SetCameraID(const Byte value)
{
    if(value > 0)
    {
        mCameraID = value;
        return JAUS_OK;
    }
    else
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
int ReportSelectedCamera::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        written += msg.Write(mCameraID);
        expected += JAUS_BYTE_SIZE;

        if( expected == written )
        {
            return written;
        }        
        SetJausError(ErrorCodes::WriteFailure); 
        return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); 
        return -1;
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
int ReportSelectedCamera::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        read += msg.Read(mCameraID);
        expected += JAUS_BYTE_SIZE;

        if( expected == read )
        {
            return read;
        }
        
        
        SetJausError(ErrorCodes::ReadFailure); 
        return -1;
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); 
        return -1;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportSelectedCamera::ClearMessageBody()
{
    mCameraID = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportSelectedCamera &ReportSelectedCamera::operator=(const ReportSelectedCamera &msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);

        mCameraID = msg.mCameraID;
    }

    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Test case for message transmission
///
///   \return JAUS_OK for success, JAUS_FAILURE for errors
///
////////////////////////////////////////////////////////////////////////////////////
int ReportSelectedCamera::RunTestCase() const
{
    Stream packet;          // Used to store serialized form of message.
    ReportSelectedCamera sendingMessage;  // Stores message data natively.
    ReportSelectedCamera receivedMessage; // Stores message data natively.

    // Set the destination and source of the message (who to send to, who did the sending).
    sendingMessage.SetDestinationID( Address(1, 2, 3, 4) );
    sendingMessage.SetSourceID( Address(1, 1, 1, 1) );
    // If it is a high priority message, indicate that
    sendingMessage.SetPriority( Header::Priority::High );
    // If you want acknowledgement, say so!
    sendingMessage.SetAckNack( Header::AckNack::Request );

    // Set the message fields
    sendingMessage.SetCameraID(65);


    //Copy our message to a new object, then clone that object so that we're
    //dealing with a clone of a copy of the original. They should all have
    //identical data. Examine the output to make sure. 

    ReportSelectedCamera copiedMessage(sendingMessage);
    ReportSelectedCamera *clonedMessage = dynamic_cast<ReportSelectedCamera*>(copiedMessage.Clone());

    // Now that we have a populated message structure, lets
    // serialize it so we can transmit it to other components.

    if(clonedMessage->Write(packet))
    {
        cout << "Size of serialized data is (including header): " << packet.Length() << endl;
        // Now lets pretend we just received a message stream (serialized data)
        // and read it out (de-serialize).
        if(receivedMessage.Read(packet))
        {
            // Data should match! Check the output to make sure through the debug windows
        }
        else
        {
            cout << "Error: Could not de-serialize message.\n";
            return JAUS_FAILURE;
        }
    }
    else
    {
        cout << "Error: Could not serialize message data.\n";
        return JAUS_FAILURE;
    }

    delete clonedMessage;
    clonedMessage = NULL;

    copiedMessage.ClearMessageBody();
    sendingMessage.ClearMessageBody();

    return JAUS_OK;
}

/*  End of File */
