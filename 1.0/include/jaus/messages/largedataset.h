////////////////////////////////////////////////////////////////////////////////////
///
///  \file largedataset.h
///  \brief This file contains an interface for storing and generating
///  Large Data Sets in JAUS.
///
///  Author(s): Daniel Barber
///  Created: 26 July 2007
///  Last Modified: 26 February 2008
///  Copyright (c) 2007
///  Applied Cognition and Training in Immersive Virtual Environments
///  (ACTIVE) Laboratory
///  Institute for Simulation and Training (IST)
///  University of Central Florida (UCF)
///  Email: dbarber@ist.ucf.edu
///  Web:  http://active.ist.ucf.edu
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
#ifndef __JAUS_LARGE_DATA_SET_H
#define __JAUS_LARGE_DATA_SET_H

#include "stream.h"
#include <string.h>

#ifdef __cplusplus

#include <set>
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class LargeDataSet
    ///   \brief Interface for creating and assembling multi-packet stream 
    ///   sequences.
    ///
    ///   If a JAUS message stream (Stream) has a size larger than 
    ///   JAUS_MAX_PACKET_SIZE, then with accordance to the JAUS reference 
    ///   architecture we are dealing with a Large Data Set.  Large Data Sets
    ///   contain a sequence of messages that use the Data Control values
    ///   (First, Normal, Retransmit, Last) that can be re-assembled into
    ///   a single message stream.
    ///
    ///   This class can be used to convert a Stream into several smaller
    ///   JAUS packets.  It can also be used to assemble a sequence of JAUS
    ///   packets that are part of a large data set, and then merge them into a
    ///   single Stream structure which can be read by any of the Message 
    ///   based classes in this library.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL LargeDataSet
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Key
        ///   \brief Hash key structor for sorting/organizing LargeDataSet data.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Key
        {
        public:
            Key()
            {
                mCommandCode = 0;
                mPresenceVector = 0;
                mIdentifier = 0;
            }
            Key(const Address& src,
                const UShort code,
                const UInt pv = 0)
            {
                mSourceID = src;
                mCommandCode = code;
                mPresenceVector = pv;
                mIdentifier = 0;
            }
            ~Key()
            {
            }
            bool operator<(const Key& key) const
            {
                Stream left, right;
                left.Write(mSourceID.GetID());
                left.Write(mCommandCode);
                left.Write(mPresenceVector);
                left.Write(mIdentifier);
                
                right.Write(key.mSourceID.GetID());
                right.Write(key.mCommandCode);
                right.Write(key.mPresenceVector);
                right.Write(key.mIdentifier);

                if(memcmp(left.Ptr(), right.Ptr(), left.Length()) < 0)
                {
                    return true;
                }
                return false;
            }
            bool operator<=(const Key& key) const
            {
                Stream left, right;
                left.Write(mSourceID.GetID());
                left.Write(mCommandCode);
                left.Write(mPresenceVector);
                left.Write(mIdentifier);
                
                right.Write(key.mSourceID.GetID());
                right.Write(key.mCommandCode);
                right.Write(key.mPresenceVector);
                right.Write(key.mIdentifier);

                if(memcmp(left.Ptr(), right.Ptr(), left.Length()) <= 0)
                {
                    return true;
                }
                return false;
            }
            Address mSourceID;     ///<  Message source ID.
            UShort mCommandCode;   ///<  Type of message.
            UInt mPresenceVector;  ///<  Presence vector.
            Byte mIdentifier;      ///<  Additional identifier for multiple streams.
        };
        LargeDataSet();
        LargeDataSet(const LargeDataSet& stream);
        ~LargeDataSet();
        //  Create data set from stream, and save list internally.
        int CreateLargeDataSet(const Stream& packet, Header* sheader = NULL);
        //  Start assembling a multi-packet stream sequence.
        int StartLargeDataSet(const Stream& packet, const Header* header = NULL);
        //  Add stream to the data set being assembled.
        int AddToDataSet(const Stream& packet);
        //  If data set is complete, merge to single stream.
        int GetMergedStream(Stream& packet, Header* header = NULL);
        //  Changes the destination ID in all the packets of the data set.
        int ChangeDestinationID(const Address& id);
        //  Is data set complete? true/false.
        bool IsDataSetComplete() const { return mCompleteFlag; }
        //  Check if packet with "First Packet' data control flag received.
        bool HaveFirstPacket() const { return mFirstPacketFlag; }
        //  Check if packet with "Last Packet" data control flag received.
        bool HaveLastPacket() const { return mLastPacketFlag; }
        //  Get the number of message packets in stream.
        unsigned int GetNumPackets() const { return (unsigned int)mDataSet.size(); }
        //  Get total number of bytes in the stream.
        unsigned int GetDataSetSize() const;
        //  Get mult-sequence stream message type
        UShort GetCommandCode() const { return mStreamHeader.mCommandCode; }
        //  Get pointer to stream data (faster).
        const Stream::List* GetDataSet() const;
        //  Clear all data.
        void Clear();
        //  Print data to console.
        void Print() const;
        // Check if a sequence number has already been added.
        bool HavePacketNumber(const UShort sequenceNumber) const;
        //  Get set of missing packets in sequence.
        std::set<UShort> GetMissingPackets() const { return mMissing; }
        //  Generate or merge multi-packet streams.
        static int CreateLargeDataSet(const Stream& packet, 
                                      Stream::List& stream);
        static int CreateLargeDataSet(const Stream& packet,
                                      unsigned char *buffer, 
                                      const unsigned int len,
                                      unsigned int* result);
        static int MergeLargeDataSet(const Stream::List& stream, 
                                     Stream& packet,
                                     Header* header = NULL);
        //  Get total size of stream in bytes after converting single large packet.
        static unsigned int GetDataSetSize(const Stream& packet);
        //  Get number of packets that will be generated creating multi-packet stream.
        static unsigned int GetNumberOfPackets(const Stream& packet);
        //  Get the time in ms of last addition to stream.
        unsigned int GetUpdateTimeMs() const { return mUpdateTimeMs; }
        //  Get header data for the data set.
        Header GetHeaderInfo() const { return mStreamHeader; }
        //  Make copy of data.
        LargeDataSet& operator=(const LargeDataSet& stream);
    protected:
        bool mCompleteFlag;             ///<  Are all packets assembled in the stream?
        bool mFirstPacketFlag;          ///<  Have first stream in stream.
        bool mLastPacketFlag;           ///<  Has last stream in sequence been added?
        UShort mMaxSeqNumber;           ///<  Maximum sequence number in stream.
        UShort mBaseSeqNumber;          ///<  Starting sequence number in series.
        Stream::List mDataSet;          ///<  Array containing stream of packets.
        std::set<UShort> mMissing;      ///<  List of missing packets.
        Header mStreamHeader;           ///<  Main header for stream.
        unsigned int mUpdateTimeMs;     ///<  Last time a stream was added to stream in ms.
    }; 

    typedef std::map<LargeDataSet::Key, LargeDataSet*> LargeDataSetMap;  ///<  STL Map of Large Data Set information.
}


#endif  // __cplusplus

#endif  // __JAUS_MULTIPLE_PACKET_H
/*  End of File */
