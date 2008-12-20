////////////////////////////////////////////////////////////////////////////////////
///
///  \file types.h
///  \brief This file contains definitions of and about basic data types
///  used throughout the JAUS library.
///
///  Author(s): Daniel Barber
///  Created: 8 January 2007
///  Last Modified: 24 April 2007
///  Copyright (c) 2007
///  Applied Cognition and Training in Immersive Virtual Environments
///  (ACTIVE) Laboratory
///  Institute for Simulation and Training (IST)
///  University of Central Florida (UCF)
///  Email: dbarber@ist.ucf.edu
///  Web:  http://active.ist.ucf.edu
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
#ifndef _JAUS_TYPES_H
#define _JAUS_TYPES_H
#include <limits.h>
#include <cxutils/math/cxmath.h>
#include "msglibdll.h"

#if defined(WIN32) || (defined(_WIN32_WCE))

#else
#include <unistd.h>
#endif


#ifdef __cplusplus

#ifndef NULL
#define NULL 0
#endif

//////////////////////////////////////////////////////////////////////////////////// 
///  
///  \namespace Jaus
///  \brief Name space used for the JAUS library.
///
//////////////////////////////////////////////////////////////////////////////////// 
namespace Jaus
{
    typedef unsigned char Byte;                  ///<  JAUS byte (8 bit unsigned integer).
    typedef short Short;                         ///<  JAUS short integer (16 bit signed integer).
    typedef int Int;                             ///<  JAUS integer (32 bit signed integer).
    typedef long long int Long;                  ///<  JAUS long integer (64 bit signed integer).
    typedef unsigned short UShort;               ///<  JAUS unsigned short integer (16 bit unsigned integer).
    typedef unsigned int UInt;                   ///<  JAUS unsigned integer (32 bit unsigned integer).
    typedef unsigned long long int ULong;        ///<  JAUS unsigned long integer (64 bit unsigned integer).
    typedef float Float;                         ///<  JAUS float (IEEE 32 8 bit floating point number).
    typedef double LongFloat;                    ///<  JAUS long float (IEEE 64 bit floating point number).

    const UShort JAUS_VERSION_2        = 0;                    ///<  Version 2 of JAUS RA.
    const UShort JAUS_VERSION_3        = 1;                    ///<  Version 3 of JAUS RA.
    const UShort JAUS_VERSION_3_2      = 2;                    ///<  Version 3.2 of JAUS RA.
    const UShort JAUS_VERSION_3_3      = 2;                    ///<  Version 3.3 of JAUS RA. Default JAUS version for messages.
    const UShort JAUS_VERSION_3_4      = 3;                    ///<  Version 3.4 of JAUS RA. Default JAUS version for messages.
    const UShort JAUS_DEFAULT_VERSION  = JAUS_VERSION_3_3;     ///<  By default all JAUS messages write this version unless specified otherwise.
    const int JAUS_OK                       =  1;              ///<  Return code for success on functions.
    const int JAUS_FAILURE                  =  0;              ///<  Return code for errors/failure on functions.
    const int OK                            = JAUS_OK;         ///<  Shorter version of JAUS_OK.
    const int FAILURE                       = JAUS_FAILURE;    ///<  Shorter version of JAUS_FAILURE.
    //  Data type enumerations.
    const Byte JAUS_BYTE                        = 0;         ///<  Type number.
    const Byte JAUS_SHORT                       = 1;         ///<  Type number.
    const Byte JAUS_INT                         = 2;         ///<  Type number.
    const Byte JAUS_LONG                        = 3;         ///<  Type number.
    const Byte JAUS_USHORT                      = 4;         ///<  Type number.
    const Byte JAUS_UINT                        = 5;         ///<  Type number.
    const Byte JAUS_ULONG                       = 6;         ///<  Type number.
    const Byte JAUS_FLOAT                       = 7;         ///<  Type number.
    const Byte JAUS_LONG_FLOAT                  = 8;         ///<  Type number.
    const Byte JAUS_RGB                         = 9;         ///<  RGB type of data.

    const Byte JAUS_PRESENCE_VECTOR_BYTE        = 17;        ///<  Type number
    const Byte JAUS_PRESENCE_VECTOR_USHORT      = 18;        ///<  Type number.
    const Byte JAUS_PRESENCE_VECTOR_ULONG       = 19;        ///<  Type number.
    const Byte JAUS_BYTE_ARRAY                  = 20;        ///<  Array of byte data.  

