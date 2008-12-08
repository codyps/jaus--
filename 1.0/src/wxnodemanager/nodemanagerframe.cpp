////////////////////////////////////////////////////////////////////////////////////
///
///  \file nodemanagerframe.cpp
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
#include "wxnodemanager/nodemanagerframe.h"
#include "jaus/components/node/nodeconnectionhandler.h"
#include <string>
#include <wx/image.h>
#include <wx/aboutdlg.h>

#define TIMER_ID    1000
#define ENABLE_BROADCAST_ID  1001
#define ENABLE_BROADCAST_UDP_ID 1002

#ifdef _WIN32_WCE
#define TEXT_TYPE _T
#else
#define TEXT_TYPE wxT
#endif


// specialized definese used in wxwidgets programming
BEGIN_EVENT_TABLE(NodeManagerFrame, wxFrame)
    EVT_TIMER(TIMER_ID, NodeManagerFrame::OnTimer)
    EVT_MENU(wxID_ABOUT, NodeManagerFrame::OnAbout)
    EVT_MENU(wxID_EXIT, NodeManagerFrame::OnQuit)
    EVT_MENU(wxID_NEW, NodeManagerFrame::OnNewID)
    EVT_MENU(wxID_OPEN, NodeManagerFrame::OnOpenXML)
    EVT_MENU(ENABLE_BROADCAST_ID, NodeManagerFrame::OnToggleBroadcast)
    EVT_MENU(ENABLE_BROADCAST_UDP_ID, NodeManagerFrame::OnToggleBroadcastUDP)
    EVT_BUTTON(wxID_PRINT, NodeManagerFrame::OnLog)
    EVT_BUTTON(wxID_REFRESH, NodeManagerFrame::OnRestart)
END_EVENT_TABLE()

#ifdef _WIN32_WCE
////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor of NodeManagerFrame
///
///  This initialization creates new instances of wx components and sets up initial
///     "look&feel" of the window.  Also, JAddresses in JAUS space are scanned and
///     displayed in the mpNodesTreeCtrl.  Special version for WINCE.
///
///  \param node Pointer to a JnodeManager instance
///              unique wxwindowID is created
///  \param title A string seen in the program's title
///
////////////////////////////////////////////////////////////////////////////////////
NodeManagerFrame::NodeManagerFrame(Jaus::JNodeManager* node, const wxString& title)
                :wxFrame( NULL, wxID_ANY, title)
#else
////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor of NodeManagerFrame
///
///  This initialization creates new instances of wx components and sets up initial
///     "look&feel" of the window.  Also, JAddresses in JAUS space are scanned and
///     displayed in the mpNodesTreeCtrl.
///
///  \param node Pointer to a JnodeManager instance
///  \param parent Pointer to the owner/parent of this NodeManagerFrame
///  \param id Required unique wxWindowID, if wxID_ANY is passed in, then a new
///            unique wxwindowID is created
///  \param title A string seen in the program's title
///  \param pos Location of the window upon initialization
///  \param size Size of the window upon initialization
///  \param style Type of visual style used in this window
///
////////////////////////////////////////////////////////////////////////////////////
NodeManagerFrame::NodeManagerFrame(Jaus::NodeManager* node,
                                   wxWindow* parent,
                                   wxWindowID id,
                                   const wxString& title,
                                   const wxPoint& pos,
                                   const wxSize& size,
                                   long style) : wxFrame( parent, id, title, pos, size, style )
