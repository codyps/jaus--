/*==========================================================================

    Filename: imagepanel.cpp

    Copyright (C) 2006  Brian C. Becker
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
    EVT_ERASE_BACKGROUND(wxImagePanel::OnEraseBackground)
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
                           const int border /*= 0*/, 
                           const wxPoint &pos /*= wxDefaultPosition*/, 
                           const wxSize &size /*= wxDefaultSize*/, 
                           const long style /*= 0*/)
{
    this->Init();
    this->Create(parent, id, border, pos, size, style);
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
#ifdef WIN32
    this->Create(parent, id, 0, pos, wxSize(image.GetWidth(), image.GetHeight()), wxIP_CACHE_IMAGE & wxIP_WINDOWS_OPTIMIZE);
#else
    this->Create(parent, id, 0, pos, wxSize(image.GetWidth(), image.GetHeight()), wxIP_CACHE_IMAGE);
#endif
    this->SetImage(image);
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes memory used by wxImagePanel
///
//////////////////////////////////////////////////////////////////////////////
wxImagePanel::~wxImagePanel()
{
    if (this->mImage && (this->mType == IT_IPLIMAGE))
        delete this->mImage;

    if (this->mBitmap)
        delete this->mBitmap;

    if (this->mBuffer)
        delete this->mBuffer;

#if defined(WIN32)
    if (mBits)
        delete[] mBits;

    if (hbitmap)
    {
        DeleteObject(hbitmap);
        hbitmap = 0;
    }

    if (hbrush)
    {
        DeleteObject(hbrush);
        hbrush = 0;
    }

    if (hbrushBack)
    {
        DeleteObject(hbrushBack);
        hbrushBack = 0;
    }
#endif
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Initializes the wxImagePanel members and temporary data
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::Init()
{
    this->mImage = 0;
    this->mBitmap = 0;
    this->mType = IT_NONE;

    this->mTransparent = false;
    this->mSelected = false;
    this->mBorder = 0;
    this->mBuffer = 0;
    this->mUpdated = false;
    this->mSizing = false;
    this->verWidth = 0;
    this->horWidth = 0;
    
    // Windows optimization stuff
#if defined(WIN32)
    this->mBits = 0;
    this->mBitsRows = this->mBitsCols = 0;
    this->hbitmap = 0;
    this->hbrush = 0;
    this->hbrushBack = 0;
#endif
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
                          const int border /*= 0*/, 
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

    // Set the brder
    this->mBorder = border;

    // If we want a border, let's add it and resize the control
    if (mBorder > 0)
    {
        // This is the default background color for a selection
        this->SetForegroundColour(wxColour(49, 106, 197));
    }

    // Set the border background to the colour of the parent's background
    this->SetBackgroundColour(parent->GetBackgroundColour());

    // Only use the buffer if we are in non-optimized mode
    if (!(style & wxIP_WINDOWS_OPTIMIZE))
    {
        this->mBuffer = new wxBitmap(size.x, size.y);
    }

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
///   \brief Sets the border width in pixels
///
///   You can re-set the border of the control to a specified number of pixels
///   This automatically resizes the control to accommodate the new border
///
///   \param border Border in # of pixels
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::SetBorderWidth(const int border)
{
    if (border >= 0 && border != this->mBorder)
    {
        this->mBorder = border;
    }
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
    if (img)
    {
#ifdef WIN32
        if (this->GetWindowStyle() & wxIP_WINDOWS_OPTIMIZE)
        {
            if (mBits && (mBitsRows != img->GetHeight() || mBitsCols != img->GetWidth()))
            {
                delete[] mBits;
                mBits = 0;
                
                if (hbitmap)
                {
                    DeleteObject(hbitmap);
                    hbitmap = 0;
                }
            }
            //  Allocate new memory if needed.
            if(!mBits)
            {
                mBitsRows = img->GetHeight();
                mBitsCols = img->GetWidth();
                mBits = new unsigned char[mBitsRows*mBitsCols*4];
            }

            //  Now copy the image data.
            unsigned char *bitsPtr = mBits;
            unsigned char *imgPtr = img->GetData();

            for(int i = 0; i < mBitsRows; i++)
            {
                for(int j = 0; j < mBitsCols; j++)
                {
                    *bitsPtr++ = *(imgPtr + 2);
                    *bitsPtr++ = *(imgPtr + 1);
                    *bitsPtr++ = *imgPtr;
                    *bitsPtr++ = 0;
                    imgPtr += 3;
                }
            }
            //  Update display
            this->Refresh(false);
        }
        else
        {
#endif
            wxClientDC realDC(this);
            wxBufferedDC dc(&realDC, *this->mBuffer);
            
            // Previous calls using Color/GrayscaleImage will create a new wxImage
            // We must delete it to prevent memory leaks
            if (this->mType == IT_IPLIMAGE)
            {
                if (this->mImage)
                    delete this->mImage;
            }

            this->mImage = (wxImage *)(img);
            this->mType = IT_WX;

            // Display the new image
            this->mUpdated = true;
            this->Paint(&dc);
#ifdef WIN32
        }
#endif
        
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
           
    if (this->mType == IT_WX)
    {
        if(this->mImage)
            delete this->mImage;
        this->mImage = 0;
    }

    this->mType = IT_IPLIMAGE;
    //  If the image has an ROI (region of interest) use
    //  it instead of the whole image.
    x = (img->roi) ? img->roi->xOffset : 0;
    y = (img->roi) ? img->roi->yOffset : 0;
    rows = (img->roi) ? img->roi->height : img->height;
    cols = (img->roi) ? img->roi->width : img->width;
    
#if defined(WIN32)
    if (this->GetWindowStyle() & wxIP_WINDOWS_OPTIMIZE)
    {
        if (mBits && (mBitsRows != rows || mBitsCols != cols))
        {
            delete[] mBits;
            mBits = 0;
            
            if (hbitmap)
            {
                DeleteObject(hbitmap);
                hbitmap = 0;
            }
        }
        //  Allocate new memory if needed.
        if(!mBits)
        {
            mBitsRows = rows;
            mBitsCols = cols;
            mBits = new unsigned char[mBitsRows*mBitsCols*4];
        }

        //  Now copy the image data.
        unsigned char *bitsPtr = mBits;
        for(int i = y; i < y + rows; i++)
        {
            unsigned char *rowPtr = ((unsigned char *)(img->imageData + img->widthStep*i));
            
            for(int j = x; j < x + cols; j++)
            {
                //  Color data
                if(img->nChannels == 3)
                {
                    int offset = j*3;
                    if(img->channelSeq[0] == 'R')
                    {
                        *bitsPtr++ = rowPtr[offset + 2];
                        *bitsPtr++ = rowPtr[offset + 1];
                        *bitsPtr++ = rowPtr[offset];
                    }
                    else
                    {
                        *bitsPtr++ = rowPtr[offset];
                        *bitsPtr++ = rowPtr[offset + 1];
                        *bitsPtr++ = rowPtr[offset + 2];
                    }
                    *bitsPtr++ = 0;
                }
                //  Grayscale data
                else
                {
                    *bitsPtr++ = rowPtr[j];
                    *bitsPtr++ = rowPtr[j];
                    *bitsPtr++ = rowPtr[j];
                    *bitsPtr++ = 0;
                }
            }
        }
        //  Update display
        this->Refresh(false);
    }
    else
    {
#endif
        wxClientDC realDC(this);
        wxBufferedDC dc(&realDC, *this->mBuffer);
        
        if (this->mImage && (this->mImage->GetWidth() != cols || this->mImage->GetHeight() != rows))
        {
            delete this->mImage;
            this->mImage = 0;
        }

        if (!this->mImage)
        {
            this->mImage = new wxImage(cols, rows);
        }
        
        //  Now copy the image data.
        unsigned char *bitsPtr = mImage->GetData();
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
        
        // Update
        this->mUpdated = true;
        this->Paint(&dc);
#ifdef WIN32
    }
#endif
}

#endif

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Selects the control (border color becomes foreground color)
///
///   You can toggle color of the image border by calling Select. By default,
///   when the control is not selected, the border color is set to the color
///   of the background color (initialized to the color of the 
///   parent's background). When you select the control, the color toggles to
///   the foreground colour (initialized to blue).
///
///   \param yesno Whether or not the image has been selected
///   \return True on success, false on failure
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::Select(const bool yesno /*= true*/)
{
    if (this->mBorder > 0 && this->mSelected != yesno)
    {
        this->mSelected = yesno;

#if defined(WIN32)
        if (this->GetWindowStyle() & wxIP_WINDOWS_OPTIMIZE)
        {
            if (hbrush)
            {
                DeleteObject(hbrush);
                hbrush = 0;
            }
            this->Refresh(false);
        }
        else
        {
#endif
            wxClientDC realDC(this);
            wxBufferedDC dc(&realDC, *this->mBuffer);
            this->Paint(&dc);
#if defined(WIN32)
        }
#endif
    }
}

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
    if(e.GetId()){}
    wxSize imgSize = this->GetSize() - wxSize(mBorder*2, mBorder*2);
    wxSize ctrlSize = this->GetSize();
    static int ctr = 0;                   // How many times this function has been called
    const int framesPerRefresh = 100;     // How often to refresh the GDI handles
    double xScale = 1.0, yScale = 1.0;    // X & Y scale of the image

    // Optimized version
#if defined(WIN32)
    // The optimized version for Windows uses the raw WINAPI for everything
    if (this->GetWindowStyle() & wxIP_WINDOWS_OPTIMIZE)
    {
        wxPaintDC dc(this);
        RECT rect;
        wxColour color;

        // Optimize the display for Windows using the Windows API
        HDC hdc, hdcMem;
        HBITMAP hbmOld;

        // Get the drawing handle
        hdc = GetDC((HWND)this->GetHandle());

        // Create a compatable Device Context and bitmap
        hdcMem = CreateCompatibleDC(hdc);
        if (!hbitmap)
        {
            hbitmap = CreateCompatibleBitmap(hdc, mBitsCols, mBitsRows);
        }

        //memset(bits, 255, rows*cols*4);
        
        // Store our existing RGB-Alpha array in the bitmap
        SetBitmapBits(hbitmap, mBitsRows * mBitsCols * 4, mBits);
        
        // Figure out what scale to use, preserving aspect ratio
        calcScale(xScale, yScale);

        // Calculate the width on the sides and top
        verWidth = (int)(this->mBorder + (imgSize.x - mBitsCols*xScale) / 2.0);
        horWidth = (int)(this->mBorder + (int)(imgSize.y - mBitsRows*yScale) / 2.0);

        // Draw mBorder
        if (this->mBorder > 0)
        {
            // Border color is either background or foreground color depending on whether the image is selected
            if (this->mSelected)
            {
                color = this->GetForegroundColour();
            }
            else
            {
                color = this->GetBackgroundColour();
            }
            
            // Create brush based on color
            if (!hbrush)
            {
                hbrush = CreateSolidBrush(RGB(color.Red(), color.Green(), color.Blue()));
            }

            // Create brush based on background color
            if (!hbrushBack)
            {
                hbrushBack = CreateSolidBrush(RGB(this->GetBackgroundColour().Red(), this->GetBackgroundColour().Green(), this->GetBackgroundColour().Blue()));
            }

            // Paint sizes & top & bottom
            if (mBitsRows > 0 && mBitsCols > 0)
            {
                // Top
                rect.top = 0;
                rect.left = 0;
                rect.bottom = horWidth - mBorder;
                ground(rect.bottom);
                rect.right = ctrlSize.x;
                FillRect(hdc, &rect, hbrushBack);

                rect.top = horWidth - mBorder;
                ground(rect.top);
                rect.left = verWidth - mBorder;
                ground(rect.left);
                rect.bottom = horWidth;
                rect.right = mBitsCols*xScale + verWidth + mBorder;
                ground(rect.right, ctrlSize.x);
                FillRect(hdc, &rect, hbrush);

                // Left
                rect.top = 0;
                rect.left = 0;
                rect.bottom = ctrlSize.y;
                rect.right = verWidth - mBorder;
                ground(rect.right);
                FillRect(hdc, &rect, hbrushBack);

                rect.top = horWidth - mBorder;
                ground(rect.top);
                rect.left = verWidth - mBorder;
                ground(rect.left);
                rect.bottom = mBitsRows*yScale + horWidth + mBorder;
                ground(rect.bottom, ctrlSize.y);
                rect.right = verWidth;
                FillRect(hdc, &rect, hbrush);

                // Bottom
                rect.top = mBitsRows*yScale + horWidth + mBorder;
                ground(rect.top, ctrlSize.y);
                rect.left = 0;
                rect.bottom = ctrlSize.y;
                rect.right = ctrlSize.x;
                FillRect(hdc, &rect, hbrushBack);

                rect.top =  mBitsRows*yScale + horWidth;
                rect.left = verWidth - mBorder;
                ground(rect.left);
                rect.bottom = rect.top + mBorder;
                ground(rect.bottom, ctrlSize.y);
                rect.right = mBitsCols*xScale + verWidth + mBorder;
                ground(rect.right, ctrlSize.x);
                FillRect(hdc, &rect, hbrush);

                // Right
                rect.top = 0;
                rect.left = (int)(mBitsCols*xScale) + verWidth + mBorder;
                ground(rect.left, ctrlSize.x);
                rect.bottom = ctrlSize.y;
                rect.right = ctrlSize.x;
                FillRect(hdc, &rect, hbrushBack);

                rect.top = horWidth - mBorder;
                ground(rect.top);
                rect.left = (int)(mBitsCols*xScale) + verWidth;
                rect.bottom = mBitsRows*yScale + horWidth + mBorder;
                ground(rect.bottom, ctrlSize.y);
                rect.right = rect.left + mBorder;
                ground(rect.right, ctrlSize.x);
                FillRect(hdc, &rect, hbrush);
            }
            else // Color everything
            {
                rect.top = 0;
                rect.left = 0;
                rect.bottom = ctrlSize.y;
                rect.right = ctrlSize.x;
                FillRect(hdc, &rect, hbrush);
            }
        }

        // Select the object to the drawing handle
        hbmOld = (HBITMAP)SelectObject(hdcMem, hbitmap);

        // Draw the image (original or scaled)
        if (mBitsRows > 0 && mBitsCols > 0)
        {
            // And copy it to the screen, either directly or stretched
            if (mBitsRows != imgSize.y || mBitsCols != imgSize.x)
            {
                // Definitely want to stretch with color!!!
                SetStretchBltMode(hdc, COLORONCOLOR);

                StretchBlt(hdc, verWidth, horWidth, mBitsCols*xScale, mBitsRows*yScale, hdcMem, 0, 0, mBitsCols, mBitsRows, SRCCOPY);
            }
            else
            {
                BitBlt(hdc, mBorder, mBorder, imgSize.x, imgSize.y, hdcMem, 0, 0, SRCCOPY);
            }
        }

        // Release memory and objects
        SelectObject(hdcMem, hbmOld);
        DeleteDC(hdcMem);
        ReleaseDC((HWND)this->GetHandle(), hdc);

        // Because GDI can be finiky and device context properties might change (say change screen color depth)
        // We want to periodically refresh the GDI handles, but not 30 times a second. 
        // So for the first framesPerRefresh and then every framesPerRefresh afterwards, the GDI handles are
        // destroyed and re-created
        ++ctr;
        if (ctr < framesPerRefresh || !(ctr % framesPerRefresh))
        {
            DeleteObject(hbitmap);
            hbitmap = 0;
            DeleteObject(hbrush);
            hbrush = 0;
        }
    }
    else
    {
#endif
        //Paint the image
        wxBufferedPaintDC dc(this);
        this->Paint(&dc);
#if defined(WIN32)
    }
#endif
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
    if(!mImage)
        return;
    //wxSize imgSize = this->GetSize() - wxSize(mBorder*2, mBorder*2);
    wxSize imgSize, panelSize;
    double xScale = 1.0, yScale = 1.0;
    PrepareDC(*dc);

    
    imgSize.x = mImage->GetWidth();
    imgSize.y = mImage->GetHeight();
    panelSize = this->GetSize();

    // Create a bitmap from the wxImage
    if (this->mUpdated || !this->mBitmap || !(this->GetWindowStyle() & wxIP_CACHE_IMAGE))
    {
        if (this->mBitmap)
        {
            delete this->mBitmap;
            this->mBitmap = 0;
        }

        if (this->mImage)
        {
            this->mBitmap = new wxBitmap(*mImage);
        }
    }

    // Paint the mBorder
    if (this->mBorder > 0)
    {
        this->PaintBorder(dc);
    }

    if (!this->mBitmap)
        return;

    // Calculate the scale
    calcScale(xScale, yScale);

#if defined(WIN32)
    // The default dithering is pretty bad, let's set it to something tolerable
    ::SetStretchBltMode((HDC)dc->GetHDC(), HALFTONE);
    // If alignment errors occur, check into SetBrushOrgEx because the MSDN docs
    // say that it needs to be called after SetStretchBltMode
#endif

    // Set the scale, draw the bitmap, and reset the scale
    dc->SetUserScale(xScale, yScale);
    dc->DrawBitmap(*mBitmap,
                   (int)((mBorder + panelSize.x - mBitmap->GetWidth()*xScale)/2.0),
                   (int)((mBorder + panelSize.y - mBitmap->GetHeight()*yScale)/2.0),
                   this->mTransparent);
    //dc->DrawBitmap(*mBitmap, (int)((this->mBorder + (imgSize.GetWidth() - mBitmap->GetWidth()*xScale) / 2.0) / xScale), (int)((this->mBorder + (int)(imgSize.GetHeight() - mBitmap->GetHeight()*yScale) / 2.0) / yScale), this->mTransparent);
    dc->SetUserScale(1.0, 1.0);

    // Wait until something has been changed again (we don't need to update right now)
    this->mUpdated = false;
}

//////////////////////////////////////////////////////////////////////////////
///
///   \brief Paint border
///
///   Paints the border for the image (wxWidget's only, optimized version 
///   has its own version)
///
///   \param dc Device context to use
///
//////////////////////////////////////////////////////////////////////////////
void wxImagePanel::PaintBorder(wxDC *dc)
{
    wxBrush brush(this->GetBackgroundColour(), wxSOLID);

    if (this->mSelected)
    {
        brush.SetColour(this->GetForegroundColour());
    }

    dc->SetPen(wxPen(*wxBLACK, 0, wxTRANSPARENT));
    dc->SetBrush(brush);
    dc->DrawRectangle(0, 0, this->GetSize().x, this->GetSize().y);
    dc->SetBrush(wxNullBrush);
    dc->SetPen(wxNullPen);
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

    this->calcScale(xScale, yScale);
    cmdEvent.SetRow((int)((e.GetY() - horWidth) / yScale));
    cmdEvent.SetCol((int)((e.GetX() - verWidth) / xScale));
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
void wxImagePanel::calcScale(double &xScale, double &yScale)
{
    wxSize imgSize = this->GetSize() - wxSize(mBorder*2, mBorder*2);

#if defined(WIN32)
    if (mBitsCols > 0 && mBitsRows > 0 && this->GetWindowStyle() & wxIP_WINDOWS_OPTIMIZE)
    {
        xScale = (double)imgSize.x / mBitsCols;
        yScale = (double)imgSize.y / mBitsRows;
    }
    else if (mBitmap)
    {
#endif
        xScale = (double)imgSize.x / this->mBitmap->GetWidth();
        yScale = (double)imgSize.y / this->mBitmap->GetHeight();
#if defined(WIN32)
    }
#endif
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
