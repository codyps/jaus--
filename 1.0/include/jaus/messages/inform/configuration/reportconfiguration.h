////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportconfiguration.h
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
#ifndef __JAUS_REPORT_CONFIGURATION__H
#define __JAUS_REPORT_CONFIGURATION__H

#include "jaus/messages/message.h"
#include "jaus/messages/common/configuration/configuration.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportConfiguration
    ///   \brief Report configuration of a subsystem or node.   
    ///   
    ///   This message shall provide the receiving component a table of all
    ///   existing components located on the source's subsystem or node depending on
    ///   the value of field 1, (Type), of the Query Configuration messages. 
    ///
    ///   If reporting node configuration, just include one node in the subsystem
    ///   configuration data.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportConfiguration : public Message
    {
    public:
        ReportConfiguration();
        ReportConfiguration(const ReportConfiguration& msg);
        ~ReportConfiguration();
        void SetConfiguration(const Configuration::Subsystem& config);
        void AddToConfiguration(Configuration& config) const;
        void SetConfiguration(const Byte subsystemID, const Configuration::Node& config);
        Configuration::Subsystem* GetConfiguration() const;
        virtual int Read(const Stream& msg);
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual int RunTestCase() const;
        virtual Message* Clone() const { return new ReportConfiguration(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void Print() const; 
        virtual void ClearMessageBody();
        ReportConfiguration& operator=(const Configuration::Subsystem& config);
        ReportConfiguration& operator=(const ReportConfiguration& msg);
    protected:
        Configuration::Subsystem mConfiguration; ///<  Configuration data.
    };
}

#endif
/*  End of File */
