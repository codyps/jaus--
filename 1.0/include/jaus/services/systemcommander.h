////////////////////////////////////////////////////////////////////////////////////
///
///  \file systemcommander.h
///  \brief This file contains a basic system commander interface.  It will
///  discover basic information about JAUS subsystem it can talk to including
///  Global Pose.  It also incorporates a joystick interface for telemanipulation
///  of platforms.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 18 December 2008
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
#ifndef __JAUS_SYSTEM_COMMANDER__H
#define __JAUS_SYSTEM_COMMANDER__H

#include "jaus/services/joystickdriver.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SystemCommander
    ///   \brief Basic System Commander interface.  This component will gather
    ///   system configuration data, and allows for direct control of a vehicle.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL SystemCommander : public JoystickDriver
    {
    public:
        SystemCommander();
        virtual ~SystemCommander();
        virtual int Initialize(const Byte subsystem, const Byte node, const Byte instance = 0);
        virtual int SetupService();
        // If discovery is enabled, this function is called when an event happens.
        virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                          const SubscriberComponent::DiscoveryEvents eventType);
        // Sets what rates are desired for get GPS and other data for platforms.
        void SetSubscriptionPeriodicUpdateRate(const double desired, const double minimum = 0.5);
    protected:
        double mDesiredPeriodicUpdateRate;  ///<  Desired update rate for global pose and other data.
        double mMinimumPeriodicUpdateRate;  ///<  Minimum acceptable update rate for global pose and other data.
    };
}


#endif
/*  End of File */
