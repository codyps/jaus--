////////////////////////////////////////////////////////////////////////////////////
///
///  \file visualsensorframe.cpp
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
#include "wxvisualsensor/visualsensorframe.h"
#include <jaus/messages/query/querymessages.h>
#include <jaus/messages/inform/informmessages.h>
#include <wx/image.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>
#include <tinyxml.h>

#define TIMER_ID 1000
#define TOGGLE_VIDEO_ID 1001

// specialized definese used in wxwidgets programming
BEGIN_EVENT_TABLE(VisualSensorFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, VisualSensorFrame::OnAbout)
    EVT_MENU(wxID_EXIT, VisualSensorFrame::OnQuit)
    EVT_MENU(TOGGLE_VIDEO_ID, VisualSensorFrame::OnShowImage)
    EVT_MENU(wxID_OPEN, VisualSensorFrame::OnOpenXML)
END_EVENT_TABLE()

#define wxMENU_BAR_OFFSET 15


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor, initializes the window.
///
////////////////////////////////////////////////////////////////////////////////////
VisualSensorFrame::VisualSensorFrame(wxWindow* parent,
                                     wxWindowID id,
                                     const wxString& title,
                                     const wxPoint& pos,
                                     const wxSize& size,
                                     long style) : wxFrame(parent, 
                                                           id,
                                                           title,
                                                           pos,
                                                           size,
                                                           style)
{
    mDisplayImageFlag = true;
    mpTimer = NULL;
    mpFileMenu = NULL;
    mpHelpMenu = NULL;
    mpMenuBar = NULL;
    mpMainPanel = NULL;
    mpImagePanel = NULL;
    
    mpFileMenu = new wxMenu;
    mpFileMenu->Append(wxID_OPEN, wxT("&Load Settings\tAlt-L"), wxT("Load Sensor Settings"));
    mpFileMenu->Append(wxID_ABOUT, wxT("&About Client\tAtl-A"), wxT("About Video Client"));
    mpFileMenu->Append(TOGGLE_VIDEO_ID, wxT("&Toggle Video\tAtl-T"), wxT("Toggle Video Display"));
    mpFileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Exit Program"));
    
    mpMenuBar = new wxMenuBar();
    mpMenuBar->Append(mpFileMenu, wxT("&File"));

    SetMenuBar(mpMenuBar);

    mpMainPanel = new wxPanel(this, wxID_ANY, pos, size);

    // Create the video panel.
    mpImagePanel = new wxImagePanel(mpMainPanel, 
                                    wxID_ANY, 0, 
                                    wxPoint(0, 0), 
                                    size/*, 
                                    wxIP_WINDOWS_OPTIMIZE*/);

    
    CreateStatusBar(2);
    mSourceWidth = mSourceHeight = 0;
    mInterlacedVideoFlag = false;

    // Load the default image.
    wxImage image;
    wxInitAllImageHandlers();
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
    mpImagePanel->SetImage(image);
    SetClientSize(mpImagePanel->GetBestSize());
    mVisualSource = new CvImageCapture();
    SetStatusText(wxT("Not Initialized"), 1);

    wxImage logo;
    if(logo.LoadFile(wxString("icons/camera-photo.png", wxConvUTF8)))
    {
        wxIcon icon;
        icon.CopyFromBitmap(logo);
        SetIcon(icon);
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Takes an XML file name, loads the file and initializes the Visual 
///         Sensor component and connects to video source.
///
////////////////////////////////////////////////////////////////////////////////////
bool VisualSensorFrame::Initialize(const std::string& settings)
{
    // Initialize JAUS Component.
    Jaus::Address nodeID;

    wxImage image;
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
    mpImagePanel->SetImage(image);
    SetClientSize(mpImagePanel->GetBestSize());
    
    if(Jaus::Component::IsNodeManagerPresent(&nodeID) == false)
    {
        wxMessageBox(wxT("No Node Manager Running"), wxT("Failed to Initialize"));
        return false;
    }
    
    TiXmlDocument xml;
    if(xml.LoadFile(settings.c_str()) == 0)
    {
        wxMessageBox(wxT("Unable to Load XML File"), wxT("Failed to Initialize"));
        return false;
    }

    Jaus::Byte instanceID = 1;
    std::string sourceName;
    unsigned int messageBoxSize = 2097152;
    
    TiXmlHandle docHandle(&xml);

    TiXmlNode* node = docHandle.FirstChild("Jaus").FirstChild("VisualSensorComponent").FirstChild("InstanceID").ToNode();
    if(node == NULL)
    {
        wxMessageBox(wxT("Invalid XML File"), wxT("Failed to Initialize"));
        return false;
    }
    instanceID = (Jaus::Byte)(atoi(node->FirstChild()->Value()));


    node = docHandle.FirstChild("Jaus").FirstChild("VisualSensorComponent").FirstChild("Source").ToNode();
    if(node == NULL)
    {
        wxMessageBox(wxT("Invalid XML File"), wxT("Failed to Initialize"));
        return false;
    }
    sourceName = node->FirstChild()->Value();

    node = docHandle.FirstChild("Jaus").FirstChild("VisualSensorComponent").FirstChild("MessageBoxSize").ToNode();
    if(node == NULL)
    {
        wxMessageBox(wxT("Invalid XML File"), wxT("Failed to Initialize"));
        return false;
    }
    messageBoxSize = (unsigned int)(atoi(node->FirstChild()->Value()));

    TiXmlElement* element = docHandle.FirstChild("Jaus").FirstChild("VisualSensorComponent").FirstChild("ScaledResolution").ToElement();
    if(element && element->Attribute("maxWidth") && element->Attribute("maxHeight"))
    {
        mMaxWidth = (Jaus::UShort)atoi(element->Attribute("maxWidth"));
        mMaxHeight = (Jaus::UShort)atoi(element->Attribute("maxHeight"));
    }
    else
    {
        mMaxWidth = mMaxHeight = 0;
    }

    element = docHandle.FirstChild("Jaus").FirstChild("VisualSensorComponent").FirstChild("Settings").ToElement();
    if(element && element->Attribute("width") && element->Attribute("height") && element->Attribute("interlaced"))
    {
        mSourceWidth = (unsigned short)atoi(element->Attribute("width"));
        mSourceHeight = (unsigned short)atoi(element->Attribute("height"));
        std::string value = element->Attribute("interlaced");
        if(value == "true" || value == "yes" || value == "True")
        {
            mInterlacedVideoFlag = true;
        }
        else
            mInterlacedVideoFlag = false;
        {
        }
        if(element->Attribute("show"))
        {
            value = element->Attribute("show");
            if(value == "true" || value == "yes" || value == "True")
            {
                mDisplayImageFlag = true;
            }
            else
            {
                mDisplayImageFlag = false;
            }
        }
    }

    mSensor.Initialize(nodeID.mSubsystem, nodeID.mNode, instanceID, messageBoxSize);
    
    if(!mSensor.IsInitialized())
    {
        wxMessageBox(wxT("Could not Initialize JAUS Visual Sensor Component"), wxT("Failed to Initialize"));
        return false;
    }
    
    SetStatusText(wxString(mSensor.GetID().ToString().c_str(), wxConvUTF8), 1);
    mVisualSource->Start(sourceName.c_str(), mSourceWidth, mSourceHeight, mInterlacedVideoFlag);
    // Start thread to capture video data.
    mCaptureThread.CreateThread(&VisualSensorFrame::VideoCaptureThread, this);
    CxUtils::SleepMs(100);

    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
VisualSensorFrame::~VisualSensorFrame()
{
    mSensor.Shutdown();
    mCaptureThread.StopThread();
    if(mVisualSource)
    {
        delete mVisualSource;
    }
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    } 
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method called when an OnQuit event is generated.
///
////////////////////////////////////////////////////////////////////////////////////
void VisualSensorFrame::OnQuit(wxCommandEvent& event)
{
    mSensor.Shutdown();
    CxUtils::SleepMs(1000);
    mCaptureThread.StopThread();
    mVisualSource->Stop();
    Jaus::MessageCreator::CleanupMessageCreator();
    Close(true);
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method called when an On Display Image is selected.
///
////////////////////////////////////////////////////////////////////////////////////
void VisualSensorFrame::OnShowImage(wxCommandEvent& event)
{
    wxMessageDialog quitDialog(NULL, TEXT_TYPE("Show Image Data"), TEXT_TYPE("Display Video?"), wxNO_DEFAULT|wxYES_NO|wxICON_QUESTION);

    switch (quitDialog.ShowModal())
    {
    case wxID_YES:
        mDisplayImageFlag = true;
        break;
    case wxID_NO:
        {
            mDisplayImageFlag = false;
            CxUtils::SleepMs(250);
            wxImage image;
            image.LoadFile(wxT("images/please_stand_by.png"));
            mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
            mpImagePanel->SetImage(image);
            SetClientSize(mpImagePanel->GetBestSize());
        }
        break;
    default:
        break;
    }
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when about is clicked.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void VisualSensorFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    wxString text;
    info.SetName(TEXT_TYPE("JAUS++ Visual Sensor Client"));
    info.SetVersion(TEXT_TYPE("1.0"));

    text.Append(TEXT_TYPE("This program is a JAUS Visual Sensor Component and\n"));
    text.Append(TEXT_TYPE("which produces video data for subscribing components."));
    info.SetDescription(text);
    text.Clear();

    info.AddDeveloper(TEXT_TYPE("Daniel Barber"));

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
///  \brief Updates the image on the screen when a new image is available.
///
////////////////////////////////////////////////////////////////////////////////////
void VisualSensorFrame::UpdateImage(const CvImageCapture::Image& image)
{
    if(true == mDisplayImageFlag)
    {
        mpImagePanel->SetImage(wxImage(image.mWidth, image.mHeight, image.mpData, true));
        mpImagePanel->SetClientSize(image.mWidth, image.mHeight);
        SetClientSize(mpImagePanel->GetBestSize());
    }
    
    // Update JAUS stuff later.
    if(mSensor.IsInitialized())
    {
        mSensor.SetCurrentFrame(image.mpData, image.mWidth, image.mHeight, 3, false, mMaxWidth, mMaxHeight);
        char buffer[256];
        sprintf(buffer, "JAUS++ Visual Sensor - %d", mSensor.GetFrameNumber());
        SetTitle(wxString(buffer, wxConvUTF8));
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Thread function which captures from the video source and then
///         updates the Visual Sensor Component values.
///
////////////////////////////////////////////////////////////////////////////////////
void VisualSensorFrame::VideoCaptureThread(void* args)
{
    VisualSensorFrame* sensor = (VisualSensorFrame*)args;
    
    while(sensor && sensor->mCaptureThread.QuitThreadFlag() == false)
    {
        if(sensor->mVisualSource->GetFrame(&sensor->mImage, true))
        {
            
            unsigned char* ptr = sensor->mImage.mpData;
            unsigned char temp;
            for(unsigned int i = 0; i < (unsigned int)((unsigned int)(sensor->mImage.mHeight)*(unsigned int)(sensor->mImage.mWidth)*3); i+= 3)
            {
                temp = *ptr;
                *ptr = *(ptr + 2);
                *(ptr + 2) = temp;
                ptr += 3;
            }
            
            sensor->UpdateImage(sensor->mImage);           
        }
        CxUtils::SleepMs(1);
    }
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
void VisualSensorFrame::OnOpenXML(wxCommandEvent& event)
{
    wxString caption = wxT("Choose Visual Sensor XML Settings file");
    wxString wildcard = wxT("XML files (*.xml)|*.xml");
    wxString defaultDir = wxT(".\\");
    wxString defaultFile = wxT("");
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
        int filterIndex = fileDialog.GetFilterIndex();
        
        mSensor.Shutdown();
        CxUtils::SleepMs(500);
        // Shutdown the capture thread.
        mCaptureThread.StopThread(); 

        wxImage image;
        image.LoadFile(wxT("images/please_stand_by.png"));
        mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
        mpImagePanel->SetImage(image);
        SetClientSize(mpImagePanel->DoGetBestSize());
              
        CxUtils::SleepMs(500);
        this->Initialize(path.ToAscii().data());
    }
}

/*  End of File */
