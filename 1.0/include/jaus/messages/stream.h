////////////////////////////////////////////////////////////////////////////////////
///
///  \file stream.h
///  \brief This file contains a the most commonly used structure throughout the
///  JAUS library (Stream).  This structure is used for serializing/de-serializing
///  data into a buffer.  It is used to store all JAUS message data that is in
///  a serialize format.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 8 March 2008
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
#ifndef _JAUS_STREAM_H
#define _JAUS_STREAM_H

#include "header.h"
#include "errorhistory.h"
#include "bitvector.h"
#include "scaledinteger.h"
#include "vartype.h"
#include <cxutils/packet.h>

#ifdef __cplusplus

#include <vector>


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Stream
    ///   \brief Data structure for storing message data into a buffer.
    ///
    ///   All serialized messages are stored within a Stream.  This data structure 
    ///   handles all memory allocation and deletion for byte data.  It contains
    ///   methods for automatic reading and writing of data to byte array
    ///   format (serializtion) for reading/writing JAUS messages.
    ///
    ///   Only one JAUS message can be stored in a Stream at a time.
    ///   Attempts to write a second Header object will result in errors
    ///   throughout your porgram.  Just write your JAUS message into a Stream 
    ///   with the data control flag set to Header::DataControl::Single. 
    ///   Even if the total message data is larger than JAUS_MAX_PACKET, 
    ///   write it all into a single Stream object.  
    ///   
    ///   Other JAUS software in this library will detect if the stream is too 
    ///   large and split it up into a multi-packet stream automatically using 
    ///   LargeDataSet.  Or you can split it up yourself using
    ///   LargeDataSet if you want to, but I wouldn't.
    ///
    ///   All Message based class structures write there data into a 
    ///   single Stream object and read from a single Stream object.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Stream : public ErrorHistory, public CxUtils::Packet
    {
    public:
        typedef std::vector<Stream> List;
        typedef std::vector<Stream*> PointerList;
        Stream();
        Stream(const Stream& another);
        virtual ~Stream();
     
        virtual int Write(const Header& header, const unsigned int pos = UINT_MAX);
        virtual int Write(const Byte val, const unsigned int pos = UINT_MAX);
        virtual int Write(const Int val, const unsigned int pos = UINT_MAX);
        virtual int Write(const Short val, const unsigned int pos = UINT_MAX);
        virtual int Write(const UShort val, const unsigned int pos = UINT_MAX);
        virtual int Write(const UInt val, const unsigned int pos = UINT_MAX);
        virtual int Write(const Long val, const unsigned int pos = UINT_MAX);
        virtual int Write(const ULong val, const unsigned int pos = UINT_MAX);
        virtual int Write(const Float val, const unsigned int pos = UINT_MAX);
        virtual int Write(const LongFloat val, const unsigned int pos = UINT_MAX);
        virtual int Write(const unsigned char *buff, const unsigned int len, const unsigned int pos = UINT_MAX);
        virtual int Write(const LongFloat val, 
                          const LongFloat upper,
                          const LongFloat lower, 
                          const ScaledInteger::Type type,
                          const unsigned int pos = UINT_MAX);  
        virtual int Write(const VarType& vtype, const unsigned int len = UINT_MAX);
        virtual int Write(const Stream& stream, const unsigned int pos = UINT_MAX);
        virtual int Write(const std::string& str, const unsigned int pos = UINT_MAX);
        
        virtual int Read(Header& header, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Byte& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Int& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Short& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(UShort& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(UInt& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Long& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(ULong& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Float& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(LongFloat& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(VarType& val, const unsigned int pos = UINT_MAX) const;
        virtual int Read(Stream& stream, const unsigned int len, const unsigned int pos = UINT_MAX) const;
        virtual int Read(std::string& str, const unsigned int len, const unsigned int pos = UINT_MAX) const;
        virtual int Read(unsigned char* buff, 
                         const unsigned int len, 
                         const unsigned int pos = UINT_MAX) const;
        virtual int Read(LongFloat& val, 
                         const LongFloat upper, 
                         const LongFloat lower, 
                         const ScaledInteger::Type type,
                         const unsigned int pos = UINT_MAX) const;        
        virtual int Read(Stream& stream, Header& header, const unsigned int pos = UINT_MAX) const;
        
        int ReadMessage(const unsigned char* buff,
                        const unsigned int len,
                        Header* header = 0);
        static int ReadHeader(const unsigned char* buff, 
                              const unsigned int len,
                              Header& header);
        static int RunTestCase();
        void Print() const;
        bool IsValid() const;
        Stream* Clone() const { return new Stream(*this); }
        operator Packet() const { return *( (Packet*)(this) ); }
        bool operator <(const Stream& another) const;
        bool operator <=(const Stream& another) const;
        Stream& operator=(const Stream& another);  
        Stream& operator+=(const Stream& another);
    private:
        int SetByteOrder(const unsigned int order);
    };    

} //  End Jaus namespace

#endif
#endif
/* End of File */
