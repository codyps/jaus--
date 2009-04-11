////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportwaterdepth.h
///  \brief This message is used to provide the receiver the water depth in meters.
///
///  <br>Author(s): David Adams
///  <br>Created: 4 April 2009
///  <br>Last Modified: 4 April 2009
///  <br>Copyright (c) 2009
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#ifndef _JAUS_REPORT_WATER_DEPTH_H
#define _JAUS_REPORT_WATER_DEPTH_H

#include "jaus/messages/message.h"

namespace Jaus
{

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportWaterDepth
    ///   \brief This message is used to provide the receiver the water depth in meters.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportWaterDepth : public Message
    {
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Contains constants for limit values of data members of class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MinWaterDepth;       ///<  Minimum depth. (0m)
            const static double MaxWaterDepth;       ///<  Maximum depth. (20000m)
        };
    public:
        ReportWaterDepth();
        ReportWaterDepth(const ReportWaterDepth &gp);
        virtual ~ReportWaterDepth();
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual Message* Clone() const { return new ReportWaterDepth(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody();
        ReportWaterDepth &operator=(const ReportWaterDepth &gp);

        double GetDepth ()const {return mWaterDepth;}
        int SetDepth(const double value);
    protected:
        double mWaterDepth; ///< Water depth in meters. [0,20000]
    };


}

#endif
/* End of File */