    const Short     JAUS_SHORT_MAX      = SHRT_MAX;     ///<  Maximum value of scaled short (Short).
    const Short     JAUS_SHORT_MIN      = SHRT_MIN;     ///<  Minimum value of a scaled short (Short).
    const UShort    JAUS_USHORT_MAX     = USHRT_MAX;    ///<  Maximum value of a scaled unsigned short (UShort).
    const UShort    JAUS_USHORT_MIN     =  0;           ///<  Minimum value of scaled unsigned short (UShort).
    const Int       JAUS_INT_MIN        = INT_MIN;      ///<  Minimum value of scaled integer (Int).
    const Int       JAUS_INT_MAX        = INT_MAX;      ///<  Maximum value of scaled integer (Int).
    const UInt      JAUS_UINT_MIN       = 0;            ///<  Minimum value of scaled unsigned integer (UInt).
    const UInt      JAUS_UINT_MAX       = UINT_MAX;     ///<  Maximum value of scaled unsigned integer (UInt).
    const Long      JAUS_LONG_MIN                       ///<  Minimum value of long integer (Long).
    #ifdef _WIN32_WCE
                                        = _I64_MIN;
    #else
       #ifdef __GNUC__
            #if __GNUC__ > 3
                                        = LLONG_MIN;
            #else
                                        = LONG_LONG_MIN;
            #endif
       #else
                                        = LLONG_MIN;
       #endif
    #endif
    const Long      JAUS_LONG_MAX                       ///<  Maximum value of long integer (Long).
    #ifdef _WIN32_WCE
                                        = _I64_MAX;
    #else
        #ifdef __GNUC__
            #if __GNUC__ > 3
                                        = LLONG_MAX;
            #else
                                        = LONG_LONG_MAX;
            #endif
        #else
                                        = LLONG_MAX;    
        #endif
    #endif
    const ULong JAUS_ULONG_MIN          = 0;            ///<  Minimum value of unsigned long integer (ULong).
    const ULong JAUS_ULONG_MAX                          ///<  Maximum value of unsigned long integer (ULong).
    #ifdef _WIN32_WCE
                                        = _UI64_MAX;
    #else
        #ifdef __GNUC__
            #if __GNUC__ > 3
                                        = ULLONG_MAX;
            #else
                                        = ULONG_LONG_MAX;
            #endif
        #else
                                        = ULLONG_MAX;    
        #endif
    #endif

    const UInt JAUS_BYTE_SIZE                = 1;          ///<  Size in bytes.
    const UInt JAUS_SHORT_SIZE               = 2;          ///<  Size in bytes.
    const UInt JAUS_USHORT_SIZE              = 2;          ///<  Size in bytes.
    const UInt JAUS_INT_SIZE                 = 4;          ///<  Size in bytes.
    const UInt JAUS_UINT_SIZE                = 4;          ///<  Size in bytes.
    const UInt JAUS_FLOAT_SIZE               = 4;          ///<  Size in bytes.
    const UInt JAUS_LONG_SIZE                = 8;          ///<  Size in bytes.
    const UInt JAUS_ULONG_SIZE               = 8;          ///<  Size in bytes.
    const UInt JAUS_LONG_FLOAT_SIZE          = 8;          ///<  Size in bytes.      

    const UInt JAUS_BYTE_BITS                = 8;          ///<  Size in bits of Byte.
    const UInt JAUS_SHORT_BITS               = 16;         ///<  Size in bits of Short.
    const UInt JAUS_USHORT_BITS              = 16;         ///<  Size in bits of UShort.
    const UInt JAUS_INT_BITS                 = 32;         ///<  Size in bits of Int.
    const UInt JAUS_UINT_BITS                = 32;         ///<  Size in bits of UInt.
    const UInt JAUS_FLOAT_BITS               = 32;         ///<  Size in bits of Float.
    const UInt JAUS_LONG_BITS                = 64;         ///<  Size in bits of Long.
    const UInt JAUS_ULONG_BITS               = 64;         ///<  Size in bits of ULong.
    const UInt JAUS_LONG_FLOAT_BITS          = 64;         ///<  Size in bits of LongFloat.
    
    const double JAUS_EPSILON        = CxUtils::CX_EPSILON;         ///<  Used to prevent divide by 0.

    const double JAUS_PI                = CxUtils::CX_PI;           ///<  Value of PI.
    const double JAUS_HALF_PI           = CxUtils::CX_HALF_PI;      ///<  PI divided by 2.0
    const double JAUS_FOURTH_PI         = CxUtils::CX_FOURTH_PI;    ///<  PI divided by 4.0   
    const double JAUS_PI_OVER_180       = CxUtils::CX_PI_OVER_180;  ///<  PI divided by 180.0
    const double JAUS_180_OVER_PI       = CxUtils::CX_180_OVER_PI;  ///<  180 divided by PI.
    const double JAUS_TWO_PI            = CxUtils::CX_TWO_PI;       ///<  PI * 2.
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \def JAUS_DEG2RAD(deg)
///   \brief Converts \a deg to radians from degrees.
///
////////////////////////////////////////////////////////////////////////////////////
#ifndef JAUS_DEG2RAD
#define JAUS_DEG2RAD(deg) ( (deg*Jaus::JAUS_PI_OVER_180) )
#endif

////////////////////////////////////////////////////////////////////////////////////
///
///   \def JAUS_RAD2DEG(rad)
///   \brief Converts \a rad to degrees from radians.
///
////////////////////////////////////////////////////////////////////////////////////
#ifndef JAUS_RAD2DEG
#define JAUS_RAD2DEG(rad) ( (rad*Jaus::JAUS_180_OVER_PI) )
#endif

#ifndef WIN32
inline void Sleep(unsigned int ms)
{
    usleep(ms*1000);
}
#endif

#endif // __cplusplus

#endif
/*  End of File */
