////////////////////////////////////////////////////////////////////////////////////
///
///  \file messagecreator.cpp
///  \brief This file contains methods for allocating Message based
///  structures based on desired command code.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 26 February 2007
///  <br>Last Modified: 8 March 2008
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
#include "jaus/messages/messagecreator.h"
#include "jaus/messages/command/commandmessages.h"
#include "jaus/messages/query/querymessages.h"
#include "jaus/messages/inform/informmessages.h"
#include "jaus/messages/experimental/experimentalmessages.h"
#include <cxutils/mutex.h>
#include <map>
#include <assert.h>
#include <string.h>

using namespace Jaus;
using namespace std;

std::map<UShort, Message*> sCustomMessages;
CxUtils::Mutex sCustomMessagesMutex;

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
MessageCreator::MessageCreator()
{
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Desturctor.
///
////////////////////////////////////////////////////////////////////////////////////
MessageCreator::~MessageCreator()
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is used to add a custom message, one not defined by
///   the JAUS standard, to the MessageCreator so that components can
///   de-serialize received data automatically.
///
///   This functionality is used to add new messages you have created to the
///   JAUS interfaces used within this library.  It allows you to add new
///   messages without the need to modify any of the JAUS++ files.
///
///   \param[in] customMessage Pointer to a custom message which will be used
///              to create more messages of the type defined for reading
///              received message data by components.
///
///   \return OK if added, FAILURE if another message with the same command
///           code has been used.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageCreator::AddCustomMessage(Message* customMessage)
{
    int result = FAILURE;
    sCustomMessagesMutex.Enter();
    std::map<UShort, Message*>::iterator msg;
    msg = sCustomMessages.find(customMessage->GetCommandCode());
    if(msg == sCustomMessages.end())
    {
        sCustomMessages[customMessage->GetCommandCode()] = customMessage;
        customMessage->ClearMessage();
        result = OK;
    }
    sCustomMessagesMutex.Leave();
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes any memory allocated by message creator statically.
///   Examples are custom messages.
///
////////////////////////////////////////////////////////////////////////////////////
void MessageCreator::CleanupMessageCreator()
{
    sCustomMessagesMutex.Enter();
    std::map<UShort, Message*>::iterator msg;
    for(msg = sCustomMessages.begin(); msg != sCustomMessages.end(); msg++)
    {
        if(msg->second)
        {
            delete msg->second;
        }
    }
    sCustomMessages.clear();
    sCustomMessagesMutex.Leave();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Based on the inform message type, this method will return the
///          message code of the query that generated it.
///
///   This method is useful for automatic lookup of the expected response
///   to a command or query message.  For example, if you send a query, you
///   can use this to lookup what the inform message resposne will be.  Or if
///   you send a command like CreateEventRequest, the return value will be
///   ConfirmEventRequest.
///
///   However, for messages like CreateEventRequest or UpdateEvent
///   which can have multiple return types, use the second parameter
///   (responses) and you will get all possible responses.  For the
///   CreateEventRequest example, responses would have Confirm Event Request
///   and Reject Event Request inside it.
///
///   \param mcode The message code to get the reponse code to.
///   \param responses Optional parameter.  If not NULL then all possible
///                    responses to the message will be put in the vector.
///
///   \return The message code of the response to the message code passed.  If
///           the message has no response, then 0 is returned.
///
////////////////////////////////////////////////////////////////////////////////////
UShort MessageCreator::GetResponseCodes(const UShort mcode, std::set<UShort>* responses)
{
    UShort rcode = 0;
    if( responses )
    {
        responses->clear();
    }

    rcode = GetQueryResponseType(mcode);

    if( rcode == 0 )
    {
        switch(mcode)
        {
            // Core Subgroup
            case JAUS_CREATE_SERVICE_CONNECTION:
                rcode = JAUS_CONFIRM_SERVICE_CONNECTION;
                break;

            case JAUS_REQUEST_COMPONENT_CONTROL:
                rcode = JAUS_CONFIRM_COMPONENT_CONTROL;
                if( responses )
                {
                    responses->insert(rcode);
                    responses->insert(JAUS_REJECT_COMPONENT_CONTROL);
                }
                break;

            // Event Setup and Control Subgroup
            case JAUS_CREATE_EVENT:
                rcode = JAUS_CONFIRM_EVENT_REQUEST;
                if( responses )
                {
                    responses->insert(rcode);
                    responses->insert(JAUS_REJECT_EVENT_REQUEST);
                }
                break;
            case JAUS_UPDATE_EVENT:
                rcode = JAUS_CONFIRM_EVENT_REQUEST;
                if( responses )
                {
                    responses->insert(rcode);
                    responses->insert(JAUS_REJECT_EVENT_REQUEST);
                }
                break;
            case JAUS_CANCEL_EVENT:
                rcode = JAUS_CONFIRM_EVENT_REQUEST;
                if( responses )
                {
                    responses->insert(rcode);
                    responses->insert(JAUS_REJECT_EVENT_REQUEST);
                }
                break;

            // Communications Subgroup

            // Platform Subgroup

            // Manipulator Subgroup

            // Environment Sensor Subgroup

            // World Model Subgroup
            case JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS:
                rcode = JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_OBJECTS_CREATION;
                break;

            // Dynamic Configuration Subgroup

            // Planning Subgroup

            // Experimental
            case JAUS_QUERY_SICK_LIDAR:
                rcode = JAUS_REPORT_SICK_LIDAR;
                break;

            case JAUS_QUERY_PIXEL_ON_SCREEN:
                rcode = JAUS_QUERY_PIXEL_ON_SCREEN;
                break;

            default:
                rcode = 0;
                break;
        }
    }

    // If a response code has been set, but not
    // put into the responses vector, put it in there automatically.
    if( rcode > 0 && responses && responses->size() == 0 )
    {
        responses->insert(rcode);
    }

    return rcode;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Based on the inform message type, this method will return the
///          message code of the query that generated it.
///
///   This method is useful for automatic lookup of a query message code that
///   generated an inform message from a component.  For example, if you
///   passed this method JAUS_REPORT_TIME, the query code that generated it is
///   QUERY_TIME.
///
///   For the inverse of this method see GetQueryResponseType method.
///
///   \param informCode The inform message to get the query code for.
///
///   \return Query code that generates the provided informCode, 0 on failure to
///           find a match.
///
////////////////////////////////////////////////////////////////////////////////////
UShort MessageCreator::GetInformQueryType(const UShort informCode)
{
    UShort code;

    switch(informCode)
    {
        // Core Subgroup.
        case JAUS_REPORT_COMPONENT_AUTHORITY:
            code = JAUS_QUERY_COMPONENT_AUTHORITY;
            break;
        case JAUS_REPORT_COMPONENT_STATUS:
            code = JAUS_QUERY_COMPONENT_STATUS;
            break;
        case JAUS_REPORT_TIME:
            code = JAUS_QUERY_TIME;
            break;
        case JAUS_REPORT_COMPONENT_CONTROL:
            code = JAUS_QUERY_COMPONENT_CONTROL;
            break;

        // Event Setup and Control
        case JAUS_REPORT_EVENTS:
            code = JAUS_QUERY_EVENTS;
            break;

        // Communications Subgroup
        case JAUS_REPORT_DATA_LINK_STATUS:
            code = JAUS_QUERY_DATA_LINK_STATUS;
            break;
        case JAUS_REPORT_SELECTED_DATA_LINK_STATUS:
            code = JAUS_QUERY_SELECTED_DATA_LINK_STATUS;
            break;
        case JAUS_REPORT_HEARTBEAT_PULSE:
            code = JAUS_QUERY_HEARTBEAT_PULSE;
            break;

        // Platform Subgroup
        case JAUS_REPORT_PLATFORM_SPECIFICATIONS:
            code = JAUS_QUERY_PLATFORM_SPECIFICATIONS;
            break;
        case JAUS_REPORT_PLATFORM_OPERATIONAL_DATA:
            code = JAUS_QUERY_PLATFORM_OPERATIONAL_DATA;
            break;
        case JAUS_REPORT_GLOBAL_POSE:
            code = JAUS_QUERY_GLOBAL_POSE;
            break;
        case JAUS_REPORT_LOCAL_POSE:
            code = JAUS_QUERY_LOCAL_POSE;
            break;
        case JAUS_REPORT_VELOCITY_STATE:
            code = JAUS_QUERY_VELOCITY_STATE;
            break;
        case JAUS_REPORT_WRENCH_EFFORT:
            code = JAUS_QUERY_WRENCH_EFFORT;
            break;
        case JAUS_REPORT_DISCRETE_DEVICES:
            code = JAUS_QUERY_DISCRETE_DEVICES;
            break;
        case JAUS_REPORT_GLOBAL_VECTOR:
            code = JAUS_QUERY_GLOBAL_VECTOR;
            break;
        case JAUS_REPORT_LOCAL_VECTOR:
            code = JAUS_QUERY_LOCAL_VECTOR;
            break;
        case JAUS_REPORT_TRAVEL_SPEED:
            code = JAUS_QUERY_TRAVEL_SPEED;
            break;
        case JAUS_REPORT_WAYPOINT_COUNT:
            code = JAUS_QUERY_WAYPOINT_COUNT;
            break;
        case JAUS_REPORT_GLOBAL_WAYPOINT:
            code = JAUS_QUERY_GLOBAL_WAYPOINT;
            break;
        case JAUS_REPORT_LOCAL_WAYPOINT:
            code = JAUS_QUERY_LOCAL_WAYPOINT;
            break;
        case JAUS_REPORT_PATH_SEGMENT_COUNT:
            code = JAUS_QUERY_PATH_SEGMENT_COUNT;
            break;
        case JAUS_REPORT_GLOBAL_PATH_SEGMENT:
            code = JAUS_QUERY_GLOBAL_PATH_SEGMENT;
            break;
        case JAUS_REPORT_LOCAL_PATH_SEGMENT:
            code = JAUS_QUERY_LOCAL_PATH_SEGMENT;
            break;

        // Manipulator Subgroup
        case JAUS_REPORT_MANIPULATOR_SPECIFICATIONS:
            code = JAUS_QUERY_MANIPULATOR_SPECIFICATIONS;
            break;
        case JAUS_REPORT_JOINT_EFFORTS:
            code = JAUS_QUERY_JOINT_EFFORTS;
            break;
        case JAUS_REPORT_JOINT_POSITIONS:
            code = JAUS_QUERY_JOINT_POSITIONS;
            break;
        case JAUS_REPORT_JOINT_VELOCITIES:
            code = JAUS_QUERY_JOINT_VELOCITIES;
            break;
        case JAUS_REPORT_TOOL_POINT:
            code = JAUS_QUERY_TOOL_POINT;
            break;
        case JAUS_REPORT_JOINT_FORCE_TORQUES:
            code = JAUS_QUERY_JOINT_FORCE_TORQUES;
            break;

        // Environment Sensor Subgroup
        case JAUS_REPORT_CAMERA_POSE:
            code = JAUS_QUERY_CAMERA_POSE;
            break;
        case JAUS_REPORT_CAMERA_COUNT:
            code = JAUS_QUERY_CAMERA_COUNT;
            break;
        case JAUS_REPORT_RELATIVE_OBJECT_POSITION:
            code = JAUS_QUERY_RELATIVE_OBJECT_POSITION;
            break;
        case JAUS_REPORT_SELECTED_CAMERA:
            code = JAUS_QUERY_SELECTED_CAMERA;
            break;
        case JAUS_REPORT_CAMERA_CAPABILITIES:
            code = JAUS_QUERY_CAMERA_CAPABILITIES;
            break;
        case JAUS_REPORT_CAMERA_FORMAT_OPTIONS:
            code = JAUS_QUERY_CAMERA_FORMAT_OPTIONS;
            break;
        case JAUS_REPORT_IMAGE:
            code = JAUS_QUERY_IMAGE;
            break;

        // World Model Subgroup
        case JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA:
            code = JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA;
            break;
        case JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_BOUNDS:
            code = JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_BOUNDS;
            break;
        case JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_OBJECTS:
            code = JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_OBJECTS;
            break;

        // Dynamic Configuration Subgroup
        case JAUS_REPORT_IDENTIFICATION:
            code = JAUS_QUERY_IDENTIFICATION;
            break;
        case JAUS_REPORT_CONFIGURATION:
            code = JAUS_QUERY_CONFIGURATION;
            break;
        case JAUS_REPORT_SUBSYSTEM_LIST:
            code = JAUS_QUERY_SUBSYSTEM_LIST;
            break;
        case JAUS_REPORT_SERVICES:
            code = JAUS_QUERY_SERVICES;
            break;

        // Payload Subgroup
        case JAUS_REPORT_PAYLOAD_INTERFACE_MESSAGE:
            code = JAUS_QUERY_PAYLOAD_INTERFACE_MESSAGE;
            break;
        case JAUS_REPORT_PAYLOAD_DATA_ELEMENT:
            code = JAUS_QUERY_PAYLOAD_DATA_ELEMENT;
            break;

        // Planning Subgroup
        case JAUS_REPORT_SPOOLING_PREFERENCE:
            code = JAUS_QUERY_SPOOLING_PREFERENCE;
            break;
        case JAUS_REPORT_MISSION_STATUS:
            code = JAUS_QUERY_MISSION_STATUS;
            break;

        // Experimental
        case JAUS_REPORT_SICK_LIDAR:
            code = JAUS_QUERY_SICK_LIDAR;
            break;
        case JAUS_REPORT_PIXEL_ON_SCREEN:
            code = JAUS_REPORT_PIXEL_ON_SCREEN;
            break;
        // Default Output
        default:
            code = 0;
            break;
    };

    return code;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Based on the query message type, this method will return the
///          message code of the inform message it will generate in response.
///
///   This method is useful for automatic lookup of a inform message code that
///   generated from a query by a component.  For example, if you
///   passed this method JAUS_QUERY_TIME, the inform code for the associated
///   response is JAUS_REPORT_TIME.
///
///   For the inverse of this method see GetInformQueryType method.
///
///   \param queryCode The query to look up the inform response code for.
///
///   \return Inform message code for the response to the query provided.  0 on
///           failure to find a match.
///
////////////////////////////////////////////////////////////////////////////////////
UShort MessageCreator::GetQueryResponseType(const UShort queryCode)
{
    UShort code;

    switch(queryCode)
    {
        // Core Subgroup.
        case JAUS_QUERY_COMPONENT_AUTHORITY:
            code = JAUS_REPORT_COMPONENT_AUTHORITY;
            break;
        case JAUS_QUERY_COMPONENT_STATUS:
            code = JAUS_REPORT_COMPONENT_STATUS;
            break;
        case JAUS_QUERY_TIME:
            code = JAUS_REPORT_TIME;
            break;
        case JAUS_QUERY_COMPONENT_CONTROL:
            code = JAUS_REPORT_COMPONENT_CONTROL;
            break;

        // Event Setup and Control
        case JAUS_QUERY_EVENTS:
            code = JAUS_REPORT_EVENTS;
            break;

        // Communications Subgroup
        case JAUS_QUERY_DATA_LINK_STATUS:
            code = JAUS_REPORT_DATA_LINK_STATUS;
            break;
        case JAUS_QUERY_SELECTED_DATA_LINK_STATUS:
            code = JAUS_REPORT_SELECTED_DATA_LINK_STATUS;
            break;
        case JAUS_QUERY_HEARTBEAT_PULSE:
            code = JAUS_REPORT_HEARTBEAT_PULSE;
            break;

        // Platform Subgroup
        case JAUS_QUERY_PLATFORM_SPECIFICATIONS:
            code = JAUS_REPORT_PLATFORM_SPECIFICATIONS;
            break;
        case JAUS_QUERY_PLATFORM_OPERATIONAL_DATA:
            code = JAUS_REPORT_PLATFORM_OPERATIONAL_DATA;
            break;
        case JAUS_QUERY_GLOBAL_POSE:
            code = JAUS_REPORT_GLOBAL_POSE;
            break;
        case JAUS_QUERY_LOCAL_POSE:
            code = JAUS_REPORT_LOCAL_POSE;
            break;
        case JAUS_QUERY_VELOCITY_STATE:
            code = JAUS_REPORT_VELOCITY_STATE;
            break;
        case JAUS_QUERY_WRENCH_EFFORT:
            code = JAUS_REPORT_WRENCH_EFFORT;
            break;
        case JAUS_QUERY_DISCRETE_DEVICES:
            code = JAUS_REPORT_DISCRETE_DEVICES;
            break;
        case JAUS_QUERY_GLOBAL_VECTOR:
            code = JAUS_REPORT_GLOBAL_VECTOR;
            break;
        case JAUS_QUERY_LOCAL_VECTOR:
            code = JAUS_REPORT_LOCAL_VECTOR;
            break;
        case JAUS_QUERY_TRAVEL_SPEED:
            code = JAUS_REPORT_TRAVEL_SPEED;
            break;
        case JAUS_QUERY_WAYPOINT_COUNT:
            code = JAUS_REPORT_WAYPOINT_COUNT;
            break;
        case JAUS_QUERY_GLOBAL_WAYPOINT:
            code = JAUS_REPORT_GLOBAL_WAYPOINT;
            break;
        case JAUS_QUERY_LOCAL_WAYPOINT:
            code = JAUS_REPORT_LOCAL_WAYPOINT;
            break;
        case JAUS_QUERY_PATH_SEGMENT_COUNT:
            code = JAUS_REPORT_PATH_SEGMENT_COUNT;
            break;
        case JAUS_QUERY_GLOBAL_PATH_SEGMENT:
            code = JAUS_REPORT_GLOBAL_PATH_SEGMENT;
            break;
        case JAUS_QUERY_LOCAL_PATH_SEGMENT:
            code = JAUS_REPORT_LOCAL_PATH_SEGMENT;
            break;

        // Manipulator Subgroup
        case JAUS_QUERY_MANIPULATOR_SPECIFICATIONS:
            code = JAUS_REPORT_MANIPULATOR_SPECIFICATIONS;
            break;
        case JAUS_QUERY_JOINT_EFFORTS:
            code = JAUS_REPORT_JOINT_EFFORTS;
            break;
        case JAUS_QUERY_JOINT_POSITIONS:
            code = JAUS_REPORT_JOINT_POSITIONS;
            break;
        case JAUS_QUERY_JOINT_VELOCITIES:
            code = JAUS_REPORT_JOINT_VELOCITIES;
            break;
        case JAUS_QUERY_TOOL_POINT:
            code = JAUS_REPORT_TOOL_POINT;
            break;
        case JAUS_QUERY_JOINT_FORCE_TORQUES:
            code = JAUS_REPORT_JOINT_FORCE_TORQUES;
            break;

        // Environment Sensor Subgroup
        case JAUS_QUERY_CAMERA_POSE:
            code = JAUS_REPORT_CAMERA_POSE;
            break;
        case JAUS_QUERY_CAMERA_COUNT:
            code = JAUS_REPORT_CAMERA_COUNT;
            break;
        case JAUS_QUERY_RELATIVE_OBJECT_POSITION:
            code = JAUS_REPORT_RELATIVE_OBJECT_POSITION;
            break;
        case JAUS_QUERY_SELECTED_CAMERA:
            code = JAUS_REPORT_SELECTED_CAMERA;
            break;
        case JAUS_QUERY_CAMERA_CAPABILITIES:
            code = JAUS_REPORT_CAMERA_CAPABILITIES;
            break;
        case JAUS_QUERY_CAMERA_FORMAT_OPTIONS:
            code = JAUS_REPORT_CAMERA_FORMAT_OPTIONS;
            break;
        case JAUS_QUERY_IMAGE:
            code = JAUS_REPORT_IMAGE;
            break;

        // World Model Subgroup
        case JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA:
            code = JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_FEATURE_CLASS_METADATA;
            break;
        case JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_BOUNDS:
            code = JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_BOUNDS;
            break;
        case JAUS_QUERY_VECTOR_KNOWLEDGE_STORE_OBJECTS:
            code = JAUS_REPORT_VECTOR_KNOWLEDGE_STORE_OBJECTS;
            break;

        // Dynamic Configuration Subgroup
        case JAUS_QUERY_IDENTIFICATION:
            code = JAUS_REPORT_IDENTIFICATION;
            break;
        case JAUS_QUERY_CONFIGURATION:
            code = JAUS_REPORT_CONFIGURATION;
            break;
        case JAUS_QUERY_SUBSYSTEM_LIST:
            code = JAUS_REPORT_SUBSYSTEM_LIST;
            break;
        case JAUS_QUERY_SERVICES:
            code = JAUS_REPORT_SERVICES;
            break;

        // Payload Subgroup
        case JAUS_QUERY_PAYLOAD_INTERFACE_MESSAGE:
            code = JAUS_REPORT_PAYLOAD_INTERFACE_MESSAGE;
            break;
        case JAUS_QUERY_PAYLOAD_DATA_ELEMENT:
            code = JAUS_REPORT_PAYLOAD_DATA_ELEMENT;
            break;

        // Planning Subgroup
        case JAUS_QUERY_SPOOLING_PREFERENCE:
            code = JAUS_REPORT_SPOOLING_PREFERENCE;
            break;
        case JAUS_QUERY_MISSION_STATUS:
            code = JAUS_REPORT_MISSION_STATUS;
            break;

        // Default Output
        default:
            code = 0;
            break;
    };

    return code;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the size of a presence vector based on the type of
///   message.
///
///   \param code Message type number.
///   \param version What version of JAUS to get the size for.
///   \param successFlag Pointer to boolean which is set if size retrieved.  
///                      If no size received and successFlag is != NULL then
///                      the value is set to false.
///
///   \return 0 if no presence vector associated with message, otherwise the
///   size in bytes (JAUS_BYTE_SIZE, JAUS_USHORT_SIZE, JAUS_UINT_SIZE).
///
////////////////////////////////////////////////////////////////////////////////////
UShort MessageCreator::GetPresenceVectorSize(const UShort code,
                                             const UShort version,
                                             bool* successFlag)
{
    UShort size = 0;
    if(successFlag)
    {
        *successFlag = false;
    }

    Message* tempMessage = NULL;

    tempMessage = MessageCreator::CreateMessage( code );
    if( tempMessage )
    {
        size = tempMessage->GetPresenceVectorSize(version);
        delete tempMessage;
        tempMessage = NULL;
        if(successFlag)
        {
            *successFlag = true;
        }
    }

    return size;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets a bit mask which can be used to determine how many bits of
///          a presence vector are used for any type of message.
///
///   For example, if you request the bit mask for Report Global Pose, you'll
///   get 0x1FF.
///
///   \param code Message type (command code).
///   \param version What version of JAUS to get the mask for.
///   \param successFlag Pointer to boolean which is set if mask retrieved.  
///                      If not mask received and successFlag is != NULL then
///                      the value is set to false.
///
///   \return A bit mask indicating what bits of a presence vector are used by 
///           a given message type.  If no presence vector available, 0 is
///           returned.
///
////////////////////////////////////////////////////////////////////////////////////
UInt MessageCreator::GetPresenceVectorMask(const UShort code, const UShort version, bool* successFlag)
{
    UInt mask = 0;

    if(successFlag)
    {
        *successFlag = false;
    }

    Message* tempMessage = CreateMessage(code);
    if( tempMessage )
    {
        mask = tempMessage->GetPresenceVectorMask(version);
        delete tempMessage;
        tempMessage = NULL;
        if(successFlag)
        {
            *successFlag = true;
        }
    }
    else if(successFlag)
    {
        *successFlag = false;
    }

    return mask;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the message contains a presence vector, this function will
///   read it.
///
///   \param msg The written JAUS message to get the presence vector for.
///              The msg parameter must contain a single JAUS message.
///   \param v The presence vector extracted from written message.
///
///   \return JAUS_OK if presence vector extracted, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageCreator::GetPresenceVector(const Stream &msg, UInt& v)
{
    UInt size = 0;
    Header header;

    v = 0;
    msg.SetReadPos(0);

    if( msg.Read(header, 0) &&
        (size = GetPresenceVectorSize(header.mCommandCode)) > 0)
    {
        if(size == JAUS_BYTE_SIZE)
        {
            Byte pv;
            if(msg.Read(pv, JAUS_HEADER_SIZE))
            {
                v = pv;
                return JAUS_OK;
            }
        }
        else if(size == JAUS_USHORT_SIZE)
        {
            UShort pv;
            if(msg.Read(pv, JAUS_HEADER_SIZE))
            {
                v = pv;
                return JAUS_OK;
            }
        }
        else
        {
            if(msg.Read(v, JAUS_HEADER_SIZE))
            {
                return JAUS_OK;
            }
        }
    }

    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the message contains a presence vector, this function will
///   read it.
///
///   \param buff Buffer containing written JAUS message.
///   \param len Length of buffer.
///   \param v The presence vector extracted from written message.
///
///   \return JAUS_OK if presence vector extracted, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageCreator::GetPresenceVector(const Byte *buff, const unsigned int len, UInt &v)
{
    int result = JAUS_FAILURE;
    UShort size = 0;
    Header header;

    if(len < JAUS_HEADER_SIZE)
        return 0;

    v = 0;

    if( Stream::ReadHeader(buff, len, header) )
    {
        size = (UShort)GetPresenceVectorSize(header.mCommandCode);

        if( size > 0 && len >= (unsigned int)(JAUS_HEADER_SIZE + size))
        {
            if(size == JAUS_BYTE_SIZE) {
                Byte val;
                memcpy(&val, buff + JAUS_HEADER_SIZE, JAUS_BYTE_SIZE);
                v = val;
                result = JAUS_OK;
            }
            else if(size == JAUS_USHORT_SIZE) {
                UShort val;
                memcpy(&val, buff + JAUS_HEADER_SIZE, JAUS_USHORT_SIZE);
                v = val;
                result = JAUS_OK;
            }
            else if(size == JAUS_UINT_SIZE) {
                UInt val;
                memcpy(&val, buff + JAUS_HEADER_SIZE, JAUS_UINT_SIZE);
                v = val;
                result = JAUS_OK;
            }
        }
    }
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Runs any test case for the message type desired.
///
///   This method validates that a message can read/write header data
///   correctly to a packet, and runs the messages Test Case (RunTestCase()).
///   If all succeed, then this Test Case is a success.
///
///   \param code Message type number.
///
///   \return JAUS_OK on successfull test, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int MessageCreator::RunTestCase(const UShort code)
{
    int result = JAUS_FAILURE;
    Message* msg;
    msg = CreateMessage(code);
    if( msg )
    {
        if( msg->RunTestCase() )
        {
            result = JAUS_OK;
        }
    }
    if( msg )
        delete msg;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the string name of the message.
///
///   \param code The class code of the message to get.
///
///   \return String representing the message name, empty string if unknown.
///
////////////////////////////////////////////////////////////////////////////////////
std::string MessageCreator::GetMessageString(const UShort code)
{
    if(code <= 0x1FFF)
    {
        return GetCommandMessageString(code);
    }
    else if(code >= 0x2000 && code <= 0x3FFF)
    {
        return GetQueryMessageString(code);
    }
    else if(code >= 0x4000 && code <= 0x5FFF)
    {
        return GetInformMessageString(code);
    }
    else
    {
        return GetExperimentalMessageString(code);
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the string name of the message.
///
///   \param code The class code of the message to get.
///               (see messages/command/commandcodes.h).
///
///   \return String representing the message name, empty string if unknown.
///
////////////////////////////////////////////////////////////////////////////////////
std::string MessageCreator::GetCommandMessageString(const UShort code)
{
    std::string str;
    switch(code)
    {
    //  Core Subgroup
    case JAUS_SET_COMPONENT_AUTHORITY:
        str = "Set Component Authority";
        break;
    case JAUS_SHUTDOWN:
        str = "Shutdown";
        break;
    case JAUS_STANDBY:
        str = "Standby";
        break;
    case JAUS_RESUME:
        str = "Resume";
        break;
    case JAUS_RESET:
        str = "Reset";
        break;
    case JAUS_SET_EMERGENCY:
        str = "Set Emergency";
        break;
    case JAUS_CLEAR_EMERGENCY:
        str = "Clear Emergency";
        break;
    case JAUS_CREATE_SERVICE_CONNECTION:
        str = "Create Service Connection";
        break;
    case JAUS_CONFIRM_SERVICE_CONNECTION:
        str = "Confirm Service Connection";
        break;
    case JAUS_ACTIVATE_SERVICE_CONNECTION:
        str = "Activate Service Connection";
        break;
    case JAUS_SUSPEND_SERVICE_CONNECTION:
        str = "Suspend Service Connection";
        break;
    case JAUS_TERMINATE_SERVICE_CONNECTION:
        str = "Terminate Service Connection";
        break;
    case JAUS_REQUEST_COMPONENT_CONTROL:
        str = "Request Component Control";
        break;
    case JAUS_RELEASE_COMPONENT_CONTROL:
        str = "Release Component Control";
        break;
    case JAUS_CONFIRM_COMPONENT_CONTROL:
        str = "Confirm Component Control";
        break;
    case JAUS_REJECT_COMPONENT_CONTROL:
        str = "Reject Component Control";
        break;
    case JAUS_SET_TIME:
        str = "Set Time";
        break;
    //  Event Setup and Control
    case JAUS_CREATE_EVENT:
        str = "Create Event Request";
        break;
    case JAUS_UPDATE_EVENT:
        str = "Update Event";
        break;
    case JAUS_CANCEL_EVENT:
        str = "Cancel Event";
        break;
    case JAUS_CONFIRM_EVENT_REQUEST:
        str = "Confirm Event Request";
        break;
    case JAUS_REJECT_EVENT_REQUEST:
        str = "Reject Event Request";
        break;
    //  Communications Subgroup
    case JAUS_SET_DATA_LINK_STATE:
        str = "Set Data Link State";
        break;
    case JAUS_SET_SELECTED_DATA_LINK_STATE:
        str = "Set Selected Data Link State";
        break;
    case JAUS_SET_DATA_LINK_SELECT:
        str = "Set Data Link Select";
        break;
    //  Platform Subgroup
    case JAUS_SET_WRENCH_EFFORT:
        str = "Set Wrench Effort";
        break;
    case JAUS_SET_DISCRETE_DEVICES:
        str = "Set Discrete Devices";
        break;
    case JAUS_SET_GLOBAL_VECTOR:
        str = "Set Global Vector";
        break;
    case JAUS_SET_LOCAL_VECTOR:
        str = "Set Local Vector";
        break;
    case JAUS_SET_TRAVEL_SPEED:
        str = "Set Travel Speed";
        break;
    case JAUS_SET_GLOBAL_WAYPOINT:
        str = "Set Global Waypoint";
        break;
    case JAUS_SET_LOCAL_WAYPOINT:
        str = "Set Local Waypoint";
        break;
    case JAUS_SET_GLOBAL_PATH_SEGMENT:
        str = "Set Global Path Segment";
        break;
    case JAUS_SET_LOCAL_PATH_SEGMENT:
        str = "Set Local Path Segment";
        break;
    //  Manipulator Subgroup
    case JAUS_SET_JOINT_EFFORTS:
        str = "Set Joint Efforts";
        break;
    case JAUS_SET_JOINT_POSITIONS:
        str = "Set Joint Positions";
        break;
    case JAUS_SET_JOINT_VELOCITIES:
        str = "Set Joint Velocities";
        break;
    case JAUS_SET_TOOL_POINT:
        str = "Set Tool Point";
        break;
    case JAUS_SET_END_EFFECTOR_POSE:
        str = "Set End Effector Pose";
        break;
    case JAUS_SET_END_EFFECTOR_VELOCITY_STATE:
        str = "Set End Effector Velocity State";
        break;
    case JAUS_SET_JOINT_MOTION:
        str = "Set Joint Motion";
        break;
    case JAUS_SET_END_EFFECTOR_PATH_MOTION:
        str = "Set End Effector Path Motion";
        break;
    //  Environment Sensor Subgroup
    case JAUS_SET_CAMERA_POSE:
        str = "Set Camera Pose";
        break;
    case JAUS_SELECT_CAMERA:
        str = "Select Camera";
        break;
    case JAUS_SET_CAMERA_CAPABILITIES:
        str = "Set Camera Capabilities";
        break;
    case JAUS_SET_CAMERA_FORMAT_OPTIONS:
        str = "Set Camera Format Options";
        break;
    //  World Model Subgroup
    case JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS:
        str = "Create Vector Knowledge Store Objects";
        break;
    //  Dynamic Configuration Subgroup
    //  Payload Subgroup
    //  Planning Subgroup
    case JAUS_SPOOL_MISSION:
        str = "Spool Mission";
        break;
    case JAUS_RUN_MISSION:
        str = "Run Mission";
        break;
    case JAUS_ABORT_MISSION:
        str = "Abort Mission";
        break;
    case JAUS_PAUSE_MISSION:
        str = "Pause Mission";
        break;
    case JAUS_RESUME_MISSION:
        str = "Resume Mission";
        break;
    case JAUS_REMOVE_MESSAGES:
        str = "Remove Messages";
        break;
    case JAUS_REPLACE_MESSAGES:
        str = "Replace Messages";
        break;
    //  COULD NOT FIND MESSAGE!
    default:
        {
            char buff[10];
            sprintf(buff, "0x%X", code);
            str = buff;
        }
        break;
    };
    return str;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the string name of the message.
///
///   \param code The class code of the message to get.
///               (see messages/command/querycodes.h).
///
///   \return String representing the message name, empty string if unknown.
///
////////////////////////////////////////////////////////////////////////////////////
std::string MessageCreator::GetQueryMessageString(const UShort code)
{
    std::string str;
    switch(code)
    {
    // Core Messages:
    case JAUS_QUERY_COMPONENT_AUTHORITY:
        str = "Query Component Authority";
        break;
    case JAUS_QUERY_COMPONENT_STATUS:
        str = "Query Component Status";
        break;
    case JAUS_QUERY_TIME:
        str = "Query Time";
        break;
    case JAUS_QUERY_COMPONENT_CONTROL:
        str = "Query Component Control";
        break;
    // Event Setup and Control Messages:
    case JAUS_QUERY_EVENTS:
        str = "Query Events";
        break;
    // Communications Messages:
    case JAUS_QUERY_DATA_LINK_STATUS:
        str = "Query Data Link Status";
        break;
    case JAUS_QUERY_SELECTED_DATA_LINK_STATUS:
        str = "Query Selected Data Link Status";
        break;
    case JAUS_QUERY_HEARTBEAT_PULSE:
        str = "Query Heartbeat Pulse";
        break;
    // Platform Messages:
    case JAUS_QUERY_PLATFORM_SPECIFICATIONS:
        str = "Query  Platform Specifications";
        break;
    case JAUS_QUERY_PLATFORM_OPERATIONAL_DATA:
        str = "Query PlatformOperationalData";
        break;
    case JAUS_QUERY_GLOBAL_POSE:
        str = "Query GlobalPose";
        break;
    case JAUS_QUERY_LOCAL_POSE:
         str = "Query LocalPose";
         break;
    case JAUS_QUERY_VELOCITY_STATE:
         str = "Query VelocityState";
         break;
    case JAUS_QUERY_WRENCH_EFFORT:
         str = "Query WrenchEffort";
         break;
    case JAUS_QUERY_DISCRETE_DEVICES:
         str = "Query Discrete Devices";
         break;
    case JAUS_QUERY_GLOBAL_VECTOR:
         str = "Query Global Vector";
         break;
    case JAUS_QUERY_LOCAL_VECTOR:
         str = "Query Local Vector";
         break;
    case JAUS_QUERY_TRAVEL_SPEED:
         str = "Query Travel Speed";
         break;
    case JAUS_QUERY_WAYPOINT_COUNT:
         str = "Query Waypoint Count";
         break;
    case JAUS_QUERY_GLOBAL_WAYPOINT:
         str = "Query Global Waypoint";
         break;
    case JAUS_QUERY_LOCAL_WAYPOINT:
         str = "Query Local Waypoint";
         break;
    case JAUS_QUERY_PATH_SEGMENT_COUNT:
         str = "Query Path Segment Count";
         break;
    case JAUS_QUERY_GLOBAL_PATH_SEGMENT:
         str = "Query Global Path Segment";
         break;
    case JAUS_QUERY_LOCAL_PATH_SEGMENT:
         str = "Query Local Path Segment";
         break;
    // Manipulator Messages:
    case JAUS_QUERY_MANIPULATOR_SPECIFICATIONS:
         str = "Query Manipulator Specifications";
         break;
    case JAUS_QUERY_JOINT_EFFORTS:
         str = "Query Joint Efforts";
         break;
    case JAUS_QUERY_JOINT_POSITIONS:
         str = "Query Joint Positions";
         break;
    case JAUS_QUERY_JOINT_VELOCITIES:
         str = "Query Joint Velocities";
         break;
    case JAUS_QUERY_TOOL_POINT:
         str = "Query Tool Point";
         break;
    case JAUS_QUERY_JOINT_FORCE_TORQUES:
         str = "Query Joint Force Torques";
         break;
    // Environment Messages:
    case JAUS_QUERY_CAMERA_POSE:
         str = "Query Camera Pose";
         break;
    case JAUS_QUERY_CAMERA_COUNT:
         str = "Query Camera Count";
         break;
    case JAUS_QUERY_RELATIVE_OBJECT_POSITION:
         str = "Query Relative Object Position";
         break;
    case JAUS_QUERY_SELECTED_CAMERA:
         str = "Query Selected Camera";
         break;
    case JAUS_QUERY_CAMERA_CAPABILITIES:
         str = "Query Camera Capabilities";
         break;
    case JAUS_QUERY_CAMERA_FORMAT_OPTIONS:
         str = "Query Camera Format Options";
         break;
    case JAUS_QUERY_IMAGE:
         str = "Query Image";
         break;
    //  World Model Subgroup
    //  Dynamic Configuration (Discovery) Subgroup
    case JAUS_QUERY_IDENTIFICATION:
         str = "Query Identification";
         break;
    case JAUS_QUERY_CONFIGURATION:
         str = "Query Configuration";
         break;
    case JAUS_QUERY_SUBSYSTEM_LIST:
         str = "Query Subsystem List";
         break;
    case JAUS_QUERY_SERVICES:
         str = "Query Services";
         break;
    //// Planning Subgroup
    case JAUS_QUERY_SPOOLING_PREFERENCE:
        str = "Query Spooling Preference";
        break;
    case JAUS_QUERY_MISSION_STATUS:
        str = "Query Mission Status";
        break;
    default:
        {
            char buff[10];
            sprintf(buff, "0x%X", code);
            str = buff;
        }
        break;
    };
    return str;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the string name of the message.
///
///   \param code The class code of the message to get.
///               (see messages/command/informodes.h).
///
///   \return String representing the message name, empty string if unknown.
///
////////////////////////////////////////////////////////////////////////////////////
std::string MessageCreator::GetInformMessageString(const UShort code)
{
    std::string str;
    switch(code)
    {
    // Core Subgroup
    case JAUS_REPORT_COMPONENT_AUTHORITY:
        str = "Report Component Authority";
        break;
    case JAUS_REPORT_COMPONENT_STATUS:
        str = "Report Component Status";
        break;
    case JAUS_REPORT_TIME:
        str = "Report Time";
        break;
    case JAUS_REPORT_COMPONENT_CONTROL:
        str = "Report Component Control";
        break;
    // Event Setup and Control Subgroup
    case JAUS_REPORT_EVENTS:
        str = "Report Events";
        break;
    case JAUS_EVENT:
        str = "Event";
        break;
    // Communication Subgroup
    case JAUS_REPORT_DATA_LINK_STATUS:
        str = "Report Data Link Status";
        break;
    case JAUS_REPORT_SELECTED_DATA_LINK_STATUS:
        str = "Report Selected Data Link Status";
        break;
    case JAUS_REPORT_HEARTBEAT_PULSE:
        str = "Report Heartbeat Pulse";
        break;
    // Platform Subgroup
    case JAUS_REPORT_PLATFORM_SPECIFICATIONS:
        str = "Report Platform Specifications";
        break;
    case JAUS_REPORT_PLATFORM_OPERATIONAL_DATA:
        str = "Report Platform Operational Data";
        break;
    case JAUS_REPORT_GLOBAL_POSE:
        str = "Report Global Pose";
        break;
    case JAUS_REPORT_LOCAL_POSE:
        str = "Report Local Pose";
        break;
    case JAUS_REPORT_VELOCITY_STATE:
        str = "Report Velocity State";
        break;
    case JAUS_REPORT_WRENCH_EFFORT:
        str = "Report Wrench Effort";
        break;
    case JAUS_REPORT_DISCRETE_DEVICES:
        str = "Report Discrete Devices";
        break;
    case JAUS_REPORT_GLOBAL_VECTOR:
        str = "Report Global Vector";
        break;
    case JAUS_REPORT_LOCAL_VECTOR:
        str = "Report Local Vector";
        break;
    case JAUS_REPORT_TRAVEL_SPEED:
        str = "Report Travel Speed";
        break;
    case JAUS_REPORT_WAYPOINT_COUNT:
        str = "Report Waypoint Count";
        break;
    case JAUS_REPORT_GLOBAL_WAYPOINT:
        str = "Report Global Waypoint";
        break;
    case JAUS_REPORT_LOCAL_WAYPOINT:
        str = "Report Local Waypoint";
        break;
    case JAUS_REPORT_PATH_SEGMENT_COUNT:
        str = "Report Path Segment Count";
        break;
    case JAUS_REPORT_GLOBAL_PATH_SEGMENT:
        str = "Report Global Path Segment";
        break;
    case JAUS_REPORT_LOCAL_PATH_SEGMENT:
        str = "Report Local Path Segment";
        break;
    // Manipulator Subgroup
    case JAUS_REPORT_JOINT_EFFORTS:
        str = "Report Joint Efforts";
        break;
    case JAUS_REPORT_JOINT_POSITIONS:
        str = "Report Joint Positions";
        break;
    case JAUS_REPORT_JOINT_VELOCITIES:
        str = "Report Joint Velocities";
        break;
    case JAUS_REPORT_TOOL_POINT:
        str = "Report Tool Point";
        break;
    case JAUS_REPORT_MANIPULATOR_SPECIFICATIONS:
        str = "Report Manipulator Specifications";
        break;
    case JAUS_REPORT_JOINT_FORCE_TORQUES:
        str = "Report Joint Force Torques";
        break;
    // Environmental Sensor Subgroup
    case JAUS_REPORT_CAMERA_POSE:
        str = "Report Camera Pose";
        break;
    case JAUS_REPORT_CAMERA_COUNT:
        str = "Report Camera Count";
        break;
    case JAUS_REPORT_RELATIVE_OBJECT_POSITION:
        str = "Report Relative Object Position";
        break;
    case JAUS_REPORT_SELECTED_CAMERA:
        str = "Report Selected Camera";
        break;
    case JAUS_REPORT_CAMERA_CAPABILITIES:
        str = "Report Camera Capabilities";
        break;
    case JAUS_REPORT_CAMERA_FORMAT_OPTIONS:
        str = "Report Camera Format Options";
        break;
    case JAUS_REPORT_IMAGE:
        str = "Report Image";
        break;
    //  Dynamic Configuration Subgroup
    case JAUS_REPORT_IDENTIFICATION:
        str = "Report Identification";
        break;
    case JAUS_REPORT_SUBSYSTEM_LIST:
        str = "Report SubsystemList";
        break;
    case JAUS_REPORT_SERVICES:
        str = "Report Services";
        break;
    case JAUS_REPORT_CONFIGURATION:
        str = "Report Configuration";
        break;
    // Planning Subgroup
    case JAUS_REPORT_SPOOLING_PREFERENCE:
        str = "Report Spooling Preference";
        break;
    case JAUS_REPORT_MISSION_STATUS:
        str = "Report Mission Status";
        break;
    default:
        {
            char buff[10];
            sprintf(buff, "0x%X", code);
            str = buff;
        }
        break;
    };
    return str;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the string name of the message.
///
///   \param code The class code of the message to get.
///               (see messages/command/experimentalcodes.h).
///
///   \return String representing the message name, empty string if unknown.
///
////////////////////////////////////////////////////////////////////////////////////
std::string MessageCreator::GetExperimentalMessageString(const UShort code)
{
    std::string str;
    switch(code)
    {
    //  User defined messages
    case JAUS_QUERY_SICK_LIDAR:
         str = "Query SICK LIDAR";
         break;
    case JAUS_REPORT_SICK_LIDAR:
        str = "Report SICK_LIDAR";
        break;
    case JAUS_QUERY_PIXEL_ON_SCREEN:
        str = "Query Pixl On Screen";
        break;
    case JAUS_REPORT_PIXEL_ON_SCREEN:
        str = "Report Pixel On Screen";
        break;
    //  COULD NOT FIND MESSAGE!
    default:
        {
            char buff[10];
            sprintf(buff, "0x%X", code);
            str = buff;
        }
        break;
    };
    return str;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Function for generating a JAUS Message structure
///   (Message) based on the class code of the message.
///
///   \param code The class code of the message to.
///
///   \return Pointer to newly allocated message structure.  If
///   the class code is not identified, NULL is returned. You must delete
///   this message!
///
////////////////////////////////////////////////////////////////////////////////////
Message *MessageCreator::CreateMessage(const UShort code)
{
    Message *msg = NULL;

    if(code <= 0x1FFF)
    {
        msg = CreateCommandMessage(code);
    }
    else if(code >= 0x2000 && code <= 0x3FFF)
    {
        msg = CreateQueryMessage(code);
    }
    else if(code >= 0x4000 && code <= 0x5FFF)
    {
        msg = CreateInformMessage(code);
    }
    else
    {
        msg = CreateExperimentalMessage(code);
    }
    
    if(msg == NULL)
    {
        // See if it is a custom message added to Creator.
        std::map<UShort, Message*>::iterator mitr;
        sCustomMessagesMutex.Enter();
        mitr = sCustomMessages.find(code);
        if(mitr != sCustomMessages.end())
        {
            msg = mitr->second->Clone();
        }
        sCustomMessagesMutex.Leave();
    }

    return msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Reads an written message in packet, and gets a pointer to
///   the newly allocated read message.
///
///   This function will only read the first message within the packet.  If
///   more than one is written within, keep calling until NULL returned.
///
///   \param msg The message to read into a Message structure.
///   \param info Optional header information that may already be read.
///
///   \return Pointer to newly allocted Message structure if read was a
///   success, otherwise NULL. (Remember to delete Message when done!).
///
////////////////////////////////////////////////////////////////////////////////////
Message *MessageCreator::CreateMessage(const Stream &msg, const Header *info)
{
    Message *newMsg = NULL;
    Header h;
    unsigned int dPos = msg.GetReadPos();

    if(info)
    {
        h = *info;
    }
    else
    {
        if(msg.Read(h) == 0)
        {
            msg.SetReadPos(dPos);
            return NULL;
        }
    }

    newMsg = MessageCreator::CreateMessage(h.mCommandCode);
    if(newMsg == NULL)
    {
        return NULL;
    }

    if(newMsg->Read(msg))
    {
        return newMsg;
    }

    if(newMsg)
    {
        delete newMsg;
    }

    return NULL;

}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for creating command type messages.
///
///   \param code The class code of the message to get.
///               (see messages/command/commandcodes.h).
///
///   \return Pointer to newly allocated message structure.  If
///   the class code is not identified, NULL is returned.  You must delete
///   this message!
///
////////////////////////////////////////////////////////////////////////////////////
Message* MessageCreator::CreateCommandMessage(const UShort code)
{
    Message* msg = NULL;
    switch(code)
    {
    //  Core Subgroup
    case JAUS_SET_COMPONENT_AUTHORITY:
        msg = new SetComponentAuthority();
        break;
    case JAUS_SHUTDOWN:
        msg = new Shutdown();
        break;
    case JAUS_STANDBY:
        msg = new Standby();
        break;
    case JAUS_RESUME:
        msg = new Resume();
        break;
    case JAUS_RESET:
        msg = new Reset();
        break;
    case JAUS_SET_EMERGENCY:
        msg = new SetEmergency();
        break;
    case JAUS_CLEAR_EMERGENCY:
        msg = new ClearEmergency();
        break;
    case JAUS_CREATE_SERVICE_CONNECTION:
        msg = new CreateServiceConnection();
        break;
    case JAUS_CONFIRM_SERVICE_CONNECTION:
        msg = new ConfirmServiceConnection();
        break;
    case JAUS_ACTIVATE_SERVICE_CONNECTION:
        msg = new ActivateServiceConnection();
        break;
    case JAUS_SUSPEND_SERVICE_CONNECTION:
        msg = new SuspendServiceConnection();
        break;
    case JAUS_TERMINATE_SERVICE_CONNECTION:
        msg = new TerminateServiceConnection();
        break;
    case JAUS_REQUEST_COMPONENT_CONTROL:
        msg = new RequestComponentControl();
        break;
    case JAUS_RELEASE_COMPONENT_CONTROL:
        msg = new ReleaseComponentControl();
        break;
    case JAUS_CONFIRM_COMPONENT_CONTROL:
        msg = new ConfirmComponentControl();
        break;
    case JAUS_REJECT_COMPONENT_CONTROL:
        msg = new RejectComponentControl();
        break;
    case JAUS_SET_TIME:
        msg = new SetTime();
        break;
    //  Event Setup and Control
    case JAUS_CREATE_EVENT:
        msg = new CreateEventRequest();
        break;
    case JAUS_UPDATE_EVENT:
        msg = new UpdateEvent();
        break;
    case JAUS_CANCEL_EVENT:
        msg = new CancelEvent();
        break;
    case JAUS_CONFIRM_EVENT_REQUEST:
        msg = new ConfirmEventRequest();
        break;
    case JAUS_REJECT_EVENT_REQUEST:
        msg = new RejectEventRequest();
        break;
    //  Communications Subgroup
    case JAUS_SET_DATA_LINK_STATE:
        msg = new SetDataLinkState();
        break;
    case JAUS_SET_SELECTED_DATA_LINK_STATE:
        msg = new SetSelectedDataLinkState();
        break;
    case JAUS_SET_DATA_LINK_SELECT:
        msg = new SetDataLinkSelect();
        break;
    //  Platform Subgroup
    case JAUS_SET_WRENCH_EFFORT:
        msg = new SetWrenchEffort();
        break;
    case JAUS_SET_DISCRETE_DEVICES:
        msg = new SetDiscreteDevices();
        break;
    case JAUS_SET_GLOBAL_VECTOR:
        msg = new SetGlobalVector();
        break;
    case JAUS_SET_LOCAL_VECTOR:
        msg = new SetLocalVector();
        break;
    case JAUS_SET_TRAVEL_SPEED:
        msg = new SetTravelSpeed();
        break;
    case JAUS_SET_GLOBAL_WAYPOINT:
        msg = new SetGlobalWaypoint();
        break;
    case JAUS_SET_LOCAL_WAYPOINT:
        msg = new SetLocalWaypoint();
        break;
    case JAUS_SET_GLOBAL_PATH_SEGMENT:
        msg = new SetGlobalPathSegment();
        break;
    case JAUS_SET_LOCAL_PATH_SEGMENT:
        msg = new SetLocalPathSegment();
        break;
    //  Manipulator Subgroup
    //case JAUS_SET_JOINT_EFFORTS:
    //    msg = new SetJointEfforts();
    //    break;
    //case JAUS_SET_JOINT_POSITIONS:
    //    msg = new SetJointPositions();
    //    break;
    //case JAUS_SET_JOINT_VELOCITIES:
    //    msg = new SetJointVelocities();
    //    break;
    //case JAUS_SET_TOOL_POINT:
    //    msg = new SetToolPoint();
    //    break;
    //case JAUS_SET_END_EFFECTOR_POSE:
    //    msg = new SetEndEffectorPose();
    //    break;
    //case JAUS_SET_END_EFFECTOR_VELOCITY_STATE:
    //    msg = new SetEndEffectorVelocityState();
    //    break;
    //case JAUS_SET_JOINT_MOTION:
    //    msg = new SetJointMotion();
    //    break;
    //case JAUS_SET_END_EFFECTOR_PATH_MOTION:
    //    msg = new SetEndEffectorPathMotion();
    //    break;
    ////  Environment Sensor Subgroup
    case JAUS_SET_CAMERA_POSE:
        msg = new SetCameraPose();
        break;
    case JAUS_SELECT_CAMERA:
        msg = new SelectCamera();
        break;
    case JAUS_SET_CAMERA_CAPABILITIES:
        msg = new SetCameraCapabilities();
        break;
    case JAUS_SET_CAMERA_FORMAT_OPTIONS:
        msg = new SetCameraFormatOptions();
        break;
    //  World Model Subgroup
    case JAUS_CREATE_VECTOR_KNOWLEDGE_STORE_OBJECTS:
 //       msg = new CreateVectorKnowledgeStoreObjects();
        break;
    ////  Dynamic Configuration Subgroup
    ////  Payload Subgroup
    ////  Planning Subgroup
    //case JAUS_SPOOL_MISSION:
    //    msg = new SpoolMission();
    //    break;
    //case JAUS_RUN_MISSION:
    //    msg = new RunMission();
    //    break;
    //case JAUS_ABORT_MISSION:
    //    msg = new AbortMission();
    //    break;
    //case JAUS_PAUSE_MISSION:
    //    msg = new PauseMission();
    //    break;
    //case JAUS_RESUME_MISSION:
    //    msg = new ResumeMission();
    //    break;
    //case JAUS_REMOVE_MESSAGES:
    //    msg = new RemoveMessages();
    //    break;
    //case JAUS_REPLACE_MESSAGES:
    //    msg = new ReplaceMessages();
    //    break;
    //  COULD NOT FIND MESSAGE!
    default:
        msg = NULL;
        break;
    };
    return msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for creating query type messages.
///
///   \param code The class code of the message to get.
///               (see messages/query/querycodes.h).
///
///   \return Pointer to newly allocated message structure.  If
///   the class code is not identified, NULL is returned.  You must delete
///   this message!
///
////////////////////////////////////////////////////////////////////////////////////
Message* MessageCreator::CreateQueryMessage(const UShort code)
{
    Message* msg = NULL;
    switch(code)
    {
    // Core Messages:
    case JAUS_QUERY_COMPONENT_AUTHORITY:
        msg = new QueryComponentAuthority();
        break;
    case JAUS_QUERY_COMPONENT_STATUS:
        msg = new QueryComponentStatus();
        break;
    case JAUS_QUERY_TIME:
        msg = new QueryTime();
        break;
    case JAUS_QUERY_COMPONENT_CONTROL:
        msg = new QueryComponentControl();
        break;
    // Event Setup and Control Messages:
    case JAUS_QUERY_EVENTS:
        msg = new QueryEvents();
        break;
    // Communications Messages:
    case JAUS_QUERY_DATA_LINK_STATUS:
        msg = new QueryDataLinkStatus();
        break;
    case JAUS_QUERY_SELECTED_DATA_LINK_STATUS:
        msg = new QuerySelectedDataLinkStatus();
        break;
    case JAUS_QUERY_HEARTBEAT_PULSE:
        msg = new QueryHeartbeatPulse();
        break;
    // Platform Messages:
    case JAUS_QUERY_PLATFORM_SPECIFICATIONS:
        msg = new QueryPlatformSpecifications();
        break;
    case JAUS_QUERY_PLATFORM_OPERATIONAL_DATA:
        msg = new QueryPlatformOperationalData();
        break;
    case JAUS_QUERY_GLOBAL_POSE:
        msg = new QueryGlobalPose();
        break;
    case JAUS_QUERY_LOCAL_POSE:
         msg = new QueryLocalPose();
         break;
    case JAUS_QUERY_VELOCITY_STATE:
         msg = new QueryVelocityState();
         break;
    case JAUS_QUERY_WRENCH_EFFORT:
         msg = new QueryWrenchEffort();
         break;
    case JAUS_QUERY_DISCRETE_DEVICES:
         msg = new QueryDiscreteDevices();
         break;
    case JAUS_QUERY_GLOBAL_VECTOR:
         msg = new QueryGlobalVector();
         break;
    case JAUS_QUERY_LOCAL_VECTOR:
         msg = new QueryLocalVector();
         break;
    case JAUS_QUERY_TRAVEL_SPEED:
         msg = new QueryTravelSpeed();
         break;
    case JAUS_QUERY_WAYPOINT_COUNT:
         msg = new QueryWaypointCount();
         break;
    case JAUS_QUERY_GLOBAL_WAYPOINT:
         msg = new QueryGlobalWaypoint();
         break;
    case JAUS_QUERY_LOCAL_WAYPOINT:
         msg = new QueryLocalWaypoint();
         break;
    case JAUS_QUERY_PATH_SEGMENT_COUNT:
         msg = new QueryPathSegmentCount();
         break;
    case JAUS_QUERY_GLOBAL_PATH_SEGMENT:
         msg = new QueryGlobalPathSegment();
         break;
    case JAUS_QUERY_LOCAL_PATH_SEGMENT:
         msg = new QueryLocalPathSegment();
         break;
    //// Manipulator Messages:
    //case JAUS_QUERY_MANIPULATOR_SPECIFICATIONS:
    //     msg = new QueryManipulatorSpecifications();
    //     break;
    //case JAUS_QUERY_JOINT_EFFORTS:
    //     msg = new QueryJointEfforts();
    //     break;
    //case JAUS_QUERY_JOINT_POSITIONS:
    //     msg = new QueryJointPositions();
    //     break;
    //case JAUS_QUERY_JOINT_VELOCITIES:
    //     msg = new QueryJointVelocities();
    //     break;
    //case JAUS_QUERY_TOOL_POINT:
    //     msg = new QueryToolPoint();
    //     break;
    //case JAUS_QUERY_JOINT_FORCE_TORQUES:
    //     msg = new QueryJointForceTorques();
    //     break;
    //// Environment Messages:
    //case JAUS_QUERY_CAMERA_POSE:
    //     msg = new QueryCameraPose();
    //     break;
    case JAUS_QUERY_CAMERA_COUNT:
         msg = new QueryCameraCount();
         break;
    case JAUS_QUERY_RELATIVE_OBJECT_POSITION:
         msg = new QueryRelativeObjectPosition();
         break;
    case JAUS_QUERY_SELECTED_CAMERA:
         msg = new QuerySelectedCamera();
         break;
    case JAUS_QUERY_CAMERA_CAPABILITIES:
         msg = new QueryCameraCapabilities();
         break;
    case JAUS_QUERY_CAMERA_FORMAT_OPTIONS:
         msg = new QueryCameraFormatOptions();
         break;
    case JAUS_QUERY_IMAGE:
         msg = new QueryImage();
         break;
    ////  World Model Subgroup
    //  Dynamic Configuration (Discovery) Subgroup
    case JAUS_QUERY_IDENTIFICATION:
         msg = new QueryIdentification();
         break;
    case JAUS_QUERY_CONFIGURATION:
         msg = new QueryConfiguration();
         break;
    case JAUS_QUERY_SUBSYSTEM_LIST:
         msg = new QuerySubsystemList();
         break;
    case JAUS_QUERY_SERVICES:
         msg = new QueryServices();
         break;
    //// Planning Subgroup
    //case JAUS_QUERY_SPOOLING_PREFERENCE:
    //    msg = new QuerySpoolingPreference();
    //    break;
    //case JAUS_QUERY_MISSION_STATUS:
    //    msg = new QueryMissionStatus();
    //    break;
    default:
        msg = NULL;
        break;
    };
    return msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for creating inform type messages.
///
///   \param code The class code of the message to get.
///               (see messages/query/querycodes.h).
///
///   \return Pointer to newly allocated message structure.  If
///   the class code is not identified, NULL is returned.  You must delete
///   this message!
///
////////////////////////////////////////////////////////////////////////////////////
Message* MessageCreator::CreateInformMessage(const UShort code)
{
    Message* msg = NULL;
    switch(code)
    {
    // Core Subgroup
    case JAUS_REPORT_COMPONENT_AUTHORITY:
        msg = new ReportComponentAuthority();
        break;
    case JAUS_REPORT_COMPONENT_STATUS:
        msg = new ReportComponentStatus();
        break;
    case JAUS_REPORT_TIME:
        msg = new ReportTime();
        break;
    case JAUS_REPORT_COMPONENT_CONTROL:
        msg = new ReportComponentControl();
        break;
    // Event Setup and Control Subgroup
    case JAUS_REPORT_EVENTS:
        msg = new ReportEvents();
        break;
    case JAUS_EVENT:
        msg = new EventMessage();
        break;
    // Communication Subgroup
    case JAUS_REPORT_DATA_LINK_STATUS:
        msg = new ReportDataLinkStatus();
        break;
    case JAUS_REPORT_SELECTED_DATA_LINK_STATUS:
        msg = new ReportSelectedDataLinkStatus();
        break;
    case JAUS_REPORT_HEARTBEAT_PULSE:
        msg = new ReportHeartbeatPulse();
        break;
    // Platform Subgroup
    case JAUS_REPORT_PLATFORM_SPECIFICATIONS:
        msg = new Jaus::ReportPlatformSpecifications();
        break;
    case JAUS_REPORT_PLATFORM_OPERATIONAL_DATA:
        msg = new ReportPlatformOperationalData();
        break;
    case JAUS_REPORT_GLOBAL_POSE:
        msg = new ReportGlobalPose();
        break;
    case JAUS_REPORT_LOCAL_POSE:
        msg = new ReportLocalPose();
        break;
    case JAUS_REPORT_VELOCITY_STATE:
        msg = new ReportVelocityState();
        break;
    case JAUS_REPORT_WRENCH_EFFORT:
        msg = new ReportWrenchEffort();
        break;
    case JAUS_REPORT_DISCRETE_DEVICES:
        msg = new ReportDiscreteDevices();
        break;
    case JAUS_REPORT_GLOBAL_VECTOR:
        msg = new ReportGlobalVector();
        break;
    case JAUS_REPORT_LOCAL_VECTOR:
        msg = new ReportLocalVector();
        break;
    case JAUS_REPORT_TRAVEL_SPEED:
        msg = new ReportTravelSpeed();
        break;
    case JAUS_REPORT_WAYPOINT_COUNT:
        msg = new ReportWaypointCount();
        break;
    case JAUS_REPORT_GLOBAL_WAYPOINT:
        msg = new ReportGlobalWaypoint();
        break;
    case JAUS_REPORT_LOCAL_WAYPOINT:
        msg = new ReportLocalWaypoint();
        break;
    case JAUS_REPORT_PATH_SEGMENT_COUNT:
        msg = new ReportPathSegmentCount();
        break;
    case JAUS_REPORT_GLOBAL_PATH_SEGMENT:
        msg = new ReportGlobalPathSegment();
        break;
     case JAUS_REPORT_LOCAL_PATH_SEGMENT:
        msg = new ReportLocalPathSegment();
        break;
    //// Manipulator Subgroup
    //case JAUS_REPORT_JOINT_EFFORTS:
    //    msg = new ReportJointEfforts();
    //    break;
    //case JAUS_REPORT_JOINT_POSITIONS:
    //    msg = new ReportJointPositions();
    //    break;
    //case JAUS_REPORT_JOINT_VELOCITIES:
    //    msg = new ReportJointVelocities();
    //    break;
    //case JAUS_REPORT_TOOL_POINT:
    //    msg = new ReportToolPoint();
    //    break;
    //case JAUS_REPORT_MANIPULATOR_SPECIFICATIONS:
    //    msg = new ReportManipulatorSpecifications();
    //    break;
    //case JAUS_REPORT_JOINT_FORCE_TORQUES:
    //    msg = new ReportJointForceTorques();
    //    break;
    //// Environmental Sensor Subgroup
    case JAUS_REPORT_CAMERA_POSE:
        msg = new ReportCameraPose();
        break;
    case JAUS_REPORT_CAMERA_COUNT:
        msg = new ReportCameraCount();
        break;
    case JAUS_REPORT_RELATIVE_OBJECT_POSITION:
        msg = new ReportRelativeObjectPosition();
        break;
    case JAUS_REPORT_SELECTED_CAMERA:
        msg = new ReportSelectedCamera();
        break;
    case JAUS_REPORT_CAMERA_CAPABILITIES:
        msg = new ReportCameraCapabilities();
        break;
    case JAUS_REPORT_CAMERA_FORMAT_OPTIONS:
        msg = new ReportCameraFormatOptions();
        break;
    case JAUS_REPORT_IMAGE:
        msg = new ReportImage();
        break;
    //  Dynamic Configuration Subgroup
    case JAUS_REPORT_IDENTIFICATION:
        msg = new ReportIdentification();
        break;
    case JAUS_REPORT_SUBSYSTEM_LIST:
        msg = new ReportSubsystemList();
        break;
    case JAUS_REPORT_SERVICES:
        msg = new ReportServices();
        break;
    case JAUS_REPORT_CONFIGURATION:
        msg = new ReportConfiguration();
        break;
    //// Planning Subgroup
    //case JAUS_REPORT_SPOOLING_PREFERENCE:
    //    msg = new ReportSpoolingPreference();
    //    break;
    //case JAUS_REPORT_MISSION_STATUS:
    //    msg = new ReportMissionStatus();
    //    break;
    default:
        msg = NULL;
        break;
    };
    return msg;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Method for creating user defined type messages.  These are
///   messages not defined by the JAUS Reference Architecture and have a
///   command code between 0xD000-0xFFFF.
///
///   \param code The class code of the message to
///               get. (see jclasscodes.h).
///
///   \return Pointer to newly allocated message structure.  If
///   the class code is not identified, NULL is returned.  You must delete
///   this message!
///
////////////////////////////////////////////////////////////////////////////////////
Message* MessageCreator::CreateExperimentalMessage(const UShort code)
{
    Message* msg = NULL;
    switch(code)
    {
    //  User defined messages
    case JAUS_QUERY_SICK_LIDAR:
         msg = new QuerySickLidar(); 
         break;
    case JAUS_REPORT_SICK_LIDAR:
        msg = new ReportSickLidar();
        break;
    case JAUS_QUERY_PIXEL_ON_SCREEN:
        msg = new QueryPixelOnScreen();
        break;
    case JAUS_REPORT_PIXEL_ON_SCREEN:
        msg = new ReportPixelOnScreen();
        break;
    //  COULD NOT FIND MESSAGE!
    default:
        msg = NULL;
        break;
    };
    return msg;
}


/*  End of File */
