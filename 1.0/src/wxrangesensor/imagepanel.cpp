/*==========================================================================

    Filename: imagepanel.cpp

    Copyright (C) 2008  Brian C. Becker
                        Daniel Barber
                        Robotics Lab at UCF
                        http://robotics.ucf.edu

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Lesser General Public
    License as published by the Free Software Foundation; either
    version 2.1 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public
    License along with this library; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

===========================================================================*/
#include "wxvisualsensor/imagepanel.h"


// Define the new event type
DEFINE_EVENT_TYPE(wxEVT_COMMAND_IMAGEPANEL_MOUSE_EVENTS)

// Whatever this does...
IMPLEMENT_CLASS(wxImagePanelEvent, wxNotifyEvent)

// We want to handle painting, resizing, and mouse events
BEGIN_EVENT_TABLE(wxImagePanel, wxControl)
    EVT_PAINT(wxImagePanel::OnPaint)
    EVT_MOUSE_EVENTS(wxImagePanel::OnMouse)
    EVT_ERASE_BACKGROUND(wxImagePanel::OnBackgroundErase)
    EVT_SIZE(wxImagePanel::OnSize)
END_EVENT_TABLE()


inline void ground(long &a){if (a < 0) a = 0; }
inline void ground(long &a, int max){if (a > max) a = max; }

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the wxImagePanel but does not create it
///
//////////////////////////////////////////////////////////////////////////////
wxImagePanel::wxImagePanel()
{
    this->Init();
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Create a wxImagePanel with a border
///
///   Constructor that takes the standard wxWidget's parameters and a border
///   and creates the wxImagePanel. See Create for parameter details.
///
//////////////////////////////////////////////////////////////////////////////
wxImagePanel::wxImagePanel(wxWindow *parent, wxWindowID id, 
                           const wxPoint &pos /*= wxDefaultPosition*/, 
                           const wxSize &size /*= wxDefaultSize*/, 
                           const long style /*= 0*/)
{
    this->Init();
    this->Create(parent, id, pos, size, style);
}


//////////////////////////////////////////////////////////////////////////////
///
///   \brief Create a wxImagePanel to display an image
///
///   If you just want to display one wxImage, call this constructor. The size
///   of the wxImagePanel is automatically determined from the image you pass in.
///
///   \param parent Parent wxWindow
///   \param id ID for the control
///   \param image The wxImage to display on the control (may be destroyed)
///          after constructing the control
///   \param pos Location of the wxImagePanel
///   \return True on success, false on failure
///
//////////////////////////////////////////////////////////////////////////////
wxImagePanel::wxImagePanel(wxWindow *parent, wxWindowID id, 
                           const wxImage &image, 
                           const wxPoint &pos /*= wxDefaultPosition*/)
{
    this->Init();
    this->Create(parent, id, pos, wxSize(image.GetWidth(), image.GetHeight()));
    this->SetImage(image);
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes memory used by wxImagePanel
///
//////////////////////////////////////////////////////////////////////////////
wxImagePanel::~wxImagePanel()
{
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the wxImagePanel members and temporary data
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::Init()
{
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Creates the wxImagePanel
///
///   Creates the wxImagePanel with a border, size, and style. The border 
///   defaults to the background colour of the parent window. The size
///   is the "image viewing area" (either the size of the image or
///   a zoomed size (like half the image)). The real size of the control
///   is based on the image viewing area and the border. Look at the docs for 
///   the class to check out the available style options.
///
///   \param parent Parent wxWindow
///   \param id ID for the control
///   \param border Number of pixels to border the image
///   \param pos Location of the control
///   \param size Size of the image (not including the border)
///   \param style The style of the control (wxIP_CACHE_IMAGE/wxIP_WINDOWS_OPTIMIZE)
///   \return True on success, false on failure
///
//////////////////////////////////////////////////////////////////////////////
bool wxImagePanel::Create(wxWindow *parent, 
                          wxWindowID id, 
                          const wxPoint &pos /*= wxDefaultPosition*/, 
                          const wxSize &size /*= wxDefaultSize*/, 
                          const long style /*= 0*/)
{
    // Attempt to create the wxControl that wxImagePanel is based off of
    if (!wxControl::Create(parent, id, pos, size, wxNO_BORDER | style)) 
    {
        return false;
    }

    // Disable background erasing, we'll handle that on our own
    this->SetBackgroundColour(wxBG_STYLE_CUSTOM);

    // Set the border background to the colour of the parent's background
    this->SetBackgroundColour(parent->GetBackgroundColour());

    return true;
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Gets the new size of the control (say after setting the border)
///
//////////////////////////////////////////////////////////////////////////////
wxSize wxImagePanel::DoGetBestSize() const
{
    return wxSize(50, 50);
}


//////////////////////////////////////////////////////////////////////////////
///
///   \brief Handles resizing the image
///
///   If the sizing flag is true, this function does nothing (ie the control
///   is trying to set a new size and we don't want an infinite loop). If
///   the sizing flag is false (ie the user is trying to set a new size), we
///   want to resize the control again to accomodate the border. 
///
///   \param e wxSizeEvent containing the new size
///   \return True on success, false on failure
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::OnSize(wxSizeEvent &e)
{
    if(e.GetId()){}
    this->Refresh(false);
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets a wxImage and displays it
///
///   Displays a new wxImage on the screen. If the size is not exactly the
///   size of the control, the image will be automatically scaled (keeping
///   the correct aspect ratio). Don't use wxImages to display video, though.
///
///   \param img wxImage to display
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::SetImage(const wxImage *img)
{
    if (img && img->Ok())
    {
        wxSize clientSize = GetSize();
        int width = img->GetWidth();
        int height = img->GetHeight();
        double scale = 1.0;
        bool resize = false;
        if(width > clientSize.GetWidth())
        {
            resize = true;
            scale = clientSize.GetWidth()/(double)width;
            width = clientSize.GetWidth();
            height = (int)(height*scale);
        }
        if(height > clientSize.GetHeight())
        {
            resize = true;
            scale = clientSize.GetHeight()/(double)height;
            height = clientSize.GetHeight();
            width = (int)(width*scale);
        }
        mMutex.Lock();
        if(resize)
        {
            mBitmap = wxBitmap(img->Scale(width, height, wxIMAGE_QUALITY_HIGH));
        }
        else
        {
            mBitmap = wxBitmap(*img);
        }
        this->mUpdated = true;
        wxClientDC realDC(this);
        wxBufferedDC dc(&realDC, mBitmap);
        mMutex.Unlock();        
        this->Paint(&dc);
    }
}

#ifndef WX_DO_NOT_USE_OPENCV

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Set a ColorImage and display it
///
///   Displays a ColorImage with the border and automatic scaling. 
///
///   \param img ColorImage to display
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::SetImage(const IplImage * img)
{       
    int x, y, rows, cols;
    assert(img && 
           img->imageData &&
           img->depth == IPL_DEPTH_8U &&
           img->nChannels >= 1 && img->nChannels <= 3);
           
    //  If the image has an ROI (region of interest) use
    //  it instead of the whole image.
    x = (img->roi) ? img->roi->xOffset : 0;
    y = (img->roi) ? img->roi->yOffset : 0;
    rows = (img->roi) ? img->roi->height : img->height;
    cols = (img->roi) ? img->roi->width : img->width;
    
    
    wxImage newImage(cols, rows);
    //  Now copy the image data.
    unsigned char *bitsPtr = newImage.GetData();
    for(int i = y; i < y + rows; i++)
    {
        unsigned char *rowPtr = ((unsigned char *)(img->imageData + img->widthStep*i));
        
        for(int j = x; j < x + cols; j++)
        {
            //  Color data
            if(img->nChannels == 3)
            {
                int offset = j*3;
                *bitsPtr++ = rowPtr[offset + 2];
                *bitsPtr++ = rowPtr[offset + 1];
                *bitsPtr++ = rowPtr[offset];
            }
            //  Grayscale data
            else
            {
                *bitsPtr++ = rowPtr[j];
                *bitsPtr++ = rowPtr[j];
                *bitsPtr++ = rowPtr[j];
            }
        }
    }
    SetImage(newImage);
}

#endif


//////////////////////////////////////////////////////////////////////////////
///
///   \brief Paints the image on the screen
///
///   Paints the border and image on the screen, performing automatic 
///   aspect-preserving scaling and positioning
///
///   \param e wxPaintEvent sturf
///   \return True on success, false on failure
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::OnPaint(wxPaintEvent &e)
{
    if(e.GetId()) // Gets rid of warning in Windows.
    {
    }
    //Paint the image
    wxBufferedPaintDC dc(this);
    Paint(&dc);
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Paint the image on the screen
///
///   Paints the image on the screen (using wxWidget's way, which is slower)
///
///   \param dc The device context to draw on (should be buffered)
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::Paint(wxDC *dc)
{
    mMutex.Lock();
    if(mBitmap.Ok())
    {
        wxSize panelSize = GetSize();
        double xScale = 1.0, yScale = 1.0;
        xScale = (double)panelSize.x / mBitmap.GetWidth();
        yScale = (double)panelSize.y / mBitmap.GetHeight();
        //  Find the smallest scale value
        //  to scale evenly in both x and y directions
        if (xScale < yScale)
            yScale = xScale;
        else
            xScale = yScale;

        if (xScale > 1.0)
            xScale = 1.0;
        if (yScale > 1.0)
            yScale = 1.0;
    
        // Set the scale, draw the bitmap, and reset the scale
        dc->SetUserScale(xScale, yScale);
        dc->DrawBitmap(mBitmap,
                       (int)((panelSize.x - mBitmap.GetWidth()*xScale)/2.0),
                       (int)((panelSize.y - mBitmap.GetHeight()*yScale)/2.0),
                       mTransparent);
        dc->SetUserScale(1.0, 1.0);

        // Wait until something has been changed again (we don't need to update right now)
        mUpdated = false;
    }
    mMutex.Unlock();
}


//////////////////////////////////////////////////////////////////////////////
///
///   \brief Passess mouse events onto the parent window as an wxImagePanelEvent
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::OnMouse(wxMouseEvent &e)
{
    double xScale, yScale;
    wxImagePanelEvent cmdEvent(wxEVT_COMMAND_IMAGEPANEL_MOUSE_EVENTS, GetId());
    cmdEvent.SetEventObject(this);
    cmdEvent.SetMouseEvent(e);

    wxSize imgSize = this->GetSize();
    this->CalcScale(xScale, yScale);
    cmdEvent.SetRow((int)((e.GetY() - imgSize.GetWidth()) / yScale));
    cmdEvent.SetCol((int)((e.GetX() - imgSize.GetHeight()) / xScale));
    GetEventHandler()->ProcessEvent(cmdEvent);
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Calculates the scale to display the image, preserving the aspect
///          ratio
///
///   wxImagePanel will automatically rescale the image to display it, so 
///   so this function will automatically calculate the necessary X & Y
///   scale to do so. The aspect ratio is preserved.
///
///   \param xScale The X scale to use
///   \param yScale The Y scale to use
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::CalcScale(double &xScale, double &yScale)
{
    wxSize imgSize = this->GetSize();
    
    mMutex.Lock();
    if(mBitmap.Ok())
    {
        xScale = (double)imgSize.x / mBitmap.GetWidth();
        yScale = (double)imgSize.y / mBitmap.GetHeight();
    }
    else
    {
        xScale = yScale = 1.0;
    }
    mMutex.Unlock();
    //  Find the smallest scale value
    //  to scale evenly in both x and y directions
    if (xScale < yScale)
        yScale = xScale;
    else
        xScale = yScale;

    if (xScale > 1.0)
        xScale = 1.0;
    if (yScale > 1.0)
        yScale = 1.0;
}    

/*  End of File */
