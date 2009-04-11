////////////////////////////////////////////////////////////////////////////////////
///
///  \file querycodes.h
///  \brief This file lists all command code definitions for query messeages.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 27 January 2007
///  <br>Last Modified: 24 July 2007
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
#ifndef _JAUS_QUERY_CLASS_CODES_H
#define _JAUS_QUERY_CLASS_CODES_H

#include "jaus/messages/types.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \brief Checks if the message code is for a query.
    ///
    ///   \param code The message code to check.
    ///
    ///   \return True if query message, otherwise false.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    inline bool IsQueryMessage(const UShort code)
    {
        if(code >= 0x2000 && code <= 0x3FFF)
        {
            return true;
        }
        return false;
    }

    //  Core Subgroup - Codes 2000-21FF
    const UShort JAUS_QUERY_COMPONENT_AUTHORITY                            =   0x2001;
    const UShort JAUS_QUERY_COMPONENT_STATUS                               =   0x2002;
    const UShort JAUS_QUERY_TIME                                           =   0x2011;
    const UShort JAUS_QUERY_COMPONENT_CONTROL                              =   0x200D;
    //  Event Setup and Control Subgroup - Code 21F0
    const UShort JAUS_QUERY_EVENTS                                         =   0x21F0;
    //  Communication Subgroup - Codes 2200-23FF
    const UShort JAUS_QUERY_DATA_LINK_STATUS                               =   0x2200;
    const UShort JAUS_QUERY_SELECTED_DATA_LINK_STATUS                      =   0x2201;
    const UShort JAUS_QUERY_HEARTBEAT_PULSE                                =   0x2202;
    //  Platform Subgroup - Codes 2400-25FF
    const UShort JAUS_QUERY_PLATFORM_SPECIFICATIONS                        =   0x2400;
    const UShort JAUS_QUERY_PLATFORM_OPERATIONAL_DATA                      =   0x2401;
    const UShort JAUS_QUERY_GLOBAL_POSE                                    =   0x2402;
    const UShort JAUS_QUERY_LOCAL_POSE                                     =   0x2403;
    const UShort JAUS_QUERY_VELOCITY_STATE                                 =   0x2404;
    const UShort JAUS_QUERY_WRENCH_EFFORT                                  =   0x2405;
    const UShort JAUS_QUERY_DISCRETE_DEVICES                               =   0x2406;
    const UShort JAUS_QUERY_GLOBAL_VECTOR                                  =   0x2407;
    const UShort JAUS_QUERY_LOCAL_VECTOR                                   =   0x2408;
    const UShort JAUS_QUERY_TRAVEL_SPEED                                   =   0x240A;
    const UShort JAUS_QUERY_WAYPOINT_COUNT                                 =   0x240B;
    const UShort JAUS_QUERY_GLOBAL_WAYPOINT                                =   0x240C;
    const UShort JAUS_QUERY_LOCAL_WAYPOINT                                 =   0x240D;
    const UShort JAUS_QUERY_PATH_SEGMENT_COUNT                             =   0x240E;
    const UShort JAUS_QUERY_GLOBAL_PATH_SEGMENT                            =   0x240F;
    const UShort JAUS_QUERY_LOCAL_PATH_SEGMENT                             =   0x2410;
    //  Manipulator Subgroup - Codes 4001-41FF                                       
    const UShort JAUS_QUERY_MANIPULATOR_SPECIFICATIONS                     =   0x2600;
    const UShort JAUS_QUERY_JOINT_EFFORTS                                  =   0x2601;
    const UShort JAUS_QUERY_JOINT_POSITIONS                                =   0x2602;
    const UShort JAUS_QUERY_JOINT_VELOCITIES                               =   0x2603;
    const UShort JAUS_QUERY_TOOL_POINT                                     =   0x2604;
    const UShort JAUS_QUERY_JOINT_FORCE_TORQUES                            =   0x2605;
    //  Environment Sensor Subgroup - Codes 2800-29FF                                
    const UShort JAUS_QUERY_CAMERA_POSE                                    =   0x2800;
    const UShort JAUS_QUERY_CAMERA_COUNT                                   =   0x2801;
    const UShort JAUS_QUERY_RELATIVE_OBJECT_POSITION                       =   0x2802;
    const UShort JAUS_QUERY_SELECTED_CAMERA                                =   0x2804;
    const UShort JAUS_QUERY_CAMERA_CAPABILITIES                            =   0x2805;
    const UShort JAUS_QUERY_CAMERA_FORMAT_OPTIONS                          =   0x2806;
    const UShort JAUS_QUERY_IMAGE                                          =   0x2807;
    const UShort JAUS_QUERY_WATER_DEPTH                                    =   0x280C;
    //  World Model Subgroup - Codes 2A00-2AFF
    const UShort JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA  =   0x2A21;
    const UShort JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_BOUNDS                  =   0x2A22;
    const UShort JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_OBJECTS                 =   0x2A23;
    //  Dynamic Configuration Subgroup - Codes 2B00-2BFF
    const UShort JAUS_QUERY_IDENTIFICATION                                 =   0x2B00;
    const UShort JAUS_QUERY_CONFIGURATION                                  =   0x2B01;
    const UShort JAUS_QUERY_SUBSYSTEM_LIST                                 =   0x2B02;
    const UShort JAUS_QUERY_SERVICES                                       =   0x2B03;
    //  Payload Subgroup - Codes 2D00-2DFF
    const UShort JAUS_QUERY_PAYLOAD_INTERFACE_MESSAGE                      =   0x2D00;
    const UShort JAUS_QUERY_PAYLOAD_DATA_ELEMENT                           =   0x2D01;
    //  Planning Subgroup - Codes 2E00-2EFF
    const UShort JAUS_QUERY_SPOOLING_PREFERENCE                            =   0x2E00;
    const UShort JAUS_QUERY_MISSION_STATUS                                 =   0x2E01;
}

#endif
/*  End of File */
