////////////////////////////////////////////////////////////////////////////////////
///
///  \file scaledinteger.h
///  \brief This file contains the ScaledInteger class which is used for the
///         conversion to/from scaled integers and real numbers as defined in the
///         JAUS RA.
///
///  <br>Author(s): Daniel Barber
///  <br>Last Modified: 25 February 2008
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
#ifndef __JAUS_SCALED_INTEGERS_H
#define __JAUS_SCALED_INTEGERS_H

#include "jaus/messages/types.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ScaledInteger
    ///   \brief Methods for conversion to/from scaled integers and real numbers.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class ScaledInteger
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Type
        ///   \brief Enumerations of the different types of scaled integers.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Type 
        {
            Byte = 10,
            Short,
            UShort,
            Int,
            UInt,
            Long,
            ULong
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::Byte scaled, const double upper, const double lower)
        {
            return scaled * (upper - lower) / ByteRange + lower;
            //return scaled * ((upper - lower) / ByteRange) + (upper + lower) / 2.0;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::Byte scaled, const double upper, const double lower)
        {
            return (float)(scaled * (upper - lower) / ByteRange + lower);
            //return (float)(scaled * ((upper - lower) / ByteRange) + (upper + lower) / 2.0);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Byte ToScaledByte(const double real, const double upper, const double lower)
        {
            return (Jaus::Byte)((real - lower) / ((upper - lower) / ByteRange));
            //return (Jaus::Byte)((real - (upper + lower) / 2.0) * (ByteRange / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Byte ToScaledByte(const float real, const float upper, const float lower)
        {
            return (Jaus::Byte)((real - lower) / ((upper - lower) / ByteRange));
            //return (Jaus::Byte)((real - (upper + lower) / 2.0) * (ByteRange / (upper - lower)));
        }

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::Short scaled, const double upper, const double lower)
        {
            return scaled * ((upper - lower) / Int16Range) + (upper + lower) / 2.0;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::Short scaled, const double upper, const double lower)
        {
            return (float)(scaled * ((upper - lower) / Int16Range) + (upper + lower) / 2.0);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Short ToScaledShort(const double real, const double upper, const double lower)
        {
            return (Jaus::Short)((real - (upper + lower) / 2.0) * (Int16Range / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Short ToScaledShort(const float real, const float upper, const float lower)
        {
            return (Jaus::Short)((real - (upper + lower) / 2.0) * (Int16Range / (upper - lower)));
        }

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::UShort scaled, const double upper, const double lower)
        {
            return scaled * (upper - lower) / UInt16Range + lower;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::UShort scaled, const double upper, const double lower)
        {
            return (float)(scaled * (upper - lower) / UInt16Range + lower);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::UShort ToScaledUShort(const double real, const double upper, const double lower)
        {
            return (Jaus::UShort)((real - lower) / ((upper - lower) / UInt16Range));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::UShort ToScaledUShort(const float real, const float upper, const float lower)
        {
            return (Jaus::UShort)((real - lower) / ((upper - lower) / UInt16Range));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::Int scaled, double upper, double lower)
        {
            return scaled * ((upper - lower) / Int32Range) + (upper + lower) / 2.0;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::Int scaled, const float upper, const float lower)
        {
            return (float)(scaled * ((upper - lower) / Int32Range) + (upper + lower) / 2.0);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Int ToScaledInt(const double real, double upper, double lower)
        {
            return (Jaus::Int)((real - (upper + lower) / 2.0) * (Int32Range / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Int ToScaledInt(const float real, const float upper, const float lower)
        {
            return (Jaus::Int)((real - (upper + lower) / 2.0) * (Int32Range / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::UInt scaled, double upper, double lower)
        {
            return scaled * (upper - lower) / UInt32Range + lower;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::UInt scaled, const float upper, const float lower)
        {
            return (float)(scaled * (upper - lower) / UInt32Range + lower);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::UInt ToScaledUInt(const double real, double upper, double lower)
        {
            return (Jaus::UInt)((real - lower) / ((upper - lower) / UInt32Range));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::UInt ToScaledUInt(const float real, const float upper, const float lower)
        {
            return (Jaus::UInt)((real - lower) / ((upper - lower) / UInt32Range));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::Long scaled, double upper, double lower)
        {
            return scaled / 2.0 * ((upper - lower) / Int64HalfRange) + (upper + lower) / 2.0;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::Long scaled, const float upper, const float lower)
        {
            return (float)(scaled / 2.0 * ((upper - lower) / Int64HalfRange) + (upper + lower) / 2.0);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Long ToScaledLong(const double real, double upper, double lower)
        {
            return (Jaus::Long)((real - (upper + lower) / 2.0) * 2 * (Int64HalfRange / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::Long ToScaledLong(const float real, const float upper, const float lower)
        {
            return (Jaus::Long)((real - (upper + lower) / 2.0) * 2 * (Int64HalfRange / (upper - lower)));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static double ToDouble(const Jaus::ULong scaled, double upper, double lower)
        {
            return scaled * (upper - lower) / UInt64HalfRange + lower;
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts a scaled integer to a real number based on the upper and
        ///          lower limits of the real number.
        ///
        ///   \param scaled Scaled integer to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Real number conversion of the scaled integer.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static float ToFloat(const Jaus::ULong scaled, const float upper, const float lower)
        {
            return (float)(scaled * (upper - lower) / UInt64HalfRange + lower);
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::ULong ToScaledULong(const double real, double upper, double lower)
        {
            return (Jaus::ULong)((real - lower) / ((upper - lower) / UInt64HalfRange));
        }
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Converts from a real number to a Scaled Integer value.
        ///
        ///   \param real Real number to convert.
        ///   \param upper Upper limit of real number.
        ///   \param lower Lower limit of real number.
        ///
        ///   \return Scaled integer representation of the real number.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        inline static Jaus::ULong ToScaledULong(const float real, const float upper, const float lower)
        {
            return (Jaus::ULong)((real - lower) / ((upper - lower) / UInt64HalfRange));
        }
    private:
        static double ByteRange;        ///<  Range of values for a Byte.
        static double UInt64HalfRange;  ///<  Half the range of values for 64 bit unsigned int.
        static double Int64HalfRange;   ///<  Half the range of values for 64 bit integer.
        static double UInt32Range;      ///<  Range of values for a 32 bit unsigned int.
        static double Int32Range;       ///<  Range of values for a 32 bit int.
        static double UInt16Range;      ///<  Range of values for 16 bit unsigned int.
        static double Int16Range;       ///<  Range of values for 16 bit int.
        static double Epsilon;          ///<  Small number to prevent divide by zero errors.
    };
}


#endif
/*  End of File */
