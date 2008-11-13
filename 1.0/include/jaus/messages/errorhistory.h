////////////////////////////////////////////////////////////////////////////////////
///
///  \file errorhistory.h
///  \brief This file contains a base class for setting error codes within
///  JAUS data structures.  All error codes are defined here.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 February 2007
///  <br>Last Modified: 28 July 2007
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
#ifndef __JAUS_ERROR_HISTORY_H
#define __JAUS_ERROR_HISTORY_H

#include "jaus/messages/errorcodes.h"
#include <string>

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ErrorHistory
    ///   \brief Base class for pretty much all JAUS objects.  Handles setting
    ///   of common errors that may occur during use of structure.
    ///
    ///   This interface is used to keep track of all errors that occur during
    ///   commong operations in the library.  Use it's functions to set
    ///   errors, throw errors, or to find out what went wrong.
    ///
    ///   This class is similar to how the GetLastError function 
    ///   performs when using Windows Sockets.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ErrorHistory
    {
    public:
        ErrorHistory();
        virtual ~ErrorHistory();
        virtual const char* GetLastJausErrorString() const;
        virtual void PrintJausError() const;
        virtual ErrorCodes::Type GetLastJausError() const;
    protected:
        virtual void ClearLastJausError() const;
        virtual int SetJausError(const ErrorCodes::Type error) const;
        virtual int AssertJausError(const ErrorCodes::Type error) const;
    private:
        ErrorCodes::Type mJausErrorCode;    ///<  Error code.
        std::string mJausErrorString;       ///<  String describing error.
    }; // ErrorHistory
} //  Jaus


#endif //  __cplusplus

#endif // __JAUS_ERRORS_H

/*  End of File */
