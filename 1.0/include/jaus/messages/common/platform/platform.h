////////////////////////////////////////////////////////////////////////////////////
///
///  \file platform.h
///  \brief Simple structure for storing information about a platform including
///  configuration, identification, and pose.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 April 2008
///  <br>Last Modified: 25 April 2008
///  <br>Copyright (c) 2008
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
#ifndef __JAUS_PLATFORM__H
#define __JAUS_PLATFORM__H

#include "jaus/messages/common/platform/globalpose.h"
#include "jaus/messages/common/configuration/configuration.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Platform
    ///   \brief Data structure for storing basic information about a vehicle/platform.
    ///   
    ///   This class is designed to store basic information about an unmanned system
    ///   including its configuration, identification, and global pose.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Platform
    {
    public:
        typedef std::map<Byte,Platform> Map; ///<  STL map of Platforms.
        Platform();
        Platform(const Platform& platform);
        virtual ~Platform();      
        int SetGlobalPose(const GlobalPose& pose);
        int AddGlobalPoseData(const GlobalPose& pose);
        int SetIdentification(const Identification& identification);
        int SetConfiguration(const Configuration::Subsystem& config);
        Byte GetSubsystemID() const { return mConfiguration.mSubsystemID; }
        GlobalPose* GetGlobalPose() const { return mpGlobalPose; }
        Identification* GetIdentification() const { return mpIdentification; }
        Configuration::Subsystem* GetConfiguration() const { return (Configuration::Subsystem*)&mConfiguration; }
        bool HaveGlobalPose() const { return mpGlobalPose != NULL ? true : false; }
        bool HaveIdentification() const { return mpIdentification != NULL ? true : false; }
        void ClearGlobalPose();
        void ClearIdentification();
        void Print() const;
        bool operator<(const Platform& platform) const;
        Platform& operator=(const Platform& platform);
    protected:  
        Configuration::Subsystem mConfiguration;  ///<  Configuration data.
        Identification* mpIdentification;         ///<  Identification info.
        GlobalPose* mpGlobalPose;                 ///<  Global pose info.
    };
};

#endif
/*  End of File */
