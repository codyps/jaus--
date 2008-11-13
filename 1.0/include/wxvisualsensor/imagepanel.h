/*==========================================================================

    Filename: imagepanel.h

    Copyright (C) 2006  Brian C. Becker
                        Daniel Barber
                        Robotics Lab at UCF
                        http://robotics.ucf.edu

    Description:
    ------------------------------------------------------------------------
    This is a control designed to display an image or flickerless realtime
    video. This control is optimized for Windows by directly using the 
    Windows API. Once you give it an image to display, it continues
    to display that image until you set another one. The transition 
    between images is realtime when optimized and almost flickerless.

    License:
    ------------------------------------------------------------------------
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

#ifndef _WX_IMAGEPANEL_H
#define _WX_IMAGEPANEL_H

// Disables the old MS Visual C++ 6 warnings about debugging symbols exceeding 255 chars
#pragma warning(disable:4786)

#include <assert.h>
#include <wx/wx.h>
#include <wx/image.h>
#include <wx/dcbuffer.h>

//#define WX_DO_NOT_USE_OPENCV //  Define this to not use OpenCV compatibility

#ifndef WX_DO_NOT_USE_OPENCV
#include "cv.h"
#endif

////////////////////////////////////////////////////////////////////////////////
///  
///   \def wxIP_CACHE_IMAGE
///   Tells wxImagePanel to cache the wxImage you display.
///   This allows you to create an wxImage and then delete it without 
///   wxImagePanel crashing
///
////////////////////////////////////////////////////////////////////////////////
#define wxIP_CACHE_IMAGE        0x0010

////////////////////////////////////////////////////////////////////////////////
///  
///   \def wxIP_WINDOWS_OPTIMIZE
///   Bypasses the inefficiency of converting a wxImage to a wxBitmap. Works only
///   on Windows. Highly recommended when you are using wxImagePanel to display 
///   video. Uses an extra meg or two of memory, though.
///
////////////////////////////////////////////////////////////////////////////////
#define wxIP_WINDOWS_OPTIMIZE   0x0020

////////////////////////////////////////////////////////////////////////////////
///  
///   \def wxIP_RESIZE_WITH_BORDER
///   When the border is resized, the border will be taken into account, so that
///   resizing to 320,240 will actually resize the control to 330,250 with a 5
///   pixel border
///
////////////////////////////////////////////////////////////////////////////////
#define wxIP_RESIZE_WITH_BORDER   0x0040

////////////////////////////////////////////////////////////////////////////////
///  
///   \enum wxImageTypes
///   Used internally by wxImagePanel
///
////////////////////////////////////////////////////////////////////////////////
enum wxImageTypes {IT_NONE = 0, IT_WX, IT_IPLIMAGE};


////////////////////////////////////////////////////////////////////////////////
///  
///   \class wxImagePanel
///   \brief Displays a series of images or a video to the screen ussing a buffer
///   to prevent flickering.  Also makes display of an image easy.
///
///   wxImagePanel is is similar to wxStaticBitmap as it displays an image on the 
///   screen. However, it is is designed primarily for displaying a series of
///   images, such as a video. It is flickerless, but does tear (i.e. it starts
///   painting before it has finished painting the frame). This is usually not 
///   that big of a deal, but if you want to get rid of it, you will have to
///   use DirectDraw or something.
///
///   wxImagePanel is also capable of resizing the image based on the size
///   of the control. You can set a border in pixels that swaps between
///   the foreground and background color of the control based on whether or
///   not the image has been selected (useful for thumbnails or something). 
///   However, the automatic scaling only works for scaling down. wxImagePanel
///   will not enlarge your image to fit the control, instead it will center
///   it at 100% zoom.
///
///   wxImagePanel has several modes of operation:
///   <ol>
///   <li>Display static wxImage to screen. In this case, you will want to specify
///       wxIP_CACHE_IMAGE. </li>
///   <li>Display live video to the screen through a series of OpenCV IplImage
///       images. Specify wxIP_WINDOWS_OPTIMIZE if you are using
///       Windows. This will help cut down on the CPU usage at the slight cost
///       of memory. This flag is ignored under Linux</li>
///   <li>Display live video to screen through a series of wxImages.</li>
///   </ol>
///
///   <b>Important Sizing Note:</b><br>Whenever you set the size of this control,
///   you are setting the size of the image, not the image + border. So if you
///   are capturing from a DV camera at 720x480, you initialize the size of the
///   wxImagePanel with a size of wxSize(720, 480). If you set a border of 5 
///   pixels, the final size of the control will be 730x480. Also note that the
///   resolutions are backwards than normally expected with ColorImage and
///   GrayscaleImage.
///
///   <b><span style="font-color:red;">Do not change the style of the control 
///   once you have created it. If you turn wxIP_WINDOWS_OPTIMIZE on or off 
///   during runtime, the results will probably NOT be what you want. </span></b>
///
///   Required Libraries: wxWidgets (only version 2.6.3 Windows has been tested)
///   If WX_USE_OPENCV is defined, than you will also need OpenCV library from
///   Intel.
///
////////////////////////////////////////////////////////////////////////////////
class wxImagePanel : public wxControl
{
public:
    wxImagePanel();
#ifdef WIN32
    wxImagePanel(wxWindow *parent, wxWindowID id, 
                 const int border = 0, 
                 const wxPoint &pos = wxDefaultPosition, 
                 const wxSize &size = wxDefaultSize, 
                 const long style = wxIP_WINDOWS_OPTIMIZE);
#else
    wxImagePanel(wxWindow *parent, wxWindowID id, 
                 const int border = 0, 
                 const wxPoint &pos = wxDefaultPosition, 
                 const wxSize &size = wxDefaultSize, 
                 const long style = 0);
#endif
    wxImagePanel(wxWindow *parent, wxWindowID id, 
                 const wxImage &image, 
                 const wxPoint &pos = wxDefaultPosition);
    ~wxImagePanel();

    bool Create(wxWindow *parent, wxWindowID id, 
                const int border = 0, 
                const wxPoint &pos = wxDefaultPosition, 
                const wxSize &size = wxDefaultSize, 
                const long style = 0);
    wxSize DoGetBestSize() const;

    void SetImage(const wxImage *img);
    void SetImage(const wxImage &img) { SetImage(&img); }
    void ClearImage();
#ifndef WX_DO_NOT_USE_OPENCV
    void SetImage(const IplImage * img);
#endif
    void Select(const bool yesno = true);
    /// Returns whether or not the image is selected
    bool IsSelected(){ return mSelected; }
    /// Returns the width of the border in pixels
    int GetBorderWidth(){ return mBorder; }
    /// Sets the width of the border in pixels
    void SetBorderWidth(const int border);

    void OnPaint(wxPaintEvent &e);
    void OnMouse(wxMouseEvent &e);
    void OnSize(wxSizeEvent &e);

    wxSize GetBestSize() const { return this->GetSize(); }

    /// Don't erase the backround under any circumstance (empty function)
    void OnBackgroundErase(wxEraseEvent &e){if(e.GetId()){}}

protected:
    void Init();
    void Paint(wxDC *dc);
    void PaintBorder(wxDC *dc);
    void calcScale(double &xScale, double &yScale);

    wxBitmap *mBitmap;                      ///< Bitmap to draw on the screen
    wxBitmap *mBuffer;                      ///< Bitmap to buffer the screen
    int mBorder;                            ///< Size of the border around the image

    wxImage *mImage;                        ///< Image pointer to wxImage to draw
    int mType;                              ///< Type of image we are drawing

    bool mTransparent;                      ///< Drawing with transparency? (not supported)
    bool mSelected;                         ///< Is the wxImagePanel created?
    bool mUpdated;                          ///< Has the image been updated (ie we need to draw it)
    bool mSizing;                           ///< Are we currently sizing the wxImagePanel?

#if defined(WIN32)
    unsigned char * mBits;                  ///< Raw RGBA image array
    int mBitsRows, mBitsCols;               ///< Dimensions of the raw RGBA image array
    HBITMAP hbitmap;                        ///< HBITMAP to draw store RGBA array
    HBRUSH hbrush;                          ///< HBRUSH to draw the border of the image
    HBRUSH hbrushBack;                      ///< HBRUSH to draw the border of the image
#endif

    int verWidth, horWidth;

    DECLARE_EVENT_TABLE()
};


////////////////////////////////////////////////////////////////////////////////
///  
///   \class wxImagePanelEvent
///   \brief Allows mouse interaction an wxImagePanel and the 
///   control that created it.
///
///   To get the actual mouse event that occurred, call GetMouseEvent.
///
////////////////////////////////////////////////////////////////////////////////
class wxImagePanelEvent : public wxNotifyEvent
{
public:
    wxImagePanelEvent(wxEventType commandType = wxEVT_NULL, int winid = 0)
        : wxNotifyEvent(commandType, winid)
        { }

    wxImagePanelEvent(const wxImagePanelEvent& event)
        : wxNotifyEvent(event)
        { }

    void SetMouseEvent(wxMouseEvent &e){mImagePanelMouseEvent = e;}
    wxMouseEvent &GetMouseEvent(){return mImagePanelMouseEvent;}

    int GetRow() const { return mRow; }
    int GetCol() const { return mCol; }
    void SetRow(const int row) { mRow = row; }
    void SetCol(const int col) { mCol = col; }
    void GetPosition(int *col, int *row) { assert(col && row); *col = mCol; *row = mRow; }
    virtual wxEvent *Clone() const { return new wxImagePanelEvent(*this); }

protected:
    wxMouseEvent mImagePanelMouseEvent;
    int mRow, mCol;

private:
    DECLARE_DYNAMIC_CLASS_NO_ASSIGN(wxImagePanelEvent)
};

// Hook up events
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_COMMAND_IMAGEPANEL_MOUSE_EVENTS, 2756)
END_DECLARE_EVENT_TYPES()

// Typedef the function pointer
typedef void (wxEvtHandler::*wxImagePanelEventFunction)(wxImagePanelEvent&);

// And hook up the entries
#define EVT_IMAGEPANEL_MOUSE_EVENTS(id, fn) DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_IMAGEPANEL_MOUSE_EVENTS, id, -1, (wxObjectEventFunction) (wxEventFunction)  wxStaticCastEvent(wxImagePanelEventFunction, & fn), NULL),

#endif // _WX_IMAGEPANEL_H
/*  End of File */
