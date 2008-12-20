////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcomponentcontrol.h
///  \brief This file contains data structures for the JAUS_REPORT_COMPONENT_CONTROL
///  message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 9 July 2007
///  <br>Last Modified: 9 July 2007
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
#ifndef __JAUS_REPORT_COMPONENT_CONTROL_H
#define __JAUS_REPORT_COMPONENT_CONTROL_H

#include "jaus/messages/message.h"

#ifdef __cplusplus


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportComponentControl
    ///   \brief Part of the core set of messages in JAUS.   
    ///   
    ///   This message informs the receiver of the current component that has
    ///   control.  The ID fields shall be set to zero (0) if there is not component
    ///   in control.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportComponentControl : public Message
    {
    public:
        ReportComponentControl();
        ReportComponentControl(const ReportComponentControl& query);
        virtual ~ReportComponentControl();
        int SetControllerID(const Address& id);
        int SetControllerAuthorityCode(const Byte code);
        Byte GetControllerAuthorityCode() const { return mAuthorityCode; }
        Address GetControllerID() const { return mAuthorityAddress; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual Message* Clone() const { return new ReportComponentControl(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody() {mAuthorityAddress(0, 0, 0, 0); mAuthorityCode = 0;}
        ReportComponentControl& operator=(const ReportComponentControl& query);
    protected:
        Address mAuthorityAddress;  ///<  JAUS ID of controlling component.
        Byte mAuthorityCode;        ///<  Authority code of controlling component.
    };  
}

#endif

#endif
/*  End of File */
