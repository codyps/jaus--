////////////////////////////////////////////////////////////////////////////////////
///
///  \file videoclientframe.cpp
///  \brief This file containts a wxWidget frame for displaying video data
///  received by a client using JAUS.
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
#include "wxvisualsensor/videoclientframe.h"
#include <jaus/messages/query/querymessages.h>
#include <jaus/messages/inform/informmessages.h>
#include <wx/image.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>

#define TIMER_ID 1000
#define DISCONNECT_ID 1001

// specialized definese used in wxwidgets programming
BEGIN_EVENT_TABLE(VideoClientFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, VideoClientFrame::OnAbout)
    EVT_MENU(wxID_NEW, VideoClientFrame::OnSelectVisualSensor)
    EVT_MENU(wxID_EXIT, VideoClientFrame::OnQuit)
    EVT_MENU(DISCONNECT_ID, VideoClientFrame::OnDisconnect)
    EVT_TIMER(TIMER_ID, VideoClientFrame::OnTimer)
END_EVENT_TABLE()

#define wxMENU_BAR_OFFSET 15


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes the window.
///
////////////////////////////////////////////////////////////////////////////////////
VideoClientFrame::VideoClientFrame(wxWindow* parent,
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
    mShutdownFlag = false;
    mpTimer = NULL;
    mpFileMenu = NULL;
    mpHelpMenu = NULL;
    mpMenuBar = NULL;
    mpMainPanel = NULL;
    mpImagePanel = NULL;
    mFrameNumber = 0;
    mFrameUpdateTimeMs = 0;

    mpFileMenu = new wxMenu;
    mpFileMenu->Append(wxID_NEW, wxT("Select &Sensor\tAlt-S"), wxT("Select Visual Sensor"));
    mpFileMenu->Append(DISCONNECT_ID, wxT("&Disconnect\tAtl-A"), wxT("Disconnect from Sensor"));
    mpFileMenu->Append(wxID_ABOUT, wxT("&About Client\tAtl-A"), wxT("About Video Client"));    
    mpFileMenu->Append(wxID_EXIT, wxT("E&xit\tAlt-X"), wxT("Exit Program"));
    
    mpMenuBar = new wxMenuBar();
    mpMenuBar->Append(mpFileMenu, wxT("&File"));

    SetMenuBar(mpMenuBar);

    mpMainPanel = new wxPanel(this, wxID_ANY, pos, size);

    // Create the video panel.
    mpImagePanel = new wxImagePanel(mpMainPanel, 
                                    wxID_ANY, 
                                    wxPoint(0, 0), 
                                    wxSize(360, 240), 
                                    0);

    
    CreateStatusBar(2);
    
    mpTimer = new wxTimer(this, TIMER_ID);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes the component interface for getting video data.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoClientFrame::Initialize()
{
    // Load the default image.
    wxImage image;
    wxInitAllImageHandlers();
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));    
    SetClientSize(mpImagePanel->GetBestSize());
    mpImagePanel->SetImage(image);
    
    wxImage logo;
    if(logo.LoadFile(wxString("icons/camera-photo.png", wxConvUTF8)))
    {
        wxIcon icon;
        icon.CopyFromBitmap(logo);
        SetIcon(icon);
    }
    
    // Initialize JAUS Component.
    Jaus::Address nodeID;
    
    if(Jaus::Component::IsNodeManagerPresent(&nodeID) == false)
    {
        wxMessageBox(wxT("No Node Manager Running"), wxT("Failed to Initialize"));
        return false;
    }
    
    for(Jaus::Byte c = 2; c <= 20; c++)
    {
        for(Jaus::Byte i = 1; i < 255; i++)
        {
            if(mClient.Initialize("Visual Sensor Client",
                                  Jaus::Address(nodeID.mSubsystem, nodeID.mNode, c, i),
                                  Jaus::JAUS_VISUAL_SENSOR_MESSAGE_BOX_SIZE*2) == Jaus::JAUS_OK)
            {
                break;
            }
        }
        if(mClient.IsInitialized())
            break;
    }

    if(!mClient.IsInitialized())
    {
        wxMessageBox(wxT("Could not Initialize JAUS Component"), wxT("Failed to Initialize"));
        return false;
    }
    
    std::string id = "ID " + mClient.GetID().ToString();
    SetStatusText(wxString(id.c_str(), wxConvUTF8), 1);
    mClient.RegisterCallback(Jaus::JAUS_REPORT_IMAGE, VideoClientFrame::ProcessMessageCallback, this);
    
    mpTimer->Start(10);

    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
