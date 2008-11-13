////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodelogger.cpp
///  \brief Contains class for creating log information for node manager.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 September 2007
///  <br>Last Modified: 4 March 2008
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
#include "jaus/components/node/nodelogger.h"
#include "jaus/components/node/nodeconnectionhandler.h"
#include "jaus/messages/time.h"
#include "jaus/messages/messagecreator.h"
#include <cxutils/timer.h>
#include <cxutils/fileio.h>

using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeLogger::NodeLogger()
{
    mpLogFile = NULL;
    mRecvAmount = mSendAmount = 0;
    mTempNameLen = 256;
    mpTempName = new char[mTempNameLen];
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeLogger::~NodeLogger()
{
    CloseLogFile();
    delete[] mpTempName;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Creates an XML log file.
///
///  \param nodeID The ID of the node manager.
///  \param file The file name to save to.
///
///  \return JAUS_OK if created, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::CreateLogFile(const Address& nodeID, const std::string& file)
{
    int result = JAUS_FAILURE;
    CloseLogFile();

    mMutex.Enter();

    if( nodeID.IsValid() && nodeID.IsBroadcast() == false )
    {
        std::string path;
        CxUtils::FileIO::GetPath(file, path);
        CxUtils::FileIO::CreateDir(path);
        mpLogFile = fopen( file.c_str(), "w+t" );
        if( mpLogFile )
        {
            mNodeID = nodeID;
            fprintf(mpLogFile, "<?xml version=\"1.0\" ?>\n");
            fprintf(mpLogFile, "<Jaus>\n");
            fprintf(mpLogFile, "\t<NodeLog verions=\"1.0\" subsystem=\"%d\" node=\"%d\">\n", mNodeID.mSubsystem, mNodeID.mNode );
            result = JAUS_OK;
            mFileName = file;
        }
    }

    mMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Closes an open XML file.
///
///  \return JAUS_OK if closed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::CloseLogFile()
{
    mMutex.Enter();

    if( mpLogFile )
    {
        fprintf(mpLogFile, "\t</NodeLog>\n");
        fprintf(mpLogFile, "</Jaus>\n");
        fclose(mpLogFile);
        mpLogFile = NULL;
    }

    mMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Writes the log message string to log file.
///
///  \param str String message to add to log.
///
///  \return JAUS_OK if wrote to log, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::WriteToLog(const std::string& str)
{
    int result = JAUS_FAILURE;

    if(mpLogFile)
    {
        result = JAUS_OK;
        mMutex.Enter();

        fprintf(mpLogFile,
                    "\t\t<LogMessage time=\"%u\">%s</LogMessage>\n",
                    (unsigned int)Time::GetUtcTimeMs(), str.c_str());

        mMutex.Leave();
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Writes event information for a specific JAUS ID.
///
///  \param id The JAUS ID that an even happened.
///  \param eventID Event type, 0 = connected, 1 = disconnected.
///
///  \return JAUS_OK if wrote to log, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::WriteToLog(const Address& id, const Byte eventID)
{
    int result = JAUS_FAILURE;

    if( mpLogFile &&
        id.IsValid() && !id.IsBroadcast())
    {
        result = JAUS_OK;
        mMutex.Enter();

        if( eventID == NodeConnectionHandler::ComponentConnect || eventID == NodeConnectionHandler::NodeConnect  )
        {
            fprintf(mpLogFile,
                    "\t\t<Connection time=\"%u\" subsystem=\"%d\" node=\"%d\" component=\"%d\" instance=\"%d\" />\n",
                    (unsigned int)Time::GetUtcTimeMs(), id.mSubsystem, id.mNode, id.mComponent, id.mInstance);
        }
        else if( eventID == NodeConnectionHandler::ComponentDisconnect || eventID == NodeConnectionHandler::NodeDisconnect )
        {
            fprintf(mpLogFile,
                    "\t\t<Disconnect time=\"%u\" subsystem=\"%d\" node=\"%d\" component=\"%d\" instance=\"%d\" />\n",
                    (unsigned int)Time::GetUtcTimeMs(), id.mSubsystem, id.mNode, id.mComponent, id.mInstance);
        }
        mMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Updates the log file with message data sent from the node to
///         components or other nodes.
///
///  \param msg Packet data to log.
///  \param info Read header for packet.
///  \param success If true, message was sent, otherwise error.
///
///  \return JAUS_OK if wrote to log, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::WriteSentDataToLog(const Stream& msg,
                                   const Header& info,
                                   const bool success)
{
    int result = JAUS_FAILURE;

    mMutex.Enter();
    mSendAmount += msg.Length();
    if( mpLogFile )
    {
        UInt pv = 0;
        std::string name = MessageCreator::GetMessageString(info.mCommandCode);
        if(info.mCommandCode == JAUS_EVENT)
        {
            UShort eventType = 0;
            if((info.mDataFlag == Header::DataControl::Single || info.mDataFlag == Header::DataControl::First)
                && msg.Read(eventType, JAUS_HEADER_SIZE + JAUS_BYTE_SIZE))
            {
                name += " - ";
                name += MessageCreator::GetMessageString(eventType);
            }
        }

        if(name.length() < 30)
        {
            memset(mpTempName, ' ', 30);
            mpTempName[30] = 0;
            memcpy(mpTempName, name.c_str(), name.length());
            mpTempName[name.length()] = '\"';
        }
        else
        {
            if(name.length() >= mTempNameLen)
            {
                delete[] mpTempName;
                mTempNameLen = (unsigned int)(name.length() + 3);
                mpTempName = new char[mTempNameLen];
            }
            name.push_back('\"');
            strcpy(mpTempName, name.c_str());
        }

        if(name.length() > 1)
        {
            fprintf(mpLogFile,
                    "\t\t<Message time=\"%u\" name=\"%s bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    mpTempName,
                    msg.Length() );
        }
        else
        {
            fprintf(mpLogFile,
                    "\t\t<Message time=\"%u\" code=\"0x%.4X\" bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    info.mCommandCode,
                    msg.Length() );
        }
        fprintf(mpLogFile,
                "sn=\"%.5u\" ",
                 info.mSequenceNumber);
        fprintf(mpLogFile,
                "src=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mSourceID.mSubsystem,
                 info.mSourceID.mNode,
                 info.mSourceID.mComponent,
                 info.mSourceID.mInstance);
        fprintf(mpLogFile,
                "dest=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mDestinationID.mSubsystem,
                 info.mDestinationID.mNode,
                 info.mDestinationID.mComponent,
                 info.mDestinationID.mInstance);

        if(success)
        {
            fprintf(mpLogFile, "sent=\"OK\" ");
        }
        else
        {
            fprintf(mpLogFile, "sent=\"FAILED\" ");
        }
        fprintf(mpLogFile,
                "dc=\"%u\" ",
                 info.mDataFlag);
        fprintf(mpLogFile,
                "priority=\"%u\" ",
                 info.mPriority);
        fprintf(mpLogFile,
                "acknack=\"%u\" ",
                 info.mAckNack);
        fprintf(mpLogFile,
                "sc=\"%u\" ",
                 info.mServiceConnectionFlag);
        MessageCreator::GetPresenceVector(msg, pv);
        fprintf(mpLogFile,
                "pv=\"%.8X\" ",
                pv);
        fprintf(mpLogFile, "/>\n");
    }

    mMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Updates the log file with message information of data received
///  by the node.
///
///  \param msg Packet data to log.
///  \param info Read header for packet.
///  \param transport Transport the message was received or sent from.
///
///  \return JAUS_OK if wrote to log, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::WriteReceivedDataToLog(const Stream& msg,
                                       const Header& info,
                                       const StreamCallback::Transport transport)
{
    int result = JAUS_FAILURE;

    mMutex.Enter();
    mRecvAmount += msg.Length();
    if( mpLogFile )
    {
        UInt pv = 0;
        std::string name = MessageCreator::GetMessageString(info.mCommandCode);
        if(info.mCommandCode == JAUS_EVENT)
        {
            UShort eventType = 0;
            if((info.mDataFlag == Header::DataControl::Single || info.mDataFlag == Header::DataControl::First)
                && msg.Read(eventType, JAUS_HEADER_SIZE + JAUS_BYTE_SIZE))
            {
                name += " - ";
                name += MessageCreator::GetMessageString(eventType);
            }
        }

        if(name.length() < 30)
        {
            memset(mpTempName, ' ', 30);
            mpTempName[30] = 0;
            memcpy(mpTempName, name.c_str(), name.length());
            mpTempName[name.length()] = '\"';
        }
        else
        {
            if(name.length() >= mTempNameLen)
            {
                delete[] mpTempName;
                mTempNameLen = (unsigned int )(name.length() + 3);
                mpTempName = new char[mTempNameLen];
            }
            name.push_back('\"');
            strcpy(mpTempName, name.c_str());
        }

        if(name.length() > 1)
        {
            fprintf(mpLogFile,
                    "\t\t<Message time=\"%u\" name=\"%s bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    mpTempName,
                    msg.Length() );
        }
        else
        {
            fprintf(mpLogFile,
                    "\t\t<Message time=\"%u\" code=\"0x%.4X\" bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    info.mCommandCode,
                    msg.Length() );
        }
        fprintf(mpLogFile,
                "sn=\"%.5u\" ",
                 info.mSequenceNumber);
        fprintf(mpLogFile,
                "src=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mSourceID.mSubsystem,
                 info.mSourceID.mNode,
                 info.mSourceID.mComponent,
                 info.mSourceID.mInstance);
        fprintf(mpLogFile,
                "dest=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mDestinationID.mSubsystem,
                 info.mDestinationID.mNode,
                 info.mDestinationID.mComponent,
                 info.mDestinationID.mInstance);
        switch(transport)
        {
        case StreamCallback::SharedMemory:
            fprintf(mpLogFile, "received=\"shared_memory\" ");
            break;
        case StreamCallback::UDP:
            fprintf(mpLogFile, "received=\"udp\" ");
            break;
        case StreamCallback::TCP:
            fprintf(mpLogFile, "received=\"tcp\" ");
            break;
        case StreamCallback::Serial:
            fprintf(mpLogFile, "received=\"serial\" ");
            break;
        case StreamCallback::Communicator:
            fprintf(mpLogFile, "received=\"communicator\" ");
            break;
        default:
            fprintf(mpLogFile, "received=\"shared_memory\" ");
            break;
        }
        fprintf(mpLogFile,
                "dc=\"%u\" ",
                 info.mDataFlag);
        fprintf(mpLogFile,
                "priority=\"%u\" ",
                 info.mPriority);
        fprintf(mpLogFile,
                "acknack=\"%u\" ",
                 info.mAckNack);
        fprintf(mpLogFile,
                "sc=\"%u\" ",
                 info.mServiceConnectionFlag);
        MessageCreator::GetPresenceVector(msg, pv);
        fprintf(mpLogFile,
                "pv=\"%.8X\" ",
                pv);
        fprintf(mpLogFile, "/>\n");
    }

    mMutex.Leave();

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Updates the log file with message information of data received
///  by the node that was dumped (not routed).
///
///  \param msg Packet data to log.
///  \param info Read header for packet.
///  \param transport Transport the message was received or sent from.
///  \param reason Why the message was dumped.
///
///  \return JAUS_OK if wrote to log, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int NodeLogger::WriteReceivedDataDumped(const Stream& msg,
                                        const Header& info,
                                        const StreamCallback::Transport transport,
                                        const Error reason)
{
    int result = JAUS_FAILURE;

    mMutex.Enter();
    mRecvAmount += msg.Length();
    if( mpLogFile )
    {
        UInt pv = 0;
        std::string name = MessageCreator::GetMessageString(info.mCommandCode);
        if(info.mCommandCode == JAUS_EVENT)
        {
            UShort eventType = 0;
            if((info.mDataSize == Header::DataControl::Single || info.mDataFlag == Header::DataControl::First)
                && msg.Read(eventType, JAUS_HEADER_SIZE + JAUS_BYTE_SIZE))
            {
                name += " - ";
                name += MessageCreator::GetMessageString(eventType);
            }
        }

        if(name.length() < 30)
        {
            memset(mpTempName, ' ', 30);
            mpTempName[30] = 0;
            memcpy(mpTempName, name.c_str(), name.length());
            mpTempName[name.length()] = '\"';
        }
        else
        {
            if(name.length() >= mTempNameLen)
            {
                delete[] mpTempName;
                mTempNameLen = (unsigned int )(name.length() + 3);
                mpTempName = new char[mTempNameLen];
            }
            name.push_back('\"');
            strcpy(mpTempName, name.c_str());
        }

        if(name.length() > 1)
        {
            fprintf(mpLogFile,
                    "\t\t<Dumped Message time=\"%u\" name=\"%s bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    mpTempName,
                    msg.Length() );
        }
        else
        {
            fprintf(mpLogFile,
                    "\t\t<Dumped Message time=\"%u\" code=\"0x%.4X\" bytes=\"%.4u\" ",
                    (unsigned int)Time::GetUtcTimeMs(),
                    info.mCommandCode,
                    msg.Length() );
        }

        switch(reason)
        {
        case InvalidHeader:
            fprintf(mpLogFile, "reason=\"invalid header\" ");
            break;
        case UnknownDestination:
            fprintf(mpLogFile, "reason=\"unknown destination\" ");
            break;
        case ReceivedByLoopback:
            fprintf(mpLogFile, "reason=\"recived sent message via loopback\" ");
            break;
        default:
            fprintf(mpLogFile, "reason=\"bad data\" ");
            break;
        }

        fprintf(mpLogFile,
                "sn=\"%.5u\" ",
                 info.mSequenceNumber);
        fprintf(mpLogFile,
                "src=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mSourceID.mSubsystem,
                 info.mSourceID.mNode,
                 info.mSourceID.mComponent,
                 info.mSourceID.mInstance);
        fprintf(mpLogFile,
                "dest=\"%.3u.%.3u.%.3u.%.3u\" ",
                 info.mDestinationID.mSubsystem,
                 info.mDestinationID.mNode,
                 info.mDestinationID.mComponent,
                 info.mDestinationID.mInstance);
        switch(transport)
        {
        case StreamCallback::SharedMemory:
            fprintf(mpLogFile, "received=\"shared_memory\" ");
            break;
        case StreamCallback::UDP:
            fprintf(mpLogFile, "received=\"udp\" ");
            break;
        case StreamCallback::TCP:
            fprintf(mpLogFile, "received=\"tcp\" ");
            break;
        case StreamCallback::Serial:
            fprintf(mpLogFile, "received=\"serial\" ");
            break;
        case StreamCallback::Communicator:
            fprintf(mpLogFile, "received=\"communicator\" ");
            break;
        default:
            fprintf(mpLogFile, "received=\"shared_memory\" ");
            break;
        }
        fprintf(mpLogFile,
                "dc=\"%u\" ",
                 info.mDataFlag);
        fprintf(mpLogFile,
                "priority=\"%u\" ",
                 info.mPriority);
        fprintf(mpLogFile,
                "acknack=\"%u\" ",
                 info.mAckNack);
        fprintf(mpLogFile,
                "sc=\"%u\" ",
                 info.mServiceConnectionFlag);
        MessageCreator::GetPresenceVector(msg, pv);
        fprintf(mpLogFile,
                "pv=\"%.8X\" ",
                pv);
        fprintf(mpLogFile, "/>\n");
    }

    mMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \return True if a log file is open, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeLogger::IsLogOpen() const
{
    bool result = false;
    mMutex.Enter();
    if( mpLogFile )
        result = true;
    mMutex.Leave();
    return result;
}


/* End of File */
