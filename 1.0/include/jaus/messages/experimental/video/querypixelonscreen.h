////////////////////////////////////////////////////////////////////////////////////
///
///  \file querypixelonscreen.h
///  \brief This file contains a custom experimental message to query a 
///  video component for information at a specific pixel.
///
///  <br>Author(s): Chris Hollander
///  <br>Created: 10 January 2009
///  <br>Copyright (c) 2009
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: cholland@ist.ucf.edu
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
#ifndef __JAUS_EXPERIMENTAL_QUERY_PIXEL_ON_SCREEN__H
#define __JAUS_EXPERIMENTAL_QUERY_PIXEL_ON_SCREEN__H


#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class QueryPixelOnScreen
    ///   \brief This is a custom message used to query a visual sensor to identify
    ///   what something is in an image.  The response is a ReportPixelOnScreen
    ///   message.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL QueryPixelOnScreen : public Message
    {
    public:
        QueryPixelOnScreen();
        QueryPixelOnScreen(const QueryPixelOnScreen& query);
        virtual ~QueryPixelOnScreen();
        int SetPixelLocation(const Float x, const Float y);
        Float GetPixelX() const { return mX; }
        Float GetPixelY() const { return mY; }
        virtual int WriteMessageBody(Stream& msg,
                                     const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, 
                                    const UShort version);
        virtual Message* Clone() const { return new QueryPixelOnScreen(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody() {}
        QueryPixelOnScreen& operator=(const QueryPixelOnScreen& query);
    protected:
        Float mX;   ///<  Normalized X position in image (0 is left side, 1.0 is right side of image).
        Float mY;   ///<  Normalized Y position in image (0 is bottom of image, 1.0 is top of image).
    };
}



#endif
/*  End of File */
