////////////////////////////////////////////////////////////////////////////////////
///
///  \file time.h
///  \brief Contains the Time class which is used to store time information in
///  the JAUS library.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 25 February 2008
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
#ifndef _JAUS_TIME_H
#define _JAUS_TIME_H

#include "jaus/messages/types.h"
#include "bitvector.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Time
    ///   \brief Structure for storing, extracting, and updating JAUS time
    ///          values.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Time
    {
    public:
        Time();
        Time(const Time &t) { *this = t; }
        ~Time();
        int SetTime(const UInt d,
                    const UInt h,
                    const UInt m,
                    const UInt s,
                    const UInt ms);
        int GetTime(UInt &d,
                    UInt &h,
                    UInt &m,
                    UInt &s,
                    UInt &ms) const;    
        UInt ToUInt() const;
        UInt Hour() const { return mHour; }
        UInt Minute() const { return mMinute; }
        UInt Second() const { return mSecond; }
        UInt Milliseconds() const { return mMilliseconds; }
        UInt Day() const { return mDay; }
        void PrintTime() const;
        Time& SetTime(const UInt time);
        Time& SetCurrentTime();      
        static Time GetUtcTime();
        static unsigned int GetUtcTimeMs();    
        operator UInt() const { return ToUInt(); }
        Time &operator=(const Time &t);
        bool operator==(const Time &t) const;
        bool operator!=(const Time &t) const { return !(*this==t); }        
        bool operator>(const Time &t) const;
        bool operator<(const Time &t) const;
    protected:        
        volatile UInt mMilliseconds;          ///<  Current time milliseconds [0,999]
        volatile UInt mSecond;                ///<  Current seconds [0,59]
        volatile UInt mMinute;                ///<  Current minutes [0,59]
        volatile UInt mHour;                  ///<  Current hour [0,23]
        volatile UInt mDay;                   ///<  Current day from time stamp [1,31]. 
    };


} //  End of Jaus namespace

#endif

#endif

/* End of File */
