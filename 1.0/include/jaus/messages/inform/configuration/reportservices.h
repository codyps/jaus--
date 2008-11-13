////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportservices.h
///  \brief This file  contains a message structure for Report Services.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 July 2007
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
#ifndef __JAUS_REPORT_SERVICES__H
#define __JAUS_REPORT_SERVICES__H

#include "jaus/messages/message.h"
#include "jaus/messages/common/configuration/service.h"

#ifdef __cplusplus

namespace Jaus
{    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportServices
    ///   \brief Message for reporting services for a component.
    ///
    ///   This message allows a component to publish its capabilities, according to
    ///   the service dictionary presented in the JAUS_RA.  If a component ID is
    ///   specified in the RA, it may report only one service in beyond the core
    ///   message support, and this service must be equal to the component ID.  If
    ///   a component ID is not listed in the RA, it may report any number of
    ///   services.  For example, a component with ID 33 must provide only service
    ///   33.  The exception to this rule is component ID 1 (the Node Manager)
    ///   which may provide any number of services in addition to core message
    ///   support.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportServices : public Message
    {
    public:
        ReportServices();
        ReportServices(const ReportServices &msg);
        ~ReportServices();        
        void SetServices(const Service::Set& services);
        Service::Set* GetServices() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual int RunTestCase() const;
        virtual Message* Clone() const { return new ReportServices(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void Print() const; 
        virtual void ClearMessageBody();
        ReportServices &operator=(const ReportServices &msg);
    protected:
        Service::Set mServices;   ///<  Services to include in message.
    };
}

#endif

#endif 

/*  End of File */
