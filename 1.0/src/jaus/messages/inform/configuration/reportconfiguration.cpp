////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportconfiguration.cpp
///  \brief This file contains a message for Report Configuration.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 26 July 2007
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
#include "jaus/messages/inform/configuration/reportconfiguration.h"
#include "jaus/messages/inform/informcodes.h"

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor, sets default values.
///
////////////////////////////////////////////////////////////////////////////////////
ReportConfiguration::ReportConfiguration() : Message(JAUS_REPORT_CONFIGURATION)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor, duplicates values.
///
////////////////////////////////////////////////////////////////////////////////////
ReportConfiguration::ReportConfiguration(const ReportConfiguration& msg) : Message(JAUS_REPORT_CONFIGURATION)
{
    CopyHeaderData(&msg);
    mConfiguration = msg.mConfiguration;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportConfiguration::~ReportConfiguration()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the configuration data for the message.
///
///   The configuration data is for the source's subsystem or node depending
///   on the query message you are responding too.  If only node configuration
///   then there is one node present in the Subsystem configuration.
///
///   \param config Configuration data located on the sources subsystem or
///                 node.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportConfiguration::SetConfiguration(const Configuration::Subsystem& config)
{
    mConfiguration = config;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the subsystem or node configuration data to the
///          system configuration from the message.
///
///   \param config Configuration data located on the sources subsystem or
///                 node.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportConfiguration::AddToConfiguration(Configuration& config) const
{
    if(GetSourceID().IsValid())
    {
        if(mConfiguration.mSubsystemID == 0 && 
            mSourceID.mSubsystem != 0)
        {
            ((Configuration::Subsystem*)(&mConfiguration))->mSubsystemID = mSourceID.mSubsystem;
        }
        // If only single node, do not overwrite subsystem data.
        config.AddSubsystemConfiguration(mConfiguration);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets node configuration for reporting. Clears any previous 
///          configuration data.
///
///   \param subsystemID Subsystem number.
///   \param config Node configuration.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportConfiguration::SetConfiguration(const Byte subsystemID, const Configuration::Node& config)
{
    mConfiguration.Clear();
    mConfiguration.mSubsystemID = subsystemID;
    mConfiguration.mNodes[config.mNodeID] = config;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Pointer to the configuration data for the message.  Use this
///           to add/get/modify configuration data.
///
////////////////////////////////////////////////////////////////////////////////////
Configuration::Subsystem* ReportConfiguration::GetConfiguration() const
{
    return (Configuration::Subsystem*)(&mConfiguration);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method takes serialized JAUS message data in a Stream and
///          extracts it, saving to internal data members.  Message extracts
///          header and message body information.
///
///   Based on the version desired, the message will read the data
///   appropriately if supported.
///
///   \param msg The message stream to read from.
///
///   \return Number of bytes read/read, 0 on failure, check 
///           GetLastJausError() for reason.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportConfiguration::Read(const Stream& msg)
{
    int result = 0;
    if((result = Message::Read(msg)) > 0)
    {
        mConfiguration.mSubsystemID = GetSourceID().mSubsystem;
    }
    return result;   
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
int ReportConfiguration::WriteMessageBody(Stream& msg, 
                                          const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;
        expected = JAUS_BYTE_SIZE;
        written += msg.WriteByte( (Byte)mConfiguration.mNodes.size() );
        Configuration::Node::Map::const_iterator node;
        for(node = mConfiguration.mNodes.begin();
            node != mConfiguration.mNodes.end();
            node++)
        {
            // Write the node number.
            if( node->first == 0 || node->first == 255 )
            {
                SetJausError(ErrorCodes::InvalidValue); return -1;
            }

            expected += JAUS_BYTE_SIZE;
            written += msg.WriteByte( node->first );
            expected += JAUS_BYTE_SIZE;
            written += msg.WriteByte( (Byte)node->second.mComponents.size() );

            Configuration::Component::Set::const_iterator cmp;
            for(cmp = node->second.mComponents.begin();
                cmp != node->second.mComponents.end();
                cmp++)
            {
                if(cmp->mID == 0 || cmp->mID == 255 ||
                    cmp->mInstance == 0 || cmp->mInstance == 255)
                {
                    SetJausError(ErrorCodes::InvalidValue); return -1;
                }
                expected += JAUS_BYTE_SIZE*2;
                written += msg.WriteByte( cmp->mID );
                written += msg.WriteByte( cmp->mInstance );
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
int ReportConfiguration::ReadMessageBody(const Stream& msg, 
                                         const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        // Clear configuration data.
        mConfiguration.Clear();

        Byte ncount = 0, ccount = 0, nid, cid, iid; // temp1, temp 2

        int read = 0;
        int expected = 0;

        expected = JAUS_BYTE_SIZE;
        read += msg.Read(ncount); // Node count.

        for(Byte n = 0; n < ncount; n++)
        {
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(nid);  // Node ID
            if( nid == 0 || nid == 255 )
            {
                SetJausError(ErrorCodes::InvalidValue); return -1;
            }
            // Now read the number of components
            expected += JAUS_BYTE_SIZE;
            read += msg.Read(ccount);

            for(Byte c = 0; c < ccount; c++)
            {
                expected += JAUS_BYTE_SIZE*2;
                read += msg.Read(cid);
                read += msg.Read(iid);
                if( cid == 0 || cid == 255 ||
                    iid == 0 || iid == 255 )
                {
                    SetJausError(ErrorCodes::InvalidValue); return -1;
                }
                mConfiguration.mNodes[nid].mNodeID = nid;
                mConfiguration.mNodes[nid].AddComponent(cid, iid);
            }

            if(ccount == 0)
            {
                // Try read header information if available, so 
                // we can set the subsystem and node ID numbers.
                Header header;
                if(msg.Read(header, 0) && header.mSourceID.IsValid())
                {
                    mConfiguration.AddComponent(header.mSourceID);
                }
            }
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
///   \brief Runs a test case to validate the correctness of the message
///          structure.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportConfiguration::RunTestCase() const
{
    Stream packet;
    int result = 0;
    ReportConfiguration msg1, msg2;

    msg1.mConfiguration.AddComponent(1, 1, 1);
    msg1.mConfiguration.AddComponent(1, 2, 1);
    msg1.mConfiguration.AddComponent(1, 2, 2);
    msg1.mConfiguration.AddComponent(2, 3, 1);
    msg1.mConfiguration.AddComponent(2, 3, 3);

    if( (result = msg1.WriteMessageBody(packet, JAUS_DEFAULT_VERSION)) > 0 && 
        msg1.ReadMessageBody(packet, JAUS_DEFAULT_VERSION) == result )
    {
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message data to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportConfiguration::Print() const
{
    Message::Print();
    mConfiguration.Print();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears the contents of the message.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportConfiguration::ClearMessageBody()
{
    mConfiguration.Clear();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies the configuration data to internal data member.
///
////////////////////////////////////////////////////////////////////////////////////
ReportConfiguration& ReportConfiguration::operator =(const Configuration::Subsystem& config)
{
    mConfiguration = config;
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the message data.
///
////////////////////////////////////////////////////////////////////////////////////
ReportConfiguration& ReportConfiguration::operator =(const ReportConfiguration& msg)
{
    if(this != &msg)
    {
        CopyHeaderData(&msg);
        mConfiguration = msg.mConfiguration;
    }
    return *this;
}

/*  End of File */
