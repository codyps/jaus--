////////////////////////////////////////////////////////////////////////////////////
///
///  \file time.cpp
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
#include "jaus/messages/time.h"
#include <iostream>
#include <assert.h>
#include <time.h>
#include <cxutils/timer.h>

using namespace std;

#ifdef WIN32
    #include <windows.h>
#else
    #include <sys/time.h>
#endif

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Time::Time()
{
    mMilliseconds = 0;
    mSecond = 0;
    mMinute = 0;
    mHour = 0;
    mDay = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Time::~Time()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the time stamp by its components.
///
///   \param ms Milliseconds.
///   \param s Seconds.
///   \param m Minutes.
///   \param h Hour.
///   \param d Day.
///
///   \return JAUS_OK on success, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Time::SetTime(const UInt d,
                  const UInt h,
                  const UInt m,
                  const UInt s,
                  const UInt ms)
{
    mMilliseconds = ms;
    mSecond = s;
    mMinute = m;
    mHour = h;
    mDay = d;

    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the time stamp by its components.
///
///   \param ms Milliseconds.
///   \param s Seconds.
///   \param m Minutes.
///   \param h Hour.
///   \param d Day.
///
///   \return JAUS_OK on success, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Time::GetTime(UInt &d,
                  UInt &h,
                  UInt &m,
                  UInt &s,
                  UInt &ms) const
{
    ms = mMilliseconds;
    s = mSecond;
    m = mMinute;
    h = mHour;
    d = mDay;

    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a UInt of the time information in the format defined by the
///          JAUS RA.
///
///   Bits 0-9: milliseconds, range 0, 999<br>
///   Bits 10-15: Seconds, range 0, 59<br>
///   Bits 16-21: Minutes, range 0, 59<br>
///   Bits 22-26: Hour (24 hour clock), range 0, 23<br>
///   Bits 27-31: Day, range 1, 31<br>
///
///   \return The timestamp data in JAUS RA format.
///
////////////////////////////////////////////////////////////////////////////////////
UInt Time::ToUInt() const
{
    UInt tstamp = 0;
    tstamp |= (UInt)(mDay)     << 27;
    tstamp |= (UInt)(mHour)    << 22;
    tstamp |= (UInt)(mMinute) << 16;
    tstamp |= (UInt)(mSecond) << 10;
    tstamp |= mMilliseconds;

    return tstamp;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints the date and time.
///
////////////////////////////////////////////////////////////////////////////////////
void Time::PrintTime() const
{
    cout << "Time Stamp: " << mDay << ":" << mHour << ":" << mMinute;
    cout << ":" << mSecond << ":" << mMilliseconds << endl;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the time using a UInt in JAUS RA Time format.
///
///   \param tstamp Bits 0-9: milliseconds, range 0, 9999
///                 Bits 10-15: Seconds, range 0, 59
///                 Bits 16-21: Minutes, range 0, 59
///                 Bits 22-26: Hour (24 hour clock), range 0, 23
///                 Bits 27-31: Day, range 1, 31
///
///   \return Current instance of the Time data.
///
////////////////////////////////////////////////////////////////////////////////////
Time& Time::SetTime(const UInt tstamp)
{
    mMilliseconds = (UInt)(tstamp & 0x3FF);
    mSecond = (UInt)((tstamp >> 10) & 0x3F);
    mMinute = (UInt)((tstamp >> 16) & 0x3F);
    mHour = (UInt)((tstamp >> 22) & 0x1F);
    mDay = (UInt)((tstamp >> 27) & 0x1F);
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the data members to the current time value in UTC.
///
///   \return Current instance of the Time data.
///
////////////////////////////////////////////////////////////////////////////////////
Time& Time::SetCurrentTime()
{
    *this = GetUtcTime();
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets Universal Coordinate Time (UTC) and returns the values in a
///          Time structure.
///
///   \return The current UTC time data.
///
////////////////////////////////////////////////////////////////////////////////////
Time Time::GetUtcTime()
{
#ifdef WIN32
    Time jTime;
    SYSTEMTIME t;
    ::GetSystemTime(&t);
    jTime.mHour = t.wHour;
    jTime.mMinute = t.wMinute;
    jTime.mSecond = t.wSecond;
    jTime.mMilliseconds = t.wMilliseconds;
    jTime.mDay = t.wDay;
    return jTime;
#else
    Time jTime;
    struct timeval tv;
    struct tm *timeinfo;
    time_t rawtime;
    gettimeofday(&tv, NULL);
    jTime.mMilliseconds = (unsigned int)(tv.tv_usec/1000.0);
    time(&rawtime);
    timeinfo = gmtime ( &rawtime );
    jTime.mHour = timeinfo->tm_hour;
    jTime.mDay = timeinfo->tm_mday;
    jTime.mMinute = timeinfo->tm_min;
    jTime.mSecond = timeinfo->tm_sec;
    return jTime;
#endif
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return The current UTC timestamp in milliseconds.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int Time::GetUtcTimeMs()
{
    Time jtime;

    jtime = GetUtcTime();

    return jtime.mMilliseconds +
           jtime.mSecond*1000 +
           jtime.mMinute*60000 +
           jtime.mHour*3600000;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
Time &Time::operator=(const Time &t)
{
    if(this != &t)
    {
        mMilliseconds = t.mMilliseconds;
        mSecond = t.mSecond;
        mMinute = t.mMinute;
        mHour = t.mHour;
        mDay = t.mDay;
    }

    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the current time is greater than t.
///
////////////////////////////////////////////////////////////////////////////////////
bool Time::operator >(const Time &t) const
{
    return ToUInt() > t.ToUInt();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the current time is less than t.
///
////////////////////////////////////////////////////////////////////////////////////
bool Time::operator <(const Time &t) const
{
    return ToUInt() < t.ToUInt();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the time data is equal, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Time::operator==(const Time &t) const
{
    if( mMilliseconds == t.mMilliseconds &&
        mSecond == t.mSecond &&
        mMinute == t.mMinute &&
        mHour == t.mHour &&
        mDay == t.mDay )
    {
        return true;
    }

    return false;
}


/* End of File */
