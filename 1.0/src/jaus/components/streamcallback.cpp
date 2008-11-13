////////////////////////////////////////////////////////////////////////////////////
///
///  \file streamcallback.cpp
///  \brief Contains an interface for getting incomming message streams/packets
///  using a callback class or function pointer.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 11 March 2008
///  <br>Last Modified: 11 March 2008
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
#include "jaus/components/streamcallback.h"

using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data::Data() : mpCallbackObject(NULL), 
                               mpFunctionCallback(NULL), 
                               mpFunctionArgs(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   \param cb Pointer to callback object.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data::Data(StreamCallback* cb) : mpCallbackObject(cb), 
                                                 mpFunctionCallback(NULL), 
                                                 mpFunctionArgs(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
///   \param cb Pointer to callback function.
///   \param funcArgs Pointer to additional arguments for function pointer.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data::Data(StreamFunctionCallback cb,
                           void* funcArgs) : mpCallbackObject(NULL), 
                                             mpFunctionCallback(cb), 
                                             mpFunctionArgs(funcArgs)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data::Data(const StreamCallback::Data& cb) : mpCallbackObject(cb.mpCallbackObject), 
                                                             mpFunctionCallback(cb.mpFunctionCallback), 
                                                             mpFunctionArgs(cb.mpFunctionArgs)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data::~Data()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs the callback objects ProcessStreamMessage or the
///   function pointer callback, depending on what data is stored internally.
///
///   \param msg The message stream to process.
///   \param header Pre-read header from stream object.
///   \param transport The transport layer the message is coming from.
///
////////////////////////////////////////////////////////////////////////////////////
void StreamCallback::Data::RunCallback(const Stream& msg,
                                       const Header* header,
                                       const StreamCallback::Transport transport)
{
    if(mpCallbackObject)
    {
        mpCallbackObject->ProcessStreamCallback(msg, header, transport);
    }
    else if(mpFunctionCallback)
    {
        mpFunctionCallback(msg, header, (Byte)(transport), mpFunctionArgs);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compares the pointers to see if the addresses are equal.
///
////////////////////////////////////////////////////////////////////////////////////
bool StreamCallback::Data::operator ==(const StreamCallback::Data& cbd) const
{
    if( mpCallbackObject == cbd.mpCallbackObject &&
        mpFunctionCallback == cbd.mpFunctionCallback &&
        mpFunctionArgs == cbd.mpFunctionArgs)
    {
        return true;
    }

    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Makes a copy of pointer data in data members.
///
////////////////////////////////////////////////////////////////////////////////////
StreamCallback::Data& StreamCallback::Data::operator =(const StreamCallback::Data& cb)
{
    mpCallbackObject = cb.mpCallbackObject;
    mpFunctionCallback = cb.mpFunctionCallback;
    mpFunctionArgs = cb.mpFunctionArgs;
    return *this;
}

/*  End of File */