VideoClientFrame::~VideoClientFrame()
{
    mShutdownFlag = true;
    mClient.Shutdown();
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    }
    Jaus::MessageCreator::CleanupMessageCreator();
    Close(true);
    
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method called when a Close event is generated.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::OnQuit(wxCommandEvent& event)
{
    mShutdownFlag = true;
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    }
    mClient.Shutdown();
    Close(true);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when about is clicked.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    wxString text;
    info.SetName(TEXT_TYPE("JAUS++ Visual Sensor Client"));
    info.SetVersion(TEXT_TYPE("1.0"));

    text.Append(TEXT_TYPE("This program connects to a JAUS Visual Sensor Component and\n"));
    text.Append(TEXT_TYPE("subscribes to Image Data using Events."));
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
///  \brief Disconnects from visual sensor.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::OnDisconnect(wxCommandEvent& event)
{
    Jaus::Address id;
    mMutex.Enter();
    id = mVisualSensorID;
    mVisualSensorID(0, 0, 0, 0);

    // Load the default image.
    wxImage image;
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
    SetClientSize(mpImagePanel->GetBestSize());
    mpImagePanel->SetImage(image);
    mFrameNumber = 0;
    mMutex.Leave();

    mClient.CancelEvents(id);
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief When the select visual sensor menu option is selected this method
///         asks the user for JAUS ID information and then attempts to create
///         a video subscription.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::OnSelectVisualSensor(wxCommandEvent& event)
{
    Jaus::Address id;
    mMutex.Enter();
    id = mVisualSensorID;
    mVisualSensorID(0, 0, 0, 0);

    // Load the default image.
    wxImage image;
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
    mpImagePanel->SetImage(image);
    SetClientSize(mpImagePanel->GetBestSize());
    mFrameNumber = 0;
    mMutex.Leave();

    // Cancel previous events.
    if(id.IsValid())
    {
        mClient.CancelEvents(mVisualSensorID);
    }

    id(0, 0, 0, 0);

    wxNumberEntryDialog subsystemDialog(this, 
                                        wxT("Enter Subsystem ID"), 
                                        wxT("Subsystem ID:"), 
                                        wxT("Select Visual Sensor"), 
                                        mClient.GetID().mSubsystem, 1, 254);
    if(subsystemDialog.ShowModal() == wxID_OK)
    {
        id.mSubsystem = (Jaus::Byte)subsystemDialog.GetValue();
        wxNumberEntryDialog nodeDialog(this, 
                                       wxT("Enter Node ID"), 
                                       wxT("Node ID:"), 
                                       wxT("Select Visual Sensor"), 
                                       mClient.GetID().mNode, 1, 254);
        if(nodeDialog.ShowModal() == wxID_OK)
        {
            id.mNode = (Jaus::Byte)nodeDialog.GetValue();
            wxNumberEntryDialog instanceDialog(this, 
                                               wxT("Enter Instance ID"), 
                                               wxT("Instance ID:"), 
                                               wxT("Select Visual Sensor"), 
                                               1, 1, 254);
            if(instanceDialog.ShowModal() == wxID_OK)
            {
                id.mInstance = (Jaus::Byte)instanceDialog.GetValue();
                id.mComponent = (Jaus::Byte)Jaus::Service::VisualSensor;
                mMutex.Enter();
                mVisualSensorID = id;
                mMutex.Leave();
                return;
            }
        }
    }
    wxMessageBox(wxT("Failed to Connect To Visual Sensor"));
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Sets visual sensor ID to use.
///
////////////////////////////////////////////////////////////////////////////////////
bool VideoClientFrame::SetVisualSensor(const Jaus::Address& id)
{
    if(id.IsValid() && !id.IsBroadcast())
    {
        mVisualSensorID  = id;
        return true;
    }
    return false;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Callback used to update display when a ReportImage message is
///         received.
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::ProcessMessageCallback(const Jaus::Message* message, void* args)
{
    const Jaus::ReportImage* report = dynamic_cast<const Jaus::ReportImage*>(message);
    VideoClientFrame* client = (VideoClientFrame*)args;
    if(!client->mShutdownFlag && 
        report && report->GetImageData() && 
        report->GetSourceID() == client->mVisualSensorID)
    {
        client->mMutex.Enter();
        client->mCurrentImage.Decompress(report->GetImageData(),
                                         report->GetDataSize(),
                                         client->mImageFormat);  
        client->mFrameNumber++;
        client->mFrameUpdateTimeMs = Jaus::Time::GetUtcTimeMs();        
        client->mMutex.Leave();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when mpTimer activates
///
///  This method checks to see if the video has been updating from a visual
///  sensor and if not, tries to re-acquire a subscription.
///
///  \param event Required wxTimerEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void VideoClientFrame::OnTimer(wxTimerEvent& event)
{
    mMutex.Enter();
    if(!mShutdownFlag &&
        mCurrentImage.Width() > 0 &&
        mCurrentImage.Height() > 0 &&
        mCurrentImage.ImageData() != NULL &&
        mVisualSensorID.IsValid() &&
        mFrameNumber > 0)
    {
        wxImage image(mCurrentImage.Width(),
                       mCurrentImage.Height(),
                       mCurrentImage.ImageData(),
                       true);
        if(image.Ok())
        {            
            wxSize imageSize(image.GetWidth(), image.GetHeight());
            mpImagePanel->SetClientSize(imageSize);
            SetClientSize(mpImagePanel->GetBestSize());    
            mpImagePanel->SetImage(&image);
        }               
    }
    
    mMutex.Leave();
    
    if(!mShutdownFlag)
    {
        char buffer[256];
        if(mFrameNumber > 0)
        {
            sprintf(buffer, "Video Client - %d", mFrameNumber);
        }
        else
        {
            sprintf(buffer, "Video Client", mFrameNumber);
        }
        SetTitle(wxString(buffer, wxConvUTF8));
    }   
    
    // If we haven't received an image in a while, and
    // have a valid visual sensor ID, try re-create subscription.
    unsigned int currentTimeMs = Jaus::Time::GetUtcTimeMs();
    if( mVisualSensorID.IsValid() &&
        currentTimeMs > mFrameUpdateTimeMs && currentTimeMs - mFrameUpdateTimeMs > 1500 )
    {
        if(mClient.HaveEventSubscription(mVisualSensorID, Jaus::JAUS_REPORT_IMAGE))
        {
            mClient.CancelEvents(mVisualSensorID);
        }

        // Try to re-create a subscription.
        Jaus::Receipt receipt;
        Jaus::QueryCameraFormatOptions queryFormats;
        Jaus::CreateEventRequest createEvent;
        Jaus::QueryCameraCount queryCameraCount;
        queryCameraCount.SetSourceID(mClient.GetID());
        queryCameraCount.SetDestinationID(mVisualSensorID);
        
        if(mClient.Send(&queryCameraCount, receipt, 0, 200))
        {
            Jaus::ReportCameraCount reportCameraCount = 
                *(dynamic_cast<const Jaus::ReportCameraCount*>(receipt.GetResponseMessage()));
            // First find out the type of image format used by sensor.
            queryFormats.SetSourceID(mClient.GetID());
            queryFormats.SetDestinationID(mVisualSensorID);
            queryFormats.SetPresenceVector(Jaus::QueryCameraFormatOptions::VectorMask::ImageFormat1);
            for(Jaus::Byte camID = 0; camID <= reportCameraCount.GetCameraCount(); camID++)
            {
                queryFormats.SetCameraID(camID);
                if(mClient.Send(&queryFormats, receipt, 0, 500, 1) == Jaus::JAUS_OK)
                {
                    const Jaus::ReportCameraFormatOptions* formats = 
                        dynamic_cast<const Jaus::ReportCameraFormatOptions*>(receipt.GetResponseMessage());
                    createEvent.SetDestinationID(mVisualSensorID);
                    createEvent.SetSourceID(mClient.GetID());
                    createEvent.SetMessageCode(Jaus::JAUS_REPORT_IMAGE);                
                    createEvent.SetEventType(Jaus::CreateEventRequest::EveryChange);
                    if(mClient.RequestEvent(createEvent, NULL, 200, 1) == Jaus::JAUS_OK)
                    {
                        mImageFormat = (Jaus::Image::Format)formats->GetImageFormat1();
                        mClient.RegisterCallback(Jaus::JAUS_REPORT_IMAGE, VideoClientFrame::ProcessMessageCallback, this);
                    }
                }
            }
        }
    }
}   

/*  End of File */
