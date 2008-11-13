////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportcomponentstatus.h
///  \brief This file contains data structures for the JAUS_REPORT_COMPONENT_STATUS
///  message belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 22 June 2007
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
#ifndef __JAUS_REPORT_COMPONENT_STATUS_H
#define __JAUS_REPORT_COMPONENT_STATUS_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportComponentStatus
    ///   \brief Part of the core set of messages in JAUS.    
    ///   
    ///   Field #1 of the status message indicates the primary status code
    ///   of the component.  This data field is segmented into two four-bit sections.
    ///   The first four-bit section (bits 0-3) is interpreted as a four bit, UInt.
    ///   The second four-bit section is available for vendor specific use.  
    ///   JAUS defines six generic primary status conditions.  The remaining 
    ///   primary status codes of the first four-bit segment (values 6-15) are reserved.
    ///
    ///   Field #2 of the status message is a secondary status code, which is
    ///   used to further qualify the primary status code.  This data field is 
    ///   segmented into two sixteen-bit sections.  The secondary status code
    ///   is interpreted on a bit-by-bit basis, as opposed to the primary status code,
    ///   which is interpreted as an integer value.  The bit-field interpretation allows 
    ///   multiple secondary status codes to be set within the same status message.  For
    ///   instace, in the emergency state, the secondary status bits could indicate 
    ///   specific emergency conditions that have been set.  Secondary status bits are
    ///   defined on a component-by-component basis.
    ///
    ///   Developers can only use the "Available" protions of the status message
    ///   data for debug and testing.  The JAUS specified codes must also be supported, 
    ///   and the vednor specific codes shall not replace information already specified
    ///   within JAUS.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportComponentStatus : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Status
        ///   \brief Class containing enumerations and constants associated with the
        ///          Primary status value of the message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Status
        {
        public:
            ////////////////////////////////////////////////////////////////////////////////////
            ///
            ///   \enum Primary
            ///   \brief Allowed values for bitw 0-3 of Primary Status.
            ///
            ////////////////////////////////////////////////////////////////////////////////////
            enum Primary
            {
                Initialized = 0,
                Ready,
                Standby,
                Shutdown,
                Failure,
                Emergency
            };
        };
        ReportComponentStatus();
        ReportComponentStatus(const ReportComponentStatus &msg);
        ~ReportComponentStatus();
        Byte GetPrimaryStatusCode() const { return mPrimaryStatusCode; }
        UInt GetSecondaryStatusCode() const { return mSecondaryStatusCode; }
        int SetPrimaryStatusCode(const Byte code);
        int SetSecondaryStatusCode(const UInt code);
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportComponentStatus(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        ReportComponentStatus &operator=(const ReportComponentStatus &msg);
    protected:
        Byte mPrimaryStatusCode;    ///<  Primary status code.
        UInt mSecondaryStatusCode;  ///<  Secondary status code.
    };
}

#endif

#endif 

/*  End of File */
