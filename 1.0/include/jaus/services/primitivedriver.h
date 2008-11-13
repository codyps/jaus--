////////////////////////////////////////////////////////////////////////////////////
///
///  \file primitivedriver.h
///  \brief This file contains software for creating a JAUS Primitive Driver
///  component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 30 April 2008
///  <br>Created: 1 May 2008
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
#ifndef __JAUS_PRIMITIVE_DRIVER__H
#define __JAUS_PRIMITIVE_DRIVER__H

#include "jaus/services/srvclibdll.h"
#include "jaus/components/commandcomponent.h"
#include "jaus/messages/command/platform/setwrencheffort.h"
#include "jaus/messages/query/platform/querywrencheffort.h"
#include "jaus/messages/inform/platform/reportwrencheffort.h"


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class PrimitiveDriver
    ///   \brief Interface for creating Primitive Driver Components (ID 33).
    ///
    ///   The Primitive Driver component performs basic driving and all related
    ///   mobility functions including operation of common platform devices such
    ///   as the engine and lights.
    ///
    ///   This component does not imply any particular platform type such as tracked or
    ///   wheeled, but describes mobility in six degrees of freedom using a percent
    ///   of available effort in each direction.  Additionally, no power plant
    ///   (gasoline, diesel, or battery) is implied and the component functions
    ///   strictly in an open loop manner, i.e. a velocity is not commanded since 
    ///   that requires a speed sensor.  Note that the specific actuator commands are
    ///   not defined by JAUS.
    ///
    ///   Inputs: Set Wrench Effort, Set Discrete Devices, Query Platform 
    ///   Specifications, Query Platform Operational Data, Query Wrench Effort,
    ///   Query Discrete Devices.
    ///
    ///   Outputs: Report Platform Specifications, Report Platform Operational Data,
    ///   Report Wrench Effort, Report Discrete Devices.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL PrimitiveDriver : public CommandComponent
    {
    public:
        PrimitiveDriver();
        ~PrimitiveDriver();
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        virtual int ProcessCommandMessage(const Message* msg, 
                                          const Byte commandAuthority);
        virtual int ProcessQueryMessage(const Message* msg);
        virtual int SetWrenchEffort(const Jaus::SetWrenchEffort* command) = 0;
        virtual int SetDiscreteDevices(const Jaus::SetDiscreteDevices* command) = 0;
        virtual int ReportWrenchEffort(const QueryWrenchEffort* query) = 0;
        virtual int ReportPlatformSpecifications(const QueryPlatformSpecifications* query) = 0;
        //virtual int ReportDiscreteDevices(const QueryDiscreteDevices* query) = 0;
        //virtual int ReportPlatformOperationalData(const QueryPlatformOperationalData* query) = 0;
    };
}

#endif
/*  End of File */
