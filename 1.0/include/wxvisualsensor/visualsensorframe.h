////////////////////////////////////////////////////////////////////////////////////
///
///  \file visualsensorframe.h
///  \brief GUI Window used for a Visual Sensor JAUS component.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 24 April 2008
///  <br>Last Modified: 11 July 2008
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
#ifndef __WX_JAUS_VIDEO_CLIENT_FRAME__H
#define __WX_JAUS_VIDEO_CLIENT_FRAME__H

#include <cv/cvimagecapture.h>

#include <wx/wx.h>
#include <wx/timer.h>
#include "imagepanel.h"
#include <cxutils/thread.h>
#include <jaus/components/cmplib.h>
#include <jaus/video/videolib.h>
#define TEXT_TYPE wxT

////////////////////////////////////////////////////////////////////////////////////
///
///  \class VisualSensorFrame
///  \brief wxFrame for showing a video data received by a JAUS Component.
///
////////////////////////////////////////////////////////////////////////////////////
class VisualSensorFrame : public wxFrame
{
public:
    VisualSensorFrame(wxWindow* parent, 
                     wxWindowID id, 
                     const wxString& title, 
                     const wxPoint& pos, 
                     const wxSize& size, 
                     long style);
    ~VisualSensorFrame();
    bool Initialize(const std::string& settings);
    void OnQuit(wxCommandEvent& event);
    void OnShowImage(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnOpenXML(wxCommandEvent& event);
    void UpdateImage(const CvImageCapture::Image &image);
    void OnTimer(wxTimerEvent& event);
private:
    static void VideoCaptureThread(void* args);
    Jaus::UShort mMaxWidth;
    Jaus::UShort mMaxHeight;
    CvImageCapture* mVisualSource;  ///<  Visual data source.
    unsigned short mSourceWidth;    ///<  Desired capture width.
    unsigned short mSourceHeight;   ///<  Desired capture height.
    bool mInterlacedVideoFlag;      ///<  Is the video data interlaced?
    volatile bool mDisplayImageFlag;///<  If true image data is updated on screen.
    DECLARE_EVENT_TABLE()
    wxMenu* mpFileMenu;         ///<  "File" category of the top menu
    wxMenu* mpHelpMenu;         ///<  "Help category of the top menu
    wxMenuBar* mpMenuBar;       ///<  Top menu bar
    wxTimer* mpTimer;           ///<  Periodic timer used re-draw image data.
    wxPanel* mpMainPanel;       ///<  Pointer to main panel for window.
    wxImagePanel* mpImagePanel; ///<  Panel for viewing image.
    CxUtils::Mutex mMutex;              ///<  Mutex for thread protection.
    Jaus::Image::Format mImageFormat;   ///<  Format of image data for decompression.
    Jaus::Image mCurrentImage;          ///<  Curent image received from the sensor.
    Jaus::Address mVisualSensorID;      ///<  ID of the visual sensor we are subscribing to.
    Jaus::VisualSensor mSensor;         ///<  JAUS Visual Sensor component.
    CvImageCapture::Image mImage;       ///<  Main image from sensor.
    CxUtils::Thread mCaptureThread;     ///<  Thread for capturing video data.
    std::string mSourceName;            ///<  Video source name.
    wxImage mImageCopy;                 ///<  Copy of image data for display.
};

#endif
/*  End of File */
