////////////////////////////////////////////////////////////////////////////////////
///
///  \file errorhistory.cpp
///  \brief This file contains a base class for setting error codes within
///  JAUS data structures.  All error codes are defined here.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 February 2007
///  <br>Last Modified: 28 June 2007
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
#include "jaus/messages/errorhistory.h"
#include <assert.h>
#include <iostream>
#include <sstream>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ErrorHistory::ErrorHistory()
{
    mJausErrorCode = ErrorCodes::None;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ErrorHistory::~ErrorHistory()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Returns an ascii string representing the error.
///
///   \return Ascii string of error.
///
////////////////////////////////////////////////////////////////////////////////////
const char *ErrorHistory::GetLastJausErrorString() const
{
    ostringstream error;
    string *pStringName = (string *)(&mJausErrorString);
    error <<  "JAUS ERROR (" << mJausErrorCode << "): ";
    switch(mJausErrorCode)
    {
    case ErrorCodes::None:
        error << "No Error" << endl;
        break;
    case ErrorCodes::InvalidAddress:
        error << "Invalid ID number in full JAUS address." << endl;
        break;
    case ErrorCodes::AddressConflict:
        error << "ID selected already in use." << endl;
        break;
    case ErrorCodes::WriteFailure:
        error << "Failure to write data to a byte stream.)" << endl;
        break;
    case ErrorCodes::ReadFailure:
        error << "Failure to read data from a byte stream." << endl;
        break;
    case ErrorCodes::UnsupportedVersion:
        error << "Unsupported Version of JAUS." << endl;
        break;
    case ErrorCodes::InvalidValue:
        error << "Invalid parameter(s)/value(s)." << endl;
        break;
    case ErrorCodes::InvalidHeader:
        error << "Invalid/Bad header information." << endl;
        break;
    case ErrorCodes::InvalidMessageType:
        error << "Invalid message code or type." << endl;
        break;
    case ErrorCodes::UnknownMessageType:
        error << "Unknown message code or type." << endl;
        break;
    case ErrorCodes::BadPacket:
        error << "Bad data or incorrectly formated packet/byte stream." << endl;
        break;
    case ErrorCodes::FieldNotPresent:
        error << "The data field is not present in the message." << endl;
        break;
    case ErrorCodes::DataExists:
        error << "Data of the same type already exists (duplicate data)." << endl;
        break;
    case ErrorCodes::NotInitialized:
        error << "Interface or Component not initialized." << endl;
        break;
    case ErrorCodes::NotConnected:
        error << "Connection not available or present." << endl;
        break;
    case ErrorCodes::NodeManagerNotFound:
        error << "Node Manager is not found on computer." << endl;
        break;
    case ErrorCodes::ConnectionFailure:
        error << "Failed to create a connection." << endl;
        break;
    case ErrorCodes::ServiceNotAvailable:
        error << "Service Not Present/Available (Services Not Setup Correctly for Component?)." << endl;
        break;
    default:
        error << "Unknown error code." << endl;
        break;
        break;
    };

    *pStringName = error.str();
    return mJausErrorString.c_str();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \return Error code of last JAUS error.
///
////////////////////////////////////////////////////////////////////////////////////
ErrorCodes::Type ErrorHistory::GetLastJausError() const { return mJausErrorCode; }

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears any JAUS errors.
///
////////////////////////////////////////////////////////////////////////////////////
void ErrorHistory::ClearLastJausError() const
{
    //  Trick the compiler, etc. into thinking this is still
    //  a constant function.
    ErrorCodes::Type *ePtr = (ErrorCodes::Type *)(&mJausErrorCode);
    *ePtr = ErrorCodes::None;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the error code.
///
///   This function always returns JAUS_FAILURE, so that users can just
///   do things like: <br>
///   return SetJausError(ErrorCodes::InvalidHeader);<br>
///   Instead of: <br>
///   SetJausError(ErrorCodes::InvalidHeader);<br>
///   return JAUS_FAILURE; <br>
///
///   \param error Error code.
///
///   \return Always returns JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ErrorHistory::SetJausError(const ErrorCodes::Type error) const
{
    //  Trick the compiler, etc. into thinking this is still
    //  a constant function.
    ErrorCodes::Type *ePtr = (ErrorCodes::Type *)(&mJausErrorCode);
    *ePtr = error;
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the error code, and in Debug applications, performs an
///   assert which will halt operations.
///
///   This function always returns JAUS_FAILURE, so that users can just
///   do things like: <br>
///   return SetJausError(ErrorCodes::InvalidHeader);<br>
///   Instead of: <br>
///   SetJausError(ErrorCodes::InvalidHeader);<br>
///   return JAUS_FAILURE; <br>
///
///   \param error Error code.
///
///   \return Always returns JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ErrorHistory::AssertJausError(const ErrorCodes::Type error) const
{
    //  Trick the compiler, etc. into thinking this is still
    //  a constant function.
    ErrorCodes::Type *ePtr = (ErrorCodes::Type *)(&mJausErrorCode);
    *ePtr = error;
    PrintJausError();
    assert(0);
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Displays error to console.
///
////////////////////////////////////////////////////////////////////////////////////
void ErrorHistory::PrintJausError() const
{
    cout << GetLastJausErrorString() << endl;
}

/*  End of File */
