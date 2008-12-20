////////////////////////////////////////////////////////////////////////////////////
///
///  \file confirmserviceconnection.h
///  \brief This file contains data structures for the JAUS_CONFIRM_SERVICE_CONNECTION message
///  belonging to the CORE set of JAUS messages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 11 October 2007
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
#ifndef __JAUS_CONFIRM_SERVICE_CONNECTION_H
#define __JAUS_CONFIRM_SERVICE_CONNECTION_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ConfirmServiceConnection
    ///   \brief Response to a components request for a service
    ///   connection. Part of the core set of messages in JAUS.    
    ///   
    ///   This message shall notify the SC creator of the result of the
    ///   Create request.  This message shall provide the creator with
    ///   an instance ID of the specific SC being created, the confirmed
    ///   periodic data update rate at which the SC is established, and
    ///   a response code.
    ///
    ///   The instance ID shall distinguish this instance of the SC from
    ///   other instances which might exist for the same message ID, but
    ///   whose data content may differ due to the presence vector.
    ///   Inform type SCs, which use the same command code and presence vector,
    ///   shall assign the same instance ID.  Command type SCs chall be 
    ///   always unique to a specific commander/provider pair of components.
    ///
    ///   The confirmed periodic data update rate informs the creator of the
    ///   actual rate of the SC.  Normally, the confirmed periodic data rate 
    ///   of the SC will equal the requested rate.  Performance and/or message
    ///   transport issues (e.g. bandwidth) might prevent a provider from
    ///   establishing the SC at the requested rate.  It is the responsibility
    ///   of the creator to determine whether the confirmed periodic data
    ///   update rate is sufficient.  If the confirmed periodic data
    ///   update rate is insufficient for proper and/or safe operation, then the
    ///   creator shall terminate the connection.
    ///
    ///   The response code provides the SC status information back to the
    ///   creator.  The response code chall be set as follows:
    ///   0= SC created successfully
    ///   1= Node does not support SCs.
    ///   2= Component does not support SCs.
    ///   3= Unused.
    ///   4= Service Connection is refused due to internal component restrictions.
    ///   5= The Create SC message used one or more invalid parameters.
    ///   6= The Create SC command is not supported by the receiving component.
    /// 
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ConfirmServiceConnection : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum ResponseValues
        ///   \brief The possible response values that can be used when sending this
        ///    message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum ResponseValues
        {
            CreatedSuccessfully = 0,
            NodeDoesNotSupport,
            ComponentDoesNotSupport,
            Unused,
            Refused,
            InvalidParameters,
            MessageNotSupported,
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by message class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxPeriodicRate;    ///<  Maximum confirmed periodic rate.
            const static double MinPeriodicRate;    ///<  Minimum confirmed periodic rate.
        };
        ConfirmServiceConnection();
        ConfirmServiceConnection(const ConfirmServiceConnection &msg);
        virtual ~ConfirmServiceConnection();
        double GetConfirmedPeriodicUpdateRate() const;
        UShort GetMessageCode() const { return mScCommandCode; }        
        Byte GetInstanceID() const { return mInstanceID; }
        Byte GetResponseCode() const { return mResponseCode; }
        int SetMessageCode(const UShort code);
        int SetConfirmedPeriodicUpdateRate(const double rate);
        int SetInstanceID(const Byte id);
        int SetResponseCode(const Byte rcode);
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ConfirmServiceConnection(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        ConfirmServiceConnection &operator=(const ConfirmServiceConnection &msg);
    protected:
        UShort mScCommandCode;         ///<  Command code for the service connection.
        Byte mInstanceID;              ///<  The specific SC for the message indicated by command code.
        UShort mConfirmedPeriodicRate; ///<  Confirmed periodic update rate [0,1092].
        Byte mResponseCode;            ///<  Response code for SC.
    };
}

#endif

#endif 

/*  End of File */
