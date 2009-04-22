////////////////////////////////////////////////////////////////////////////////////
///
///  \file informcodes.h
///  \brief This file lists all command code definitions for inform messeages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 27 January 2007
///  <br>Last Modified: 9 July 2007
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
#ifndef _JAUS_INFORM_CLASS_CODES_H
#define _JAUS_INFORM_CLASS_CODES_H

#include "jaus/messages/types.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief Checks if the message code is for an inform.
    ///
    ///   \param code The message code to check.
    ///
    ///   \return True if inform  message, otherwise false.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    inline bool IsInformMessage(const UShort code)
    {
        if(code >= 0x4000 && code <= 0x5FFF)
        {
            return true;
        }
        return false;
    }
    //  Core Subgroup - Codes 4001-41FF
    const UShort JAUS_REPORT_COMPONENT_AUTHORITY                                = 0x4001;
    const UShort JAUS_REPORT_COMPONENT_STATUS                                   = 0x4002;
    const UShort JAUS_REPORT_TIME                                               = 0x4003;
    const UShort JAUS_REPORT_COMPONENT_CONTROL                                  = 0x400D;
    //  Event Setup and Control - Codes 41F0-41F
    const UShort JAUS_REPORT_EVENTS                                             = 0x41F0;
    const UShort JAUS_EVENT                                                     = 0x41F1;
    //  Communications Subgroup - Codes 4200-43FF
    const UShort JAUS_REPORT_DATA_LINK_STATUS                                   = 0x4200;
    const UShort JAUS_REPORT_SELECTED_DATA_LINK_STATUS                          = 0x4201;
    const UShort JAUS_REPORT_HEARTBEAT_PULSE                                    = 0x4202;
    //  Platform Subgroup - Codes 4400-45FF
    const UShort JAUS_REPORT_PLATFORM_SPECIFICATIONS                            = 0x4400;
    const UShort JAUS_REPORT_PLATFORM_OPERATIONAL_DATA                          = 0x4401;
    const UShort JAUS_REPORT_GLOBAL_POSE                                        = 0x4402;
    const UShort JAUS_REPORT_LOCAL_POSE                                         = 0x4403;
    const UShort JAUS_REPORT_VELOCITY_STATE                                     = 0x4404;
    const UShort JAUS_REPORT_WRENCH_EFFORT                                      = 0x4405;
    const UShort JAUS_REPORT_DISCRETE_DEVICES                                   = 0x4406;
    const UShort JAUS_REPORT_GLOBAL_VECTOR                                      = 0x4407;
    const UShort JAUS_REPORT_LOCAL_VECTOR                                       = 0x4408;
    const UShort JAUS_REPORT_TRAVEL_SPEED                                       = 0x440A;
    const UShort JAUS_REPORT_WAYPOINT_COUNT                                     = 0x440B;
    const UShort JAUS_REPORT_GLOBAL_WAYPOINT                                    = 0x440C;
    const UShort JAUS_REPORT_LOCAL_WAYPOINT                                     = 0x440D;
    const UShort JAUS_REPORT_PATH_SEGMENT_COUNT                                 = 0x440E;
    const UShort JAUS_REPORT_GLOBAL_PATH_SEGMENT                                = 0x440F;
    const UShort JAUS_REPORT_LOCAL_PATH_SEGMENT                                 = 0x4410;
    const UShort JAUS_REPORT_DEVICE_POWER_STATUS                                = 0x4415;
    //  Manipulator Subgroup - Codes 4600-47FF
    const UShort JAUS_REPORT_MANIPULATOR_SPECIFICATIONS                         = 0x4600;
    const UShort JAUS_REPORT_JOINT_EFFORTS                                      = 0x4601;
    const UShort JAUS_REPORT_JOINT_POSITIONS                                    = 0x4602;
    const UShort JAUS_REPORT_JOINT_VELOCITIES                                   = 0x4603;
    const UShort JAUS_REPORT_TOOL_POINT                                         = 0x4604;
    const UShort JAUS_REPORT_JOINT_FORCE_TORQUES                                = 0x4605;
    //  Environmental Sensor Subgroup - Codes 4800-49FF
    const UShort JAUS_REPORT_CAMERA_POSE                                        = 0x4800;
    const UShort JAUS_REPORT_CAMERA_COUNT                                       = 0x4801;
    const UShort JAUS_REPORT_RELATIVE_OBJECT_POSITION                           = 0x4802;
    const UShort JAUS_REPORT_SELECTED_CAMERA                                    = 0x4804;
    const UShort JAUS_REPORT_CAMERA_CAPABILITIES                                = 0x4805;
    const UShort JAUS_REPORT_CAMERA_FORMAT_OPTIONS                              = 0x4806;
    const UShort JAUS_REPORT_IMAGE                                              = 0x4807;
    const UShort JAUS_REPORT_WATER_DEPTH                                        = 0x480C;
    //  World Model Subgroup - Codes 4A00-4AFF
    const UShort JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_OBJECTS_CREATION            = 0x4A20;
    const UShort JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA      = 0x4A21;
    const UShort JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_BOUNDS                      = 0x4A22;
    const UShort JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_OBJECTS                     = 0x4A23;
    const UShort JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_DATA_TRANSFER_TERMINATION   = 0x4A24;
    //  Dynamic Configuration Subgroup - Codes 4B00-4BFF
    const UShort JAUS_REPORT_IDENTIFICATION                                     = 0x4B00;
    const UShort JAUS_REPORT_CONFIGURATION                                      = 0x4B01;
    const UShort JAUS_REPORT_SUBSYSTEM_LIST                                     = 0x4B02;
    const UShort JAUS_REPORT_SERVICES                                           = 0x4B03;
    //  Payload Subgroup - Codes 4D00-4DFF
    const UShort JAUS_REPORT_PAYLOAD_INTERFACE_MESSAGE                          = 0x4D00;
    const UShort JAUS_REPORT_PAYLOAD_DATA_ELEMENT                               = 0x4D01;
    //  Planning Subgroup - Codes 4E00-4EFF
    const UShort JAUS_REPORT_SPOOLING_PREFERENCE                                = 0x4E00;
    const UShort JAUS_REPORT_MISSION_STATUS                                     = 0x4E01;
}

#endif
/*  End of File */
