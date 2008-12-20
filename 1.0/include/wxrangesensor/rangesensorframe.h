////////////////////////////////////////////////////////////////////////////////////
///
///  \file rangesensorframe.h
///  \brief This file containts a wxWidget frame for range sensor data from
///  a JAUS Range Sensor.
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
#ifndef __WX_JAUS_RANGE_SENSOR_FRAME__H
#define __WX_JAUS_RANGE_SENSOR_FRAME__H

#include <wx/wx.h>
#include <wx/timer.h>
#include <wx/image.h>
#include "imagepanel.h"
#include <jaus/services/srvclib.h>
#include <jaus/messages/inform/environment/reportrelativeobjectposition.h>
#include <jaus/messages/experimental/sick/reportsicklidar.h>
#define TEXT_TYPE wxT

////////////////////////////////////////////////////////////////////////////////////
///
///  \class RangeSensorFrame
///  \brief wxFrame for showing range sensor data.
///
////////////////////////////////////////////////////////////////////////////////////
class RangeSensorFrame : public wxFrame
{
public:
    RangeSensorFrame(wxWindow* parent, 
                     wxWindowID id, 
                     const wxString& title, 
                     const wxPoint& pos, 
                     const wxSize& size, 
                     long style);
    ~RangeSensorFrame();
    bool Initialize();
    void SetRefreshRate(const double rate);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnDisconnect(wxCommandEvent& event);
    void OnSelectRangeSensor(wxCommandEvent& event);
    void SetRangeSensorID(const Jaus::Address& id) { mRangeSensorID = id; }
private:
    static void ProcessMessageCallback(const Jaus::Message* message, void* args);
    DECLARE_EVENT_TABLE()
    volatile bool mShutdownFlag;///<  Tells everything to shutdown.
    wxMenu* mpFileMenu;         ///<  "File" category of the top menu
    wxMenu* mpHelpMenu;         ///<  "Help category of the top menu
    wxMenuBar* mpMenuBar;       ///<  Top menu bar
    wxTimer* mpTimer;           ///<  Periodic timer used re-draw image data.
    wxPanel* mpMainPanel;       ///<  Pointer to main panel for window.
    wxImagePanel* mpImagePanel; ///<  Panel for viewing image.
    wxImage mCurrentImage;      ///<  Curent image data.
    unsigned int mFrameNumber;  ///<  Frame number.
    unsigned int mDataUpdateTimeMs;           ///<  The last time a message was received.
    double mFrameUpdateTimeMs;                ///<  Timestamp of current frame number.
    double mRefreshRate;                      ///<  How often to redraw the screen and delete old data.
    double mPixelsPerMeter;                   ///<  How many pixels is 1 meter equal to.
    std::vector<Jaus::ReportRelativeObjectPosition> mRangeData; ///<  Object data to draw.
    CxUtils::Mutex mMutex;                    ///<  Mutex for thread protection.
    Jaus::Address mRangeSensorID;             ///<  ID of the range sensor.
    Jaus::SubscriberComponent mClient;        ///<  JAUS Client component.
    Jaus::ReportSickLidar  mLidarScan;        ///<  LIDAR Scan data.
};

#endif
/*  End of File */
