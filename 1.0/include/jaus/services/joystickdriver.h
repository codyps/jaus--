////////////////////////////////////////////////////////////////////////////////////
///
///  \file joystickdriver.h
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
#ifndef __JAUS_JOYSTICK_DRIVER__H
#define __JAUS_JOYSTICK_DRIVER__H

#include "jaus/services/srvclibdll.h"
#include "jaus/messages/command/platform/setwrencheffort.h"
#include "jaus/messages/command/environment/setcamerapose.h"
#include "jaus/components/commandcomponent.h"
#include <cxutils/joystick.h>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class JoystickDriver
    ///   \brief Interface for using a joystick to control a Primitive Driver.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL JoystickDriver : public CommandComponent
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Enumeration of different wrench efforts the joystick data can
        ///          be mapped to.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum WrenchEffort
        {
            PropulsiveLinearEffortX = 0,
            PropulsiveLinearEffortY,
            PropulsiveLinearEffortZ,
            PropulsiveRotationalEffortX,
            PropulsiveRotationalEffortY,
            PropulsiveRotationalEffortZ,
            ResistiveLinearEffortX,
            ResistiveLinearEffortY,
            ResistiveLinearEffortZ,
            ResistiveRotationalEffortX,
            ResistiveRotationalEffortY,
            ResistiveRotationalEffortZ,
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Enumeration of different camera efforts the joystick data can
        ///          be mapped to.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum CameraWrench
        {
            XDisplacementOrLinearRate = 0,
            YDisplacementOrLinearRate,
            ZDisplacementOrLinearRate,
            XAngleOrAngularRotationRate,
            YAngleOrAngularRotationRate,
            ZAngleOrAngularRotationRate,
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \brief Enumeration of possible button mappings.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum ButtonActions
        {
            RequestDriveControl = 0, ///<  Request drive control.
            ReleaseDriveControl,     ///<  Release drive control.
            RequestCameraControl,    ///<  Request control of camera.
            ReleaseCameraControl,    ///<  Release control of camera.
            ResetCameraPose          ///<  Resets the camera position to original values.
        };
        // Constructor.
        JoystickDriver();
        // Destructor.
        ~JoystickDriver();
        // Initialize the joystick interface.
        int InitializeJoystick(const unsigned int i = UINT_MAX);
        // Initialize joystick interface and configuration from XML file
        int InitializeJoystick(const std::string& settingsXML);
        // Shutdown the component.
        virtual int Shutdown();
         // If discovery is enabled, this function is called when an event happens.
        virtual int ProcessDiscoveryEvent(const Platform& subsystem,
                                          const SubscriberComponent::DiscoveryEvents eventType = SubsystemUpdate);
        // Set the subsystem ID of the platform to drive with joystick.
        int SetSubsystemToControl(const Byte sid);
        // Set the ID of a camera to control.
        int SetCameraID(const Address& sensor, const Byte id);
        // Map a joystick axes to a role.
        int MapAxisToWrench(const CxUtils::Joystick::Axes axis,
                            const WrenchEffort wrench,
                            const bool invertFlag = false,
                            const short int deadzone = 0);
        int ClearAxisToWrenchMap(const CxUtils::Joystick::Axes axis);
        // Map a joystick axes to a role.
        int MapAxisToCameraWrench(const CxUtils::Joystick::Axes axis,
                                  const CameraWrench wrench,
                                  const bool rate = true,
                                  const bool invertFlag = false,
                                  const short int deadzone = 0);
        int ClearAxisToCameraWrenchMap(const CxUtils::Joystick::Axes axis);
        // Map a joystick button to an action.
        int MapButtonToFunction(const int buttonNumber,
                                const ButtonActions function);
        // Clears the mapping of a button to an acction.
        int ClearButtonToFunctionMap(const int buttonNumber);
        // Invert a joystick axis.
        int InvertAxis(const CxUtils::Joystick::Axes axis,
                       const bool invertValue);
        // Take/Release control of subsystems driving component.
        int TakeDriveControl(const bool enable = true);
        // Take/Release control of a subsystems visual sensor pose.
        int TakeCameraControl(const bool enable = true);
        // Get ID of the subsystem being driven.
        Byte GetSubsystemID(const Byte sid) { return mJoystickSubsystemID; }
        // Get the ID of the camera to control (if set).
        Byte GetCameraID() const { return mCameraID; }
        // Get the mode indicator vector for camera pose. Bits 0-5 indicate Rate/Position 1/0 for Pose Axis.
        Byte GetCamerModeIndicator() const { return mCameraModeIndicator; }
        // Get the visual sensor ID of the camera being controlled.
        Address GetVisualSensorID() const { return mVisualSensorID; }
        // Get the ID of the Primitive Driver being controlled/detected.
        Address GetPrimitiveDriverID() const { return mDriverID; }
        // Print the wrench effort being generated by joystick data.
        void PrintWrenchEffort() const;
        // Print the camera wrench effort being generated by joystick data.
        void PrintCameraWrench() const;
    protected:
        static void JoystickCallback(const CxUtils::Joystick& joystick, void* args);
        Byte mJoystickSubsystemID;        ///<  ID of the subsystem to drive.
        Byte mCameraID;                   ///<  ID of the camera to control.
        Address mDriverID;                ///<  ID of primitive driver to control.
        Address mVisualSensorID;          ///<  ID of visual sensor to control.
        bool mTakeDriveControlFlag;       ///<  Take control of driver?
        bool mTakeCameraControlFlag;      ///<  Take control of camera.
        CxUtils::Joystick *mpJoystick;    ///<  Joystick interface.
        CxUtils::Mutex mJoystickMutex;    ///<  Mutex for thread protection.
        SetWrenchEffort mWrenchEffort;    ///<  Wrench effort to send to primitive driver.
        SetCameraPose mCameraPose;        ///<  Camera pose to set.
        int mButtonValues[32];            ///<  State of buttons (up/down).
        unsigned int mControlCheckTimeMs; ///<  Last time control was checked.
        std::map<CxUtils::Joystick::Axes,bool> mInvertFlags;                ///<  Invert joystick axis?
        std::map<CxUtils::Joystick::Axes,Short> mDeadZones;                 ///<  Joystick deadzone values.
        std::map<CxUtils::Joystick::Axes, WrenchEffort> mAxesMapping;       ///<  Mapping of joystick axis to driver axis.
        std::map<CxUtils::Joystick::Axes, CameraWrench> mCameraAxesMapping; ///<  Mapping of joystick axis to a camera pose axis.
        Byte mCameraModeIndicator;                                          ///<  Mode indicator for camera axis (rotation vs. displacement).
        std::map<int, ButtonActions> mButtonsMap;                           ///<  Mapping of button to action.
    };
}


#endif
/*  End of File */
