////////////////////////////////////////////////////////////////////////////////////
///
///  \file videoclientapp.cpp
///  \brief WxWidgets program for JAUS++ Library Visual Sensor client
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 15 June 2008
///  <br>Last Modified: 15 June 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)node
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
#include <wx/app.h>
#include <wx/cmdline.h>

#include "wxvisualsensor/visualsensorframe.h"

////////////////////////////////////////////////////////////////////////////////////
///
///  \class VisualSensorApp
///  \brief Creates a JAUS++ Node Manager and its GUI.
///
////////////////////////////////////////////////////////////////////////////////////
class VisualSensorApp : public wxApp
{
public:
    virtual bool OnInit();
};

DECLARE_APP(VisualSensorApp)

IMPLEMENT_APP(VisualSensorApp)


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Called when VisualSensorApp is initialized.
///
///  Creates and Initializes a VisualSensorFrame object.
///
////////////////////////////////////////////////////////////////////////////////////
bool VisualSensorApp::OnInit()
{
    VisualSensorFrame* sensor = new VisualSensorFrame(NULL,
                                                      wxID_ANY,
                                                      wxT("JAUS++ Visual Sensor"),
                                                      wxPoint(0,0),
                                                      wxSize(640, 480),
                                                      wxDEFAULT_FRAME_STYLE);
    
    if(this->argc > 0)
    {
        sensor->Initialize(wxString(this->argv[1]).ToAscii().data());
    }
    sensor->Show();        
    return true;
}

/*  End of File */
