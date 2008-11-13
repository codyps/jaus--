////////////////////////////////////////////////////////////////////////////////////
///
///  \file date.cpp
///  \brief Contains the Date class which is used to store Date information in
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
#include "jaus/messages/date.h"
#include <iostream>
#include <assert.h>
#include <time.h>

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
Date::Date()
{
    mYear = mMonth = mDay = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Date::~Date() 
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the date by its components.
///
///   \param y Year.
///   \param m Month.
///   \param d Day.
///
///   \return JAUS_OK on success, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Date::SetDate(const UShort y,
                  const UShort m,
                  const UShort d)
{
    mYear = y;
    mMonth = m;
    mDay = d;

    return JAUS_OK;
}
    
////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the date by its components.
///
///   \param y Year.
///   \param m Month.
///   \param d Day.
///
///   \return JAUS_OK on success, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Date::GetDate(UShort& y,
                  UShort& m,
                  UShort& d) const
{
    y = mYear;
    m = mMonth;
    d = mDay;

    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates a UShort of the date information in the format defined by the
///          JAUS RA.
///
///   Bits 0-4: Day, range 1, 31<br>
///   Bits 5-8: Month, range 1, 12<br>
///   Bits 9-15: Year, range 2000, 2127 where 0 is 2000, 1 is 2001...<br>
///
///   \return The Date data in JAUS RA format.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Date::ToUShort() const
{
    UShort dstamp = 0;
    
    dstamp |= (mYear - 2000) << 9;
    dstamp |= mMonth << 5;
    dstamp |= mDay;

    return dstamp;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints the date and time.
///
////////////////////////////////////////////////////////////////////////////////////
void Date::PrintDate() const
{    
    cout << "Date Stamp: " << mMonth << "/" << mDay << "/" << mYear << endl;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the time using a UShort in JAUS RA Date format.
///
///   \param dstamp Bits 0-4: Day, range 1, 31
///                 Bits 5-8: Month, range 1, 12
///                 Bits 9-15: Year, range 2000, 2127 where 0 is 2000, 1 is 2001
///                 etc.
///
///   \return Current instance of the Date data.
///
////////////////////////////////////////////////////////////////////////////////////
Date& Date::SetDate(const UShort dstamp)
{
    mDay = (dstamp & 0x001F);
    mMonth = (dstamp & 0x01E0) >> 5;
    mYear = ((dstamp & 0xFE00) >> 9) + 2000;
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the data members to the current time value in UTC.
///
///   \return Current instance of the Date data.
///
////////////////////////////////////////////////////////////////////////////////////
Date& Date::SetCurrentDate()
{
#ifdef WIN32
    SYSTEMTIME t;
    ::GetSystemTime(&t);
    mDay = t.wDay;
    mMonth = t.wMonth;
    mYear = t.wYear;
#else
    struct timeval tv;
    struct tm *timeinfo;
    time_t rawtime;
    gettimeofday(&tv, NULL);
    time(&rawtime);
    timeinfo = gmtime ( &rawtime );
    mYear = timeinfo->tm_year + 1900;
    mMonth = timeinfo->tm_mon;
    mDay = timeinfo->tm_mday;
#endif

    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets Universal Coordinate Date (UTC) and returns the values in a
///          Date structure.
///
///   \return The current UTC time data.
///
////////////////////////////////////////////////////////////////////////////////////
Date Date::GetUtcDate()
{
    Date date;
#ifdef WIN32
    SYSTEMTIME t;
    ::GetSystemTime(&t);
    date.mDay = t.wDay;
    date.mMonth = t.wMonth;
    date.mYear = t.wYear;
#else
    struct timeval tv;
    struct tm *timeinfo;
    time_t rawtime;
    gettimeofday(&tv, NULL);
    time(&rawtime);
    timeinfo = gmtime ( &rawtime );
    date.mYear = timeinfo->tm_year + 1900;
    date.mMonth = timeinfo->tm_mon;
    date.mDay = timeinfo->tm_mday;
#endif

    return date;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \param month Month to get number of days to (1-12).
///
///   \return Number of days in the month (1-31).
///
////////////////////////////////////////////////////////////////////////////////////
UShort Date::DaysInMonth(UShort month)
{
    switch(month)
    {
    case 2:
        return 28;
        break;
    case 4:
        return 30;
        break;
    case 6:
        return 30;
        break;
    case 9:
        return 30;
        break;
    case 11:
        return 30;
        break;
    default:
        return 31;
        break;
    };
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Assignment operator.
///
////////////////////////////////////////////////////////////////////////////////////
Date &Date::operator=(const Date &t)
{
    if(this != &t)
    {
        mYear = t.mYear;
        mMonth = t.mMonth;
        mDay = t.mDay;
    }

    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the current time is greater than t.
///
////////////////////////////////////////////////////////////////////////////////////
bool Date::operator >(const Date &t) const
{
    return ToUShort() > t.ToUShort();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the current time is less than t.
///
////////////////////////////////////////////////////////////////////////////////////
bool Date::operator <(const Date &t) const
{
    return ToUShort() < t.ToUShort();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return True if the time data is equal, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Date::operator==(const Date &t) const
{
    if( mYear == t.mYear &&
        mMonth == t.mMonth &&
        mDay == t.mDay) 
    {
        return true;
    }

    return false;
}


/* End of File */
