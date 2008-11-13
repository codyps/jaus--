////////////////////////////////////////////////////////////////////////////////////
///
///  \file header.cpp
///  \brief This file contains the Header data structure which is used to
///  represent all JAUS header information.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 8 January 2007
///  <br>Last Modified: 1 August 2007
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
#include "jaus/messages/header.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <iomanip>

using namespace std;
using namespace Jaus;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Header::Header()
{
    mPriority = Header::Priority::Default;
    mAckNack = JAUS_NO_ACK_NACK;
    mServiceConnectionFlag = JAUS_NO_SERVICE_CONNECTION;
    mExperimentalFlag = Header::MessageType::Normal;
    mVersion = JAUS_DEFAULT_VERSION;
    mCommandCode = 0;
    mDataFlag = Header::DataControl::Single;
    mDataSize = 0;
    mSequenceNumber = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Header::Header(const Header &header)
{
    *this = header;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Combines the data size filed and the data flags into a single
///   unsigned short integer.  Data control is bits (0-11) and data flag is
///   bits (12-15).
///
///   \return Serialized data control field.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Header::WriteDataControl() const
{
    UShort dc = 0;
    UShort size = 0;
    if( mDataSize > JAUS_MAX_DATA_SIZE )
        size = JAUS_MAX_DATA_SIZE;
    else
        size = (UShort)(mDataSize);

    switch(mDataFlag)
    {
    case Header::DataControl::First:
        dc = (UShort)(size | 0x1000);
        break;
    case Header::DataControl::Normal:
        dc = (UShort)(size | 0x2000);
        break;
    case Header::DataControl::Retransmit:
        dc = (UShort)(size | 0x4000);
        break;
    case Header::DataControl::Last:
        dc = (UShort)(size | 0x8000);
        break;
    default:
        dc = (UShort)(size & 0x0FFF);
        break;
    };

    return dc;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks the message code to determine if the value is in the range
///          of experimental messages.
///
///   Experimental messages are not define in JAUS, and there use should only
///   be done based on the rules defined in the JAUS RA.
///
///   \return True if message is experimental, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Header::IsExperimentalMessage(const UShort messageCode)
{
    if( messageCode >= 0xD000)
    {
            return true;
    }
    return false;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message property bits into an unsigned short.  This
///   value contains the message priority, ack/nack request, service connection
///   and user defined flag bits, and message version number.
///
///   \return Serialized message properties field.
///
////////////////////////////////////////////////////////////////////////////////////
UShort Header::WriteMessageProperties() const
{
    return (mPriority & 0x000F)                   |
           (mAckNack & 0x0003) << 4               |
           (mServiceConnectionFlag & 0x0001) << 6     |
           (mExperimentalFlag & 0x0001) << 7           |
           (mVersion & 0x003F) << 8;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Combines the data size filed and the data flags into a single
///   unsigned short integer.  Data control is bits (0-11) and data flag is
///   bits (12-15).
///
///   \param dc Serialized data control field.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::WriteDataControl(UShort &dc) const { dc = WriteDataControl(); }


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message property bits into an unsigned short.  This
///   value contains the message priority, ack/nack request, service connection
///   and user defined flag bits, and message version number.
///
///   \param properties Serialized message properties field.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::WriteMessageProperties(UShort &properties) const
{
    properties = WriteMessageProperties();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads data control field to internal data members saving the
///   data size and data flag field values.
///
///   \param dc Serialized data control field.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::ReadDataControl(const UShort dc)
{
    mDataSize = dc & 0x0FFF;
    mDataFlag = (dc & 0xF000) >> 12;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message property bits from an unsigned short.  This
///   value contains the message priority, ack/nack request, service connection
///   and user defined flag bits, and message version number.
///
///   Results are saved to data members.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::ReadMessageProperties(const UShort properties)
{
    ReadMessageProperties(properties, mPriority, mAckNack,
                            mServiceConnectionFlag, mExperimentalFlag ,mVersion);
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the data size and data flags from the data control field.
///
///   \param dc Serialized data control field.
///   \param ds Data size extracted from dc.
///   \param df Data flags extracted from dc.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::ReadDataControl(const UShort dc,
                                UShort &ds,
                                UShort &df)
{
    ds = dc & 0x0FFF;
    df = (dc & 0xF000) >> 12;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Swaps the source and destination ID values.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::SwapSourceAndDestination()
{
    Address t;
    t = mSourceID;
    mSourceID = mDestinationID;
    mDestinationID = t;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message property bits from an unsigned short.  This
///   value contains the message priority, ack/nack request, service connection
///   and user defined flag bits, and message version number.
///
///   \param prop Serialized message properties field.
///   \param p Message priority.
///   \param an Acknowledge/Negative acknowledge values.
///   \param sc Service connection flag bits.
///   \param ud User defined message flag bits.
///   \param v Message version number.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::ReadMessageProperties(const UShort prop,
                                    UShort &p,
                                    UShort &an,
                                    UShort &sc,
                                    UShort &ud,
                                    UShort &v)
{
    p = prop&0x000F;
    an = (prop >> 4)&0x0003;
    sc = (prop >> 6)&0x0001;
    ud = (prop >> 7)&0x0001;
    v =  (prop >> 8)&0x003F;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints out JAUS header data to console.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::Print() const
{
    cout << "JAUS Message Version ";
    switch(mVersion)
    {
    case JAUS_VERSION_2:
        cout << "2\n";
        break;
    case JAUS_VERSION_3:
        cout << "3 or 3.1\n";
        break;
    case JAUS_VERSION_3_4:
        cout << "3.3\n";
        break;
    default:
        cout << "3.3\n";
        break;
    }
    cout << "Command Code: 0x" << setw(4) << setfill('0') << setbase (16) << mCommandCode << setbase(10) << endl;
    cout << "Priority: " << mPriority << endl;
    cout << "Acknowlege/Negative Acknowledge: " << mAckNack << endl;
    if(mServiceConnectionFlag == JAUS_SERVICE_CONNECTION)
        cout << "Service Connection Message" << endl;
    if(mExperimentalFlag == Header::MessageType::Experimental)
        cout << "User Defined Message" << endl;
    cout << "Source ID: " << (UInt)mSourceID.mSubsystem << ":"
                          << (UInt)mSourceID.mNode << ":"
                          << (UInt)mSourceID.mComponent << ":"
                          << (UInt)mSourceID.mInstance << endl;
    cout << "Destination ID: " << (UInt)mDestinationID.mSubsystem << ":"
                               << (UInt)mDestinationID.mNode << ":"
                               << (UInt)mDestinationID.mComponent << ":"
                               << (UInt)mDestinationID.mInstance << endl;
    switch(mDataFlag)
    {
    case Header::DataControl::First:
        cout << "Data Flag: First stream in sequence" << endl;
        break;
    case Header::DataControl::Normal:
        cout << "Data Flag: Normal stream within sequence" << endl;
        break;
    case Header::DataControl::Retransmit:
        cout << "Data Flag: Retransmitted packet" << endl;
        break;
    case Header::DataControl::Last:
        cout << "Data Flag: Last stream in sequence" << endl;
        break;
    default:
        cout << "Data Flag: Single Stream" << endl;
        break;
    };
    cout << "Sequence Number: " << mSequenceNumber << endl;
    cout << "Data Size: " << mDataSize << endl;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Resets all header values to defaults.
///
////////////////////////////////////////////////////////////////////////////////////
void Header::Clear()
{
    mPriority = Header::Priority::Default;
    mAckNack = JAUS_NO_ACK_NACK;
    mServiceConnectionFlag = JAUS_NO_SERVICE_CONNECTION;
    mExperimentalFlag = Header::MessageType::Normal;
    mVersion = JAUS_DEFAULT_VERSION;
    mCommandCode = 0;
    mDataFlag = Header::DataControl::Single;
    mDataSize = 0;
    mSequenceNumber = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks for valid header data.
///
///   Header values that are invalid are:<br>
///   Source ID that is broadcast or has 0 in it.<br>
///   Destination ID that has a value of 0 in it.<br>
///   ACK/NACK set on a with service connection bit set to 1.<br>
///   Data size set to 0 with data flag != Header::DataControl::Single.<br>
///   User defined command code without the user defined flag set.<br>
///
///   \return True if valid, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Header::IsValid() const
{
    //  If source and destination are not valid values, bail out.
    if( !mSourceID.IsValid() || mSourceID.IsBroadcast() || !mDestinationID.IsValid() )
        return false;

    //  According to JAUS you cannot have the service connection
    //  flag set and have the ACK/NACK field set.
    if( mAckNack != JAUS_NO_ACK_NACK && mServiceConnectionFlag == JAUS_SERVICE_CONNECTION ) {
        return false;
    }

    //  If user defined is set, and message is not a
    //  user defined class type, then ERROR.
    if( mExperimentalFlag == Header::MessageType::Experimental &&
        !Header::IsExperimentalMessage( mCommandCode ) ) {
            return false;
    }

    //  If there is no data, then this cannot be part of
    //  a large data set.
    if( mDataSize == 0 && mDataFlag != Header::DataControl::Single )
        return false;

    // Sequence number should always be 0 unless it is
    // a service connection message, heartbeat pulse, or
    // multi-packet sequence.
    if(mDataSize > 0 &&
        mDataSize < JAUS_MAX_DATA_SIZE &&
        mDataFlag == Header::DataControl::Single &&
        mServiceConnectionFlag != JAUS_SERVICE_CONNECTION &&
        mSequenceNumber != 0 &&
        mCommandCode != JAUS_REPORT_HEARTBEAT_PULSE)
    {
       return false;
    }

    return true;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to the header.
///
////////////////////////////////////////////////////////////////////////////////////
Header &Header::operator =(const Header &header)
{
    if(this != &header)
    {
        mPriority = header.mPriority;
        mAckNack = header.mAckNack;
        mServiceConnectionFlag = header.mServiceConnectionFlag;
        mExperimentalFlag = header.mExperimentalFlag;
        mVersion = header.mVersion;
        mCommandCode = header.mCommandCode;
        mDataFlag = header.mDataFlag;
        mDataSize = header.mDataSize;
        mSequenceNumber = header.mSequenceNumber;
        mSourceID = header.mSourceID;
        mDestinationID = header.mDestinationID;
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Checks to see if the header data is equal.
///
///   \return True if equal, otherwise false.
///
////////////////////////////////////////////////////////////////////////////////////
bool Header::operator==(const Header &header) const
{
    if( mPriority == header.mPriority &&
        mAckNack == header.mAckNack &&
        mServiceConnectionFlag == header.mServiceConnectionFlag &&
        mExperimentalFlag == header.mExperimentalFlag &&
        mVersion == header.mVersion &&
        mCommandCode == header.mCommandCode &&
        mDataFlag == header.mDataFlag &&
        mDataSize == header.mDataSize &&
        mSequenceNumber == header.mSequenceNumber &&
        mDestinationID == header.mDestinationID &&
        mSourceID == header.mSourceID) {

            return true;
    }

    return false;
}

/* End of File */
