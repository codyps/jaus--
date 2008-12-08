////////////////////////////////////////////////////////////////////////////////////
///
///  \file image.h
///  \brief This file contains the main data structure for storing and
///  compressing/decompressing image data for the JAUS++ video library.
///
///  <br>Author(s): Sergey Leontyev
///  <br>Created: 20 July 2007
///  <br>Last Modified: 10 June 2008
///  <br>Copyright (c) 2007
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: sleontye@ist.ucf.edu
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
#ifndef __JAUS_IMAGE_HEADER_H
#define __JAUS_IMAGE_HEADER_H


#include <string>
#include <iostream>
#include "jaus/video/videolibdll.h"
#include "jaus/messages/errorhistory.h"
#include "jaus/video/jpeg/jpeg.h"
#include "jaus/video/png/png.h"
#include "jaus/video/pgm/pgm.h"
#include "jaus/video/ppm/ppm.h"
#include <cxutils/virtualfile.h>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///  \class Image
    ///  \brief Image compression and decompression software and image data storage.
    ///  formats supported:
    ///      + JPG  - libjpeg is used
    ///      + PPM  only P6 -  3 channels, binary format supported
    ///      + PGM  only P5 -  1 channel , binary format supported
    ///      + PNG  - libpng is used
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_VIDEO_DLL Image : public ErrorHistory
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///  \brief Enumeration of possible image formats supported by JAUS.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum Format
        {
            Unused = 0,
            MPEG2,
            MPEG4,
            MJPEG,
            NTSC,
            PAL,
            TIFF,
            JPEG,
            GIF,
            H263,
            H264,
            PNG,
            BMP,
            RAW,
            PPM,
            PGM,
            PNM
        };
        Image();
        Image(const Image& img);
        virtual ~Image();
        int Create(const UShort width,
                   const UShort height,
                   const Byte channels,
                   const Byte* rawImage,
                   const bool vflip = false);
        int Create(const UShort width,
                   const UShort height,
                   const Jaus::Byte channels,
                   const Jaus::Byte* rawImage,
                   const double scale,
                   const bool vflip);
        int Create(const UShort width,
                   const UShort height,
                   const Jaus::Byte channels,
                   const Jaus::Byte* rawImage,
                   const UShort maxWidth,
                   const UShort maxHeight,
                   const bool vflip);
        int SetImageNumber(const UInt num);
        int Destroy();
        int Decompress(const Jaus::Byte* compressed,
                       const unsigned int len,
                       const Format format = Unused);
        int Compress(Jaus::Byte** buffer,
                     unsigned int& len,
                     unsigned int& clen,
                     const Format format) const;
        static int Compress(const UShort width,
                            const UShort height,
                            const Jaus::Byte channels,
                            const Jaus::Byte* rawImage,
                            Jaus::Byte** buffer,
                            unsigned int& len,
                            unsigned int& clen,
                            const Format format);
        int ConvertToGrayscale();
        void FlipChannels();
        int LoadFrame(const std::string& file);
        int SaveFrame(const std::string& file) const;

        inline UShort Width() const { return mWidth; }
        inline UShort Height() const { return mHeight; }
        inline Byte Channels() const { return mChannels; }
        inline unsigned int DataSize() const { return mDataSize; }
        inline Byte* ImageData() const { return mpImage; }
        inline UInt GetImageNumber() const { return mImageNumber; }
        static int ConvertToGrayscale(const UShort width,
                                      const UShort height,
                                      const Byte* input,
                                      Byte* output);
        static Format GetFormat(const std::string& file);
        Image& operator=(const Image& img);
    protected:
        CxUtils::VirtualFile mVirtualFile; ///< Used to write compressed image files to memory buffer.
        Byte mChannels;         ///<  Number of color channels 1, or 3.
        UShort mWidth;          ///<  Horizontal resolution of image in pixels.
        UShort mHeight;         ///<  Vertical resolution of image in pixels.
        unsigned int mDataSize; ///<  Size of RAW data in bytes.
        Byte* mpImage;          ///<  Raw uncompressed image data.
        UInt mImageNumber;      ///<  Unique number associated with image (optional).
    };
}

#endif
/*  End of File */
