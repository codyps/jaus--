////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodemanagerapp.cpp
///  \brief wxWidgets program for JAUS++ Library Node Manager.
///
///  <br>Author(s): Bo Sun
///  <br>Created: 15 January 2008
///  <br>Last Modified: 1 April 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)node
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: bsun@ist.ucf.edu
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
#include "wxnodemanager/nodemanagerframe.h"
#ifdef WIN32
#include <vld.h>
#endif

#ifdef _WIN32_WCE
#define TEXT_TYPE _T
#else
#define TEXT_TYPE wxT
#endif


////////////////////////////////////////////////////////////////////////////////////
///
///  \class NodeManagerApp
///  \breif Creates a JAUS++ Node Manager and its GUI.
///
////////////////////////////////////////////////////////////////////////////////////
class NodeManagerApp : public wxApp
{
public:
    virtual bool OnInit();
    Jaus::NodeManager* GetNodeManager() { return mpNode; }
private:
    Jaus::NodeManager* mpNode;      ///<  Main component that speaks "JAUS", a pointer is passed to mpFrame
    NodeManagerFrame* mpFrame;      ///<  Main wxFrame derivative that contain GUI components
};

DECLARE_APP(NodeManagerApp)

IMPLEMENT_APP(NodeManagerApp)


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Called when NodeManagerApp is initialized.
///
///  Creates and Initializes NodeManager and wxFrame.
///
////////////////////////////////////////////////////////////////////////////////////
bool NodeManagerApp::OnInit()
{
    mpNode = new Jaus::NodeManager();

    if(argc == 1)
    {
        if(mpNode->Initialize("settings/nodesettings.xml") == Jaus::FAILURE)
        {
            mpNode->Initialize(1, 1);
        }
    }
    else
    {
        wxString file = argv[1];
        mpNode->Initialize(file.ToAscii().data());
    }

#ifdef _WIN32_WCE
    mpFrame = new NodeManagerFrame(mpNode, TEXT_TYPE("JAUS Node Manager"));
#else
    mpFrame = new NodeManagerFrame(mpNode,
                                   this->GetTopWindow(),
                                   wxID_ANY, TEXT_TYPE("JAUS Node Manager"),
                                   wxDefaultPosition,
                                   wxSize(375,450),
                                   wxDEFAULT_FRAME_STYLE);
#endif
    mpNode = NULL;
    mpFrame->SetDisplay();
    mpFrame->Show(true);

    return true;
}

/*  End of File */
