////////////////////////////////////////////////////////////////////////////////////
///
///  \file createserviceconnection.h
///  \brief This file contains data structures for the JAUS_CREATE_SERVICE_CONNECTION message
///  belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 20 July 2007
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
#ifndef __JAUS_CREATE_SERVICE_CONNECTION_H
#define __JAUS_CREATE_SERVICE_CONNECTION_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class CreateServiceConnection
    ///   \brief Used by components to create a service connection to another
    ///   component. Part of the core set of messages in JAUS.    
    ///   
    ///   This message shall create a service connection (SC) between
    ///   two components at the requested rate.  The range of requested
    ///   rates is from 0 to 1092 Hertz.  In actuality, the minimum
    ///   requested rate is not 0, but rather 0.01666 Hz, which has a
    ///   period of approximately 60 seconds.
    ///
    ///   SCs created for messages that support selection of optional
    ///   data fields use field #3 to specifiy the presence vector.  It is
    ///   specified as an unsigned integer type, which is the maximum size 
    ///   for a presence vector.  Unused bytes of the presence vector, as 
    ///   required by the command code in field #1, shall be set to zero.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL CreateServiceConnection : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by message class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            static const double MaxPeriodicRate;    ///<  Maximum confirmed periodic rate.
            static const double MinPeriodicRate;    ///<  Minimum confirmed periodic rate.
        };
        CreateServiceConnection();
        CreateServiceConnection(const CreateServiceConnection &msg);
        ~CreateServiceConnection();
        double GetPeriodicUpdateRate() const;
        UShort GetMessageCode() const { return mScCommandCode; }        
        UInt GetPresenceVector() const { return mPresenceVector; }
        int SetMessageCode(const UShort code);
        int SetPeriodicUpdateRate(const double rate);
        int SetPresenceVector(const UInt &vector);
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new CreateServiceConnection(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        CreateServiceConnection &operator=(const CreateServiceConnection &msg);
    protected:
        UShort mScCommandCode;      ///<  Command code of the service connection. (Field #1).
        UShort mPeriodicRate;       ///<  Desired update rate of service connection [0,1092]. (Field #2).
        UInt mPresenceVector;       ///<  Presence vector associated with command code. (Field #3).
    };
}


#endif

#endif 

/*  End of File */
