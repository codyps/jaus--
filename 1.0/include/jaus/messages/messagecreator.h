////////////////////////////////////////////////////////////////////////////////////
///
///  \file messagecreator.h
///  \brief This file contains methods for allocating Message based 
///  structures based on desired command code.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 26 February 2007
///  <br>Last Modified: 5 February 2009
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
#ifndef __JAUS_MESSAGE_CREATOR_H
#define __JAUS_MESSAGE_CREATOR_H

#include "jaus/messages/message.h"
#include "jaus/messages/command/commandcodes.h"
#include "jaus/messages/query/querycodes.h"
#include "jaus/messages/inform/informcodes.h"
#include "jaus/messages/experimental/experimentalcodes.h"
#include <set>
#include <string>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///    \class MessageCreator
    ///    \brief Class created for dynamic creation of Message
    ///    based structures using class codes.
    ///
    ///    Current implementation has messages hard-coded into
    ///    function, but future capabilities will including
    ///    XML parsing for dynamic message creation.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL MessageCreator
    {
    public:
        MessageCreator();
        ~MessageCreator();
        static int AddCustomMessage(Message* customMessage);
        static UShort GetPresenceVectorSize(const UShort code, 
                                            const UShort version = JAUS_DEFAULT_VERSION,
                                            bool* successFlag = NULL);
        static UInt GetPresenceVectorMask(const UShort code, 
                                          const UShort version = JAUS_DEFAULT_VERSION,
                                          bool* successFlag = NULL);
        static int GetPresenceVector(const Stream &msg, UInt& v);
        static int GetPresenceVector(const Byte* buff, const unsigned int len, UInt &v);        
        static int RunTestCase(const UShort code);
        static void CleanupMessageCreator();
        static UShort GetResponseCodes(const UShort mcode, std::set<UShort>* responses = NULL);
        static UShort GetInformQueryType(const UShort informCode);
        static UShort GetQueryResponseType(const UShort queryCode);
        static std::string GetMessageString(const UShort code);
        static std::string GetCommandMessageString(const UShort code);
        static std::string GetQueryMessageString(const UShort code);
        static std::string GetInformMessageString(const UShort code);
        static std::string GetExperimentalMessageString(const UShort code);
        static Message* CreateMessage(const UShort code);
        static Message* CreateMessage(const Stream &msg, const Header* info);
        static Message* CreateCommandMessage(const UShort code);
        static Message* CreateQueryMessage(const UShort code);
        static Message* CreateInformMessage(const UShort code);
        static Message* CreateExperimentalMessage(const UShort code);                
    };
    typedef MessageCreator MessageFactory;  ///<  Message Creator is basically a factory for creating messages.
}

#endif //  __JAUS_MESSAGE_CREATOR_H
/*  End of File */