#endif
{
    if(node->IsInitialized() == false)
    {
        if(node->GetLastJausError() == Jaus::ErrorCodes::AddressConflict)
        {
            int subsysNew, nodeNew;

            wxNumberEntryDialog subsysDialog(this, TEXT_TYPE("Select Subsystem ID"), TEXT_TYPE("Subsystem:"), TEXT_TYPE("Select ID"), 1, 1, 254);
            if(subsysDialog.ShowModal() == wxID_OK)
            {
                subsysNew = (int) subsysDialog.GetValue();

                wxNumberEntryDialog nodeDialog(this, TEXT_TYPE("Select Node ID"), TEXT_TYPE("Node:"), TEXT_TYPE("Select ID"), 1, 1, 254);

                if(nodeDialog.ShowModal() == wxID_OK)
                {
                    nodeNew = (int) nodeDialog.GetValue();

                    node->Initialize(subsysNew, nodeNew);
                }
            }
        }

        if(node->IsInitialized() == false)
        {
            wxMessageDialog quitDialog(parent, wxString(node->GetLastJausErrorString(), wxConvUTF8), TEXT_TYPE("Failure to Initialize Node"), wxOK);
            quitDialog.ShowModal();
            delete node;
            node = NULL;
            Close(true);
        }
    }

    mConfigurationEventTimeMs = 0;

    mpTimer = new wxTimer(this, TIMER_ID);
    mpNode = node;

	mpMainFrameBoxSizer = new wxBoxSizer( wxVERTICAL );

    mpNotebook = new wxNotebook( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, 0 );
	mpNetworkPanel = new wxPanel( mpNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mpNodesBoxSizer = new wxBoxSizer( wxVERTICAL );
    mpNodesTreeCtrl = new wxTreeCtrl( mpNetworkPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
    mpNodesBoxSizer->Add( mpNodesTreeCtrl, 1, wxALL|wxEXPAND, 5 );

	mpNetworkPanel->SetSizer( mpNodesBoxSizer );
	mpNetworkPanel->Layout();
	mpNodesBoxSizer->Fit( mpNetworkPanel );
	mpNotebook->AddPage( mpNetworkPanel, TEXT_TYPE("Network"), true );
	mpOptionsPanel = new wxPanel( mpNotebook, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL );
	mpSubsystemBoxSizer = new wxBoxSizer( wxVERTICAL );

	mpSubsystemTreeCtrl = new wxTreeCtrl( mpOptionsPanel, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE );
	mpSubsystemBoxSizer->Add( mpSubsystemTreeCtrl, 1, wxALL|wxEXPAND, 5 );

	mpOptionsPanel->SetSizer( mpSubsystemBoxSizer );
	mpOptionsPanel->Layout();
	mpSubsystemBoxSizer->Fit( mpOptionsPanel );
	mpNotebook->AddPage( mpOptionsPanel, TEXT_TYPE("Bandwidth"), false );
	mpNotebook->RemovePage(1);

	mpMainFrameBoxSizer->Add( mpNotebook, 1, wxEXPAND | wxALL, 5 );

	mpButtonsBoxSizer = new wxBoxSizer( wxHORIZONTAL );
	mpLogButton = new wxButton( this, wxID_PRINT, TEXT_TYPE("Start Log"), wxDefaultPosition, wxDefaultSize, 0 );
	mpButtonsBoxSizer->Add( mpLogButton, 1, wxALL|wxALIGN_BOTTOM, 5 );

//disable logging in WINCE
#ifdef _WIN32_WCE
    mpLogButton->Enable(true);
#endif

	mpRestartButton = new wxButton( this, wxID_REFRESH, TEXT_TYPE("Restart"), wxDefaultPosition, wxDefaultSize, 0 );
	mpButtonsBoxSizer->Add( mpRestartButton, 1, wxALL|wxALIGN_BOTTOM, 5 );

	mpMainFrameBoxSizer->Add( mpButtonsBoxSizer, 0, wxEXPAND, 5 );

	this->SetSizer( mpMainFrameBoxSizer );
	this->Layout();

#if wxUSE_MENUS
    //setup menu & status bar
    mpFileMenu = new wxMenu;
    mpHelpMenu = new wxMenu;

    //menu options
    mpHelpMenu->Append(wxID_ABOUT, TEXT_TYPE("&About...\tF1"), TEXT_TYPE("Show about dialog"));
    mpFileMenu->Append(wxID_NEW, TEXT_TYPE("Change ID"), TEXT_TYPE("Change JAUS ID"));
    mpFileMenu->Append(wxID_OPEN, TEXT_TYPE("Load Settings"), TEXT_TYPE("Load XML Settings"));
    mpFileMenu->Append(ENABLE_BROADCAST_ID, TEXT_TYPE("&Heartbeat Options"), TEXT_TYPE("Heartbeat Options"));
    mpFileMenu->Append(ENABLE_BROADCAST_UDP_ID, TEXT_TYPE("&Broadcast Options"), TEXT_TYPE("UDP Broadcast Options"));
    mpFileMenu->Append(wxID_EXIT, TEXT_TYPE("E&xit\tAlt-X"), TEXT_TYPE("Quit this program"));

    mpMenuBar = new wxMenuBar();

    mpMenuBar->Append(mpFileMenu, TEXT_TYPE("&File"));
    mpMenuBar->Append(mpHelpMenu, TEXT_TYPE("&Help"));

    SetMenuBar(mpMenuBar);

#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    // create universal root for mpNodesTreeCtrl

    wxTreeItemId rootID = mpNodesTreeCtrl->AddRoot(wxT("Connections"), 0, 0);
    rootID = mpNodesTreeCtrl->GetRootItem();
    mpNodesTreeCtrl->AppendItem(rootID, wxString("Subsystem", wxConvUTF8), 1, 1);
    mpNodesTreeCtrl->AppendItem(rootID, wxString("Node List", wxConvUTF8), 1, 1);
    mpNodesTreeCtrl->AppendItem(rootID, wxString("Subsystem List", wxConvUTF8), 1, 1);
    mpNodesTreeCtrl->ExpandAll();
    mpNodesTreeCtrl->Refresh();

    wxImage imageIcon;
    wxInitAllImageHandlers();
    if(imageIcon.LoadFile(wxString("icons/network-server.png", wxConvUTF8)))
    {
        wxIcon icon;
        icon.CopyFromBitmap(imageIcon);
        SetIcon(icon);
    }

    mpTimer->Start(2);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
NodeManagerFrame::~NodeManagerFrame()
{
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    }

    if(mpNode)
    {
        mpNode->Shutdown();
        delete mpNode;
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when mpTimer activates
///
///  Updates the mpMap internally and mpNodesTreeCtrl visually.  Currently does not
///  use an additional thread to update but because of the light load, it does not
///  block GUI updating.
///
///  \param event Required wxTimerEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnTimer(wxTimerEvent& event)
{

    if(mpNode)
    {
        unsigned int connectionUpdateTimeMs = mpNode->GetConnectionEventTimeMs();
        Jaus::Configuration system;
        Jaus::Configuration::Subsystem::Map::iterator ss;
        Jaus::Address::List addresses;
        Jaus::Address::List::iterator id;
        Jaus::Address::Set addressSet;
        Jaus::Address::Set::iterator setID;
        Jaus::Identification identification = mpNode->GetSubsystemIdentification();;

        if( mConfigurationEventTimeMs != connectionUpdateTimeMs)
        {
            mConfigurationEventTimeMs = connectionUpdateTimeMs;
            system = mpNode->GetSystemConfiguration();

            wxTreeItemId rootID, configID, nodeListID, subListID;
            rootID = mpNodesTreeCtrl->GetRootItem();
            mpNodesTreeCtrl->DeleteChildren(rootID);
            if(!identification.GetIdentification().empty())
            {
                configID = mpNodesTreeCtrl->AppendItem(rootID,
                                                       wxString("Subsystem (", wxConvUTF8) +
                                                       wxString(identification.GetIdentification().c_str(), wxConvUTF8) +
                                                       wxString(")", wxConvUTF8),
                                                       1,
                                                       1);
            }
            else
            {
                configID = mpNodesTreeCtrl->AppendItem(rootID, wxString("Subsystem", wxConvUTF8), 1, 1);
            }
            nodeListID = mpNodesTreeCtrl->AppendItem(rootID, wxString("Node List", wxConvUTF8), 1, 1);
            subListID = mpNodesTreeCtrl->AppendItem(rootID, wxString("Subsystem List", wxConvUTF8), 1, 1);

            ss = system.mSubsystems.find(mpNode->GetNodeID().mSubsystem);

            if(ss != system.mSubsystems.end())
            {
                addresses = ss->second.GetAddresses();
                for( id = addresses.begin(); id != addresses.end(); id++)
                {
                    mpNodesTreeCtrl->AppendItem(configID, wxString(id->ToString().c_str(), wxConvUTF8), 1, 1);
                }
            }

            addressSet = ((Jaus::NodeConnectionHandler*)(mpNode->GetConnectionHandler()))->GetNodeConnections();
            for( setID = addressSet.begin(); setID != addressSet.end(); setID++)
            {
                mpNodesTreeCtrl->AppendItem(nodeListID, wxString(setID->ToString().c_str(), wxConvUTF8), 1, 1);
            }

            addressSet = mpNode->GetSubsystemList();
            for(setID = addressSet.begin(); setID != addressSet.end(); setID++)
            {
                mpNodesTreeCtrl->AppendItem(subListID, wxString(setID->ToString().c_str(), wxConvUTF8), 1, 1);
            }
            mpNodesTreeCtrl->Expand(configID);
            mpNodesTreeCtrl->Refresh();
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when about is clicked.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    wxString text;
    info.SetName(TEXT_TYPE("JAUS++ Node Manager"));
    info.SetVersion(TEXT_TYPE("1.0"));

    text.Append(TEXT_TYPE("This program is an implementation of a Node Manager described\n"));
    text.Append(TEXT_TYPE("by the Joint Architecture for Unmanned Systems (JAUS)."));
    info.SetDescription(text);
    text.Clear();

    info.AddDeveloper(TEXT_TYPE("Daniel Barber"));
    info.AddDeveloper(TEXT_TYPE("Bo Sun"));

    text.Append(TEXT_TYPE("Copyright (c) 2008\n"));
    text.Append(TEXT_TYPE("Applied Cognition and Training in Immersive Virtual Environments Laboratory\n"));
    text.Append(TEXT_TYPE("Institute for Simulation and Training (IST)\n"));
    text.Append(TEXT_TYPE("Univeristy of Central Florida"));
    info.SetCopyright(text);
    text.Clear();

    info.SetWebSite(TEXT_TYPE("http://active.ist.ucf.edu"));

    //text.Append(TEXT_TYPE("GNU LESSER GENERAL PUBLIC LICENSE\n"));
    //text.Append(TEXT_TYPE("This library is free software; you can redistribute it and/or\n"));
    //text.Append(TEXT_TYPE("modify it under the terms of the GNU Lesser General Public\n"));
    //text.Append(TEXT_TYPE("License as published by the Free Software Foundation; either\n"));
    //text.Append(TEXT_TYPE("version 2.1 of the License, or (at your option) any later version.\n"));
    //text.Append(TEXT_TYPE("\n"));
    //text.Append(TEXT_TYPE("This library is distributed in the hope that it will be useful,\n"));
    //text.Append(TEXT_TYPE("but WITHOUT ANY WARRANTY; without even the implied warranty of\n"));
    //text.Append(TEXT_TYPE("MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU\n"));
    //text.Append(TEXT_TYPE("Lesser General Public License for more details.\n"));
    //text.Append(TEXT_TYPE("\n"));
    //text.Append(TEXT_TYPE("You should have received a copy of the GNU Lesser General Public\n"));
    //text.Append(TEXT_TYPE("License along with this library; if not, write to the Free Software\n"));
    //text.Append(TEXT_TYPE("Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA\n"));
    //info.SetLicence(text);

    wxImage logo;
    if(logo.LoadFile(wxString("logo/jaus++_logo_100x100.gif", wxConvUTF8)))
    {
        wxIcon icon;
        icon.CopyFromBitmap(logo);
        info.SetIcon(icon);
    }
    wxAboutBox(info);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when user exits.
///
///  Closes any open logs and frees heap memory used.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnQuit(wxCommandEvent& event)
{
    wxMessageDialog quitDialog(NULL, TEXT_TYPE("Please Confirm Quit."), TEXT_TYPE("Exit from wxNodeManager?"), wxNO_DEFAULT|wxYES_NO|wxICON_QUESTION);

    switch (quitDialog.ShowModal())
    {
    case wxID_YES:
        if(mpNode != NULL)
        {
            if(mpNode->IsLogOpen())
            {
                mpNode->CloseLogFile();
            }

            delete mpNode;
            delete mpTimer;
            mpNode = NULL;
            mpTimer = NULL;
        }

        Sleep(100);
        Close(true);
        break;
    case wxID_NO:
        break;
    }
    mConfigurationEventTimeMs = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Toggles heartbeat pulse broadcasting to/from 255.255.1.1.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnToggleBroadcast(wxCommandEvent& event)
{
    wxMessageDialog broadcastDialog(NULL,
                                    TEXT_TYPE("Send Heartbeat Pulse Messages to 255.255.1.1?"),
                                    TEXT_TYPE("Heartbeat Options"),
                                    wxNO_DEFAULT|wxYES_NO|wxICON_QUESTION);

    switch (broadcastDialog.ShowModal())
    {
    case wxID_YES:
        if(mpNode != NULL)
        {
            mpNode->EnableSubsystemConfigDiscovery(true);
        }
        break;
    case wxID_NO:
        mpNode->EnableSubsystemConfigDiscovery(false);
        break;
    }
    mConfigurationEventTimeMs = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Toggles heartbeat pulse UDP broadcasting/multicasting.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnToggleBroadcastUDP(wxCommandEvent& event)
{
    wxMessageDialog broadcastDialog(NULL,
                                    TEXT_TYPE("Enable UDP Broadcasting?"),
                                    TEXT_TYPE("UDP Broadcast Options"),
                                    wxNO_DEFAULT|wxYES_NO|wxICON_QUESTION);

    switch (broadcastDialog.ShowModal())
    {
    case wxID_YES:
        if(mpNode != NULL)
        {
            mpNode->ToggleBroadcastUDP(true);
        }
        break;
    case wxID_NO:
        mpNode->ToggleBroadcastUDP(true);
        break;
    }
    mConfigurationEventTimeMs = 0;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when "Load Settings" is clicked.
///
///  Closes current mpNode and re-initializes with XML settings file.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnOpenXML(wxCommandEvent& event)
{
    wxString caption = TEXT_TYPE("Choose Node Manager XML Settings file");
    wxString wildcard = TEXT_TYPE("XML files (*.xml)|*.xml");
    wxString defaultDir = TEXT_TYPE(".\\");

    wxString defaultFile = TEXT_TYPE("nodesettings.xml");

#ifdef _WIN32_WCE

    wxFileDialog fileDialog(this, caption, defaultDir, defaultFile, wildcard, wxOPEN);

    //At http://www.lpthe.jussieu.fr/~zeitlin/wxWindows/docs/wxwin_wxmswport.html,
    //it was advised that we use wxGenericFileDialog in WINCE, however there are problems with
    //timestamp access violations that don't seem to go away.
    //Currently, the wxFileDialog under WINCE will only read files from
    //"My Documents" folder and subfolders.

    //wxGenericFileDialog fileDialog(this, caption, defaultDir, defaultFile, wildcard);
#else
    wxFileDialog fileDialog(this, caption, defaultDir, defaultFile, wildcard, wxOPEN);
#endif
    if(fileDialog.ShowModal() == wxID_OK)
    {
         wxString path = fileDialog.GetPath();
         mpNode->Shutdown();
         mpNode->Initialize(path.ToAscii().data());

         SetDisplay();
    }

    if(mpNode->IsLogOpen() && mpLogButton)
    {
        mpLogButton->SetLabel(TEXT_TYPE("End Log"));
    }
    else
    {
        mpLogButton->SetLabel(TEXT_TYPE("Start Log"));
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when "Open Address" is clicked.
///
///     Closes current mpNode and re-initializes with inputted address.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnNewID(wxCommandEvent& event)
{
    int subsys = mpNode->GetNodeID().mSubsystem;
    int node = mpNode->GetNodeID().mNode;
    int subsysNew, nodeNew;

    wxNumberEntryDialog subsysDialog(this, TEXT_TYPE("Change Subsystem"), TEXT_TYPE("Subsystem:"), TEXT_TYPE("Change ID"), subsys, 1, 254);
        if(subsysDialog.ShowModal() == wxID_OK)
    {
        subsysNew = (int) subsysDialog.GetValue();

        wxNumberEntryDialog nodeDialog(this, TEXT_TYPE("Change Node"), TEXT_TYPE("Node:"), TEXT_TYPE("Change ID"), node, 1, 254);

        if(nodeDialog.ShowModal() == wxID_OK)
        {
            nodeNew = (int) nodeDialog.GetValue();

            mpNode->Shutdown();
            mpNode->Initialize(subsysNew, nodeNew);

            SetDisplay();
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when mpLogButton is clicked.
///
///  Creates and saves UDP action to a log.  Toggles CreateLogFile and CloseLogFile.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnLog(wxCommandEvent & event)
{
    if(!mpNode->IsLogOpen())
    {
        mpLogButton->SetLabel(TEXT_TYPE("End Log"));
        char buff[256];
        unsigned short day, hour, minute, second, msecond;
        CxUtils::GetSystemTime(day, hour, minute, second, msecond);

#ifdef _WIN32_WCE
        sprintf(buff,
                "node_log_%0.3d.%0.3d_%d.%d.%d.%d.%d.xml",
                mpNode->GetNodeID().mSubsystem, mpNode->GetNodeID().mNode, day, hour, minute, second, msecond);
#else
        sprintf(buff,
                "logs/node_log_%3d.%3d_%d.%d.%d.%d.%d.xml",
                mpNode->GetNodeID().mSubsystem, mpNode->GetNodeID().mNode, day, hour, minute, second, msecond);
#endif
        mpNode->CreateLogFile(buff);
    }
    else {
        mpNode->CloseLogFile();
        mpLogButton->SetLabel(TEXT_TYPE("Start Log"));
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when restart is clicked.
///
///  Clears the mpNodesTreeCtrl, it will be later filled automatically by the
///  mpTimer's callback function.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::OnRestart(wxCommandEvent & event)
{
    // On failure...
    if( mpNode->Restart() == Jaus::JAUS_FAILURE )
    {
        #if wxUSE_STATUSBAR
            SetStatusText(wxT("Failed to restart Node Manager."), 1);
        #endif

        wxTreeItemId rootId = mpNodesTreeCtrl->GetRootItem();
        mpNodesTreeCtrl->DeleteChildren(rootId);

    }
    //if mpNode successfully initilized
    else
    {
        std::string nodeID = "ID " + mpNode->GetNodeID().ToString();
        #if wxUSE_STATUSBAR
            SetStatusText(wxString(nodeID.c_str(), wxConvUTF8), 1);
        #endif
        wxTreeItemId rootId = mpNodesTreeCtrl->GetRootItem();
        mpNodesTreeCtrl->DeleteChildren(rootId);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets StatusBar (second element) and mTreeCtrl, used when
///         mpNode is (re)initialized.
///
////////////////////////////////////////////////////////////////////////////////////
void NodeManagerFrame::SetDisplay()
{

    if(mpNode && mpNode->IsInitialized())
    {
        std::string nodeID = "ID " + mpNode->GetNodeID().ToString();
        #if wxUSE_STATUSBAR
            SetStatusText(wxString(nodeID.c_str(), wxConvUTF8), 1);
        #endif
    }
    else
    {
        #if wxUSE_STATUSBAR
            SetStatusText(wxT("Failed to initialize Node Manager."), 1);
        #endif
    }

    wxTreeItemId rootId = mpNodesTreeCtrl->GetRootItem();
    mpNodesTreeCtrl->DeleteChildren(rootId);

}

/*  End of File */
