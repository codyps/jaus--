////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodemanagerframe.h
///  \brief Contains a wxFrame for a Node Manager GUI for JAUS.
///
///  <br>Author(s): Bo Sun, Daniel Barber
///  <br>Created: 30 March 2007
///  <br>Last Modified: 7 April 2008
///  <br>Copyright (c) 2007
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: bsun@ist.ucf.edu, dbarber@ist.ucf.edu
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
#ifndef __WX_JAUS_NODE_MANAGER_FRAME__H
#define __WX_JAUS_NODE_MANAGER_FRAME__H

#include <wx/wx.h>
#include <wx/numdlg.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/cmdline.h>
#include <wx/button.h>
#include <wx/image.h>
#include <wx/timer.h>

#include <jaus/components/node/nodemanager.h>


////////////////////////////////////////////////////////////////////////////////////
///
///  \class NodeManagerFrame
///  \breif wxFrame for showing Node Manager information.
///
////////////////////////////////////////////////////////////////////////////////////
class NodeManagerFrame : public wxFrame
{
public:
#ifdef _WIN32_WCE
    NodeManagerFrame(Jaus::JNodeManager* node, const wxString& title);
#else
    NodeManagerFrame(Jaus::NodeManager* node, 
                     wxWindow* parent, 
                     wxWindowID id, 
                     const wxString& title, 
                     const wxPoint& pos, 
                     const wxSize& size, 
                     long style);
#endif
    ~NodeManagerFrame();
    void OnNewID(wxCommandEvent& event);
    void OnToggleBroadcast(wxCommandEvent& event);
    void OnToggleBroadcastUDP(wxCommandEvent& event);
    void OnOpenXML(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
    void OnLog(wxCommandEvent & event);
    void OnRestart(wxCommandEvent & event);
    void OnTimer(wxTimerEvent& event);
    void SetDisplay();
private:
    DECLARE_EVENT_TABLE()

    wxBoxSizer* mpMainFrameBoxSizer;                             ///<  Outer-most vertical divider
    wxNotebook* mpNotebook;                                      ///<  Tabbing control
    wxPanel* mpNetworkPanel;                                     ///<  Panel/Pane for Nodes Tab
    wxBoxSizer* mpNodesBoxSizer;                                 ///<  Divider for Nodes Tab
    wxTreeCtrl* mpNodesTreeCtrl;                                 ///<  Tree-display control
    wxPanel* mpOptionsPanel;                                     ///<  Panel/Pane for Options Tab
    wxBoxSizer* mpSubsystemBoxSizer;                             ///<  Scheduled to be removed
    wxTreeCtrl* mpSubsystemTreeCtrl;                             ///<  Scheduled to be removed
    wxBoxSizer* mpButtonsBoxSizer;                               ///<  Divider for bottom buttons
    wxButton* mpLogButton;                                       ///<  Button used to toggle Logging
    wxButton* mpRestartButton;                                   ///<  Button to restart the JNodeManager mpNode
    wxMenu* mpFileMenu;                                          ///<  "File" category of the top menu
    wxMenu* mpHelpMenu;                                          ///<  "Help category of the top menu
    wxMenuBar* mpMenuBar;                                        ///<  Top menu bar
    wxTimer* mpTimer;                                            ///<  Periodic timer used to update JNodeManager mpNode
    std::map<Jaus::UInt, wxTreeItemId> mTreeItemIdMap;           ///<  Map to keep track of dynamic wxTreeCtrl children
    Jaus::NodeManager* mpNode;                                   ///<  Node manager.
    unsigned int mConfigurationEventTimeMs;                      ///<  Time of last configuration event.
};

#endif
/*  End of File */
