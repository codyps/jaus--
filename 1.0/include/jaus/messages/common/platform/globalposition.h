////////////////////////////////////////////////////////////////////////////////////
///
///  \file globalposition.h
///  \brief This file contains a simple data structure for global position
///  data in JAUS.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 26 August 2007
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
#ifndef __JAUS__GLOBAL_POSITION_H
#define __JAUS__GLOBAL_POSITION_H

#include "jaus/messages/types.h"

#ifdef __cplusplus

namespace Jaus
{

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class GlobalPosition
    ///   \brief Simple position structure in JAUS.  Position data is stored 
    ///   using WGS 84 standard.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL GlobalPosition
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MaxLatitude;
            const static double MaxLongitude;
            const static double MaxElevation;
            const static double MinLatitude;
            const static double MinLongitude;
            const static double MinElevation;
        };
        GlobalPosition();
        GlobalPosition(const double lat,
                       const double lon);
        GlobalPosition(const double lat, 
                       const double lon, 
                       const double elev);
        GlobalPosition(const GlobalPosition& pos);
        virtual ~GlobalPosition();
        virtual int SetPosition(const double lat,
                                const double lon,
                                const double elev);
        virtual int SetLatitude(const double lat);
        virtual int SetLongitude(const double lon);
        virtual int SetElevation(const double elev);
        virtual int GetPosition(double& lat,
                                double& lon,
                                double& elev) const;
        virtual int GetLatitude(double& lat) const;
        virtual int GetLongitude(double& lon) const;
        virtual int GetElevation(double& elev) const;
        virtual void ClearPosition();
        virtual void ClearLatitude();
        virtual void ClearLongitude();
        virtual void ClearElevation();
        virtual void PrintPosition() const;
        virtual double GetLatitude() const;
        virtual double GetLongitude() const;
        virtual double GetElevation() const;
        virtual bool HaveElevation() const { return mHaveElevation; }
        GlobalPosition& operator=(const GlobalPosition& pos);
    protected:
        bool mHaveElevation;    ///<  Is elevation data present?
        double mLatitude;       ///<  Latittude in degrees (WGS 84).
        double mLongitude;      ///<  Longitude in degrees (WGS 84).
        double mElevation;      ///<  Elevation in meters around mean sea level. [-10,000,30,000].
    };

}

#endif
#endif
/*  End of File */
