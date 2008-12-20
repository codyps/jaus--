////////////////////////////////////////////////////////////////////////////////////
///
///  \file rangesensorapp.cpp
///  \brief WxWidgets program for JAUS++ Library Range Sensor.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 18 December 2008
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
#include <vld.h>

#include "wxrangesensor/rangesensorframe.h"

////////////////////////////////////////////////////////////////////////////////////
///
///  \class RangeSensorApp
///  \brief Creates a JAUS++ Node Manager and its GUI.
///
////////////////////////////////////////////////////////////////////////////////////
class RangeSensorApp : public wxApp
{
public:
    virtual bool OnInit();
};

DECLARE_APP(RangeSensorApp)

IMPLEMENT_APP(RangeSensorApp)


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Called when RangeSensorApp is initialized.
///
///  Creates and Initializes a RangeSensorFrame object.
///
////////////////////////////////////////////////////////////////////////////////////
bool RangeSensorApp::OnInit()
{
    RangeSensorFrame* client = new RangeSensorFrame(NULL,
                                                    wxID_ANY,
                                                    wxT("JAUS++ Range Sensor"),
                                                    wxPoint(0,0),
                                                    wxSize(640, 640),
                                                    wxDEFAULT_FRAME_STYLE);
    
    if(client->Initialize())
    {
        client->Show();   
        if(this->argc > 1)
        {
            wxString argString(this->argv[1]);
            int s, n, c, i;
            if(sscanf(argString.ToAscii().data(), "%d.%d.%d.%d", &s, &n, &c, &i))
            {
                Jaus::Address id((Jaus::Byte)s, (Jaus::Byte)n, (Jaus::Byte)c, (Jaus::Byte)i);
                client->SetRangeSensorID(id);
            }
        }
        return true;
    }
    return false;
}

/*  End of File */
