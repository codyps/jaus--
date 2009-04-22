////////////////////////////////////////////////////////////////////////////////////
///
///  \file commandcodes.h
///  \brief This file lists all command code definitions for command messeages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 27 January 2007
///  <br>Last Modified: 26 February 2008
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
#ifndef _JAUS_COMMAND_CLASS_CODES_H
#define _JAUS_COMMAND_CLASS_CODES_H

#include <string>
#include "jaus/messages/types.h"

namespace Jaus 
{
    bool IsCommandMessage(const UShort code);

    //  Core Subgroup - Codes 0001-01FF
    const UShort JAUS_SET_COMPONENT_AUTHORITY                              =       0x0001;
    const UShort JAUS_SHUTDOWN                                             =       0x0002;
    const UShort JAUS_STANDBY                                              =       0x0003;
    const UShort JAUS_RESUME                                               =       0x0004;
    const UShort JAUS_RESET                                                =       0x0005;
    const UShort JAUS_SET_EMERGENCY                                        =       0x0006;
    const UShort JAUS_CLEAR_EMERGENCY                                      =       0x0007;
    const UShort JAUS_CREATE_SERVICE_CONNECTION                            =       0x0008;
    const UShort JAUS_CONFIRM_SERVICE_CONNECTION                           =       0x0009;
    const UShort JAUS_ACTIVATE_SERVICE_CONNECTION                          =       0x000A;
    const UShort JAUS_SUSPEND_SERVICE_CONNECTION                           =       0x000B;
    const UShort JAUS_TERMINATE_SERVICE_CONNECTION                         =       0x000C;
    const UShort JAUS_REQUEST_COMPONENT_CONTROL                            =       0x000D;
    const UShort JAUS_RELEASE_COMPONENT_CONTROL                            =       0x000E;
    const UShort JAUS_CONFIRM_COMPONENT_CONTROL                            =       0x000F;
    const UShort JAUS_REJECT_COMPONENT_CONTROL                             =       0x0010;
    const UShort JAUS_SET_TIME                                             =       0x0011;
    //  Event Setup and Control - Codes 01F0-01FF
    const UShort JAUS_CREATE_EVENT                                         =       0x01F0;
    const UShort JAUS_UPDATE_EVENT                                         =       0x01F1;
    const UShort JAUS_CANCEL_EVENT                                         =       0x01F2;
    const UShort JAUS_CONFIRM_EVENT_REQUEST                                =       0x01F3;
    const UShort JAUS_REJECT_EVENT_REQUEST                                 =       0x01F4;
    //  Communications Subgroup - Codes 0200-03FF
    const UShort JAUS_SET_DATA_LINK_STATE                                  =       0x0200;
    const UShort JAUS_SET_DATA_LINK_SELECT                                 =       0x0201;
    const UShort JAUS_SET_SELECTED_DATA_LINK_STATE                         =       0x0202;
    //  Platfrom Subroup - Codes 0400-05FF
    const UShort JAUS_SET_WRENCH_EFFORT                                    =       0x0405;
    const UShort JAUS_SET_DISCRETE_DEVICES                                 =       0x0406;
    const UShort JAUS_SET_GLOBAL_VECTOR                                    =       0x0407;
    const UShort JAUS_SET_LOCAL_VECTOR                                     =       0x0408;
    const UShort JAUS_SET_TRAVEL_SPEED                                     =       0x040A;
    const UShort JAUS_SET_GLOBAL_WAYPOINT                                  =       0x040C;
    const UShort JAUS_SET_LOCAL_WAYPOINT                                   =       0x040D;
    const UShort JAUS_SET_GLOBAL_PATH_SEGMENT                              =       0x040F;
    const UShort JAUS_SET_LOCAL_PATH_SEGMENT                               =       0x0410;
    const UShort JAUS_SET_DEVICE_POWER_STATUS                              =       0x0415;
    //  Manipulator Subgroup - Codes 0600-07FF
    const UShort JAUS_SET_JOINT_EFFORTS                                    =       0x0601;
    const UShort JAUS_SET_JOINT_POSITIONS                                  =       0x0602;
    const UShort JAUS_SET_JOINT_VELOCITIES                                 =       0x0603;
    const UShort JAUS_SET_TOOL_POINT                                       =       0x0604;
    const UShort JAUS_SET_END_EFFECTOR_POSE                                =       0x0605;
    const UShort JAUS_SET_END_EFFECTOR_VELOCITY_STATE                      =       0x0606;
    const UShort JAUS_SET_JOINT_MOTION                                     =       0x0607;
    const UShort JAUS_SET_END_EFFECTOR_PATH_MOTION                         =       0x0608;
    //  Environment Sensor Subgroup - Codes 0800-09FF   
    const UShort JAUS_SET_CAMERA_POSE                                      =       0x0801;
    const UShort JAUS_SELECT_CAMERA                                        =       0x0802;
    const UShort JAUS_SET_CAMERA_CAPABILITIES                              =       0x0805;
    const UShort JAUS_SET_CAMERA_FORMAT_OPTIONS                            =       0x0806;
    //  World Model Subgroup - Codes 0A00-0AFF
    const UShort JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS                =       0x0A20;
    const UShort JAUS_SET_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA    =       0x0A21;
    const UShort JAUS_TERMINATE_VECTOR_KNOWLEDGE_STORE_DATA_TRANSFER       =       0x0A24;
    const UShort JAUS_DELETE_KNOWLEDGE_STORE_OBJECTS                       =       0x0A25;
    //  Dynamic Configuration Subgroups - Codes 0B00-0BFF               
    //  Payload Subgroup - Codes 0D00-0DFF
    const UShort JAUS_SET_PAYLOAD_DATA_ELEMENT                             =       0x0D01;
    //  Planning Subgroup - Codes 0E00-0EFF
    const UShort JAUS_SPOOL_MISSION                                        =       0x0E00;
    const UShort JAUS_RUN_MISSION                                          =       0x0E01;
    const UShort JAUS_ABORT_MISSION                                        =       0x0E02;
    const UShort JAUS_PAUSE_MISSION                                        =       0x0E03;
    const UShort JAUS_RESUME_MISSION                                       =       0x0E04;
    const UShort JAUS_REMOVE_MESSAGES                                      =       0x0E05;
    const UShort JAUS_REPLACE_MESSAGES                                     =       0x0E06;

    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief Checks if the message code is for a command.
    ///
    ///   \param code The message code to check.
    ///
    ///   \return True if command message, otherwise false.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    inline bool IsCommandMessage(const UShort code)
    {
        if(code > 0x0000 && code <= 0x1FFF)
        {
            return true;
        }
        return false;
    }
}

#endif
/*  End of File */
