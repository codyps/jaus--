////////////////////////////////////////////////////////////////////////////////////
///
///  \file largedataset.cpp
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
#include "jaus/messages/largedataset.h"
#include "jaus/messages/time.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
LargeDataSet::LargeDataSet()
{
    mCompleteFlag = false;
    mLastPacketFlag = false;
    mFirstPacketFlag = false;
    mMaxSeqNumber = 0;
    mBaseSeqNumber = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
LargeDataSet::LargeDataSet(const LargeDataSet& stream)
{
    mCompleteFlag = false;
    mLastPacketFlag = false;
    mFirstPacketFlag = false;
    mMaxSeqNumber = 0;
    *this = stream;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
LargeDataSet::~LargeDataSet()
{
    Clear();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes a single JAUS message and splits it into a multi-packet
///   stream sequence.
///
///   The stream passed must contain a single JAUS message, and the data
///   size must be large than JAUS_MAX_PACKET_SIZE for it to be split into
///   a stream.
///
///   \param stream The message stream to split into a multi-packet stream
///                 sequence.  The size of the stream must be larger than
///                 JAUS_MAX_PACKET_SIZE.
///   \param sheader Optional paramter.  This is a pointer to a Header
///                  object to copy the stream header to.
///
///   \return JAUS_OK if successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::CreateLargeDataSet(const Stream& stream, Header* sheader)
{
    Clear();
    if( CreateLargeDataSet( stream, mDataSet ) ) {
        mLastPacketFlag = true;
        mFirstPacketFlag = true;
        mCompleteFlag = true;
        mMaxSeqNumber = (UShort)(((unsigned int)mDataSet.size()) - 1);
        stream.Read( mStreamHeader, 0 );
        mStreamHeader.mDataFlag = Header::DataControl::Single;
        if( sheader )
            *sheader = mStreamHeader;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Start assembling a multi-packet stream.  All previous data is
///   cleared out, and a new start is made.
///
///   This function tells the stream to start putting together a sequence
///   from a large-data set in JAUS.  After this initializer function, use
///   the AddToDataSet function to added the remaining packets in the 
///   multi-packet sequence.
///
///   \param stream The first stream received in a multi-packet stream sequence.
///   \param header Pointer to read header for the packet.  Set to NULL
///                 if data is not already read.
///
///   \return JAUS_OK if multi-packet stream assembly started, otherwise 
///           JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::StartLargeDataSet(const Stream& stream, const Header* header)
{
    if(header) 
    {
        mStreamHeader = *header;
    }
    else 
    {
        if( !stream.Read(mStreamHeader, 0) ) 
        {
            return JAUS_FAILURE;
        }
    }

    mLastPacketFlag = false;
    mFirstPacketFlag = false;
    mCompleteFlag = false;
    mDataSet.clear();
    mMaxSeqNumber = mStreamHeader.mSequenceNumber;
    mBaseSeqNumber = mStreamHeader.mSequenceNumber;
    mMissing.clear();
    
    //  Verify this is part of a sequence.
    if( mStreamHeader.mDataFlag == Header::DataControl::Single ) 
    {
        mDataSet.push_back( stream );
        mLastPacketFlag = true;
        mFirstPacketFlag = true;
        mCompleteFlag = true;
        mUpdateTimeMs = Time::GetUtcTimeMs();
        return JAUS_OK;
    }

    if( mStreamHeader.mDataFlag == Header::DataControl::First ||
        (mStreamHeader.mDataFlag == Header::DataControl::Retransmit && mStreamHeader.mSequenceNumber == 0) ) 
    {
        
            //  Get the base sequence number.  If this is part of a service
            //  connection, then the base sequence number may not be 0.
            mBaseSeqNumber = mStreamHeader.mSequenceNumber;
            mFirstPacketFlag = true;
    }
    
    if( mStreamHeader.mDataFlag == Header::DataControl::Last ||
        (   mStreamHeader.mDataFlag == Header::DataControl::Retransmit && 
            mStreamHeader.mSequenceNumber > 0 && 
            mStreamHeader.mDataSize < JAUS_MAX_DATA_SIZE )  ) 
    {

        mLastPacketFlag = true;
    }

    //  Setup how many we are missing...
    if( mMaxSeqNumber > 0 ) {
        for( UShort i = mBaseSeqNumber; i < mMaxSeqNumber; i++ ) 
        {
            mMissing.insert(i);
        }
    }

    //  From this point on the mStreamHeader data will contain the 
    //  primary information about the entire message after all the
    //  individual packets have been added up.
    mStreamHeader.mDataFlag = Header::DataControl::Single;
    mStreamHeader.mSequenceNumber = 0;
    mStreamHeader.mAckNack = JAUS_NO_ACK_NACK;
    mDataSet.push_back(stream);

    mUpdateTimeMs = Time::GetUtcTimeMs();
    
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Add a stream from a multi-packet stream sequence (large data set) to
///   to multi-packet stream.
///
///   Once all the packets in the sequence have been added (received a data 
///   control flag of Header::DataControl::Last and there are no missing packets ) 
///   then the complete flag will be set.  Check for complete using the
///   IsDataSetComplete function.
///
///   If the LargeDataSet has a complete sequence already, 
///   (IsDataSetComplete() returns true), then this function will return 
///   JAUS_FAILURE.  You will need to start a new sequence using
///   StartLargeDataSet before use of this function will work
///   correctly.
///
///   \param msg JAUS message stream that is part of the multi-packet 
///          sequence being assembled.
///   
///   \return JAUS_OK if added, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::AddToDataSet(const Stream& msg)
{
    int result = JAUS_FAILURE;
    bool prevFirstPacketFlag = mFirstPacketFlag;
    Header header;

    if( IsDataSetComplete() )
        return JAUS_FAILURE;

    if( msg.Length() > JAUS_MAX_PACKET_SIZE ) 
    {
        return JAUS_FAILURE;
    }

    if( mDataSet.size() == 0 )
        return StartLargeDataSet( msg, NULL );

    if( msg.Read( header, 0 ) ) 
    {        
        //  Verify the contents of the data in the stream with
        //  internal values.
        if( header.mCommandCode != mStreamHeader.mCommandCode )
            return JAUS_FAILURE;
        if( header.mDataFlag == Header::DataControl::Single )
            return JAUS_FAILURE;
        //  If we have already received the last stream in the 
        //  sequence, and this stream has a higher sequence
        //  number, then this is not part of this sequence.
        if( header.mSequenceNumber > mMaxSeqNumber && mLastPacketFlag ) 
        {
            return JAUS_FAILURE;
        }

        //  If in order, add directly after the last msg.
        if( header.mSequenceNumber == mMaxSeqNumber + 1 ) 
        {
            mDataSet.push_back( msg );
            mMaxSeqNumber++;
            mStreamHeader.mDataSize += header.mDataSize;
            result = JAUS_OK;
        }
        //  If the sequence number is greater than the next
        //  stream in the sequence, then we missed some data.
        else if( header.mSequenceNumber > mMaxSeqNumber + 1)
        {
            //  Add the missing stream numbers.
            for(UShort i = mMaxSeqNumber + 1; i < header.mSequenceNumber; i++ ) 
            {
                mMissing.insert( i );
            }
            //  Now add the stream data at the end.
            mDataSet.push_back( msg );
            mMaxSeqNumber = header.mSequenceNumber;
            mStreamHeader.mDataSize += header.mDataSize;
            result = JAUS_OK;
        }
        //  If less than current sequence number expected, then we need
        //  to try an insert the data into the sequence in order.
        else  
        {
            if( header.mSequenceNumber < mMaxSeqNumber ) 
            {                    
                //  Now we need to insert the data into
                //  the stream sequence
                Stream::List::iterator insert = mDataSet.begin();
                while( insert != mDataSet.end() ) {
                    Header iheader;
                    insert->Read( iheader, 0 );
                    // Check and see if this data has already been 
                    // added to the set
                    if(header.mSequenceNumber == header.mSequenceNumber)
                    {
                        return JAUS_FAILURE;
                    }
                    if( iheader.mSequenceNumber > header.mSequenceNumber ) 
                    {

                        insert = mDataSet.insert( insert, msg );
                        mStreamHeader.mDataSize += header.mDataSize;
                        result = JAUS_OK;
                        break;
                    }
                    insert++;
                }
                if( header.mSequenceNumber < mBaseSeqNumber ) 
                {                        
                    for(UShort s = header.mSequenceNumber + 1; s < mBaseSeqNumber; s++ )
                        mMissing.insert(s);

                    // Update the base sequence number to the new lowest
                    // value.
                    mBaseSeqNumber = header.mSequenceNumber;
                }
                else 
                {
                    //  Make sure to remove it from the list of known
                    //  missing packets.
                    std::set<UShort>::iterator iter = mMissing.find(header.mSequenceNumber);
                    if(iter != mMissing.end())
                    {
                        mMissing.erase(iter);
                    }
                }
            }
        }

        //  Check for end of stream.
        if( header.mDataFlag == Header::DataControl::Last ) 
        {
            mLastPacketFlag = true;
        }
        //  If the last stream was retransmitted because of
        //  some error, then the last stream won't have the
        //  data control flag set to Header::DataControl::Last.  Therefore,
        //  we can check if this is the last stream if the size
        //  is less than the max (end of data set) and the 
        //  sequence number is the largest.
        else if ( header.mDataSize < JAUS_MAX_DATA_SIZE && 
                  header.mDataFlag == Header::DataControl::Retransmit &&
                  header.mSequenceNumber >= mMaxSeqNumber ) 
        {
            mLastPacketFlag = true;
        }

        //  Check that we recevied the first msg.
        if( header.mDataFlag == Header::DataControl::First ) {

            mFirstPacketFlag = true;
            mBaseSeqNumber = header.mSequenceNumber;  
            mStreamHeader.mSequenceNumber = mBaseSeqNumber;
        }
        else if ( header.mDataSize == JAUS_MAX_DATA_SIZE &&
                  header.mDataFlag == Header::DataControl::Retransmit &&
                  header.mSequenceNumber == 0 ) 
        {
            mFirstPacketFlag = true;   
            mBaseSeqNumber = header.mSequenceNumber;
            mStreamHeader.mSequenceNumber = mBaseSeqNumber;
        }
        

        //  If we just found the first stream in the sequence, then
        //  we need to get rid of or add any missing numbers
        if( prevFirstPacketFlag != mFirstPacketFlag ) 
        {
            Stream::List::iterator spacket = mDataSet.begin();
            // Clear all missing values since these are not reliable anymore.
            mMissing.clear();
            UShort prevSeqNumber;
            spacket->Read( header, 0 );
            prevSeqNumber = header.mSequenceNumber;
            spacket++;

            while( spacket != mDataSet.end() ) 
            {
                spacket->Read(header, 0);
                //  If we have skipped a sequence number, then add it
                //  to the list of those missing.
                if( header.mSequenceNumber > prevSeqNumber + 1 ) 
                {
                    //  Add all the missing sequence numbers.
                    for( UShort s = prevSeqNumber + 1; s < header.mSequenceNumber; s++ )
                        mMissing.insert( s );
                }
                prevSeqNumber = header.mSequenceNumber;
                spacket++;
            }
        }

        if( mLastPacketFlag == true && mFirstPacketFlag && mMissing.size() == 0 )
        {
            mCompleteFlag = true;
        }
    }

    if( result == JAUS_OK )
        mUpdateTimeMs = Time::GetUtcTimeMs();


    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Converts the multi-packet stream in LargeDataSet
///   into a single JAUS message and stores the result in a Stream structure.
///
///   The result of the merged packets will be placed into the stream argument.
///   When finished it will have a single JAUS header, and one continuous
///   data field block.
///
///   \param stream The result of merging the multi-packet stream data.
///   \param header Optional argument, if not NULL, the read header data
///                 for stream is copied.
///
///   \return JAUS_OK if merged, otherwise JAUS_FAILURE due to invalid data.   
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::GetMergedStream(Stream& stream, Header* header) 
{
    if( mCompleteFlag ) 
    {
        return MergeLargeDataSet( mDataSet, stream, header);
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Changes the destination ID for all packets in the large data set.
///
///   \param id The new destination ID to change to.
///
///   \return JAUS_OK if changed, otherwise JAUS_FAILURE; 
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::ChangeDestinationID(const Address& id) 
{
    Header sheader;
    Stream::List::iterator spacket = mDataSet.begin();

    if( id.IsValid() ) {
        
        while( spacket != mDataSet.end() ) {
            (*spacket).Read( sheader, 0 );
            sheader.mDestinationID = id;
            (*spacket).Write( sheader, 0 );
            spacket++;
        }
        mStreamHeader.mDestinationID = id;
        return JAUS_OK;
    }

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get a pointer to the multi-packet stream data.  Make sure
///   that IsComplete returns true before you start using this data, otherwise
///   it is incomplete.
///
////////////////////////////////////////////////////////////////////////////////////
const Stream::List* LargeDataSet::GetDataSet() const
{
    return ( (Stream::List*)(&mDataSet) );
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Get the total size in bytes of all the packets in the stream.
///
///   \return Size in bytes of all the packets in the stream.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int LargeDataSet::GetDataSetSize() const
{
    Stream::List::const_iterator spacket = mDataSet.begin();
    unsigned int total = 0;
    while( spacket != mDataSet.end() ) 
    {
        total += (*spacket++).Length();
    }

    return total;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all data.
///
////////////////////////////////////////////////////////////////////////////////////
void LargeDataSet::Clear()
{
    mStreamHeader.Clear();
    mCompleteFlag = false;
    mLastPacketFlag = false;
    mFirstPacketFlag = false;
    mDataSet.clear();
    mMissing.clear();
    mMaxSeqNumber = 0;
    mBaseSeqNumber = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Check to see if a sequence number has been received and added to 
///          this data set already.
///
///   \param sequenceNumber The sequence number to check for within data set.
///
///   \return True if the sequence number has been added to the data set, otherwise
///           false.
///
////////////////////////////////////////////////////////////////////////////////////
bool LargeDataSet::HavePacketNumber(const UShort sequenceNumber) const
{
    if(mMissing.find(sequenceNumber) == mMissing.end())
    {
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints data about the assembly to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void LargeDataSet::Print() const
{
    cout << "\n=================================================\n";
    cout << "Multi-Stream Stream Information\n";
    if( mCompleteFlag )
        cout << "Number of packets in stream: " << mDataSet.size() << endl;
    else
        cout << "Number of packets collected for stream: " << mDataSet.size() << endl;
    cout << "Largest sequence number received: " << mMaxSeqNumber << endl;
    cout << "First stream received [0=no, 1=yes]: " << (int)mFirstPacketFlag << endl;
    cout << "Last stream received [0=no, 1=yes]: " << (int)mLastPacketFlag << endl;
    cout << "Total Data Size: " << mStreamHeader.mDataSize << endl;
    for( unsigned int i = 0; i < (unsigned int)mDataSet.size(); i++ ) {
        cout << "\n=================================================\n";
        mDataSet[i].Print();
    }
    if( mMissing.size() > 0 ) {
        cout << "Missing packets:\n";
        std::set<UShort>::const_iterator miss;

        for(miss = mMissing.begin(); miss != mMissing.end(); miss++) {
            cout << *miss << endl;
        }
    }
    cout << "\n=================================================\n";
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes a single JAUS message and splits it into a multi-packet
///   stream sequence.
///
///   The stream passed must contain a single JAUS message, and the data
///   size must be large than JAUS_MAX_PACKET_SIZE for it to be split into
///   a stream.  JAUS_FAILURE will be returned if attempts to make a stream
///   out of a properly formatted stream is attempted.
///
///   \param msg The message stream to split into a multi-packet stream
///                 sequence.  The size of the stream must be larger than
///                 JAUS_MAX_PACKET_SIZE.
///   \param stream The resulting multi-packet stream sequence.  This will
///                 contain all packets in the stream in order of sequence
///                 number.
///
///   \return JAUS_OK if successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::CreateLargeDataSet(const Stream& msg, 
                                     Stream::List& stream)
{
    Header theader;     //  Main theader to build off of.
    Header sheader;     //  Header for stream in stream.
    Stream spacket;     //  Stream msg.
    unsigned char *ptr = NULL;
    unsigned int total = 0;
    stream.clear();

    if( msg.Read(theader, 0) ) {
       
        //  If this is a small msg, then there
        //  is nothing to do.
        if( theader.mDataSize < JAUS_MAX_DATA_SIZE ) {
            return JAUS_FAILURE;
        }

        ptr = (unsigned char *)(msg.pPtr()) + JAUS_HEADER_SIZE;
        sheader = theader;
        sheader.mServiceConnectionFlag = theader.mServiceConnectionFlag;
        
        if( sheader.mServiceConnectionFlag == 0 )
            sheader.mSequenceNumber = 0;
        else
            sheader.mSequenceNumber = theader.mSequenceNumber;
        
        while( total < theader.mDataSize ) {

            if( stream.size() == 0 ) {
                sheader.mDataFlag = Header::DataControl::First;
            }
            else
                sheader.mDataFlag = Header::DataControl::Normal;

            //  Try write the largest amount possible. 
            sheader.mDataSize = JAUS_MAX_DATA_SIZE;
            //  If this data size is more than is remaining, use the
            //  remaining size for the theader, and change data flag to
            //  Header::DataControl::Last since this is the last.
            if( sheader.mDataSize >= theader.mDataSize - total ) {

                sheader.mDataSize = theader.mDataSize - total;
                sheader.mDataFlag = Header::DataControl::Last;
            }
            
            //  Create sequence stream data.
            spacket.Clear();
            spacket.Write( sheader );
            spacket.Write( ptr, sheader.mDataSize );
            if( spacket.Length() != sheader.mDataSize + JAUS_HEADER_SIZE ) {
                stream.clear();
                return JAUS_FAILURE;   //  Write failure.
            }
            //  Add to stream.
            stream.push_back(spacket);
            //  Advance pointer in buffered data to write and 
            //  increase number of bytes written
            ptr += sheader.mDataSize;
            total += sheader.mDataSize;
            sheader.mSequenceNumber++; //  Increase sequence number.

        }
    }

    if( stream.size() > 0 )
        return JAUS_OK;

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes a single JAUS message and splits it into a multi-packet
///   stream sequence and writes the results to a byte array.
///
///   The stream passed must contain a single JAUS message, and the data
///   size must be large than JAUS_MAX_PACKET_SIZE for it to be split into
///   a stream.  JAUS_FAILURE will be returned if attempts to make a stream
///   out of a properly formatted stream is attempted.
///
///   \param msg The message stream to split into a multi-packet stream
///                 sequence.  The size of the stream must be larger than
///                 JAUS_MAX_PACKET_SIZE.
///   \param buff Byte array to write to.
///   \param len Size of the byte array in bytes.
///   \param result Number of bytes written to buffer (optional parameter).
///
///   \return Number of bytes written if successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::CreateLargeDataSet(const Stream& msg, 
                                     unsigned char *buff,
                                     const unsigned int len,
                                     unsigned int* result)
{
    Header theader;     //  Main theader to build off of.
    Header sheader;     //  Header for stream in stream.
    Stream spacket;     //  Stream msg.
    unsigned char *ptr = NULL;
    unsigned int total = 0;
    unsigned int written = 0;
    unsigned char* buffPtr = NULL;    

    if( buff && len > 0 && msg.Read(theader, 0) ) {
       
        //  If this is a small msg, then there
        //  is nothing to do.
        if( theader.mDataSize < JAUS_MAX_DATA_SIZE ) {
            return JAUS_FAILURE;
        }

        if( result )
            *result = 0;

        ptr = (unsigned char *)(msg.pPtr()) + JAUS_HEADER_SIZE;
        sheader = theader;
        sheader.mServiceConnectionFlag = theader.mServiceConnectionFlag;
        if( sheader.mServiceConnectionFlag == 0 )
            sheader.mSequenceNumber = 0;
        else
            sheader.mSequenceNumber = theader.mSequenceNumber;

        buffPtr = buff;

        while( total < theader.mDataSize && written < len ) {

            if( written == 0 ) {
                sheader.mDataFlag = Header::DataControl::First;
            }
            else {
                sheader.mDataFlag = Header::DataControl::Normal;
            }

            //  Try write the largest amount possible. 
            sheader.mDataSize = JAUS_MAX_DATA_SIZE;
            //  If this data size is more than is remaining, use the
            //  remaining size for the theader, and change data flag to
            //  Header::DataControl::Last since this is the last.
            if( sheader.mDataSize >= theader.mDataSize - total ) {

                sheader.mDataSize = theader.mDataSize - total;
                sheader.mDataFlag = Header::DataControl::Last;
            }
            
            //  Create sequence stream data.
            spacket.Clear();
            spacket.Write( sheader );
            spacket.Write( ptr, sheader.mDataSize );
            if( spacket.Length() != sheader.mDataSize + JAUS_HEADER_SIZE ) {
                
                return JAUS_FAILURE;            //  Write failure.
            }
            //  Add to stream buffer, but check bounds first
            if( spacket.Length() + written > len ) {
                return JAUS_FAILURE;
            }
            memcpy( buffPtr, spacket.pPtr(), spacket.Length());
            //  Advance pointer in buffered data to write and 
            //  increase number of bytes written
            ptr += sheader.mDataSize;
            buffPtr += spacket.Length();
            written += spacket.Length();

            total += sheader.mDataSize;
            sheader.mSequenceNumber++; //  Increase sequence number.
        }
    }

    if( total == theader.mDataSize ) {
        if( result )
            *result = written;
        return written;
    }

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes a list of packets that are part of a Large Data Set and
///   combines them together into a single message Stream with one header and
///   a single continuous data block.
///
///   \param stream The multi-packet stream sequence.
///   \param msg The result of merging the multi-packet stream data.
///   \param header Optional argument, if not NULL, the read header data
///                 for stream is copied.
///
///   \return JAUS_OK if merged, otherwise JAUS_FAILURE due to invalid data.   
///
////////////////////////////////////////////////////////////////////////////////////
int LargeDataSet::MergeLargeDataSet(const Stream::List& stream, 
                                    Stream& msg,
                                    Header* header)
{
    Header sheader;
    Header theader;
    unsigned char *ptr = NULL;
    unsigned int count = 0;
    UShort prevSeqNumber = 0;
    msg.Clear();

    //  Check if this is an actual stream of data.
    if( stream.size() == 1 ) 
    {
        if( stream[0].Read( sheader, 0 ) &&
            sheader.mDataFlag == Header::DataControl::Single ) 
        {            
                msg = stream[0];
                if( header )
                    *header = sheader;
                return JAUS_OK;
        }
        return JAUS_FAILURE;
    }

    Stream::List::iterator spacket = (  (Stream::List*)(&stream) )->begin();

    while( spacket != stream.end() ) 
    {

        ptr = (unsigned char *)spacket->pPtr(); // Get pointer to data buffer.
        if( ptr == NULL )
            return JAUS_FAILURE;                // No stream data.
        if( !spacket->Read( sheader, 0 ) )
            return JAUS_FAILURE;                // Failed to get header data.

        if( count == 0 ) {
            prevSeqNumber = sheader.mSequenceNumber;
        }
        else if ( sheader.mSequenceNumber != prevSeqNumber + 1 ) 
        {
            return JAUS_FAILURE;                //  Data is out of sequence.
        }
        
        prevSeqNumber = sheader.mSequenceNumber;

        if( sheader.mDataSize == 0 ) 
        {
            return JAUS_FAILURE;                //  Malformed data in stream.
        }

        if( count == 0 ) 
        {
            theader = sheader;
            theader.mDataFlag = Header::DataControl::Single;
            msg.Write( theader );
        }
        else 
        {            
            if( sheader.mDataFlag == Header::DataControl::Single ) 
            {
                    return JAUS_FAILURE;      //  Malformed stream.
            }
            if( sheader.mDataFlag == Header::DataControl::Last &&
                count + 1 != stream.size() ) 
            {
                    return JAUS_FAILURE;      //  Out of sequence.
            }
            //  Add up theader size data.
            theader.mDataSize += sheader.mDataSize;
        }
        count++;
        spacket++;
        msg.Write( ptr + JAUS_HEADER_SIZE, sheader.mDataSize );        
    }

    if( theader.mDataSize != msg.Length() - JAUS_HEADER_SIZE )
        return JAUS_FAILURE;        // Data doesn't add up!

    //  Re-write the theader to the stream so that it
    //  contains the coorect data fields sizes, etc.
    msg.Write( theader, 0 );
    msg.SetReadPos( 0 );
    msg.SetWritePos( msg.Length() );

    if( header )
        *header = theader;

    return JAUS_OK; //  Success! Radical!
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Calculates the size of the stream that the large data stream will
///   produce.
///
///   \param msg The large data stream (greater than JAUS_MAX_PACKET_SIZE)
///                 that you want the multi-sequence stream size for.
///
///   \return Size of stream produced by splitting up stream in bytes.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int LargeDataSet::GetDataSetSize(const Stream& msg)
{
    unsigned int dsize = msg.Length() - JAUS_HEADER_SIZE;
    return GetNumberOfPackets(msg)*JAUS_HEADER_SIZE + dsize;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Calculates the number of packets that will be generated by breaking
///   up the data into a multi-packet stream.
///
///   \param msg The large data stream (greater than JAUS_MAX_PACKET_SIZE)
///                 that you want the multi-sequence stream size for.
///
///   \return Number of packets needed for the multi-packet stream.
///
////////////////////////////////////////////////////////////////////////////////////
unsigned int LargeDataSet::GetNumberOfPackets(const Stream& msg)
{
    unsigned int dsize = msg.Length() - JAUS_HEADER_SIZE;
    return ( dsize/JAUS_MAX_DATA_SIZE + ( (dsize%JAUS_MAX_DATA_SIZE > 0) ? 1 : 0) );
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Makes a copy of the assembly.
///
////////////////////////////////////////////////////////////////////////////////////
LargeDataSet& LargeDataSet::operator =(const LargeDataSet &stream)
{
    if(this != &stream) 
    {
        mCompleteFlag = stream.mCompleteFlag;
        mLastPacketFlag = stream.mLastPacketFlag;
        mFirstPacketFlag = stream.mFirstPacketFlag;
        mDataSet = stream.mDataSet;
        mMaxSeqNumber = stream.mMaxSeqNumber;
        mBaseSeqNumber = stream.mBaseSeqNumber;
        mMissing = stream.mMissing;
        mStreamHeader = stream.mStreamHeader;
    }
    return *this;
}

/*  End of File */
