////////////////////////////////////////////////////////////////////////////////////
///
///  \file streamcallback.h
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
#ifndef __JAUS_STREAM_CALLBACK_H
#define __JAUS_STREAM_CALLBACK_H

#include "jaus/components/callback.h"
#include "jaus/messages/stream.h"
#include <map>


namespace Jaus
{
    // Type definition for stream function callback.
    typedef void (*StreamFunctionCallback)(const Stream&, const Header*, const Byte, void* );
    
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class StreamCallback
    ///   \brief Callback class for incomming message streams.  
    ///
    ///   Inherit from this class if you want to register a callback to recieve 
    ///   message data as it arrives to your component.
    ///
    ///   The nested class Data is used to store callback objects or functions.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_CMP_DLL StreamCallback : public Callback
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum Transport
        ///   \brief The transport layer types that incomming streams may come from.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Transport
        {
            MessageHandler = 0,
            SharedMemory,
            UDP,
            TCP,
            Serial,
            Communicator,
            Multiple
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Data
        ///   \brief Data structure for storing stream callack objects and 
        ///          function pointers.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class Data
        {
        public:
            Data();
            Data(StreamCallback* cb);
            Data(StreamFunctionCallback cb,
                 void* funcArgs);
            Data(const StreamCallback::Data& cb);
            virtual ~Data();
            void RunCallback(const Stream& msg,
                             const Header* header = NULL,
                             const StreamCallback::Transport transport = StreamCallback::MessageHandler);
            bool operator==(const StreamCallback::Data& cbd) const;
            StreamCallback::Data& operator=(const StreamCallback::Data& cbd);
            StreamCallback* mpCallbackObject;           ///<  Pointer to callback interface.
            StreamFunctionCallback mpFunctionCallback;  ///<  Function pointer callback.
            void *mpFunctionArgs;                       ///<  Function callback additional arguments.
        };
        //////////////////////////////////////////////////////////////////////////////////// 
        ///
        ///    \brief Whenever a message is received for processing
        ///    this function is called to handle it immediately.
        ///
        ///    \param[in] msg The serialized message data in a Stream
        ///               structure.  Only a single message will
        ///               be written into the Stream, not
        ///               multiple.
        ///    \param[in] info Already read header information for
        ///                the stream if available.  NULL if data
        ///                has not been read yet.
        ///    \param[in] transport The transport layer the message comes from.
        ///    \param[in] additionalData Pointer to optional additional data to use in 
        ///                              the callback.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        virtual void ProcessStreamCallback(const Stream& msg,
                                           const Header* info,
                                           const StreamCallback::Transport transport = StreamCallback::MessageHandler,
                                           void* additionalData = 0) = 0;
    };

    typedef std::map<UShort, StreamCallback::Data> StreamCallbackMap;   ///<  STL Map of Stream Callbacks.
}

#endif
/*  End of File */
