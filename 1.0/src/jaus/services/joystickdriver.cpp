////////////////////////////////////////////////////////////////////////////////////
///
///  \file joystickdriver.cpp
///  \brief This file contains an interface for a component that uses joystick
///  inputs to control a Primitive Driver component.
///
///  Author(s): Daniel Barber
///  Created: 26 July 2007
///  Last Modified: 26 February 2008
///  Copyright (c) 2007
///  Applied Cognition and Training in Immersive Virtual Environments
///  (ACTIVE) Laboratory
///  Institute for Simulation and Training (IST)
///  University of Central Florida (UCF)
///  Email: dbarber@ist.ucf.edu
///  Web:  http://active.ist.ucf.edu
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
#include "jaus/services/joystickdriver.h"
#include "jaus/messages/query/platform/queryglobalpose.h"
#include "jaus/messages/query/platform/queryplatformspecifications.h"
#include "jaus/messages/inform/platform/reportplatformspecifications.h"
#include <iostream>
#include <tinyxml.h>

using namespace std;
using namespace Jaus;
using namespace CxUtils;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
JoystickDriver::JoystickDriver()
{
    mpJoystick = new Joystick();
    memset(mButtonValues, 0, sizeof(int)*32);
    mTakeDriveControlFlag = false;
    mTakeCameraControlFlag = false;
    mJoystickSubsystemID = 0;
    mCameraModeIndicator = 0;
    MapAxisToWrench(Joystick::Y, PropulsiveLinearEffortX, true, 2);
    MapAxisToWrench(Joystick::X, PropulsiveRotationalEffortZ, false, 2);
    MapAxisToCameraWrench(Joystick::PovY, XAngleOrAngularRotationRate, true, false);
    MapAxisToCameraWrench(Joystick::PovX, YAngleOrAngularRotationRate, true, false);
    MapAxisToVectorCommand(Joystick::Y, Speed, true, 2);
    MapAxisToVectorCommand(Joystick::X, Heading, false, 2);
    MapButtonToFunction(7, RequestDriveControl);
    MapButtonToFunction(8, ReleaseDriveControl);
    MapButtonToFunction(9, RequestCameraControl);
    MapButtonToFunction(10, ReleaseCameraControl);
    MapButtonToFunction(1, ResetCameraPose);
    mControlCheckTimeMs = 0;
    mCameraID = 1;    
    mVectorJoystickFlag = false;
    mControlledVehicleMaxSpeed = 0.0;
    mAutoBrakingFlag = true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
JoystickDriver::~JoystickDriver()
{
    if(mpJoystick)
    {
        mpJoystick->Shutdown();
        delete mpJoystick;
        mpJoystick = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initialize joystick interface.
///
///   \param i Joystick number, using UINT_MAX to connect to first available. 
///            values range from [0,12].
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::InitializeJoystick(const unsigned int i)
{
    mpJoystick->Shutdown();
    memset(mButtonValues, 0, sizeof(int)*32);
    mTakeDriveControlFlag = false;
    mTakeCameraControlFlag = false;
    if(mpJoystick->Initialize(i))
    {
        mpJoystick->RegisterCallback(JoystickDriver::JoystickCallback, this);
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Initialize joystick interface using an XML file.
///
///   \param settingsXML XML file containing settings for joystick initialization.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::InitializeJoystick(const std::string& settingsXML)
{
    mpJoystick->Shutdown();
    memset(mButtonValues, 0, sizeof(int)*32);
    mTakeDriveControlFlag = false;
    mTakeCameraControlFlag = false;

    int result = JAUS_FAILURE;

    TiXmlDocument xml;
    if(!xml.LoadFile(settingsXML.c_str()))
    {
        cout << "Joystick: Unable to parse XML file.\n";
        return result;
    }
    if(xml.FirstChildElement("Jaus") == NULL)
    {
        std::cout << "Joystick: XML file does not contain JAUS data.\n";
        SetJausError(ErrorCodes::InvalidValue);
        return result;
    }

    TiXmlHandle docHandle(&xml);
    TiXmlNode* node;

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("ComponentAuthority").ToNode();
    if(node)
    {
        this->SetComponentAuthority((Byte)atoi(node->FirstChild()->Value()));
    }
    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("PlatformSubsystemID").ToNode();
    if(node)
    {
        this->SetSubsystemToControl((Byte)atoi(node->FirstChild()->Value()));
    }
    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("CameraID").ToNode();
    if(node)
    {
        mCameraID = ((Byte)atoi(node->FirstChild()->Value()));
    }
    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("JoystickID").ToNode();
    if(node)
    {
        if(this->InitializeJoystick(atoi(node->FirstChild()->Value())))
        {
            result = JAUS_OK;
        }
    }
    
    TiXmlElement* element;

    element = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Axes").FirstChild("WrenchEffort").ToElement();
    while(element)
    {
        if(strcmp(element->Value(), "WrenchEffort") == 0 &&
            element->Attribute("wrench") &&
            element->Attribute("axis") &&
            element->Attribute("invert") &&
            element->Attribute("deadzone"))
        {
            WrenchEffort wrench = (WrenchEffort)(atoi(element->Attribute("wrench")));
            CxUtils::Joystick::Axes axis = CxUtils::Joystick::X;
            if(strcmp(element->Attribute("axis"), "X") == 0)
            {
                axis = CxUtils::Joystick::X;
            }
            if(strcmp(element->Attribute("axis"), "Y") == 0)
            {
                axis = CxUtils::Joystick::Y;
            }
            if(strcmp(element->Attribute("axis"), "Z") == 0)
            {
                axis = CxUtils::Joystick::Z;
            }
            if(strcmp(element->Attribute("axis"), "R") == 0)
            {
                axis = CxUtils::Joystick::R;
            }
            if(strcmp(element->Attribute("axis"), "U") == 0)
            {
                axis = CxUtils::Joystick::U;
            }
            if(strcmp(element->Attribute("axis"), "V") == 0)
            {
                axis = CxUtils::Joystick::V;
            }
            if(strcmp(element->Attribute("axis"), "PovX") == 0)
            {
                axis = CxUtils::Joystick::PovX;
            }
            if(strcmp(element->Attribute("axis"), "PovY") == 0)
            {
                axis = CxUtils::Joystick::PovY;
            }
            short int deadzone = 0;
            bool invert = false;
            deadzone = (short int)atoi(element->Attribute("deadzone"));
            if(strcmp(element->Attribute("invert"), "true") == 0)
            {
                invert = true;
            }
            MapAxisToWrench(axis, wrench, invert, deadzone);
        }
        element = element->NextSiblingElement();
    }

    element = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Axes").FirstChild("VectorCommand").ToElement();
    while(element)
    {
        if(strcmp(element->Value(), "VectorCommand") == 0 &&
            element->Attribute("vector") &&
            element->Attribute("axis") &&
            element->Attribute("invert") &&
            element->Attribute("deadzone"))
        {
            VectorCommand vector = (VectorCommand)(atoi(element->Attribute("vector")));
            CxUtils::Joystick::Axes axis = CxUtils::Joystick::X;
            if(strcmp(element->Attribute("axis"), "X") == 0)
            {
                axis = CxUtils::Joystick::X;
            }
            if(strcmp(element->Attribute("axis"), "Y") == 0)
            {
                axis = CxUtils::Joystick::Y;
            }
            if(strcmp(element->Attribute("axis"), "Z") == 0)
            {
                axis = CxUtils::Joystick::Z;
            }
            if(strcmp(element->Attribute("axis"), "R") == 0)
            {
                axis = CxUtils::Joystick::R;
            }
            if(strcmp(element->Attribute("axis"), "U") == 0)
            {
                axis = CxUtils::Joystick::U;
            }
            if(strcmp(element->Attribute("axis"), "V") == 0)
            {
                axis = CxUtils::Joystick::V;
            }
            if(strcmp(element->Attribute("axis"), "PovX") == 0)
            {
                axis = CxUtils::Joystick::PovX;
            }
            if(strcmp(element->Attribute("axis"), "PovY") == 0)
            {
                axis = CxUtils::Joystick::PovY;
            }
            short int deadzone = 0;
            bool invert = false;
            deadzone = (short int)atoi(element->Attribute("deadzone"));
            if(strcmp(element->Attribute("invert"), "true") == 0)
            {
                invert = true;
            }
            MapAxisToVectorCommand(axis, vector, invert, deadzone);
            // Since we have Vector Command data, this joystick will
            // operate as a Joystick Vector Driver (controls Global Vector Driver component).
            SetPrimitiveDriverJoystick(false);
        }
        element = element->NextSiblingElement();
    }

    element = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Axes").FirstChild("CameraWrench").ToElement();
    while(element)
    {
        if(strcmp(element->Value(), "CameraWrench") == 0  &&
            element->Attribute("wrench") &&
            element->Attribute("axis") &&
            element->Attribute("rate") &&
            element->Attribute("invert") &&
            element->Attribute("deadzone"))
        {
            CameraWrench wrench = (CameraWrench)(atoi(element->Attribute("wrench")));
            CxUtils::Joystick::Axes axis = CxUtils::Joystick::X;
            if(strcmp(element->Attribute("axis"), "X") == 0)
            {
                axis = CxUtils::Joystick::X;
            }
            if(strcmp(element->Attribute("axis"), "Y") == 0)
            {
                axis = CxUtils::Joystick::Y;
            }
            if(strcmp(element->Attribute("axis"), "Z") == 0)
            {
                axis = CxUtils::Joystick::Z;
            }
            if(strcmp(element->Attribute("axis"), "R") == 0)
            {
                axis = CxUtils::Joystick::R;
            }
            if(strcmp(element->Attribute("axis"), "U") == 0)
            {
                axis = CxUtils::Joystick::U;
            }
            if(strcmp(element->Attribute("axis"), "V") == 0)
            {
                axis = CxUtils::Joystick::V;
            }
            if(strcmp(element->Attribute("axis"), "PovX") == 0)
            {
                axis = CxUtils::Joystick::PovX;
            }
            if(strcmp(element->Attribute("axis"), "PovY") == 0)
            {
                axis = CxUtils::Joystick::PovY;
            }
            short int deadzone = 0;
            bool invert = false;
            bool rate = false;
            rate = atoi(element->Attribute("rate")) > 0 ? true : false;
            deadzone = (short int)atoi(element->Attribute("deadzone"));
            if(strcmp(element->Attribute("invert"), "true") == 0)
            {
                invert = true;
            }
            MapAxisToCameraWrench(axis, wrench, rate, invert, deadzone);
        }
        element = element->NextSiblingElement();
    }

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Buttons").FirstChild("RequestDriveControlButton").ToNode();
    if(node)
    {
        MapButtonToFunction(atoi(node->FirstChild()->Value()), RequestDriveControl);
    }

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Buttons").FirstChild("ReleaseDriveControlButton").ToNode();
    if(node)
    {
        MapButtonToFunction(atoi(node->FirstChild()->Value()), ReleaseDriveControl);
    }

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Buttons").FirstChild("RequestCameraControlButton").ToNode();
    if(node)
    {
        MapButtonToFunction(atoi(node->FirstChild()->Value()), RequestCameraControl);
    }

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Buttons").FirstChild("ReleaseCameraControlButton").ToNode();
    if(node)
    {
        MapButtonToFunction(atoi(node->FirstChild()->Value()), ReleaseCameraControl);
    }

    node = docHandle.FirstChild("Jaus").FirstChild("JoystickDriverComponent").FirstChild("Buttons").FirstChild("ResetCameraPose").ToNode();
    if(node)
    {
        MapButtonToFunction(atoi(node->FirstChild()->Value()), ResetCameraPose);
    }

    result = JAUS_OK;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the the joystick, releasing control of any primitive 
///   drivers or cameras being operated with it.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ShutdownJoystick()
{
    mTakeDriveControlFlag = false;
    if(mDriverID.IsValid() &&
        HaveComponentControl(mDriverID))
    {
        SendStandbyCommand(mDriverID);
        ReleaseComponentControl(mDriverID);
    }
    mTakeCameraControlFlag = false;
    
    if(mVisualSensorID.IsValid() &&
        HaveComponentControl(mVisualSensorID))
    {
        ReleaseComponentControl(mVisualSensorID);
    }

    if(mpJoystick) { mpJoystick->Shutdown(); }
    memset(mButtonValues, 0, sizeof(int)*32);
    mTakeDriveControlFlag = false;
    mTakeCameraControlFlag = false;

    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Shutsdown the component.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::Shutdown()
{
    ShutdownJoystick();
    return CommandComponent::Shutdown();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief This method is called whenever an Inform message is received
///   by the component.  This specific implementation adds support for
///   Report Platform Specifications message.
///
///   \param[in] message Message to process.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ProcessInformMessage(const Message* message)
{
    int result = FAILURE;
    if(message->GetCommandCode() == JAUS_REPORT_PLATFORM_SPECIFICATIONS)
    {
        result = OK;
        const ReportPlatformSpecifications* specs = dynamic_cast<const ReportPlatformSpecifications*>(message);
        if(specs)
        {
            mControlledVehicleMaxSpeed = specs->GetMaximumVelocityX();
        }
    }
    if(result == FAILURE)
    {
        result = CommandComponent::ProcessInformMessage(message);
    }
    else
    {
        CommandComponent::ProcessInformMessage(message);
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Map a joystick axis to a wrench effort axis.
///
///   \param axis Joystick axis.
///   \param wrench Wrench axis.
///   \param invertFlag Invert joystick true/false.
///   \param deadzone Joystick axis deadzone.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::MapAxisToWrench(const CxUtils::Joystick::Axes axis,
                                    const WrenchEffort wrench,
                                    const bool invertFlag,
                                    const short int deadzone)
{
    mJoystickMutex.Enter();
    mAxesMapping[axis] = wrench;
    mDeadZones[axis] = deadzone;
    mInvertFlags[axis] = invertFlag;
    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Map a joystick axis to a wrench effort axis.
///
///   \param axis Joystick axis.
///   \param vector Vector command to map joystick data to.
///   \param invertFlag Invert joystick true/false.
///   \param deadzone Joystick axis deadzone.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::MapAxisToVectorCommand(const CxUtils::Joystick::Axes axis,
                                           const VectorCommand vector,
                                           const bool invertFlag,
                                           const short int deadzone)
{
    mJoystickMutex.Enter();
    mAxesMappingVectors[axis] = vector;
    mDeadZones[axis] = deadzone;
    mInvertFlags[axis] = invertFlag;
    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clear joystick axis mapping.
///
///   \param axis Joystick axis.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ClearAxisToWrenchMap(const CxUtils::Joystick::Axes axis)
{
    std::map<CxUtils::Joystick::Axes, JoystickDriver::WrenchEffort>::iterator a;
    std::map<CxUtils::Joystick::Axes, Short>::iterator d;
    std::map<CxUtils::Joystick::Axes,bool>::iterator i;

    mJoystickMutex.Enter();

    a = mAxesMapping.find(axis);
    if(a != mAxesMapping.end())
        mAxesMapping.erase(a);

    i = mInvertFlags.find(axis);
    if(i != mInvertFlags.end())
        mInvertFlags.erase(i);

    d = mDeadZones.find(axis);
    if(d != mDeadZones.end())
        mDeadZones.erase(d);

    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clear joystick axis mapping to a global vector command.
///
///   \param axis Joystick axis.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ClearAxisToVectorCommand(const CxUtils::Joystick::Axes axis)
{
    std::map<CxUtils::Joystick::Axes, JoystickDriver::VectorCommand>::iterator a;
    std::map<CxUtils::Joystick::Axes, Short>::iterator d;
    std::map<CxUtils::Joystick::Axes,bool>::iterator i;

    mJoystickMutex.Enter();

    a = mAxesMappingVectors.find(axis);
    if(a != mAxesMappingVectors.end())
        mAxesMappingVectors.erase(a);

    i = mInvertFlags.find(axis);
    if(i != mInvertFlags.end())
        mInvertFlags.erase(i);

    d = mDeadZones.find(axis);
    if(d != mDeadZones.end())
        mDeadZones.erase(d);

    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Map a joystick axis to a wrench effort axis.
///
///   \param axis Joystick axis.
///   \param wrench Camera pose wrench.
///   \param rate Axis maps to rate if true, otherwise position if false.
///   \param invertFlag Invert joystick true/false.
///   \param deadzone Joystick axis deadzone.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::MapAxisToCameraWrench(const CxUtils::Joystick::Axes axis,
                                          const CameraWrench wrench,
                                          const bool rate,
                                          const bool invertFlag,
                                          const short int deadzone)
{
    mJoystickMutex.Enter();
    mCameraAxesMapping[axis] = wrench;
    mDeadZones[axis] = deadzone;
    mInvertFlags[axis] = invertFlag;
    BitVector::SetBit(mCameraModeIndicator, wrench, rate);
    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clear joystick axis mapping.
///
///   \param axis Joystick axis.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ClearAxisToCameraWrenchMap(const CxUtils::Joystick::Axes axis)
{
    std::map<CxUtils::Joystick::Axes, JoystickDriver::CameraWrench>::iterator a;
    std::map<CxUtils::Joystick::Axes, Short>::iterator d;
    std::map<CxUtils::Joystick::Axes,bool>::iterator i;

    mJoystickMutex.Enter();

    a = mCameraAxesMapping.find(axis);
    if(a != mCameraAxesMapping.end())
        mCameraAxesMapping.erase(a);

    i = mInvertFlags.find(axis);
    if(i != mInvertFlags.end())
        mInvertFlags.erase(i);

    d = mDeadZones.find(axis);
    if(d != mDeadZones.end())
        mDeadZones.erase(d);

    mJoystickMutex.Leave();

    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Map a button to a function.
///
///   \param buttonNumber Button number to set mapping for.
///   \param function Function to map button to.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::MapButtonToFunction(const int buttonNumber, const ButtonActions function)
{
    int result = JAUS_FAILURE;
    
    if(buttonNumber >= 0 && buttonNumber < 32)
    {
        result = JAUS_OK;
        mJoystickMutex.Enter();
        mButtonsMap[buttonNumber] = function;
        mJoystickMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Clear button mapping.
///
///   \param buttonNumber Button number to clear mapping for.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ClearButtonToFunctionMap(const int buttonNumber)
{
    int result = JAUS_FAILURE;
    
    if(buttonNumber >= 0 && buttonNumber < 32)
    {
        result = JAUS_OK;
        std::map<int, ButtonActions>::iterator b;
        mJoystickMutex.Enter();
        b = mButtonsMap.find(buttonNumber);
        if(b != mButtonsMap.end())
        {
            mButtonsMap.erase(b);
        }
        mJoystickMutex.Leave();
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Inverts data values for a specific axis on the joystick.
///
///   \param axis The axis to invert.
///   \param invertValue Invert value (true to invert, false to not).
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::InvertAxis(const CxUtils::Joystick::Axes axis,
                               const bool invertValue)
{
    int result = JAUS_FAILURE;

    mJoystickMutex.Enter();

    std::map<CxUtils::Joystick::Axes, bool>::iterator f;
    f = mInvertFlags.find(axis);
    if(f != mInvertFlags.end())
    {
        f->second = invertValue;
        result = JAUS_OK;
    }

    mJoystickMutex.Leave();
        
    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Tells component to take control of primitive driver and
///          maintain it.
///
///   \param enable If true drive control is set to true, otherwise false.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::TakeDriveControl(const bool enable)
{
    int result = JAUS_OK;
    
    mJoystickMutex.Enter();
    mTakeDriveControlFlag = enable;
    if(mTakeDriveControlFlag == false && mDriverID.IsValid())
    {
        SendStandbyCommand(mDriverID);
        ReleaseComponentControl(mDriverID, 50);
    }
    mJoystickMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Tells component to take control of visual sensor and
///          maintain it.
///
///   \param enable If true camera control is set to true, otherwise false.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::TakeCameraControl(const bool enable)
{
    int result = JAUS_OK;
    
    mJoystickMutex.Enter();
    mTakeCameraControlFlag = enable;
    mJoystickMutex.Leave();

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Toggle on and off automatic generation of braking data (resistive
///   efforts).
///
///   If set to true, then if any propulsive effort gets set to <= 1 then 
///   this class will check to see if a countering resistive effort is
///   supported by the Primitive Driver component and send 100% resistive
///   effort.
///
///   \param enable If true enable automatic braking, if false do nothing.
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::EnableAutoBrakingFlag(const bool enable)
{
    mAutoBrakingFlag = enable;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Use this method to seet the joytick operating mode to controlling
///   a Primitive Driver or a Global Vector Driver component.
///
///   \param primitiveController If true, joystick controls a Primitive Driver
///                              (this is default).
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::SetPrimitiveDriverJoystick(const bool primitiveController)
{
    mVectorJoystickFlag = !primitiveController;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Processes subsystem discovery event data.
///
///   \param subsystem Subsystem that has updated.
///   \param eventType Event type (disconnect, connect, update, etc.)
///
///   \return JAUS_OK if processed, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::ProcessDiscoveryEvent(const Platform& subsystem, const SubscriberComponent::DiscoveryEvents eventType)
{
    if(eventType == SubscriberComponent::SubsystemDisconnect)
    {
        mJoystickMutex.Enter();
        if(subsystem.GetSubsystemID() == mJoystickSubsystemID)
        {
            mDriverID(0, 0, 0, 0);
            mVisualSensorID(0, 0, 0, 0);
            memset(mButtonValues, 0, sizeof(int)*32);
        }
        mJoystickMutex.Leave();
    }
    else
    {
        mJoystickMutex.Enter();
        if(subsystem.GetSubsystemID() == mJoystickSubsystemID)
        {
            // If we haven't discovered a driving component yet, lets check for one.
            if(mDriverID.IsValid() == false && mJoystickSubsystemID != 0 && mJoystickSubsystemID != 255)
            {
                Address::List drivers = subsystem.GetConfiguration()->GetComponentsOfType(Service::PrimitiveDriver);
                if(drivers.size() > 0)
                {
                    mDriverID = *drivers.begin();
                }
            }
            
            // If we haven't discovered a visual sensor component yet, lets check for one.
            if(mVisualSensorID.IsValid() == false && mJoystickSubsystemID != 0 && mJoystickSubsystemID != 255)
            {
                Address::List sensors = subsystem.GetConfiguration()->GetComponentsOfType(Service::VisualSensor);
                if(sensors.size() > 0)
                {
                    mVisualSensorID = *sensors.begin();
                }
            }

            if(mDriverID.IsValid())
            {
                //  Make sure the component is still available.
                if(subsystem.GetConfiguration()->HaveComponent(mDriverID) == false)
                {                    
                    // See if there is anther.
                    Address::List drivers = subsystem.GetConfiguration()->GetComponentsOfType(Service::PrimitiveDriver);
                    if(drivers.size() > 0)
                    {
                        mDriverID = *drivers.begin();
                    }
                    else
                    {
                        mDriverID(0, 0, 0, 0); // Erase ID
                    }
                }
            }

            if(mVisualSensorID.IsValid())
            {
                //  Make sure the component is still available.
                if(subsystem.GetConfiguration()->HaveComponent(mVisualSensorID) == false)
                {                    
                    // See if there is anther.
                    Address::List sensors = subsystem.GetConfiguration()->GetComponentsOfType(Service::VisualSensor);
                    if(sensors.size() > 0)
                    {
                        mVisualSensorID = *sensors.begin();
                    }
                    else
                    {
                        mVisualSensorID(0, 0, 0, 0); // Erase ID
                    }
                }
            }

            // If we don't have control of the component at all
            // and we are need of it, try take it.
            if(mDriverID.IsValid() && mTakeDriveControlFlag && !HaveComponentControl(mDriverID))
            {
                if(RequestComponentControl(mDriverID, 250))
                {
                    SendResumeCommand(mDriverID);
                }
            }

            // If we don't have control of the component at all
            // and we are need of it, try take it.
            if(mVisualSensorID.IsValid() && mTakeCameraControlFlag && !HaveComponentControl(mVisualSensorID))
            {
                if(RequestComponentControl(mVisualSensorID, 250))
                {
                    SendResumeCommand(mVisualSensorID);
                }
            }
        }
        mJoystickMutex.Leave();
    }

    // Let parent class process too.
    return CommandComponent::ProcessDiscoveryEvent(subsystem, eventType);
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the subsystem ID to control Primitive Driver on.
///
///   \param sid Subsystem ID of subsystem to find Primitive Driver on.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::SetSubsystemToControl(const Byte sid)
{
    if(sid != 0 && sid != 255)
    {
        std::set<Byte> subsystems;
        subsystems.insert(sid);
        AddSubsystemToDiscover(sid);

        mJoystickMutex.Enter();
        mJoystickSubsystemID = sid;
        mJoystickMutex.Leave();
        
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the ID of a camera to control.
///
///   \param sensor ID of the Visual Sensor component the camera is on.
///   \param id ID of a camera to control.  This is not the same as the
///             component ID of a camera.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int JoystickDriver::SetCameraID(const Address& sensor, const Byte id)
{
    if(id >= 1 && sensor.IsValid())
    {
        mCameraID = id;
        mVisualSensorID = sensor;
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints wrench effort being generated by joystick.
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::PrintWrenchEffort() const
{
    mJoystickMutex.Enter();
    if(mDriverID.IsValid() && HaveComponentControl(mDriverID))
    {
        cout << "Have Control of Component: " << mDriverID.ToString() << endl;
    }
    mWrenchEffort.PrintWrenchEffort();
    mJoystickMutex.Leave();
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints wrench effort being generated by joystick.
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::PrintGlobalVector() const
{
    mJoystickMutex.Enter();
    if(mDriverID.IsValid() && HaveComponentControl(mDriverID))
    {
        cout << "Have Control of Component: " << mDriverID.ToString() << endl;
    }
    mGlobalVector.PrintGlobalVector();
    mJoystickMutex.Leave();
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints wrench effort being generated by joystick.
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::PrintCameraWrench() const
{
    mJoystickMutex.Enter();
    if(mVisualSensorID.IsValid() && HaveComponentControl(mVisualSensorID))
    {
        cout << "Have Control of Visual Sensor: " << mVisualSensorID.ToString() << endl;
    }
    mCameraPose.PrintCameraPose();
    mJoystickMutex.Leave();
}



////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Callback for processing joystick data.
///
///   \param joystick Joystick data to process.
///   \param args Pointer to additional arguments used in callback.
///
////////////////////////////////////////////////////////////////////////////////////
void JoystickDriver::JoystickCallback(const CxUtils::Joystick& joystick, void *args)
{
    JoystickDriver* driver = (JoystickDriver*)(args);
    std::map<CxUtils::Joystick::Axes, JoystickDriver::WrenchEffort>::iterator axis;   // Wrench Command Axis
    std::map<CxUtils::Joystick::Axes, JoystickDriver::VectorCommand>::iterator vaxis; // Vector Command Axis
    std::map<CxUtils::Joystick::Axes, JoystickDriver::CameraWrench>::iterator caxis;  // Camera Command Axis
    std::map<CxUtils::Joystick::Axes, Short>::iterator deadzone;
    std::map<CxUtils::Joystick::Axes,bool>::iterator invertFlag;
    std::map<int, ButtonActions>::iterator button;

    if(driver == NULL) { return; }

    // Populate the wrench effort message based
    // on joystick AXIS information.
    driver->mJoystickMutex.Enter();

    driver->mWrenchEffort.ClearMessageBody();

    if(!driver->mVectorJoystickFlag)
    {
        for(axis = driver->mAxesMapping.begin();
            axis != driver->mAxesMapping.end();
            axis++)
        {
            double percent = 0.0;
            Short axisDeadzone = 0;

            // See if there is a deadzone for the axis.
            deadzone = driver->mDeadZones.find(axis->first);
            if(deadzone != driver->mDeadZones.end())
            {
                axisDeadzone = deadzone->second;
            }
            // Get axis percentage.
            percent = joystick.GetAxisPercentage((short)(axis->first), axisDeadzone);
            // Invert if needed.
            invertFlag = driver->mInvertFlags.find(axis->first);
            if(invertFlag != driver->mInvertFlags.end())
            {
                if(invertFlag->second)
                {
                    percent *= -1.0;
                }
            }

            // Set the wrench effort.
            switch(axis->second)
            {
            case JoystickDriver::PropulsiveLinearEffortX:
                driver->mWrenchEffort.SetPropulsiveLinearEffortX(percent);
                break;
            case JoystickDriver::PropulsiveLinearEffortY:
                driver->mWrenchEffort.SetPropulsiveLinearEffortY(percent);
                break;
            case JoystickDriver::PropulsiveLinearEffortZ:
                driver->mWrenchEffort.SetPropulsiveLinearEffortZ(percent);
                break;
            case JoystickDriver::PropulsiveRotationalEffortX:
                driver->mWrenchEffort.SetPropulsiveRotationalEffortX(percent);
                break;
            case JoystickDriver::PropulsiveRotationalEffortY:
                driver->mWrenchEffort.SetPropulsiveRotationalEffortY(percent);
                break;
            case JoystickDriver::PropulsiveRotationalEffortZ:
                driver->mWrenchEffort.SetPropulsiveRotationalEffortZ(percent);
                break;
            case JoystickDriver::ResistiveLinearEffortX:
                driver->mWrenchEffort.SetResistiveLinearEffortX(percent);
                break;
            case JoystickDriver::ResistiveLinearEffortY:
                driver->mWrenchEffort.SetResistiveLinearEffortY(percent);
                break;
            case JoystickDriver::ResistiveLinearEffortZ:
                driver->mWrenchEffort.SetResistiveLinearEffortZ(percent);
                break;
            case JoystickDriver::ResistiveRotationalEffortX:
                driver->mWrenchEffort.SetResistiveRotationalEffortX(percent);
                break;
            case JoystickDriver::ResistiveRotationalEffortY:
                driver->mWrenchEffort.SetResistiveRotationalEffortY(percent);
                break;
            case JoystickDriver::ResistiveRotationalEffortZ:
                driver->mWrenchEffort.SetResistiveRotationalEffortZ(percent);
                break;
            default:
                percent = 0.0;
                break;
            }
        }
    }
    // Generate Global Vector
    else
    {
        driver->mGlobalVector.ClearMessageBody();

        for(vaxis = driver->mAxesMappingVectors.begin();
            vaxis != driver->mAxesMappingVectors.end();
            vaxis++)
        {
            double percent = 0.0;
            Short axisDeadzone = 0;

            // See if there is a deadzone for the axis.
            deadzone = driver->mDeadZones.find(vaxis->first);
            if(deadzone != driver->mDeadZones.end())
            {
                axisDeadzone = deadzone->second;
            }
            // Get axis percentage.
            percent = joystick.GetAxisPercentage((short)(vaxis->first), axisDeadzone);
            // Invert if needed.
            invertFlag = driver->mInvertFlags.find(vaxis->first);
            if(invertFlag != driver->mInvertFlags.end())
            {
                if(invertFlag->second)
                {
                    percent *= -1.0;
                }
            }

            // Set the wrench effort.
            switch(vaxis->second)
            {
            case JoystickDriver::Speed:
                if(fabs(driver->mControlledVehicleMaxSpeed) < .0001)
                {
                    Jaus::QueryPlatformSpecifications querySpecs;
                    querySpecs.SetPresenceVector(Jaus::QueryPlatformSpecifications::VectorMask::MaximumVelocityX);
                    querySpecs.SetSourceID(driver->GetID());
                    Jaus::Platform platform = driver->GetPlatformInfo(driver->mDriverID.mSubsystem);
                    Address::List components;
                    components = platform.GetConfiguration()->GetComponentsOfType((Byte)Service::PrimitiveDriver);
                    for(unsigned int i = 0; i < (unsigned int)components.size(); i++)
                    {
                        querySpecs.SetDestinationID(components[i]);
                        driver->Send(&querySpecs);
                    }
                }
                else
                {
                    double desiredSpeed = fabs(percent/100.0)*driver->mControlledVehicleMaxSpeed;
                    if(desiredSpeed > SetGlobalVector::Limits::MaxSpeed)
                    {
                        desiredSpeed = SetGlobalVector::Limits::MaxSpeed;
                    }
                    driver->mGlobalVector.SetSpeed(desiredSpeed);
                }
                break;
            case JoystickDriver::Elevation:
                {
                    double elev = 0;
                    elev = ((percent + 100)/200.0)*(SetGlobalVector::Limits::MaxElevation - SetGlobalVector::Limits::MinElevation) + SetGlobalVector::Limits::MinElevation;
                    driver->mGlobalVector.SetElevation(elev);
                }
                break;
            case JoystickDriver::Heading:
                {
                    // Do we have global pose data
                    Platform platform;
                    platform = driver->GetPlatformInfo(driver->mJoystickSubsystemID);
                    if(platform.HaveGlobalPose())
                    {
                        // Generate a desired heading
                        // First calculate a heading delta, then add
                        // that delta to the current vehicle heading, check
                        // for wrap around, then set value.
                        double desiredHeading = (percent/100.0)*JAUS_PI/2.0;
                        desiredHeading += platform.GetGlobalPose()->GetYaw();
                        if(desiredHeading > JAUS_PI)
                        {
                            desiredHeading -= JAUS_TWO_PI;
                        }
                        else if(desiredHeading < -JAUS_PI)
                        {
                            desiredHeading += JAUS_TWO_PI;
                        }
                        driver->mGlobalVector.SetHeading(desiredHeading);
                    }
                    else
                    {
                        Address::List sensors = platform.GetConfiguration()->GetComponentsOfType((Byte)Service::GlobalPoseSensor);
                        if(sensors.size() > 0)
                        {
                            Address::List::iterator id;
                            for(id = sensors.begin(); id != sensors.end(); id++)
                            {
                                CreateEventRequest createEvent;
                                QueryGlobalPose queryGlobalPose;
                                queryGlobalPose.SetPresenceVector(QueryGlobalPose::VectorMask::Yaw);

                                createEvent.SetEventType(CreateEventRequest::EveryChange);
                                createEvent.SetMessageCode(JAUS_REPORT_GLOBAL_POSE);
                                createEvent.SetQueryMessage(&queryGlobalPose);
                                createEvent.SetSourceID(driver->GetID());
                                createEvent.SetDestinationID(*id);

                                if(driver->RequestEvent(createEvent) == OK)
                                {
                                    break;
                                }
                            }
                        }
                    }
                }
                break;
            case JoystickDriver::Roll:
                driver->mGlobalVector.SetRoll( (percent/100.0)*SetGlobalVector::Limits::MaxAngle );
                break;
            case JoystickDriver::Pitch:
                driver->mGlobalVector.SetPitch( (percent/100.0)*SetGlobalVector::Limits::MaxAngle );
                break;
            case JoystickDriver::Depth:                
                break;
            default:
                percent = 0.0;
                break;
            }
        }
    }

    driver->mCameraPose.ClearMessageBody();

    for(caxis = driver->mCameraAxesMapping.begin();
        caxis != driver->mCameraAxesMapping.end();
        caxis++)
    {
        double percent = 0.0;
        Short axisDeadzone = 0;

        // See if there is a deadzone for the caxis.
        deadzone = driver->mDeadZones.find(caxis->first);
        if(deadzone != driver->mDeadZones.end())
        {
            axisDeadzone = deadzone->second;
        }
        // Get caxis percentage.
        percent = joystick.GetAxisPercentage((short)(caxis->first), axisDeadzone);
        // Invert if needed.
        invertFlag = driver->mInvertFlags.find(caxis->first);

        if(invertFlag != driver->mInvertFlags.end())
        {
            if(invertFlag->second)
            {
                percent *= -1.0;
            }
        }

        // Set the wrench effort.
        switch(caxis->second)
        {
        case JoystickDriver::XDisplacementOrLinearRate:
            driver->mCameraPose.SetXDisplacementOrLinearRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, XDisplacementOrLinearRate));
            break;
        case JoystickDriver::YDisplacementOrLinearRate:
            driver->mCameraPose.SetYDisplacementOrLinearRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, YDisplacementOrLinearRate));
            break;
        case JoystickDriver::ZDisplacementOrLinearRate:
            driver->mCameraPose.SetZDisplacementOrLinearRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, ZDisplacementOrLinearRate));
            break;
        case JoystickDriver::XAngleOrAngularRotationRate:
            driver->mCameraPose.SetXAngleOrAngularRotationRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, XAngleOrAngularRotationRate));
            break;
        case JoystickDriver::YAngleOrAngularRotationRate:
            driver->mCameraPose.SetYAngleOrAngularRotationRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, YAngleOrAngularRotationRate));
            break;
        case JoystickDriver::ZAngleOrAngularRotationRate:
            driver->mCameraPose.SetZAngleOrAngularRotationRate(percent, BitVector::IsBitSet(driver->mCameraModeIndicator, ZAngleOrAngularRotationRate));
            break;
        default:
            percent = 0.0;
            break;
        }
    }

    driver->mCameraPose.SetCameraID(driver->mCameraID);

    // Check buttons
    for(button = driver->mButtonsMap.begin();
        button != driver->mButtonsMap.end();
        button++)
    {
        if(button->first >= 0 && button->first < 32)
        {
            // Check for button down
            if(driver->mButtonValues[button->first] == 0 &&
                joystick.IsButtonPressed(button->first))
            {
                switch(button->second)
                {
                case JoystickDriver::RequestDriveControl:
                    driver->mTakeDriveControlFlag = true;
                    break;
                case JoystickDriver::RequestCameraControl:
                    driver->mTakeCameraControlFlag = true;
                    break;
                case JoystickDriver::ReleaseDriveControl:
                    {
                        driver->mTakeDriveControlFlag = false;
                        driver->SendStandbyCommand(driver->mDriverID);
                        driver->ReleaseComponentControl(driver->mDriverID, 50);
                    }
                    break;
                case JoystickDriver::ReleaseCameraControl:
                    {
                        driver->mTakeCameraControlFlag = false;
                        driver->ReleaseComponentControl(driver->mVisualSensorID, 50);
                    }
                    break;
                case JoystickDriver::ResetCameraPose:
                    {
                        if(driver->mVisualSensorID.IsValid())
                        {
                            SetCameraPose command;
                            command.SetSourceID(driver->GetID());
                            command.SetDestinationID(driver->mVisualSensorID);
                            std::map<CxUtils::Joystick::Axes, CameraWrench>::iterator wrench;
                            for(wrench = driver->mCameraAxesMapping.begin();
                                wrench != driver->mCameraAxesMapping.end();
                                wrench++)
                            {
                                switch(wrench->second)
                                {
                                case XDisplacementOrLinearRate:
                                    command.SetXDisplacementOrLinearRate(0, false);
                                    break;
                                case YDisplacementOrLinearRate:
                                    command.SetYDisplacementOrLinearRate(0, false);
                                    break;
                                case ZDisplacementOrLinearRate:
                                    command.SetZDisplacementOrLinearRate(0, false);
                                    break;
                                case XAngleOrAngularRotationRate:
                                    command.SetXAngleOrAngularRotationRate(0, false);
                                    break;
                                case YAngleOrAngularRotationRate:
                                    command.SetYAngleOrAngularRotationRate(0, false);
                                    break;
                                case ZAngleOrAngularRotationRate:
                                    command.SetZAngleOrAngularRotationRate(0, false);
                                    break;
                                };
                            }
                            command.SetCameraID(driver->mCameraID);
                            driver->Send(&command);
                        }
                    }
                    break;
                default:
                    // Do nothing.
                    break;
                }
            }
        }
    }
    // Copy over the button values.
    for(int b = 0; b < 32; b++)
    {
        driver->mButtonValues[b] = joystick.IsButtonPressed(b);
    }

    if(driver->mDriverID.IsValid() && 
        ((driver->mDriverID.mComponent == Service::PrimitiveDriver && driver->mVectorJoystickFlag) || (driver->mDriverID.mComponent == Service::GlobalVectorDriver && !driver->mVectorJoystickFlag)) )
    {
        if(driver->HaveComponentControl(driver->mDriverID))
        {
            driver->SendStandbyCommand(driver->mDriverID);
            driver->ReleaseComponentControl(driver->mDriverID);
        }
        driver->mDriverID(0, 0, 0, 0);
    }

    if(driver->mTakeDriveControlFlag && driver->mDriverID.IsValid() == false)
    {
        // Get the driver ID.
        Platform info;
        if(driver->GetPlatformInfo(info, driver->mJoystickSubsystemID))
        {
            Address::List drivers;
            // Depending on whether or not we are controlling a Primitive Driver or
            // Global Vector Driver, search for the right type
            if( driver->mVectorJoystickFlag == true)
            {
                drivers = info.GetConfiguration()->GetComponentsOfType((Byte)Service::GlobalVectorDriver);
            }
            else
            {
                drivers = info.GetConfiguration()->GetComponentsOfType((Byte)Service::PrimitiveDriver);
            }
            if(drivers.size() > 0)
            {
                driver->mDriverID = *drivers.begin();
            }            
        }
        if(driver->mDriverID.IsValid() == false && driver->mJoystickSubsystemID != 0)
        {
            static unsigned int queryTimeMs = 0;

            if(Time::GetUtcTimeMs() - queryTimeMs > 1000)
            {
                QueryConfiguration queryConfiguration;
                queryConfiguration.SetSourceID(driver->GetID());
                queryConfiguration.SetDestinationID(Address(driver->mJoystickSubsystemID,
                                                            255, 1, 1));
                queryConfiguration.SetQueryField((Byte)QueryConfiguration::Node);
                driver->Send(&queryConfiguration);

                QueryIdentification queryIdentification;
                queryIdentification.SetSourceID(driver->GetID());
                queryIdentification.SetDestinationID(Address(driver->mJoystickSubsystemID,
                                                             255, 1, 1));
                queryIdentification.SetQueryType((Byte)(QueryIdentification::Subsystem));
                driver->Send(&queryIdentification);
                queryTimeMs = Time::GetUtcTimeMs();
            }
        }
    }

    if(driver->mTakeCameraControlFlag && driver->mVisualSensorID.IsValid() == false)
    {
        // Get the driver ID.
        Platform info;
        if(driver->GetPlatformInfo(info, driver->mJoystickSubsystemID))
        {
            Address::List camera;
            camera = info.GetConfiguration()->GetComponentsOfType(Service::VisualSensor);
            if(camera.size() > 0)
            {
                driver->mVisualSensorID = *camera.begin();
            }
        }
    }

    if(Time::GetUtcTimeMs() - driver->mControlCheckTimeMs > 1000)
    {
        if(driver->mDriverID.IsValid() && 
            driver->mTakeDriveControlFlag && 
            !driver->HaveComponentControl(driver->mDriverID))
        {

            if(driver->RequestComponentControl(driver->mDriverID, 100))
            {
                driver->SendResumeCommand(driver->mDriverID);
            }

        }
        if(driver->mVisualSensorID.IsValid() && 
            driver->mTakeCameraControlFlag && 
            !driver->HaveComponentControl(driver->mVisualSensorID))
        {

            if(driver->RequestComponentControl(driver->mVisualSensorID, 100))
            {
                driver->SendResumeCommand(driver->mVisualSensorID);
            }
        }
        driver->mControlCheckTimeMs = Time::GetUtcTimeMs();
    }

    
    if(driver->mDriverID.IsValid() && driver->HaveComponentControl(driver->mDriverID))
    {
        if(driver->mVectorJoystickFlag == true)
        {
            driver->mGlobalVector.SetSourceID(driver->GetID());
            driver->mGlobalVector.SetDestinationID(driver->mDriverID);
            driver->Send(&driver->mGlobalVector);
        }
        else
        {
            driver->mWrenchEffort.SetSourceID(driver->GetID());
            driver->mWrenchEffort.SetDestinationID(driver->mDriverID);
            // If auto braking is enabled, turn on resistive efforts as needed.
            if(driver->mAutoBrakingFlag == true)
            {
                // Lookup the Primitive Driver service for the component
                // we are driving
                UShort pv = driver->mWrenchEffort.GetPresenceVector();
                Configuration::Component config = *driver->GetPlatformInfo(driver->mDriverID.mSubsystem).GetConfiguration()->GetComponent(driver->mDriverID);
                if(config.mpServices)
                {
                    Service::Set::iterator service;
                    for(service = config.mpServices->begin();
                        service != config.mpServices->end();
                        service++)
                    {
                        if(service->GetType() == Service::PrimitiveDriver)
                        {
                            // We found the service, now check to see if braking is supported and
                            // if so, enable braking as needed.

                            // Check to see if we are setting a propulsive effort, and if it is 0, and 
                            // we the component supports a countering resistive effort, set it to 100%
                            if( BitVector::IsBitSet(pv, SetWrenchEffort::VectorBit::PropulsiveLinearEffortX) &&
                                service->IsInputSupported(JAUS_SET_WRENCH_EFFORT, SetWrenchEffort::VectorMask::ResistiveLinearEffortX) )
                            {             
                                if(fabs(driver->mWrenchEffort.GetPropulsiveLinearEffortX()) <= 2)
                                {
                                    driver->mWrenchEffort.SetResistiveLinearEffortX(100); 
                                }
                                else
                                {
                                    driver->mWrenchEffort.SetResistiveLinearEffortX(0);
                                }
                            }

                            break;
                        }
                        
                    }
                }
                else
                {
                    QueryServices queryServices;
                    queryServices.SetSourceID(driver->GetID());
                    queryServices.SetDestinationID(driver->mDriverID);
                    driver->Send(&queryServices);
                }
            }
            driver->Send(&driver->mWrenchEffort);
            
        }
    }

    if(driver->mVisualSensorID.IsValid() && driver->HaveComponentControl(driver->mVisualSensorID))
    {
        driver->mCameraPose.SetSourceID(driver->GetID());
        driver->mCameraPose.SetDestinationID(driver->mVisualSensorID);
        driver->Send(&driver->mCameraPose);
    }

    driver->mJoystickMutex.Leave();
}

/*  End of File */
