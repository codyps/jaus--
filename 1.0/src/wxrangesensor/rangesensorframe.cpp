////////////////////////////////////////////////////////////////////////////////////
///
///  \file rangesensorframe.cpp
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
#include "wxrangesensor/rangesensorframe.h"
#include <jaus/messages/query/querymessages.h>
#include <jaus/messages/inform/informmessages.h>
#include <cxutils/math/cxmath.h>
#include <cxutils/timer.h>
#include <wx/aboutdlg.h>
#include <wx/numdlg.h>

using namespace CxUtils;

#define TIMER_ID 1000

#define DISCONNECT_ID 1001

// specialized definese used in wxwidgets programming
BEGIN_EVENT_TABLE(RangeSensorFrame, wxFrame)
    EVT_MENU(wxID_ABOUT, RangeSensorFrame::OnAbout)
    EVT_MENU(wxID_EXIT, RangeSensorFrame::OnQuit)
    EVT_MENU(DISCONNECT_ID, RangeSensorFrame::OnDisconnect)
    EVT_MENU(wxID_NEW, RangeSensorFrame::OnSelectRangeSensor)
    EVT_TIMER(TIMER_ID, RangeSensorFrame::OnTimer)
END_EVENT_TABLE()

#define wxMENU_BAR_OFFSET 15


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes the window.
///
////////////////////////////////////////////////////////////////////////////////////
RangeSensorFrame::RangeSensorFrame(wxWindow* parent,
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
    mDataUpdateTimeMs = 0;
    mpTimer = NULL;
    mpFileMenu = NULL;
    mpHelpMenu = NULL;
    mpMenuBar = NULL;
    mpMainPanel = NULL;
    mpImagePanel = NULL;
    mFrameNumber = 0;
    mFrameUpdateTimeMs = 0;
    mPixelsPerMeter = 32;  // At 640x480 we'll display 8 meters out.
    mRefreshRate = 10;    // Update at rate in Hz.
    mpFileMenu = new wxMenu;
    mpFileMenu->Append(wxID_NEW, wxT("Select &Sensor\tAlt-S"), wxT("Select Range Sensor"));
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
                                    wxSize(360, 240));

    
    CreateStatusBar(2);
    
    mpTimer = new wxTimer(this, TIMER_ID);

    // Load the default image.
    wxInitAllImageHandlers();
    mCurrentImage.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(mCurrentImage.GetWidth(), mCurrentImage.GetHeight()));
    mpImagePanel->SetImage(mCurrentImage);
    SetClientSize(mpImagePanel->GetBestSize());
    wxImage logo;
    if(logo.LoadFile(wxString("icons/camera-photo.png", wxConvUTF8)))
    {
        wxIcon icon;
        icon.CopyFromBitmap(logo);
        SetIcon(icon);
    }

    mpTimer->Start((unsigned int)(1000.0/mRefreshRate));
    mFrameUpdateTimeMs = CxUtils::Timer::GetTimeMs();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Initializes the component interface for getting video data.
