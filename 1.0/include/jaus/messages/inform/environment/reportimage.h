////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportimage.h
///  \brief This file contains a JAUS message for the Report Image message.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 20 February 2007
///  <br>Last Modified: 7 May 2008
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
#ifndef _JAUS_REPORT_IMAGE_H
#define _JAUS_REPORT_IMAGE_H

#include "jaus/messages/message.h"

#ifdef __cplusplus

#include <vector>

namespace Jaus
{

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportImage
    ///   \brief Report image/audio/video data.  
    ///   
    ///   The Report Image message contains the data for transmission of one frame
    ///   of visual data.  This can be one still image, a frame from a vidoe stream,
    ///   audio data, or the combination of audio and video when supported by the
    ///   format definition.  The message size restrictions within JAUS may 
    ///   require that multiple transmission be used to convey one frame.
    ///
    ///   The format of the data is not specified by this message.  To find out 
    ///   the formats and data types supported by a Visual Sensor you must use the
    ///   appropriate JAUS query and command messages.
    ///
    ///   No compression and decompression capabilities exist with this message
    ///   class.  This functionality has been kept seperate from the base JAUS
    ///   library tree.  Please refer to the JAUS++ Video Library. The Video Library 
    ///   has software for compression/decompression of some common formats and 
    ///   components for creating a Visual Sensor Component and a Visual Sensor
    ///   Controller/Subscriber Component.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportImage : public Message
    {
    public:
        ReportImage();
        ReportImage(const ReportImage &gp);
        ~ReportImage();
        int SetImageData(const Byte* img,
                         const UInt length,
                         const bool copy = true);
        Byte* GetImageData() const { return mpImageData; }
        UInt GetDataSize() const { return mDataSize; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual Message* Clone() const { return new ReportImage(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody();
        ReportImage &operator=(const ReportImage &gp);
    protected:
        UInt mDataSize;     ///<  Size of the data in bytes.
        Byte* mpImageData;  ///<  Pointer to array containin video/audio data that is compressed.
    };


}

#endif

#endif
/* End of File */
