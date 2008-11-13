////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportplatformspecifications.cpp
///  \brief This file contains the message class for Report Platform
///  Specifications.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
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
#include "jaus/messages/inform/platform/reportplatformspecifications.h"
#include "jaus/messages/inform/informcodes.h"
#include <iostream>
#include <string.h>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformSpecifications::ReportPlatformSpecifications() : 
    Message(JAUS_REPORT_PLATFORM_SPECIFICATIONS)
{
    mPresenceVector = 0; 
    mFront = 0;              
    mBack = 0;               
    mRight = 0;              
    mLeft = 0;               
    mBottom = 0;             
    mTop = 0;                
    mXcg = 0;                
    mYcg = 0;                
    mZcg = 0;                
    mTurningRadius = 0;      
    mWheelBase = 0;          
    mTrackWidth = 0;         
    mStaticPitchOver = 0;    
    mStaticRollOver = 0;     
    mMaximumVelocityX = 0;   
    mMaximumVelocityY = 0;   
    mMaximumVelocityZ = 0;   
    mMaximumRollRate = 0;    
    mMaximumPitchRate = 0;   
    mMaximumYawRate = 0;     
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformSpecifications::ReportPlatformSpecifications(const ReportPlatformSpecifications& msg) : 
    Message(JAUS_REPORT_PLATFORM_SPECIFICATIONS)
{
    mPresenceVector = 0; 
    mFront = 0;              
    mBack = 0;               
    mRight = 0;              
    mLeft = 0;               
    mBottom = 0;             
    mTop = 0;                
    mXcg = 0;                
    mYcg = 0;                
    mZcg = 0;                
    mTurningRadius = 0;      
    mWheelBase = 0;          
    mTrackWidth = 0;         
    mStaticPitchOver = 0;    
    mStaticRollOver = 0;     
    mMaximumVelocityX = 0;   
    mMaximumVelocityY = 0;   
    mMaximumVelocityZ = 0;   
    mMaximumRollRate = 0;    
    mMaximumPitchRate = 0;   
    mMaximumYawRate = 0;     
    *this = msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformSpecifications::~ReportPlatformSpecifications()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param name 15 character maxim ASCII string for platform.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMobilityPlatformName(const std::string& name)
{
    if(name.size() <= 15)
    {
        mMobilityPlatformName = name;
        BitVector::SetBit(mPresenceVector, VectorBit::MobilityPlatformName, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToFront(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mFront = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Front, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToBack(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mBack = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Back, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToRight(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mRight = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Right, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToLeft(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mLeft = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Left, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToBottom(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mBottom = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Bottom, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetDistanceToTop(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mTop = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Top, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters for
///              center of gravity coordinate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetXcg(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mXcg = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Xcg, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters for
///              center of gravity coordinate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetYcg(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mYcg = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Ycg, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance from center of platform [0, 32.767] meters for
///              center of gravity coordinate.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetZcg(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mZcg = val;
        BitVector::SetBit(mPresenceVector, VectorBit::Zcg, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Turning radius, [0, 65.535] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetTurningRadius(const double val)
{
    if(val >= 0.0 && val <= 65.535)
    {
        mTurningRadius = val;
        BitVector::SetBit(mPresenceVector, VectorBit::TurningRadius, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance between fore-most and aft-most axle, [0, 65.535] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetWheelBase(const double val)
{
    if(val >= 0.0 && val <= 65.535)
    {
        mWheelBase = val;
        BitVector::SetBit(mPresenceVector, VectorBit::WheelBase, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Distance between center lines of left most and right most
///               driving devices, [0, 65.535] meters.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetTrackWidth(const double val)
{
    if(val >= 0.0 && val <= 65.535)
    {
        mTrackWidth = val;
        BitVector::SetBit(mPresenceVector, VectorBit::TrackWidth, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Incline angle where platform will pitch over at 0 velocity.
///              [0, 2.565] radians.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetStaticPitchOver(const double val)
{
    if(val >= 0.0 && val <= 2.565)
    {
        mStaticPitchOver = val;
        BitVector::SetBit(mPresenceVector, VectorBit::StaticPitchOver, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Incline angle where platform will roll over at 0 velocity.
///              [0, 2.565] radians.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetStaticRollOver(const double val)
{
    if(val >= 0.0 && val <= 2.565)
    {
        mStaticRollOver = val;
        BitVector::SetBit(mPresenceVector, VectorBit::StaticRollOver, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum velocity along axis [0, 65.534] meters/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumVelocityX(const double val)
{
    if(val >= 0.0 && val <= 65.534)
    {
        mMaximumVelocityX = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumVelocityX, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum velocity along axis [0, 65.534] meters/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumVelocityY(const double val)
{
    if(val >= 0.0 && val <= 65.534)
    {
        mMaximumVelocityY = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumVelocityY, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum velocity along axis [0, 65.534] meters/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumVelocityZ(const double val)
{
    if(val >= 0.0 && val <= 65.534)
    {
        mMaximumVelocityZ = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumVelocityZ, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum rotation along axis [0, 32.767] radians/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumRollRate(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mMaximumRollRate = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumRollRate, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum rotation along axis [0, 32.767] radians/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumPitchRate(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mMaximumPitchRate = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumPitchRate, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets the value of the field.
///
///   \param val Maximum rotation along axis [0, 32.767] radians/second.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::SetMaximumYawRate(const double val)
{
    if(val >= 0.0 && val <= 32.767)
    {
        mMaximumYawRate = val;
        BitVector::SetBit(mPresenceVector, VectorBit::MaximumYawRate, true);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clears all message body data.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPlatformSpecifications::ClearMessageBody()
{
    mPresenceVector = 0; 
    mFront = 0;              
    mBack = 0;               
    mRight = 0;              
    mLeft = 0;               
    mBottom = 0;             
    mTop = 0;                
    mXcg = 0;                
    mYcg = 0;                
    mZcg = 0;                
    mTurningRadius = 0;      
    mWheelBase = 0;          
    mTrackWidth = 0;         
    mStaticPitchOver = 0;    
    mStaticRollOver = 0;     
    mMaximumVelocityX = 0;   
    mMaximumVelocityY = 0;   
    mMaximumVelocityZ = 0;   
    mMaximumRollRate = 0;    
    mMaximumPitchRate = 0;   
    mMaximumYawRate = 0; 
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Writes the message body data to the stream.
///
///   \param msg The stream to write to.
///   \param version The desired version of the message to write.
///
///   \return Number of bytes written on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::WriteMessageBody(Stream& msg, const UShort version) const
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int written = 0;

        expected = JAUS_UINT_SIZE;
        written += msg.Write(mPresenceVector);
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MobilityPlatformName)) {
            expected += 15;
            char buffer[16];
            memset(buffer, 0, 16);
            strncpy(buffer, mMobilityPlatformName.c_str(), 15);
            written += msg.Write((unsigned char *)buffer, 15);
        } 

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Front)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mFront, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Back)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mBack, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Right)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mRight, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Left)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mLeft, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Bottom)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mBottom, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Top)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mTop, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Xcg)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mXcg, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Ycg)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mYcg, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Zcg)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mZcg, 32.767, 0.0, ScaledInteger::UShort);
        }     

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::TurningRadius)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mTurningRadius, 65.535, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::WheelBase)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mWheelBase, 65.535, 0.0, ScaledInteger::UShort);
        } 

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::TrackWidth)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mTrackWidth, 65.535, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticPitchOver)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mStaticPitchOver, 2.56, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticRollOver)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mStaticRollOver, 2.56, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityX)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumVelocityX, 65.534, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityY)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumVelocityY, 65.534, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityZ)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumVelocityZ, 65.534, 0.0, ScaledInteger::UShort);
        }   

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumRollRate)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumRollRate, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumPitchRate)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumPitchRate, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumYawRate)) {
            expected += JAUS_USHORT_SIZE;
            written += msg.Write(mMaximumYawRate, 32.767, 0.0, ScaledInteger::UShort);
        }

        if( expected == written ) 
        {
            return written;
        }
        else
        {
            SetJausError(ErrorCodes::WriteFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads the message body data from the stream.
///
///   \param msg The stream to read from.
///   \param version The desired version of the message to read.
///
///   \return Number of bytes read on success.  A return of 0 is not
///           an error (some messages have no message body), only a 
///           return of -1 and setting of an error code is
///           is a failure state.
///
////////////////////////////////////////////////////////////////////////////////////
int ReportPlatformSpecifications::ReadMessageBody(const Stream& msg, const UShort version)
{
    if(version <= JAUS_VERSION_3_4) 
    {
        int expected = 0;
        int read = 0;

        expected = JAUS_UINT_SIZE;
        read += msg.Read(mPresenceVector);
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MobilityPlatformName)) {
            expected += 15;
            char buffer[16];
            memset(buffer, 0, 16);
            read += msg.Read((unsigned char *)buffer, 15);
            mMobilityPlatformName = buffer;
        } 

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Front)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mFront, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Back)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mBack, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Right)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mRight, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Left)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mLeft, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Bottom)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mBottom, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Top)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mTop, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Xcg)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mXcg, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Ycg)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mYcg, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::Zcg)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mZcg, 32.767, 0.0, ScaledInteger::UShort);
        }     

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::TurningRadius)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mTurningRadius, 65.535, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::WheelBase)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mWheelBase, 65.535, 0.0, ScaledInteger::UShort);
        } 

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::TrackWidth)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mTrackWidth, 65.535, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticPitchOver)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mStaticPitchOver, 2.56, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticRollOver)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mStaticRollOver, 2.56, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityX)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumVelocityX, 65.534, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityY)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumVelocityY, 65.534, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityZ)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumVelocityZ, 65.534, 0.0, ScaledInteger::UShort);
        }   

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumRollRate)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumRollRate, 32.767, 0.0, ScaledInteger::UShort);
        }

        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumPitchRate)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumPitchRate, 32.767, 0.0, ScaledInteger::UShort);
        }
        
        if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumYawRate)) {
            expected += JAUS_USHORT_SIZE;
            read += msg.Read(mMaximumYawRate, 32.767, 0.0, ScaledInteger::UShort);
        }

        if( expected == read ) 
        {
            return read;
        }
        else
        {
            SetJausError(ErrorCodes::ReadFailure); return -1;
        }
    }
    else 
    {
        SetJausError(ErrorCodes::UnsupportedVersion); return -1;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message information to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPlatformSpecifications::Print() const
{
    Message::Print();
    PrintPlatformSpecifications();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints message body data to console window.
///
////////////////////////////////////////////////////////////////////////////////////
void ReportPlatformSpecifications::PrintPlatformSpecifications() const
{
    cout << "Platform Specifications:\n";

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MobilityPlatformName)) {
        cout << "Platform Name: " << mMobilityPlatformName << endl;
    } 

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Front)) {
        cout << "Distance to Front: " << mFront << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Back)) {
        cout << "Distance to Back: " << mBack << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Right)) {
        cout << "Distance to Right: " << mRight << " meters" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Left)) {
        cout << "Distance to Left: " << mLeft << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Bottom)) {
        cout << "Distance to Bottom: " << mBottom << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Top)) {
        cout << "Distance to Top: " << mTop << " meters" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Xcg)) {
        cout << "X Center of Gravity: " << mXcg << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Ycg)) {
        cout << "Y Center of Gravity: " << mYcg << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::Zcg)) {
        cout << "Z Center of Gravity: " << mZcg << " meters" << endl;
    }     

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::TurningRadius)) {
        cout << "Turning Radius: " << mTurningRadius << " meters" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::WheelBase)) {
        cout << "Wheel Base: " << mWheelBase << " meters" << endl;
    } 

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::TrackWidth)) {
        cout << "Track Width: " << mTrackWidth << " meters" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticPitchOver)) {
        cout << "Static Pitch Over: " << mStaticPitchOver << " radians (" << JAUS_RAD2DEG(mStaticPitchOver) << " degrees)" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::StaticRollOver)) {
        cout << "Static Roll Over: " << mStaticRollOver << " radians (" << JAUS_RAD2DEG(mStaticRollOver) << " degrees)" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityX)) {
        cout << "Maximum Velocity X: " << mMaximumVelocityX << " meters/second" <<  endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityY)) {
        cout << "Maximum Velocity Y: " << mMaximumVelocityY << " meters/second" <<  endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumVelocityZ)) {
        cout << "Maximum Velocity Z: " << mMaximumVelocityZ << " meters/second" <<  endl;
    }   

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumRollRate)) {
        cout << "Maximum Roll Rate: " << mMaximumRollRate << " rad/s (" << JAUS_RAD2DEG(mMaximumRollRate) << " degrees/s)" << endl;
    }

    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumPitchRate)) {
        cout << "Maximum Pitch Rate: " << mMaximumPitchRate << " rad/s (" << JAUS_RAD2DEG(mMaximumPitchRate) << " degrees/s)" << endl;
    }
    
    if(BitVector::IsBitSet(mPresenceVector, VectorBit::MaximumYawRate)) {
        cout << "Maximum Yaw Rate: " << mMaximumYawRate << " rad/s (" << JAUS_RAD2DEG(mMaximumYawRate) << " degrees/s)" << endl;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
ReportPlatformSpecifications& ReportPlatformSpecifications::operator=(const ReportPlatformSpecifications& msg)
{
    mPresenceVector =     msg.mPresenceVector; 
    mFront =              msg.mFront;            
    mBack =               msg.mBack;             
    mRight =              msg.mRight;            
    mLeft =               msg.mLeft;             
    mBottom =             msg.mBottom;           
    mTop =                msg.mTop;              
    mXcg =                msg.mXcg;              
    mYcg =                msg.mYcg;              
    mZcg =                msg.mZcg;              
    mTurningRadius =      msg.mTurningRadius;    
    mWheelBase =          msg.mWheelBase;        
    mTrackWidth =         msg.mTrackWidth;       
    mStaticPitchOver =    msg.mStaticPitchOver;  
    mStaticRollOver =     msg.mStaticRollOver;   
    mMaximumVelocityX =   msg.mMaximumVelocityX; 
    mMaximumVelocityY =   msg.mMaximumVelocityY; 
    mMaximumVelocityZ =   msg.mMaximumVelocityZ; 
    mMaximumRollRate =    msg.mMaximumRollRate;  
    mMaximumPitchRate =   msg.mMaximumPitchRate; 
    mMaximumYawRate =     msg.mMaximumYawRate;   
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size in bytes of the presence vector used by
///          the message.
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  size.  Defaults is current version of library.
///
///   \return Size in bytes of presence vector associated with message.
///
////////////////////////////////////////////////////////////////////////////////////
UShort ReportPlatformSpecifications::GetPresenceVectorSize(const UShort version) const { return JAUS_UINT_SIZE; }

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the bit mask associated with the message indicating the
///          bits used in the presence vector of this message (if it has one).
///
///   \param version Version of JAUS to use for acquiring presence vector
///                  mask.  Defaults is current version of library.
///
///   \return Presence vector mask associated with message.  This value
///           is used to determine what bits are used in the presence vector.
///
////////////////////////////////////////////////////////////////////////////////////
UInt ReportPlatformSpecifications::GetPresenceVectorMask(const UShort version) const { return 0x1FFFFF; }

/*  End of File */
