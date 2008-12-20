////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportidentification.h
///  \brief This file contains the message for Report Identification.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 24 July 2007
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
#ifndef __JAUS_REPORT_IDENTIFICATION_H
#define __JAUS_REPORT_IDENTIFICATION_H

#include "jaus/messages/message.h"
#include "jaus/messages/common/configuration/identification.h"

#ifdef __cplusplus

namespace Jaus
{    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportIdentification
    ///   \brief Report identification from a component.   
    ///   
    ///   This message shall provide the requesting component an identification
    ///   summary of the subsystem, node, or component.
    ///
    ///   Query response type values:<br>
    ///   0 = Reserved<br>
    ///   1 = System Identification<br>
    ///   2 = SS Identification<br>
    ///   3 = Node Identification<br>
    ///   4 = Component Identification<br>
    ///   5 - 255 Reserved<br>
    ///
    ///   Authority is the lowest level of authority required to gain control of
    ///   the subsystem, node, or component capabilities.
    ///
    ///   Type is till undefined, however 0 = reserved, 6000-65535 are reserved.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportIdentification : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Type
        ///   \brief Types of identification being reported.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Type
        {
            Reserved = 0,
            System,
            Subsystem,
            Node,
            Component
        };
        ReportIdentification();
        ReportIdentification(const ReportIdentification &msg);
        virtual ~ReportIdentification();
        void SetQueryType(const Byte query);
        void SetIdentification(const Identification& ident);
        Byte GetQueryType() const { return mQueryType; }
        Identification* GetIdentification() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual int RunTestCase() const;
        virtual Message* Clone() const { return new ReportIdentification(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void Print() const; 
        virtual void ClearMessageBody();
        ReportIdentification& operator=(const Identification& iden);
        ReportIdentification& operator=(const ReportIdentification &msg);
    protected:
        Byte mQueryType;                ///<  The type of identification query the message is responding to.
        Identification mIdentification; ///<  Identification data.
    };
}

#endif

#endif 

/*  End of File */
