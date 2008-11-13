////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodelogger.h
///  \brief Contains class for creating log information for node manager.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 September 2007
///  <br>Last Modified: 20 March 2008
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
#ifndef __JAUS_NODE_LOGGER_H
#define __JAUS_NODE_LOGGER_H

#include "jaus/messages/types.h"
#include "jaus/messages/stream.h"
#include "jaus/messages/largedataset.h"
#include "jaus/components/streamcallback.h"
#include <cxutils/mutex.h>
#include <string>
#include <map>

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \class NodeLogger
    ///  \brief Logger structure for logging messages that pass through the node
    ///  manager.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL NodeLogger
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \enum Event
        ///  \brief Types of events that take place in the node.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Event
        {
            Connection = 0,
            Disconnect,
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \enum Error
        ///  \brief Error reasons for messages being dumpled/ignored.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Error
        {
            InvalidHeader = ErrorCodes::InvalidHeader, // Bad Header
            UnknownDestination,                        // Unknown destination
            ReceivedByLoopback                         // Received message sent by node 
        };
        NodeLogger();
        ~NodeLogger();
        int CreateLogFile(const Address& nodeID, 
                          const std::string& logName);
        int CloseLogFile();
        int WriteToLog(const std::string& str);
        int WriteToLog(const Address& id, const Byte eventID);
        int WriteSentDataToLog(const Stream& msg, 
                               const Header& info, 
                               const bool success);
        int WriteReceivedDataToLog(const Stream& msg, 
                                   const Header& info, 
                                   const StreamCallback::Transport transport);
        int WriteReceivedDataDumped(const Stream& msg, 
                                    const Header& info, 
                                    const StreamCallback::Transport transport, 
                                    const Error reason);
        ULong GetRecvDataSize() const;
        ULong GetSendDataSize() const;
        bool IsLogOpen() const;
        std::string GetLogFileName() const { return mFileName; }
    protected:
        CxUtils::Mutex mMutex;      ///<  For thread safety!
        FILE* mpLogFile;            ///<  Log file.
        Address mNodeID;            ///<  ID of the node.
        std::string mFileName;      ///<  File name to write to.
        char* mpTempName;           ///<  Temporary name buffer.
        unsigned int mTempNameLen;  ///<  Length of the temporary name buffer.
        ULong mRecvAmount;          ///<  Number of bytes received by manager.
        ULong mSendAmount;          ///<  Number of bytes sent from node manager.
    };
}

#endif
#endif

/*  End of File */
