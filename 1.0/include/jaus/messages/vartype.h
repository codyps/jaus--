////////////////////////////////////////////////////////////////////////////////////
///
///  \file vartype.h
///  \brief Contains a data structure for storing variable type field data for
///         JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 6 March 2008
///  <br>Created: 6 March 2008
///  <br>Copyright (c) 2008
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
#ifndef __JAUS_VARIABLE_TYPE__H
#define __JAUS_VARIABLE_TYPE__H

#include "jaus/messages/types.h"
#include <string>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class VarType
    ///   \brief VarType stands for Variable Type.  This class is used to deal with
    ///          fields in JAUS messages which vary based on what kind of data needs
    ///          to be stored.  This class encapsulates the storage of variable
    ///          type information.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL VarType
    {
    public:
        VarType();
        VarType(const Byte v);
        VarType(const Short v);
        VarType(const Int v);
        VarType(const Long v);
        VarType(const UShort v);
        VarType(const UInt v);
        VarType(const ULong v);
        VarType(const Float v);
        VarType(const LongFloat v);
        VarType(const Byte r, const Byte g, const Byte b);
        VarType(const VarType& vt);
        virtual ~VarType();
        // Get the variable type information.
        Byte Type() const;
        // Get the size of the variable type.
        Byte Size() const;
        // Return as a specific type of variable.
        Byte ToByte() const;
        // Return as a specific type of variable.
        Short ToShort() const;
        // Return as a specific type of variable.
        Int ToInt() const;
        // Return as a specific type of variable.
        Long ToLong() const;
        // Return as a specific type of variable.
        UShort ToUShort() const;
        // Return as a specific type of variable.
        UInt ToUInt() const;
        // Return as a specific type of variable.
        ULong ToULong() const;
        // Return as a specific type of variable.
        Float ToFloat() const;
        // Return as a specific type of variable.
        LongFloat ToLongFloat() const;
        // Return as a specific type of variable.
        Byte* ToRGB() const;
        // Print variable information to console.
        void Print() const;
        // Convert value to a string.
        std::string ToString() const;
        // Check fi the values are equal.
        bool Equals(const VarType& vt, const double ferror = .000001) const;
        // Run a test case to validate methods of the class.
        static int RunTestCase();
        // Compare values of VarType.
        bool operator==(const VarType& vt) const;
        // Compare values of to see if they are equal.
        bool operator==(const Byte v) const;
        // Compare values of to see if they are equal.
        bool operator==(const Short v) const;
        // Compare values of to see if they are equal.
        bool operator==(const Int v) const;
        // Compare values of to see if they are equal.
        bool operator==(const Long v) const;
        // Compare values of to see if they are equal.
        bool operator==(const UShort v) const;
        // Compare values of to see if they are equal.
        bool operator==(const UInt v) const;
        // Compare values of to see if they are equal.
        bool operator==(const ULong v) const;
        // Compare values of to see if they are equal.
        bool operator==(const Float v) const;
        // Compare values of to see if they are equal.
        bool operator==(const LongFloat v) const; 
        // Compare values of VarType
        bool operator!=(const VarType& vt) const;
        // Compare values of to see if they are not equal
        bool operator!=(const Byte v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const Short v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const Int v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const Long v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const UShort v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const UInt v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const ULong v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const Float v) const;
        // Compare values of to see if they are not equal
        bool operator!=(const LongFloat v) const; 
        // Sets the type as Byte and copies the value.
        VarType& operator=(const Byte v);
        // Sets the type as Short and copies the value.
        VarType& operator=(const Short v);
        // Sets the type as Int and copies the value.
        VarType& operator=(const Int v);
        // Sets the type as Long and copies the value.
        VarType& operator=(const Long v);
        // Sets the type as UShort and copies the value.
        VarType& operator=(const UShort v);
        // Sets the type as UInt and copies the value.
        VarType& operator=(const UInt v);
        // Sets the type as ULong and copies the value.
        VarType& operator=(const ULong v);
        // Sets the type as Float and copies the value.
        VarType& operator=(const Float v);
        // Sets the type as LongFloat and copies the value.
        VarType& operator=(const LongFloat v); 
        // Sets equal to.
        VarType& operator=(const VarType& vt);
        // Sets the type as RGB and copies the value.
        VarType& operator()(const Byte r, const Byte g, const Byte b);
    private:
        void* mpData;           ///<  Pointer to data.
        Byte mType;             ///<  Type of data.
        unsigned int mLength;   ///<  Size of data in bytes.
    };
}

#endif
/*  End of File */
