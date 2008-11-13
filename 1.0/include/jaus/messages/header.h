////////////////////////////////////////////////////////////////////////////////////
///
///  \file header.h
///  \brief This file contains the Header data structure which is used to
///  represent all JAUS header information.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 26 February 2008
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
#ifndef _JAUS_HEADER_H
#define _JAUS_HEADER_H

#include "jaus/messages/types.h"
#include "address.h"

#ifdef __cplusplus

namespace Jaus
{
    const UShort JAUS_HEADER_SIZE           = 16;       ///<  Serialized JAUS Header size in bytes.
    const UInt   JAUS_MAX_DATA_SIZE         = 4079;     ///<  Maximum message size without JAUS header. (MAX DATA FIELD SIZE).
    const UInt   JAUS_MAX_PACKET_SIZE       = 4095;     ///<  Maximum size of a stream in bytes without UDP header.
               
    const UShort JAUS_NO_ACK_NACK                = 0;   ///<  No acknowledge request.
    const UShort JAUS_REQ_ACK_NACK               = 1;   ///<  Ack/Nack requested.
    const UShort JAUS_NACK                       = 2;   ///<  Message negative acknowledge on receive.
    const UShort JAUS_ACK                        = 3;   ///<  Message received OK.
    
    const UShort JAUS_SERVICE_CONNECTION         = 1;   ///<  Message has service connection flag.
    const UShort JAUS_NO_SERVICE_CONNECTION      = 0;   ///<  Message is not part of service connection.

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Header
    ///   \brief JAUS Message Header Structure.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Header
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Priority
        ///   \brief Class containing constants for different types of priority values
        ///          in the JAUS header.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class Priority
        {
        public:
            const static UShort Low                 = 0;        ///<  Low priority message.
            const static UShort Default             = 6;        ///<  Standard priority message.
            const static UShort High                = 11;       ///<  High priority message.
            const static UShort LowSafetyCritical   = 12;       ///<  Low Safety Critical message.
            const static UShort HighSafetyCritical  = 15;       ///<  High Safety Critical message.
            const static UShort MaxValue            = 15;       ///<  Maximum allowed priority value.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class DataControl
        ///   \brief Class containing constants for different types of data control 
        ///          values.
        ///
        ///   Data control values are used to identify that a message is stand alone
        ///   (single) or part of a multi-packet stream.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class DataControl
        {
        public:
            const static UShort Single      = 0;    ///<  Single-packet message stream (standard).
            const static UShort First       = 1;    ///<  First message in multi-packet stream.
            const static UShort Normal      = 2;    ///<  Normal message in multi-packet stream.
            const static UShort Retransmit  = 4;    ///<  Retransmitted message packet.
            const static UShort Last        = 8;    ///<  Last message in multi-packet stream.
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class MessageType
        ///   \brief Class containing constants for message type flags used in the
        ///          header.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class MessageType
        {
        public:
            const static UShort Normal       = 0;   ///<  Normal message (defined in JAUS RA).
            const static UShort Experimental = 1;   ///<  Experimental/Custom message. (breaks compatibility).
        };  
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class AckNack
        ///   \brief Class containing constants for acknowledgement or negative
        ///          acknowledgment of message receipt.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class AckNack
        {
        public:
            const static UShort None         = 0;   ///<  No Acknowledge/Negative acknowledge.
            const static UShort Request      = 1;   ///<  Request acknowledge/negative acknowledge.
            const static UShort Nack         = 2;   ///<  Negtive acknowledge.
            const static UShort Ack          = 3;   ///<  Acknowledge.
        };
        Header();
        Header(const Header &header);
        UShort WriteDataControl() const;
        UShort WriteMessageProperties() const;
        void WriteDataControl(UShort &dc) const;
        void WriteMessageProperties(UShort &properties) const;
        void ReadDataControl(const UShort dc);
        void ReadMessageProperties(const UShort properties);
        void Print() const;    
        void SwapSourceAndDestination();
        void Clear();
        Header &operator=(const Header &header);

        static bool IsExperimentalMessage(const UShort messageCode);
        static UShort WriteMessageProperties(const UShort p, 
                                             const UShort an,
                                             const UShort sc, 
                                             const UShort ud,
                                             const UShort v);
        static UShort WriteDataControl(const UShort ds,
                                       const UShort df);
        static void ReadDataControl(const UShort dc, 
                                    UShort &ds, 
                                    UShort &df);
        static void ReadMessageProperties(const UShort prop, 
                                          UShort &p, 
                                          UShort &an,
                                          UShort &sc, 
                                          UShort &ud, 
                                          UShort &v);   
        bool IsValid() const;
        bool operator==(const Header &header) const;
        UShort mPriority;               ///<  Message priority (0-15) property.
        UShort mAckNack;                ///<  Acknowledge/Negative Acknowledge property.
        UShort mServiceConnectionFlag;  ///<  Service connection flag.
        UShort mExperimentalFlag;       ///<  User defined/Experimental message flag.
        UShort mVersion;                ///<  JAUS Version number of message.
        UShort mCommandCode;            ///<  Message type property.
        UShort mDataFlag;               ///<  Data control flag.
        UInt mDataSize;                 ///<  Size of data excluding header.
        UShort mSequenceNumber;         ///<  Message sequence number.
        Address mDestinationID;         ///<  Message destination ID.
        Address mSourceID;              ///<  Message source ID.
    };

} //  End of Jaus namespace

#endif
#endif
/* End of File */