///
////////////////////////////////////////////////////////////////////////////////////
bool RangeSensorFrame::Initialize()
{
    // Initialize JAUS Component.
    Jaus::Address nodeID;
    
    if(Jaus::Component::IsNodeManagerPresent(&nodeID) == false)
    {
        return false;
    }
    
    for(Jaus::Byte c = 2; c <= 20; c++)
    {
        for(Jaus::Byte i = 1; i < 255; i++)
        {
            if(mClient.Initialize("Range Sensor Client",
                                  Jaus::Address(nodeID.mSubsystem, nodeID.mNode, c, i)) == Jaus::JAUS_OK)
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
    mClient.RegisterCallback(Jaus::JAUS_REPORT_RELATIVE_OBJECT_POSITION, RangeSensorFrame::ProcessMessageCallback, this);
    mClient.RegisterCallback(Jaus::JAUS_REPORT_SICK_LIDAR, RangeSensorFrame::ProcessMessageCallback, this);
    
    return true;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
RangeSensorFrame::~RangeSensorFrame()
{
    mClient.Shutdown();
    mClient.RemoveCallback(Jaus::JAUS_REPORT_RELATIVE_OBJECT_POSITION);
    mShutdownFlag = true;
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    }
    Close(true);    
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Method called when a Close event is generated.
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::OnQuit(wxCommandEvent& event)
{
    mShutdownFlag = true;
    if(mpTimer)
    {
        delete mpTimer;
        mpTimer = NULL;
    }
    Close(true);
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Disconnects from range sensor.
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::OnDisconnect(wxCommandEvent& event)
{
    Jaus::Address id;
    mMutex.Enter();
    id = mRangeSensorID;
    mRangeSensorID(0, 0, 0, 0);

    // Load the default image.
    wxImage image;
    image.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(image.GetWidth(), image.GetHeight()));
    mpImagePanel->SetImage(image);
    SetClientSize(mpImagePanel->GetBestSize());
    mFrameNumber = 0;
    mMutex.Leave();

    mClient.CancelEvents(id);
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when about is clicked.
///
///  \param event Required wxCommandEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::OnAbout(wxCommandEvent& event)
{
    wxAboutDialogInfo info;
    wxString text;
    info.SetName(TEXT_TYPE("JAUS++ Range Sensor"));
    info.SetVersion(TEXT_TYPE("1.0"));

    text.Append(TEXT_TYPE("This program demonstrates a Range Sensor using JAUS."));
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
///  \brief When the select range sensor menu option is selected this method
///         asks the user for JAUS ID information and then attempts to create
///         a video subscription.
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::OnSelectRangeSensor(wxCommandEvent& event)
{
    Jaus::Address id;
    mMutex.Enter();
    id = mRangeSensorID;
    mRangeSensorID(0, 0, 0, 0);

    // Load the default image.
    mCurrentImage.LoadFile(wxT("images/please_stand_by.png"));
    mpImagePanel->SetClientSize(wxSize(mCurrentImage.GetWidth(), mCurrentImage.GetHeight()));
    mpImagePanel->SetImage(mCurrentImage);
    SetClientSize(mpImagePanel->GetBestSize());
    mFrameNumber = 0;
    mMutex.Leave();

    // Cancel previous events.
    if(id.IsValid())
    {
        mClient.CancelEvents(mRangeSensorID);
    }

    id(0, 0, 0, 0);

    wxNumberEntryDialog subsystemDialog(this, 
                                        wxT("Enter Subsystem ID"), 
                                        wxT("Subsystem ID:"), 
                                        wxT("Select Range Sensor"), 
                                        mClient.GetID().mSubsystem, 1, 254);
    if(subsystemDialog.ShowModal() == wxID_OK)
    {
        id.mSubsystem = (Jaus::Byte)subsystemDialog.GetValue();
        wxNumberEntryDialog nodeDialog(this, 
                                       wxT("Enter Node ID"), 
                                       wxT("Node ID:"), 
                                       wxT("Select Range Sensor"), 
                                       mClient.GetID().mNode, 1, 254);
        if(nodeDialog.ShowModal() == wxID_OK)
        {
            id.mNode = (Jaus::Byte)nodeDialog.GetValue();
            wxNumberEntryDialog instanceDialog(this, 
                                               wxT("Enter Instance ID"), 
                                               wxT("Instance ID:"), 
                                               wxT("Select Range Sensor"), 
                                               1, 1, 254);
            if(instanceDialog.ShowModal() == wxID_OK)
            {
                id.mInstance = (Jaus::Byte)instanceDialog.GetValue();
                id.mComponent = (Jaus::Byte)Jaus::Service::RangeSensor;
                mMutex.Enter();
                mRangeSensorID = id;
                mMutex.Leave();
                return;
            }
        }
    }
    wxMessageBox(wxT("Failed to Connect To Range Sensor"));
}



////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function called when mpTimer activates
///
///  \param event Required wxTimerEvent parameter
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::OnTimer(wxTimerEvent& event)
{
    if(mClient.IsInitialized() == false)
    {
        Initialize();
        return;
    }

    mMutex.Enter();

    if(mRangeSensorID.IsValid())
    {
        if(!(mClient.HaveEventSubscription(mRangeSensorID, Jaus::JAUS_REPORT_RELATIVE_OBJECT_POSITION) ||
             mClient.HaveEventSubscription(mRangeSensorID, Jaus::JAUS_REPORT_SICK_LIDAR)) ||
            Jaus::Time::GetUtcTimeMs() - mDataUpdateTimeMs > 2000)
        {
            Jaus::QueryServices queryServices;
            Jaus::Receipt receipt;
            queryServices.SetSourceID(mClient.GetID());
            queryServices.SetDestinationID(mRangeSensorID);
            if(mClient.Send(&queryServices, receipt))
            {
                const Jaus::ReportServices* reportServices = dynamic_cast<const Jaus::ReportServices*>(receipt.GetResponseMessage());
                bool usesSick = false;
                Jaus::Service::Set::const_iterator srvc;
                for(srvc = reportServices->GetServices()->begin();
                    srvc != reportServices->GetServices()->end();
                    srvc++)
                {
                    if(srvc->IsOutputMessage(Jaus::JAUS_REPORT_SICK_LIDAR))
                    {
                        usesSick = true;
                        break;
                    }
                }
                Jaus::CreateEventRequest createEvent;
                createEvent.SetSourceID(mClient.GetID());
                createEvent.SetDestinationID(mRangeSensorID);
                createEvent.SetEventType(Jaus::CreateEventRequest::EveryChange);
                if(usesSick)
                {
                    // Try get LIDAR data if available, otherwise use relative object position
                    createEvent.SetMessageCode(Jaus::JAUS_REPORT_SICK_LIDAR);
                }
                else
                {
                    createEvent.SetMessageCode(Jaus::JAUS_REPORT_RELATIVE_OBJECT_POSITION);
                }
                mClient.RequestEvent(createEvent, NULL, 250, 1);
            }
        }
        
    }

    if(mRangeSensorID.IsValid())
    {
        // Draw data to image.        
        wxBitmap bitmap(640, 640, -1);
        wxMemoryDC memoryDC;
        memoryDC.SelectObject(bitmap);
        // Set line color to green, fill color to green
        memoryDC.SetPen(wxPen(*wxGREEN, 2, wxSOLID));
        memoryDC.SetBrush(wxBrush(*wxGREEN, wxSOLID));

        if(mLidarScan.GetScanData()->size() > 0)
        {
            // Range data 0 is on the right, goes to left.
            Jaus::ReportSickLidar::Scan::iterator obj;
            double bearing = 0;
            double increment = 1;
            double divider = 10.0; // centimeter.
            if(mLidarScan.GetUnits() == Jaus::ReportSickLidar::Millimeter)
            {
                divider = 100.0;
            }
            switch(mLidarScan.GetScanType())
            {
            case Jaus::ReportSickLidar::OneHundredDegreesOneRes:
                bearing = 50;
                increment = 1.0;
                break;
            case Jaus::ReportSickLidar::OneHundredDegreesHalfRes:
                bearing = 50;
                increment = 0.5;
                break;
            case Jaus::ReportSickLidar::OneHundredDegreesQuarterRes:
                bearing = 50;
                increment = 0.25;
                break;
            case Jaus::ReportSickLidar::OneEightyDegreesOneRes:
                bearing = 90;
                increment = 1.0;
                break;
            default:
                bearing = 90;
                increment = 0.5;
                break;
            };
            for(obj = mLidarScan.GetScanData()->begin();
                obj != mLidarScan.GetScanData()->end();
                obj++)
            {
                // Calculate x, y position.
                wxPoint point;
                double distanceMetersX, distanceMetersY;
                double distancePixelsX, distancePixelsY;
                distanceMetersX = cos(-CxUtils::CxToRadians(bearing) + CX_HALF_PI)*(*obj)/divider;
                distanceMetersY = sin(-CxUtils::CxToRadians(bearing) + CX_HALF_PI)*(*obj)/divider;

                distancePixelsX = distanceMetersX*mPixelsPerMeter;
                distancePixelsY = distanceMetersY*mPixelsPerMeter;

                // Draw from center of image.
                point.y = (int)(mCurrentImage.GetHeight()/2.0 - distancePixelsY);
                point.x = (int)(mCurrentImage.GetWidth()/2.0 + distancePixelsX);

                // Draw a circle.
                memoryDC.DrawCircle(point, 2);
                bearing -= increment;
            }
        }
        else
        {
            std::vector<Jaus::ReportRelativeObjectPosition>::iterator obj;
            for(obj = mRangeData.begin();
                obj != mRangeData.end();
                obj++)
            {
                if(Jaus::BitVector::IsBitSet(obj->GetPresenceVector(), Jaus::ReportRelativeObjectPosition::VectorBit::Range) &&
                    Jaus::BitVector::IsBitSet(obj->GetPresenceVector(), Jaus::ReportRelativeObjectPosition::VectorBit::Bearing))
                {
                    // Calculate x, y position.
                    wxPoint point;
                    double distanceMetersX, distanceMetersY;
                    double distancePixelsX, distancePixelsY;
                    distanceMetersX = cos(-obj->GetBearing() + CX_HALF_PI)*obj->GetRange();
                    distanceMetersY = sin(-obj->GetBearing() + CX_HALF_PI)*obj->GetRange();

                    distancePixelsX = distanceMetersX*mPixelsPerMeter;
                    distancePixelsY = distanceMetersY*mPixelsPerMeter;

                    // Draw from center of image.
                    point.y = (int)(mCurrentImage.GetHeight()/2.0 - distancePixelsY);
                    point.x = (int)(mCurrentImage.GetWidth()/2.0 + distancePixelsX);

                    // Draw a circle.
                    memoryDC.DrawCircle(point, 2);
                }
            }            

            // Delete old data.
            obj = mRangeData.begin();
            while(obj != mRangeData.end())
            {
                if(mFrameUpdateTimeMs - obj->GetTimeStamp().ToMilliseconds() >= 1000.0/mRefreshRate)
                {
                    obj = mRangeData.erase(obj);
                }
                else
                {
                    break;
                }
            }
        }
        
        char buffer[256];
        if(mFrameNumber > 0)
        {
            sprintf(buffer, "Range Sensor Client - %d", mFrameNumber);
        }
        else
        {
            sprintf(buffer, "Range Sensor Client", mFrameNumber);
        }
        SetTitle(wxString(buffer, wxConvUTF8));

        // Tidy up
        memoryDC.SelectObject(wxNullBitmap);
        memoryDC.SelectObject(wxNullBitmap);
        mCurrentImage = bitmap.ConvertToImage();
        mpImagePanel->SetClientSize(wxSize(mCurrentImage.GetWidth(), mCurrentImage.GetHeight()));
        mpImagePanel->SetImage(mCurrentImage);
        SetClientSize(mpImagePanel->GetBestSize());
    }
    

    mMutex.Leave();
}  


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Function callback used to get Report Relative Object Position
///  messages.
///
///  \param message Message to process.
///  \param args Pointer to RangeSensorFrame for callback.
///
////////////////////////////////////////////////////////////////////////////////////
void RangeSensorFrame::ProcessMessageCallback(const Jaus::Message* message, void* args)
{
    RangeSensorFrame* frame = (RangeSensorFrame*)(args);
    if(message->GetCommandCode() == Jaus::JAUS_REPORT_RELATIVE_OBJECT_POSITION)
    {
        frame->mMutex.Enter();
        frame->mRangeData.push_back(*((Jaus::ReportRelativeObjectPosition*)(message)));
        frame->mDataUpdateTimeMs = Jaus::Time::GetUtcTimeMs();
        frame->mFrameNumber++;
        frame->mFrameUpdateTimeMs = Jaus::Time::GetUtcTimeMs();
        frame->mMutex.Leave();

    }
    else if(message->GetCommandCode() == Jaus::JAUS_REPORT_SICK_LIDAR)
    {
        frame->mMutex.Enter();
        frame->mLidarScan = *((Jaus::ReportSickLidar*)(message));
        frame->mDataUpdateTimeMs = Jaus::Time::GetUtcTimeMs();
        frame->mFrameNumber++;
        frame->mFrameUpdateTimeMs = Jaus::Time::GetUtcTimeMs();
        frame->mMutex.Leave();
    }
}

/*  End of File */
