////////////////////////////////////////////////////////////////////////////////////
///
///  \file stream.cpp
///  \brief This file contains a the most commonly used structure throughout the
///  JAUS library (Stream).  This structure is used for serializing/de-serializing
///  data into a buffer.  It is used to store all JAUS message data that is in
///  a serialize format.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 25 February 2007
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
#include "jaus/messages/stream.h"
#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <cxutils/packet.h>

using namespace std;
using namespace Jaus;
using namespace CxUtils;

static int mSystemByteOrder = Packet::GetMachineEndianness();

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Stream::Stream() : Packet(CX_PACKET_LITTLE_ENDIAN)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Stream::Stream(const Stream& another) : Packet(CX_PACKET_LITTLE_ENDIAN)
{
    *this = another;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Stream::~Stream()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the header data into a byte stream.
///
///   Only write one JAUS header per Stream (Stream objects should only store 1
///   written JAUS message).
///
///   \param header The header data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return JAUS_HEADER_SIZE (number of bytes of data written).  If
///           nothing was written because of an error, then 0 is returned.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Header& header, const unsigned int pos)
{
    unsigned int ePos = mWritePos;

    if( !header.IsValid() )
    {
        return SetJausError(ErrorCodes::InvalidHeader);
    }

    if(pos != UINT_MAX)
    {
        if( pos > mLength ||
            pos + JAUS_HEADER_SIZE > mLength )
                return SetJausError(ErrorCodes::InvalidValue);

        mWritePos = pos;
    }

    if(mReserved < mWritePos + JAUS_HEADER_SIZE)
    {
        Reserve(mWritePos + JAUS_HEADER_SIZE + 1);
    }

    //  Optimized version
    if( mWritePos + JAUS_HEADER_SIZE < mReserved )
    {
        //  Get pointer to data, and create
        //  properties, data control, and address fields.
        unsigned char *ptr = &mpPacket[mWritePos];
        UShort properties = header.WriteMessageProperties();
        UShort dataControl = header.WriteDataControl();
        UShort value = 0;

        if( (UInt)mSystemByteOrder != CX_PACKET_LITTLE_ENDIAN )
            properties = CX_PACKET_BYTE_SWAP_16(properties);

        memcpy(ptr, &properties, JAUS_USHORT_SIZE);
        ptr += JAUS_USHORT_SIZE;

        if( (UInt)mSystemByteOrder != CX_PACKET_LITTLE_ENDIAN )
        {
            value = CX_PACKET_BYTE_SWAP_16(header.mCommandCode);
            memcpy(ptr, &value, JAUS_USHORT_SIZE);
        }
        else
            memcpy(ptr, &header.mCommandCode, JAUS_USHORT_SIZE);
        ptr += JAUS_USHORT_SIZE;

        *(ptr++) = header.mDestinationID.mInstance;
        *(ptr++) = header.mDestinationID.mComponent;
        *(ptr++) = header.mDestinationID.mNode;
        *(ptr++) = header.mDestinationID.mSubsystem;

        *(ptr++) = header.mSourceID.mInstance;
        *(ptr++) = header.mSourceID.mComponent;
        *(ptr++) = header.mSourceID.mNode;
        *(ptr++) = header.mSourceID.mSubsystem;

        if( (UInt)mSystemByteOrder != CX_PACKET_LITTLE_ENDIAN )
        {
            dataControl = CX_PACKET_BYTE_SWAP_16(dataControl);
            memcpy(ptr, &dataControl, JAUS_USHORT_SIZE);
            ptr += JAUS_USHORT_SIZE;

            value = CX_PACKET_BYTE_SWAP_16(header.mSequenceNumber);
            memcpy(ptr, &value, JAUS_USHORT_SIZE);
        }
        else
        {
            memcpy(ptr, &dataControl, JAUS_USHORT_SIZE);
            ptr += JAUS_USHORT_SIZE;
            memcpy(ptr, &header.mSequenceNumber, JAUS_USHORT_SIZE);
        }

        if( pos == UINT_MAX )
        {
            //  Calculate new write position.
            mWritePos = ePos + JAUS_HEADER_SIZE;
            //  If the write position is larger than
            //  the length of the packet, then adjust
            //  the length.
            if( mWritePos > mLength )
                mLength = mWritePos;
        }
        else
        {
            //  If we were not using the mWritePos data member
            //  then we were overwriting existing values
            //  so we don't have to adjust length fields, etc.
            mWritePos = ePos;
        }
        return JAUS_HEADER_SIZE;
    }

    ////  Slower version
    ////  Pack the data then add to Stream.
    //if( Write(header.WriteMessageProperties()) &&
    //    Write(header.mCommandCode) &&
    //    Write(header.mDestinationID.mInstance) &&
    //    Write(header.mDestinationID.mComponent) &&
    //    Write(header.mDestinationID.mNode) &&
    //    Write(header.mDestinationID.mSubsystem) &&
    //    Write(header.mSourceID.mInstance) &&
    //    Write(header.mSourceID.mComponent) &&
    //    Write(header.mSourceID.mNode) &&
    //    Write(header.mSourceID.mSubsystem) &&
    //    Write(header.WriteDataControl()) &&
    //    Write(header.mSequenceNumber))
    //{
    //    if(pos == UINT_MAX)
    //        mWritePos = ePos + JAUS_HEADER_SIZE;
    //    else
    //        mWritePos = ePos;

    //    return JAUS_HEADER_SIZE;
    //}

    mWritePos = ePos;

    return SetJausError(ErrorCodes::WriteFailure);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Byte val, const unsigned int pos)
{
    int result = WritePacket(this, (Byte)(val));
    if(result == 0)
        SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const UInt val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const UShort val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Int val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Short val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Long val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const ULong val, const unsigned int pos)
{
    int result = WritePacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Float val, const unsigned int pos)
{
    int result;
    Int temp;
    memcpy(&temp, &val, sizeof(val));
    result = WritePacket(this, temp);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const LongFloat val, const unsigned int pos)
{
    int result;
    Long temp;
    memcpy(&temp, &val, sizeof(val));
    result = WritePacket(this, temp);
    if(result == 0)
        this->SetJausError(ErrorCodes::WriteFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param buff The data to write.
///   \param len The length of the buffer.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const unsigned char *buff, const unsigned int len, const unsigned int pos)
{
    return Packet::Write(buff, len, pos);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Write the number and adds it to the end of the packet, but
///   converts to a scaled integer first.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param real The value to write.
///   \param upper The upper limit of real.
///   \param lower The lower limit of real.
///   \param type What type of scaled integer to convert to (ScaledInteger::Int,
///               ScaledInteger::UInt, ScaledInteger::Short, ScaledInteger::UShort or
///               ScaledInteger::Byte).
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const LongFloat real,
                   const LongFloat upper,
                   const LongFloat lower,
                   const ScaledInteger::Type type,
                   const unsigned int pos)
{
    //  Determine what type of scaled integer to
    //  convert to, do so, then write the data into
    //  the packet.
    switch( type ) {
        case ScaledInteger::Byte:
            {
                return Write(ScaledInteger::ToScaledByte(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::UShort:
            {
                return Write(ScaledInteger::ToScaledUShort(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::Short:
            {
                return Write(ScaledInteger::ToScaledShort(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::Int:
            {
                return Write(ScaledInteger::ToScaledInt(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::UInt:
            {
                return Write(ScaledInteger::ToScaledUInt(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::Long:
            {
                return Write(ScaledInteger::ToScaledLong(real, upper, lower), pos);
            }
            break;
        case ScaledInteger::ULong:
            {
                return Write(ScaledInteger::ToScaledULong(real, upper, lower), pos);
            }
            break;
        default:
            return JAUS_FAILURE;
            break;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   For this specific data type, a 1 byte value is written first which
///   is the type of variable, which is then followed by the variable
///   data.
///
///   \param varType The variable type data to write.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return varType Size + 1 on success, otherwise JAUS_FAILURE on error.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const VarType& varType, const unsigned int pos)
{
    int result = 0;
    unsigned int wpos = pos;

    result += Write(varType.Type(), pos);
    if(wpos != UINT_MAX)
    {
        wpos = pos + 1;
    }

    switch(varType.Type())
    {
    case JAUS_BYTE:
        result += Write(varType.ToByte(), wpos);
        break;
    case JAUS_SHORT:
        result += Write(varType.ToShort(), wpos);
        break;
    case JAUS_INT:
        result += Write(varType.ToInt(), wpos);
        break;
    case JAUS_LONG:
        result += Write(varType.ToLong(), wpos);
        break;
    case JAUS_USHORT:
        result += Write(varType.ToUShort(), wpos);
        break;
    case JAUS_UINT:
        result += Write(varType.ToUInt(), wpos);
        break;
    case JAUS_ULONG:
        result += Write(varType.ToULong(), wpos);
        break;
    case JAUS_FLOAT:
        result += Write(varType.ToFloat(), wpos);
        break;
    case JAUS_LONG_FLOAT:
        result += Write(varType.ToLongFloat(), wpos);
        break;
    case JAUS_RGB:
        result += Write(varType.ToRGB(), 3, wpos);
        break;
    default:
        return 0;
        break;
    }

    if( result == varType.Size() + 1)
    {
        return result;
    }

    return SetJausError(ErrorCodes::WriteFailure);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   \param stream The stream data to write into the Stream.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const Stream& stream, const unsigned int pos)
{
    return Packet::Write(stream.mpPacket, stream.mLength, pos);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the data into the stream at the byte position specified
///          or using the internal write byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the write position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   \param str String data to write to stream.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes written, 0 if nothing written.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Write(const std::string& str, const unsigned int pos)
{
    return Packet::Write(str, pos);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Attempts to read header data from the stream.  Checks are performed
///   to verify the header data extracted is valid.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param header The header data read.
///   \param pos The byte position within the stream to write at.  If pos is
///              equal to UINT_MAX, then the internal write position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Header& header, const unsigned int pos) const
{
    int result = JAUS_FAILURE;
    unsigned int dpos = 0;

    if( pos == UINT_MAX && mpPacket)  {     //  Read using internal position
        dpos = mReadPos;
        result = ReadHeader(mpPacket + mReadPos, mLength - mReadPos, header);
    }
    else if( pos < mLength && mpPacket ) {  //  Read using parameter passed.
        dpos = pos;
        result = ReadHeader(mpPacket + pos, mLength - pos, header);
    }

    if(result >= JAUS_HEADER_SIZE)
    {

        if( pos == UINT_MAX ) {
            SetReadPos( dpos + result );
        }

        if ( mLength - (dpos + result) > JAUS_MAX_PACKET_SIZE ) {
            //  Data size must be re-calculated since
            //  it is not possible to store large data field
            //  sizes using the JAUS, read method.
            header.mDataSize = mLength -  ( dpos + JAUS_HEADER_SIZE );
        }
        return result;
    }
    if( mpPacket && dpos + JAUS_HEADER_SIZE < mLength )
        this->SetJausError(ErrorCodes::InvalidHeader);
    else
        this->SetJausError(ErrorCodes::ReadFailure);

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Byte& val, const unsigned int pos) const
{
    int result = CxUtils::Packet::ReadPacket((CxUtils::Packet*)(this), val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Int& val, const unsigned int pos)  const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Short& val, const unsigned int pos) const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Long& val, const unsigned int pos) const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(UInt& val, const unsigned int pos) const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(UShort& val, const unsigned int pos) const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(ULong& val, const unsigned int pos) const
{
    int result = Packet::ReadPacket(this, val, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Float& val, const unsigned int pos) const
{
    Int temp;
    int result = Packet::ReadPacket(this, temp, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    else {
        memcpy(&val, &temp, sizeof(val));
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(LongFloat& val, const unsigned int pos) const
{
    Long temp;
    int result = Packet::ReadPacket(this, temp, pos);
    if(result == 0)
        this->SetJausError(ErrorCodes::ReadFailure);
    else {
        memcpy(&val, &temp, sizeof(val));
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads any header data in the buffer and saves to the
///          header structure.  Decoding is done from beginning of buffer.
///
///   \param buff Buffered data to read from.
///   \param len The length of the buffer.
///   \param header Read header.
///
///   \return Number of bytes extracted (JAUS_HEADER_SIZE).
///   0 is an error, or no header information present for decoding in buffer.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::ReadHeader(const unsigned char *buff,
                       const unsigned int len,
                       Header& header)
{
    int result = JAUS_FAILURE;

    // Clear out old data.
    header.Clear();

    if(len >= (unsigned int)(JAUS_HEADER_SIZE))
    {
        //  Optimized version
        UShort properties, dcontrol;
        unsigned char *ptr = ((unsigned char *)(buff));
        memcpy(&properties, ptr, JAUS_USHORT_SIZE);
        ptr += JAUS_USHORT_SIZE;
        if( (UInt)mSystemByteOrder != CX_PACKET_LITTLE_ENDIAN )
            properties = CX_PACKET_BYTE_SWAP_16(properties);
        header.ReadMessageProperties(properties);

        if( (header.mCommandCode >= 0xD000 && header.mExperimentalFlag == Header::MessageType::Normal) ||
            (header.mCommandCode < 0xD000 && header.mExperimentalFlag != Header::MessageType::Normal) )
        {
            return result;
        }

        //  Check to make sure that the 2 reserved
        //  bits in the message properties are set to 0
        if( header.mVersion <= JAUS_VERSION_3_4) {
            if( (properties & 0xC000) != 0 ) {
                return result;
            }
        }

        // If a query message then the service connection flag should
        // always be set to 0.
        if(header.mServiceConnectionFlag == JAUS_SERVICE_CONNECTION &&
            header.mCommandCode > 0x2000 && header.mCommandCode <= 0x3FFF)
        {
            return result;
        }

        // This library only supports versions 3.3 and later.
        if(header.mVersion < JAUS_VERSION_3_3)
        {
            return result;
        }

        memcpy(&header.mCommandCode, ptr, JAUS_USHORT_SIZE);
        if( (UInt)mSystemByteOrder !=  CX_PACKET_LITTLE_ENDIAN )
            header.mCommandCode = CX_PACKET_BYTE_SWAP_16(header.mCommandCode);

        ptr += JAUS_USHORT_SIZE;

        header.mDestinationID.mInstance  = *(ptr++);
        header.mDestinationID.mComponent = *(ptr++);
        header.mDestinationID.mNode      = *(ptr++);
        header.mDestinationID.mSubsystem = *(ptr++);

        header.mSourceID.mInstance  = *(ptr++);
        header.mSourceID.mComponent = *(ptr++);
        header.mSourceID.mNode      = *(ptr++);
        header.mSourceID.mSubsystem = *(ptr++);
        dcontrol = 0;
        //dcontrol = *(UShort*)(ptr);
        memcpy(&dcontrol, ptr, JAUS_USHORT_SIZE);
        ptr += JAUS_USHORT_SIZE;
        if( (UInt)mSystemByteOrder !=  CX_PACKET_LITTLE_ENDIAN )
            dcontrol = CX_PACKET_BYTE_SWAP_16(dcontrol);

        header.ReadDataControl( dcontrol );
        memcpy(&header.mSequenceNumber, ptr, JAUS_USHORT_SIZE);
        if( (UInt)mSystemByteOrder !=  CX_PACKET_LITTLE_ENDIAN )
            header.mSequenceNumber = CX_PACKET_BYTE_SWAP_16(header.mSequenceNumber);

        if( header.IsValid() )
            return JAUS_HEADER_SIZE;

        return JAUS_FAILURE;
    }
    else
    {
        return JAUS_FAILURE;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Performs a test case to validate the methods of stream.
///
///   \return JAUS_OK on success, JAUS_FAILURE otherwise.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::RunTestCase()
{
    Stream stream;
    Header header;
    Header headerCopy;
    const unsigned int dsize = 50;
    unsigned int len = 0;
    unsigned int tlen = 0;
    unsigned char data[dsize];
    unsigned char  transport[50];

    sprintf((char *)data, "Hello JAUS!");
    sprintf((char *)transport, "JAUS0.01");
    len = (unsigned int)strlen( (char *)data );
    tlen = (unsigned int)strlen( (char *)transport );

    //  Must have valid header values.
    header.mSourceID(1, 2, 3, 4);
    header.mDestinationID(1, 2, 3, 5);
    header.mDataSize = len;
    headerCopy = header;
    //  Write data for message
    stream.Write( header );
    stream.Write( data, len );
    //  Add transport header
    stream.Insert( transport, tlen, 0 );

    //  Remove transport header
    stream.Delete( tlen, 0 );

    //  Lets read the data
    stream.SetReadPos(0);
    if( stream.Read( header ) &&
        headerCopy == header &&
        stream.Read( data, len ) &&
        strcmp( (char *)data, "Hello JAUS!") == 0 )
    {
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Attempts to read a single packet/JAUS message from the current
///   read position, copying the data to the parameter, and advancing the
///   read position.
///
///   This function is useful for extracting single packets when multiple
///   ones are written into Stream.
///
///   \param pack Copy of single stream read.
///   \param header Optional header information that was read.
///
///   \return Number of bytes read, 0 on error and a ErrorHistory set.  If 0
///   bytes are read, then no packet/message extracted.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Stream& pack, Header *header)
{
    int read = 0;
    unsigned char *ptr = (unsigned char *)mpPacket + mReadPos;
    pack.Clear();

    if( (read = pack.ReadMessage(ptr, (mLength - mReadPos), header)) > 0 )
    {
        SetReadPos(mReadPos + read);
    }
    else
        SetJausError(ErrorCodes::ReadFailure);

    return read;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data from the stream at the byte position specified
///          or using the internal read byte position value.  The latter
///          is the default behavior.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///  For a VarType structure, a 1 byte header is read, followed by
///  the variable type specified in the header.
///
///   \param varType The variable type data read.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return 1 + varType.Size() on success, 0 on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(VarType& varType, const unsigned int pos) const
{
    int result = 0;
    unsigned int rpos = pos;
    Byte type = 0;
    result += Read(type, pos);

    if(result == 0)
    {
        return SetJausError(ErrorCodes::ReadFailure);
    }

    if(rpos != UINT_MAX)
    {
        rpos = pos + 1;
    }

    switch(type)
    {
    case JAUS_BYTE:
        {
            Byte val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_SHORT:
        {
            Short val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_INT:
        {
            Int val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_LONG:
        {
            Long val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_USHORT:
        {
            UShort val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_UINT:
        {
            UInt val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_ULONG:
        {
            ULong val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_FLOAT:
        {
            Float val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_LONG_FLOAT:
        {
            LongFloat val;
            if( (result = Read(val, rpos)) > 0 )
            {
                varType = val;
            }
        }
        break;
    case JAUS_RGB:
        {
            Byte rgb[3];
            if( (result = Read(rgb, 3, rpos)) > 0 )
            {
                varType(rgb[0], rgb[1], rgb[2]);
            }
        }
        break;
    default:
        return 0;
        break;
    }

    if( result == varType.Size())
    {
        return result + 1;
    }

    return SetJausError(ErrorCodes::WriteFailure);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the specified number of bytes and copies to another stream.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   \param stream The buffer to copy read data to.
///   \param len Number of bytes to read and copy to stream.
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(Stream& stream, const unsigned int len, const unsigned int pos) const
{
    stream.Clear();
    stream.Reserve(len + 1);
    stream.SetLength(len);
    if( Packet::Read( stream.mpPacket, stream.mLength, pos) == (int)len )
    {
        return (int)stream.Length();
    }
    stream.Clear();
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Read a real number at the index in the packet.
///
///   A call to this function will advance the read position the number of
///   bytes written if pos is not used because it is set to UINT_MAX.
///
///   Byte order conversion is performed automatically based on the
///   byte order set using the SetByteOrder function.  The default
///   byte order used is CX_PACKET_LITTLE_ENDIAN.
///
///   \param val The value read.
///   \param upper The upper limit of real.
///   \param lower The lower limit of real.
///   \param type What type of scaled integer to convert from (ScaledInteger::Int,
///               ScaledInteger::UInt, ScaledInteger::Short, ScaledInteger::UShort, or
///               ScaledInteger::Byte).
///   \param pos The byte position within the stream to read at.  If pos is
///              equal to UINT_MAX, then the internal read position is used,
///              which is the default behavior.
///
///   \return The number of bytes read, 0 if nothing read.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::Read(LongFloat& val,
                 const LongFloat upper,
                 const LongFloat lower,
                 const ScaledInteger::Type type,
                 const unsigned int pos) const
{
    int read = 0;
    switch(type)
    {
    case ScaledInteger::Byte:
        {
            Byte scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::Short:
        {
            Short scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::UShort:
        {
            UShort scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::Int:
        {
            Int scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::UInt:
        {
            UInt scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::Long:
        {
            Long scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    case ScaledInteger::ULong:
        {
            ULong scaled;
            read = Read(scaled, pos);
            if(read > 0)
            {
                val = ScaledInteger::ToDouble(scaled, upper, lower);
            }
        }
        break;
    default:
        read = 0;
        break;
    };
    if(read == 0)
        SetJausError(ErrorCodes::ReadFailure);
    return read;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Extracts a JAUS message from the byte array stream.
///
///   \param buff Buffered data to read from.
///   \param len The length of the buffer.
///   \param header If not NULL then the header information is copied here.
///
///   \return Number of bytes read (JAUS_HEADER_SIZE + MESSAGE BODY).
///   JAUS_FAILURE is an error.
///
////////////////////////////////////////////////////////////////////////////////////
int Stream::ReadMessage(const unsigned char *buff,
                        const unsigned int len,
                        Header *header)
{
    int result = JAUS_FAILURE;
    int read = 0;
    Header theader; //  Temporary header.
    Clear();
    if(len >= (JAUS_HEADER_SIZE))
    {
        UShort properties, dcontrol;
        read += ReadAtPos(buff, len, read, properties);                        // Extract message properties
        theader.ReadMessageProperties(properties);

        //  Check to make sure that the 2 reserved
        //  bits in the message properties are set to 0
        if( theader.mVersion <= JAUS_VERSION_3_4) {
            if( (properties & 0xC000) != 0 ) {
                return SetJausError(ErrorCodes::InvalidHeader);
            }
        }

        read += ReadAtPos(buff, len, read, theader.mCommandCode);              // Command code
        read += ReadAtPos(buff, len, read, theader.mDestinationID.mInstance);  //  Destination address info
        read += ReadAtPos(buff, len, read, theader.mDestinationID.mComponent);
        read += ReadAtPos(buff, len, read, theader.mDestinationID.mNode);
        read += ReadAtPos(buff, len, read, theader.mDestinationID.mSubsystem);
        read += ReadAtPos(buff, len, read, theader.mSourceID.mInstance);       //  Source address info
        read += ReadAtPos(buff, len, read, theader.mSourceID.mComponent);
        read += ReadAtPos(buff, len, read, theader.mSourceID.mNode);
        read += ReadAtPos(buff, len, read, theader.mSourceID.mSubsystem);
        read += ReadAtPos(buff, len, read, dcontrol);                          //  Get the size of the data
        theader.ReadDataControl(dcontrol);                                     //  Remove flag bits.
        read += ReadAtPos(buff, len, read, theader.mSequenceNumber);           //  Get the sequence number

        if(read == (JAUS_HEADER_SIZE))
        {
            if( !theader.IsValid() )
                return SetJausError(ErrorCodes::InvalidHeader);

            //  Now read the rest of the stream if possible.
            if( len >= (unsigned int)((JAUS_HEADER_SIZE) + theader.mDataSize) )
            {

                if(Write(buff, (JAUS_HEADER_SIZE) + theader.mDataSize))
                {
                    result = mLength;
                    if(header)
                        *header = theader;

                    //  Reset the write and read positions
                    SetReadPos(0);
                    SetWritePos(0);
                }
            }
        }
        if(result == 0)
            this->SetJausError(ErrorCodes::ReadFailure);
        return result;
    }
    else
    {
        this->SetJausError(ErrorCodes::ReadFailure);
        return JAUS_FAILURE;
    }
}

int Stream::Read(unsigned char* buff,
                 const unsigned int len,
                 const unsigned int pos) const { return Packet::Read(buff, len, pos); }

int Stream::Read(std::string& str, const unsigned int len, const unsigned int pos) const
{
    return Packet::Read(str, len, pos);
}

int Stream::SetByteOrder(const unsigned int order) { return Packet::SetByteOrder( order); }

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks if there is a valid header in the stream, and if the
///   data sizes match the stream length.
///
///   \return Returns true if stream is formed correctly with a JAUS message.
///
////////////////////////////////////////////////////////////////////////////////////
bool Stream::IsValid() const
{
    Header header;

    if( Read( header, 0 ) &&
        header.IsValid() &&
        header.mDataSize == mLength - JAUS_HEADER_SIZE )
            return true;

    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Attempts to read the message header at the beginning of the
///          stream, and prints to the console the message information.
///
////////////////////////////////////////////////////////////////////////////////////
void Stream::Print() const
{
    Header header;
    if( Read( header, false ) ) {
        header.Print();
    }
    else
        cout << "No header data present in stream.\n";
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compares streams by message header sequence number.
///
///   \param another The stream to compare with.
///
///   \return True if this sequence number of the header in the message stream
///           is less than the other, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Stream::operator<(const Stream& another) const
{
    Header left, right;
    if( Read( left, false ) && another.Read(right, false) ) {
        if( left.mSequenceNumber < right.mSequenceNumber )
            return true;
        else
            return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compares streams by message header sequence number.
///
///   \param another The stream to compare with.
///
///   \return True if this sequence number of the header in the message stream
///           is less than or equal to the other, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Stream::operator<=(const Stream& another) const
{
    Header left, right;
    if( Read( left, false ) && another.Read(right, false) ) {
        if( left.mSequenceNumber <= right.mSequenceNumber )
            return true;
        else
            return false;
    }
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies contents of packet.
///
////////////////////////////////////////////////////////////////////////////////////
Stream& Stream::operator=(const Stream& another)
{
    if(this != &another)
    {
        if(mLength < another.mLength && another.mLength > 0)
        {
            Reserve(another.mLength + 1);
        }
        if(another.mLength > 0 && another.mpPacket)
        {
            //  Just in case.
            if(!mpPacket)
            {
                Reserve(another.mLength + 1);
            }
            memcpy(mpPacket, another.mpPacket, sizeof(unsigned char)*another.mLength);
        }
        mLength = another.mLength;
        mReadPos = another.mReadPos;
        mWritePos = another.mWritePos;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the stream to the end of current packet.
///
////////////////////////////////////////////////////////////////////////////////////
Stream& Stream::operator+=(const Stream& another)
{
    Write(another);
    return *this;
}

/* End of File */
