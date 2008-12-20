////////////////////////////////////////////////////////////////////////////////////
///
///  \file reporttime.h
///  \brief This file contains data structures for the JAUS_REPORT_TIME.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 29 February 2008
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
#ifndef __JAUS_REPORT_TIME_H
#define __JAUS_REPORT_TIME_H

#include "jaus/messages/message.h"
#include "jaus/messages/time.h"
#include "jaus/messages/date.h"

#ifdef __cplusplus

namespace Jaus
{    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportTime
    ///   \brief Tells the receiving component what the current time of the
    ///   sending component.
    ///   
    ///   This message provides the receiver with the current time as
    ///   managed by the sending component.  The message data and mapping
    ///   of the presence vector of this message are identical to
    ///   code 0x0011 (Set Time).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportTime : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                Time = 0x01, // Field mask for time.
                Date = 0x02  // Field mask for date.
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                Time = 0,    // Bit number in PV for time.
                Date         // Bit number in PV for date.          
            };
        };
        ReportTime();
        ReportTime(const ReportTime &time);
        virtual ~ReportTime();
        int SetTimeStamp(const Time& t);
        int SetDateStamp(const Date& d);
        void ClearTime();
        void ClearDate();
        const Time* GetTimeStamp() const { return mpTimeStamp; }
        const Date* GetDateStamp() const { return mpDateStamp; }
        Byte GetPresenceVector() const { return mPresenceVector; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportTime(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ReportTime &operator=(const ReportTime &msg);
    protected:
        Byte mPresenceVector;   ///<  Presence vector for mesage.
        Time* mpTimeStamp;      ///<  Possible time stamp data if set.
        Date* mpDateStamp;      ///<  Possible date stamp data if set.
    };
}

#endif

#endif 

/*  End of File */
