////////////////////////////////////////////////////////////////////////////////////
///
///  \file image.cpp
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
#include "jaus/video/image.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#ifdef WIN32

#define XMD_H // prevent redefinition of INT32
#undef FAR    // prevent FAR redefinition

#endif

extern "C" {
#include <jpeglib.h>
#include <png.h>
}

using namespace Jaus;
using namespace CxUtils;

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Constructor.  
///
////////////////////////////////////////////////////////////////////////////////////
Image::Image()
{
    mpImage = NULL;
    mDataSize = 0;
    mWidth = 0;
    mHeight = 0;
    mChannels = 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Image::Image(const Image& img)
{
    mpImage = NULL;
    mDataSize = 0;
    mWidth = 0;
    mHeight = 0;
    mChannels = 0;
    *this = img;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destructor.  Destroys the Image object and closes the temporary file 
///  produced by the mVirtual file
///
////////////////////////////////////////////////////////////////////////////////////
Image::~Image()
{
    Destroy();
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This initializes the Image
///
///  \param width The width of the image
///  \param height The height of the image
///  \param channels Number of channels used (1, 3, or 4)
///  \param rawImage The image data to be copied to the image, if the pointer is NULL,
///                  only memory allocation takes place an no copying.
///  \param vflip If true the data if flipped vertically.
///
///  \return JAUS_OK on success, JAUS_FAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Create(const UShort width, 
                  const UShort height, 
                  const Jaus::Byte channels, 
                  const Jaus::Byte* rawImage,
                  const bool vflip)
{
    if(width == 0 || height == 0 || !(channels == 1 || channels == 3 || channels == 4) ) {
        SetJausError(ErrorCodes::InvalidValue);
        return JAUS_FAILURE;
    }

    if (mHeight != height || mWidth != width || mChannels != channels)  {
        Destroy();
        mHeight = height;
        mWidth = width;
        mChannels = channels;
        mDataSize = mHeight*mWidth*mChannels*sizeof(Jaus::Byte);
        mpImage = new Jaus::Byte[mDataSize + 100];
        assert(mpImage);
    }   

    if (rawImage != NULL){
        if( vflip )
        {
            unsigned int widthStep = width*channels;
            for(unsigned int row1 = 0, row2 = height - 1; 
                row1 < height && row2 >= 0;
                row1++, row2--)
            {
                memcpy(&mpImage[row1*widthStep], &rawImage[row2*widthStep], widthStep);
            }
        }
        else
        {
            memcpy(mpImage, rawImage, mDataSize);
        }
    }
    else {
        memset(mpImage, 0, mDataSize);
    }


    return JAUS_OK;

}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This initializes the Image
///
///  \param width The width of the image
///  \param height The height of the image
///  \param channels Number of channels used (1, 3, or 4)
///  \param rawImage The image data to to scale.
///  \param scale Percentage to scale the image.  A value of 1.0 is equal to
///               the original size, < 1.  A value greater than 1 enlarges.
///  \param vflip If true the data if flipped vertically.
///
///  \return JAUS_OK on success, JAUS_FAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Create(const UShort width, 
                  const UShort height, 
                  const Jaus::Byte channels, 
                  const Jaus::Byte* rawImage,
                  const double scale,
                  const bool vflip)
{
    // Must have image data.
    if(rawImage == NULL)
    {
        return JAUS_FAILURE;
    }

    if(scale >=  0.99 && scale <= 1.01)
    {
        return Create(width, height, channels, rawImage, vflip);
    }

    UShort newHeight, newWidth;
    double magnitude;
    newHeight = (UShort)(height*scale);
    newWidth = (UShort)(width*scale);

    if(newHeight == 0 || newWidth == 0) { return 0; }

    if(scale < 1.0)
    {
        magnitude = (double)(height)/newHeight;
    }
    else
    {
        magnitude = (double)(newHeight)/height;
    }

    // Allocate memory.
    Create(newWidth, newHeight, channels, NULL, false);
    UShort x, y;
    if(scale < 1) // Sub sample image
    {
        for(UShort i = 0; i < newHeight; i++)
        {
            if(vflip)
            {
                y = (UShort)((newHeight - i - 1)*magnitude);
            }
            else
            {
                y = (UShort)(i*magnitude);
            }
            for(UShort j = 0; j < newWidth; j++)
            {                   
                if(channels == 1)
                {
                    x = (UShort)(j*magnitude);
                    mpImage[i*newWidth + j] = rawImage[y*width + x];
                }
                else if(channels == 3)
                {
                    x = (UShort)(j*magnitude);
                    mpImage[i*newWidth*3 + j*3] = rawImage[y*width*3 + x*3];
                    mpImage[i*newWidth*3 + j*3 + 1] = rawImage[y*width*3 + x*3 + 1];
                    mpImage[i*newWidth*3 + j*3 + 2] = rawImage[y*width*3 + x*3 + 2];
                }
                else
                {
                    x = (UShort)(j*magnitude);
                    for(UShort k = 0; k < channels; k++)
                    {
                        mpImage[i*newWidth*channels + j*channels + k] = rawImage[y*width*channels + x*channels + k];
                    }
                }
            }
        }
    }
    else          // Enlarge image.
    {
        for(UShort i = 0; i < height; i++)
        {
            if(vflip)
            {
                y = (height - i - 1);
            }
            else
            {
                y = i;
            }
            for(UShort j = 0; j < width; j++)
            {
                // Copy the pixel multiple times based on how much larger
                    // it needs to be. 
                for(UShort m = (UShort)(i*magnitude); m < (UShort)((i+1)*magnitude); m++)
                {                    
                    for(UShort n = (UShort)(j*magnitude); n < (UShort)((j+1)*magnitude); n++)
                    {
                        if(channels == 1)
                        {
                            mpImage[m*newWidth + n] = rawImage[y*width + j];
                        }
                        else if(channels == 3)
                        {
                            mpImage[m*newWidth*3 + n*3] = rawImage[y*width*3 + j*3];
                            mpImage[m*newWidth*3 + n*3 + 1] = rawImage[y*width*3 + j*3 + 1];
                            mpImage[m*newWidth*3 + n*3 + 2] = rawImage[y*width*3 + j*3 + 2];
                        }
                        else
                        {
                            for(UShort k = 0; k < channels; k++)
                            {
                                mpImage[m*newWidth*channels + n*channels + k] = rawImage[y*width*channels + j*channels + k];
                            }
                        }
                    }
                }
            }
        }
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief This initializes the Image and scales to fit some dimension.
///
///  \param width The width of the image
///  \param height The height of the image
///  \param channels Number of channels used (1, 3, or 4)
///  \param rawImage The image data to to scale.
///  \param vflip If true the data if flipped vertically.
///  \param maxWidth Set the maximum width you want the image stored within.
///                   This will force image scaling to occur. 
///  \param maxHeight Set the maximum height you want the image stored within.
///                   This will force image scaling to occur. 
///
///  \return JAUS_OK on success, JAUS_FAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Create(const UShort width, 
                  const UShort height, 
                  const Jaus::Byte channels, 
                  const Jaus::Byte* rawImage,
                  const UShort maxWidth,
                  const UShort maxHeight,
                  const bool vflip)
{
    Image temp;
    // Find out how much to scaled image to fit scaled Width or Height.
    double scaleFactor1 = 1.0, scaleFactor2 = 1.0;

    if(maxWidth == 0 || maxHeight == 0)
    {
        return Create(width, height, channels, rawImage, vflip);
    }

    if(maxWidth > width && maxHeight > height)
    {
        return JAUS_FAILURE;
    }
    scaleFactor1 = (double)(maxWidth)/width;
    scaleFactor2 = (double)(maxHeight)/height;
    if(scaleFactor2 < scaleFactor1)
    {
        scaleFactor1 = scaleFactor2;
    }

    if(temp.Create(width, height, channels, rawImage, scaleFactor1, vflip) == JAUS_FAILURE)
    {
        return JAUS_FAILURE;
    }

    // Now copy the data internally, but make it fit in the center.
    Create(maxWidth, maxHeight, channels, NULL, false);
    Byte* ptr1, *ptr2;
    int startRow, startCol;
    ptr1 = mpImage;
    ptr2 = temp.mpImage;

    // Calculate where to start copying the scaled image into the
    // width/height contrained image.
    startRow = ((int)(maxHeight) - 1)/2 - ((int)(temp.mHeight) - 1)/2;
    startCol = ((int)(maxWidth) - 1)/2 - ((int)(temp.mWidth) - 1)/2;
    if(startRow < 0)
        startRow = 0;
    if(startCol < 0)
        startCol = 0;

    for(int i = startRow, m = 0; i < maxHeight && m < temp.mHeight; i++, m++)
    {
        for(int j = startCol, n = 0; j < maxWidth && n < temp.mWidth; j++, n++)
        {
            if(channels == 1)
            {
                mpImage[i*maxWidth*channels + j*channels] = temp.mpImage[m*temp.mWidth*channels + n*channels];
            }
            else if(channels == 3)
            {
                mpImage[i*maxWidth*channels + j*channels] = temp.mpImage[m*temp.mWidth*channels + n*channels];
                mpImage[i*maxWidth*channels + j*channels + 1] = temp.mpImage[m*temp.mWidth*channels + n*channels + 1];
                mpImage[i*maxWidth*channels + j*channels + 2] = temp.mpImage[m*temp.mWidth*channels + n*channels + 2];
            }
            else
            {
                for(int k = 0; k < channels; k++)
                {
                    mpImage[i*maxWidth*channels + j*channels + k] = temp.mpImage[m*temp.mWidth*channels + n*channels + k];
                }
            }
        }
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Optional value to set if you want to number the image/frame.
///
///  \param num Image/frame number.
///
///  \return JAUS_OK if set, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::SetImageNumber(const UInt num)
{
    mImageNumber = num;
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief Destroys the Image object and closes the temporary file 
///  produced by the mVirtual file
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Destroy()
{
    if( mpImage ) {
        delete[] mpImage;
        mpImage = NULL;
    }
    mDataSize = 0;
    mWidth = 0;
    mHeight = 0;
    mChannels = 0;
    mVirtualFile.Close();
    return JAUS_OK;
}

////////////////////////////////////////////////////////////////////////////////////
///
///  \brief The method compresses the image to buffer
///  produced by the mVirtual file
///
///  \param buffer The pointer to the output buffer
///  \param len Specify the lenght of the buffer
///  \param clen Is the length of the compressed image in the output buffer.
///  \param format The image format type to compress to. See jimageformats.h.
///
///  \return JAUS_OK on success, JAUS_FAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Compress(Jaus::Byte** buffer, 
                    unsigned int& len, 
                    unsigned int& clen,
                    const Format format) const  
{
    return Compress( mWidth, mHeight, mChannels, mpImage,
                     buffer, len, clen, format );
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes raw uncompressed image data and converts it to another
///   image compression format (i.e. jpeg, png, ppm, .. etc.).
///
///   \param width The width of the raw image data (number of columns).
///   \param height The height of the raw image data (number of rows).
///   \param channels The number of color channels (1 = Grayscale, or 3 = 24 bit
///                   color).
///   \param rawImage Raw image data.  The format of this data is interleved
///                   color channels ( RGB, RGB, RGB, ... ) with the top
///                   left corner of the image at pixel location [0, 0].
///   \param buffer Pointer to buffer pointer.  This is where the compressed
///                 image data is put.  If (*buffer) == NULL then memory is
///                 automatically allocated.
///   \param len The length of the buffer (number of bytes).  If the buffer
///              is too small to fit the data, then it will be resized, and the
///              new buffer size will be saved to len.
///   \param clen The length of the compressed data in bytes.  This value will
///               be equal to or less than len after compression.
///   \param format The image format to use for compression.  Values are 
///                 defined in jimageformats.h.
///
///   \return JAUS_OK if compression was successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Compress(const UShort width,
                    const UShort height,
                    const Jaus::Byte channels,
                    const Jaus::Byte* rawImage,
                    Jaus::Byte** buffer,
                    unsigned int& len,
                    unsigned int& clen, 
                    const Format format) const
{
    int result = JAUS_FAILURE;

    switch (format)
    {
    case PPM:
        result = CompressPPM(width, height, channels, rawImage, buffer, len, clen);
        break;
    case PNG:
        result = CompressPNG(width, height, channels, rawImage, buffer, len, clen);
        break;
    case PGM:
        result = CompressPGM(width, height, channels, rawImage, buffer, len, clen);
        break;
    case JPEG:
        result = CompressJPEG(width, height, channels, rawImage, buffer, len, clen);
        break;
    case MJPEG:
        result = CompressJPEG(width, height, channels, rawImage, buffer, len, clen);
        break;
    default:
        result = JAUS_FAILURE;
        break;
    }

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes raw uncompressed image data and converts it to another
///   image compression format PPM (P6)
///
///   \param width The width of the raw image data (number of columns).
///   \param height The height of the raw image data (number of rows).
///   \param channels The number of color channels (3 = 24 bit color)
///                  
///   \param rawImage Raw image data.  The format of this data is interleved
///                   color channels ( RGB, RGB, RGB, ... ) with the top
///                   left corner of the image at pixel location [0, 0].
///   \param buffer Pointer to buffer pointer.  This is where the compressed
///                 image data is put.  If (*buffer) == NULL then memory is
///                 automatically allocated.
///   \param len The length of the buffer (number of bytes).  If the buffer
///              is too small to fit the data, then it will be resized, and the
///              new buffer size will be saved to len.
///   \param clen The length of the compressed data in bytes.  This value will
///               be equal to or less than len after compression.
///
///   \return JAUS_OK if compression was successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::CompressPPM(const UShort width,
                       const UShort height,
                       const Jaus::Byte channels,
                       const Jaus::Byte* rawImage,
                       Jaus::Byte** buffer,
                       unsigned int& len,
                       unsigned int& clen) const
{
    int result = JAUS_FAILURE;

    if( width == 0 || height == 0 || !(channels == 3) ||
        rawImage == NULL || buffer == NULL ) {
            return result;
    }

    unsigned int bytes = width*height*channels*sizeof(Jaus::Byte);

    if( channels == 3) {
        int characters = 0;
        if( len <= bytes + 100 || *buffer == NULL ) {
            //  If buffer is already pointing to allocated
            //  memory, make sure we delete it before asking
            //  for more.
            if( (*buffer) ) {
                delete[] (*buffer);
                (*buffer) = NULL;
            }
            (*buffer) = new Jaus::Byte[bytes + 100];
            assert( (*buffer) );
            len = bytes + 100;
        }

        if (channels == 3){
            characters = sprintf( ( (char *) (*buffer) ) , "P6 %d %d\n# Compressed by Image \n %d\n", width, height, 255);
        }

        memcpy( (*buffer) + characters , rawImage, bytes );
        clen = bytes + characters;
        result = JAUS_OK;
    }
    else {
        SetJausError(ErrorCodes::InvalidValue);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes raw uncompressed image data and converts it to another
///   image compression format PGM  (P5).  Currently only supports grayscale.
///
///   \param width The width of the raw image data (number of columns).
///   \param height The height of the raw image data (number of rows).
///   \param channels The number of color channels (1 = Grayscale)
///                   
///   \param rawImage Raw image data.  The format of this data is interleved
///                   color channels ( RGB, RGB, RGB, ... ) with the top
///                   left corner of the image at pixel location [0, 0].
///   \param buffer Pointer to buffer pointer.  This is where the compressed
///                 image data is put.  If (*buffer) == NULL then memory is
///                 automatically allocated.
///   \param len The length of the buffer (number of bytes).  If the buffer
///              is too small to fit the data, then it will be resized, and the
///              new buffer size will be saved to len.
///   \param clen The length of the compressed data in bytes.  This value will
///               be equal to or less than len after compression.
///
///   \return JAUS_OK if compression was successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::CompressPGM(const UShort width,
                       const UShort height,
                       const Jaus::Byte channels,
                       const Jaus::Byte* rawImage,
                       Jaus::Byte** buffer,
                       unsigned int& len,
                       unsigned int& clen) const
{
    int result = JAUS_FAILURE;

    if( width == 0 || height == 0 || !(channels == 1) ||
        rawImage == NULL || buffer == NULL ) {
            return result;
    }

    unsigned int bytes = width*height*channels*sizeof(Jaus::Byte);

    if( channels == 1) {
        int characters = 0;
        if( len <= bytes + 100 || *buffer == NULL ) {
            //  If buffer is already pointing to allocated
            //  memory, make sure we delete it before asking
            //  for more.
            if( (*buffer) ) {
                delete[] (*buffer);
                (*buffer) = NULL;
            }
            (*buffer) = new Jaus::Byte[bytes + 100];
            assert( (*buffer) );
            len = bytes + 100;
        }

        if (channels == 1){
            characters = sprintf( ( (char *) (*buffer) ) , "P5 %d %d %d\n", width, height, 255);
        }

        memcpy( (*buffer) + characters , rawImage, bytes );
        clen = bytes + characters;
        result = JAUS_OK;
    }
    else {
        SetJausError(ErrorCodes::InvalidValue);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes raw uncompressed image data and converts it to another
///   image compression format PNG
///
///   \param width The width of the raw image data (number of columns).
///   \param height The height of the raw image data (number of rows).
///   \param channels The number of color channels (1 = Grayscale, or 3 = 24 bit
///                   color).
///   \param rawImage Raw image data.  The format of this data is interleved
///                   color channels ( RGB, RGB, RGB, ... ) with the top
///                   left corner of the image at pixel location [0, 0].
///   \param buffer Pointer to buffer pointer.  This is where the compressed
///                 image data is put.  If (*buffer) == NULL then memory is
///                 automatically allocated.
///   \param len The length of the buffer (number of bytes).  If the buffer
///              is too small to fit the data, then it will be resized, and the
///              new buffer size will be saved to len.
///   \param clen The length of the compressed data in bytes.  This value will
///               be equal to or less than len after compression.
///   \return JAUS_OK if compression was successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::CompressPNG(const UShort width,
                       const UShort height,
                       const Jaus::Byte channels,
                       const Jaus::Byte* rawImage,
                       Jaus::Byte** buffer,
                       unsigned int& len,
                       unsigned int& clen) const
{
    int result = JAUS_FAILURE;

    if( width == 0 || height == 0 || !(channels == 1 || channels == 3) ||
        rawImage == NULL || buffer == NULL ) {
            return result;
    }

    unsigned int bytes = width*height*channels*sizeof(Jaus::Byte);


    if( channels == 3 || channels == 1 ) 
    {
        if( len <= bytes + 100 || *buffer == NULL ) 
        {
            //  If buffer is already pointing to allocated
            //  memory, make sure we delete it before asking
            //  for more.
            if( (*buffer) ) {
                delete[] (*buffer);
                (*buffer) = NULL;
            }
            (*buffer) = new Jaus::Byte[bytes + 100];
            assert( (*buffer) );
            len = bytes + 100;
        }

        png_structp     png_ptr = NULL;

        png_bytep     * row_pointers = NULL;

        VirtualFile *vfPtr = ((VirtualFile*)(&mVirtualFile));
        vfPtr->Open((char*)(*buffer), len);
        vfPtr->Rewind();

        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        png_infop       info_ptr = png_create_info_struct(png_ptr);        
        png_init_io(png_ptr, *vfPtr);

        png_byte color;
        if (channels == 1){
            color = PNG_COLOR_TYPE_GRAY;
        }
        else if (channels == 3){
            color = PNG_COLOR_TYPE_RGB;
        }
        else
        {
            color = PNG_COLOR_TYPE_RGB;
        }


        png_set_IHDR(png_ptr, info_ptr, width, height,
            8, color, PNG_INTERLACE_NONE,
            PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);




        row_pointers = (png_bytep*)malloc(height*sizeof(png_bytep));
        for (int i=0 ; i< height; i++){
            row_pointers[i] = (png_byte*)malloc(info_ptr->rowbytes*sizeof(png_byte));

            memcpy( row_pointers[i], mpImage+i*width*channels, sizeof(Jaus::Byte)*width*channels);

        }

        png_set_rows(png_ptr, info_ptr, row_pointers);
        png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 , NULL);


        for (int i =0; i< height; i++){
            delete[] row_pointers[i];
        }
        delete[] row_pointers;

        ::png_destroy_write_struct(&png_ptr, &info_ptr);


        clen = vfPtr->GetCount();
        result = JAUS_OK;

    }
    else {
        SetJausError(ErrorCodes::InvalidValue);
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes raw uncompressed image data and converts it to another
///   image compression format JPEG
///
///   \param width The width of the raw image data (number of columns).
///   \param height The height of the raw image data (number of rows).
///   \param channels The number of color channels (1 = Grayscale, or 3 = 24 bit
///                   color).
///   \param rawImage Raw image data.  The format of this data is interleved
///                   color channels (RGB) with the top
///                   left corner of the image at pixel location [0, 0].
///   \param buffer Pointer to buffer pointer.  This is where the compressed
///                 image data is put.  If (*buffer) == NULL then memory is
///                 automatically allocated.
///   \param len The length of the buffer (number of bytes).  If the buffer
///              is too small to fit the data, then it will be resized, and the
///              new buffer size will be saved to len.
///   \param clen The length of the compressed data in bytes.  This value will
///               be equal to or less than len after compression.
///   \return JAUS_OK if compression was successful, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Image::CompressJPEG(const UShort width,
                        const UShort height,
                        const Jaus::Byte channels,
                        const Jaus::Byte* rawImage,
                        Jaus::Byte** buffer,
                        unsigned int& len,
                        unsigned int& clen) const
{
    int result = JAUS_FAILURE;

    try{

        if( width == 0 || height == 0 || !(channels == 1 || channels == 3) ||
            rawImage == NULL || buffer == NULL ) {
                return result;
        }

        unsigned int bytes = width*height*channels*sizeof(Jaus::Byte);


        if( channels == 3 || channels == 1) {

            if( len < bytes + 100 || *buffer == NULL ) {
                //  If buffer is already pointing to allocated
                //  memory, make sure we delete it before asking
                //  for more.
                if( (*buffer) ) {
                    delete[] (*buffer);
                    (*buffer) = NULL;
                }
                (*buffer) = new Jaus::Byte[bytes + 100];
                assert( (*buffer) );
                len = bytes + 100;
            }

            struct jpeg_compress_struct cinfo;
            struct jpeg_error_mgr jerr;

            JSAMPROW row_pointer[1];    /* pointer to JSAMPLE row[s] */
            int row_stride;            /* physical row width in image buffer */
            cinfo.err = jpeg_std_error(&jerr);


            jpeg_create_compress(&cinfo);



            VirtualFile *vfPtr = ((VirtualFile*)(&mVirtualFile));
            vfPtr->Open (  (char*)(*buffer), len);

            jpeg_stdio_dest(&cinfo, *vfPtr); 

            cinfo.image_width = width;     /* image width and height, in pixels */
            cinfo.image_height = height;
            cinfo.input_components = channels;        /* # of color components per pixel */

            cinfo.in_color_space = channels > 1 ? JCS_RGB : JCS_GRAYSCALE;


            jpeg_set_defaults(&cinfo);
            /* Now you can set any non-default parameters you wish to.
            * Here we just illustrate the use of quality (quantization table) scaling:
            */
            //jpeg_set_quality(&cinfo, 1, TRUE /* limit to baseline-JPEG values */);
            jpeg_start_compress(&cinfo, TRUE);



            row_stride = width * channels;    /* JSAMPLEs per row in image_buffer */

            while (cinfo.next_scanline < cinfo.image_height) {
                /* jpeg_write_scanlines expects an array of pointers to scanlines.
                * Here the array is only one element long, but you could pass
                * more than one scanline at a time if that's more convenient.
                */
                row_pointer[0] = & ((Jaus::Byte*)(rawImage)) [cinfo.next_scanline * row_stride];
                (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

            }


            // this should be 4096 but I added 10 because of Premature end of JPEG file
            clen = vfPtr->GetCount() + (4106-cinfo.dest->free_in_buffer);

            jpeg_finish_compress(&cinfo);
            jpeg_destroy_compress(&cinfo);

            result = JAUS_OK;

        }
        else {
            SetJausError(ErrorCodes::InvalidValue);
        }

    }
    catch( char * str){

        // Trying to catch an exception here
        std::cout<< str<<std::endl;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief The function saves a frame to file. The format will be compressed based
///   on the extension of the file 
///
///   \param file The name of the file
///
///   \return JAUS_OK on success, JAUS_FAILURE on failure                         
///
////////////////////////////////////////////////////////////////////////////////////
int Image::SaveFrame(const std::string& file) const
{
    int result = JAUS_FAILURE;
    FILE *fp = NULL;
    Format format = GetFormat(file);
    Jaus::Byte *buffer = NULL;
    unsigned len = NULL;
    unsigned int clen = NULL;

    if( format == Unused ) {
        SetJausError(ErrorCodes::InvalidValue);
    }

    if( Compress(&buffer, len, clen, format ) ) {
        fp = fopen( file.c_str() , "w+b");
        if( fp && fwrite(buffer, clen, 1, fp) ) {
            result = JAUS_OK;
        }
    }

    if(fp)
        fclose(fp);
    if( buffer )
        delete[] buffer;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief The function loads image to internal structure. The image will be 
///   decompressed based on the extension of the file 
///
///   \param file The name of the file
///
///   \return JAUS_OK on success, JAUS_FAILURE on failure                         
///
////////////////////////////////////////////////////////////////////////////////////
int Image::LoadFrame(const std::string& file)
{
    Format format = GetFormat(file);
    int result = JAUS_FAILURE;
    unsigned int len = 0;
    unsigned char* buffer = NULL;

    //  Must handle video sequences differently.
    if( format == MPEG2 ||
        format == MPEG4 ||
        format == MJPEG ||
        format == H263 ||
        format == H264 ) {

            //  Add video code in future... maybe.
            return JAUS_FAILURE;
    }

    FILE *fp = fopen(file.c_str(), "r+b");
    if( fp ) {
        //  Add up number of bytes in file so
        //  we can read it all in for decompression.
        while( !feof(fp)) {
            fgetc(fp);
            len++;
        }
        --len;
        if( len > 0 ) {
            buffer = new unsigned char[len + 1];
            rewind(fp);
            if( fread( buffer, len, 1, fp) ) {
                result = Decompress(buffer, len, format);
            }
        }
    }
    if(fp)
        fclose(fp);
    if( buffer ) {
        delete[] buffer;
    }

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief The function returns the format based on the extension 
///   of the file 
///
///   \param file The name of the file
///
///   \return The image format defined in jimageformat.h
///
////////////////////////////////////////////////////////////////////////////////////
Image::Format Image::GetFormat(const std::string& file)
{
    if ( strstr(file.c_str(), ".ppm") || strstr(file.c_str(), ".PPM") )
    {
        return PPM;
    }
    else if ( strstr(file.c_str(), ".png") || strstr(file.c_str(), ".PNG")  )
    {
        return PNG;
    }
    else if ( strstr(file.c_str(), ".pgm") || strstr(file.c_str(), ".PGM") )
    {
        return PGM;
    }
    else if ( strstr(file.c_str(), ".jpeg") || strstr(file.c_str(),".JPEG") || 
              strstr(file.c_str(), ".JPG")  || strstr(file.c_str(), ".jpg") )
    {
        return JPEG;
    }

    return Unused;
}    

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copies contents of image.
///
////////////////////////////////////////////////////////////////////////////////////
Image& Image::operator=(const Image& img)
{
    if(this != &img) {
        if (mHeight != img.mHeight || mWidth != img.mWidth || mChannels != img.mChannels) 
            Destroy();
        if (img.mpImage != NULL) {
            if (mpImage == NULL){
                Create(img.mWidth, img.mHeight, img.mChannels, img.mpImage);
            }
            else {
                memcpy(mpImage, img.mpImage, mDataSize);
            }
        }

    }
    return *this;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Decompress' The buffer into internal structure of the image
///
///   \param compressed The buffer with data
///
///   \param len The size of the buffer
///
///   \param format The format of the data in the compressed buffer
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::Decompress(const Jaus::Byte* compressed, 
                      const unsigned int len, 
                      const Format format)
{
    int result = JAUS_OK;

    switch (format)
    {
    case PGM:
       result = DecompressPGM(compressed, len);
       break;
    case PPM:
       result = DecompressPPM(compressed, len);
       break;
    case PNG:
       result = DecompressPNG(compressed, len);
       break;
    case JPEG:
       result = DecompressJPEG(compressed, len);
       break;
    case MJPEG:
       result = DecompressJPEG(compressed, len);
       break;
    default:
       result = JAUS_FAILURE;
       break;
    }    
    if(result == JAUS_FAILURE)
    {
        // Try guess what it is.
        if(DecompressJPEG(compressed, len))
            return JAUS_OK;
        if(DecompressPPM(compressed, len))
            return JAUS_OK;
        if(DecompressPNG(compressed, len))
            return JAUS_OK;
    }
    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Decompress' The buffer with PPM (3 channels) compressed image data into 
///   internal structure of the image (only P6 is supported - binary) 
///
///   \param compressed The buffer with data
///
///   \param len The size of the buffer
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::DecompressPPM(const Jaus::Byte* compressed, const unsigned int len)
{

    unsigned int width;
    unsigned int height;
    unsigned int temp;
    char *ptr = (char *)compressed;


    int params[3];
    int cur_param=0;
    char num[100];
    int channels = 0;

    memset(params, 0, sizeof(int)*3);

    if (ptr[0] == 'P' && (ptr[1] == '6')){
        int cur = 0;
        int found = 0;
        for (int i=3; i< 2000; i++){
            
            if (ptr[i] == '#'){
                while( ptr[i++] != '\n');
            }

            if (isdigit(ptr[i])){
                num[cur++] = ptr[i];
                found = 1;
            }
            else if (found){
                found = 0;
                num[cur] = '\0';
                params[cur_param++] = atoi(num);               
                cur = 0;
                if (cur_param==3) break;
            }
        }
    }

    if (cur_param == 3){
        width =  params[0];
        height = params[1];

        //if (ptr[1] == '5') 
        //    channels = 1;
        //else 

        if (ptr[1] == '6')
            channels = 3;


        temp = width * height *channels;
        if( len > temp ) {
            return Create( (UShort)(width), (UShort)(height), (Byte)(channels), &compressed[ len - temp ] );
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Decompress' The buffer with PGM (1 channels) compressed image data into 
///   internal structure of the image (only P5 is supported) 
///
///   \param compressed The buffer with data
///
///   \param len The size of the buffer
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::DecompressPGM(const Jaus::Byte* compressed, const unsigned int len)
{

    unsigned int width;
    unsigned int height;
    unsigned int temp;
    char *ptr = (char *)compressed;


    int params[3];
    int cur_param=0;
    char num[100];
    int channels = 0;

    memset(params, 0, sizeof(int)*3);

    if (ptr[0] == 'P' && (ptr[1] == '5')){
        int cur = 0;
        int found = 0;
        for (int i=3; i< 2000; i++){
            if (ptr[i] == '#'){
                while( ptr[i++] != '\n');
            }


            if (isdigit(ptr[i])){
                num[cur++] = ptr[i];
                found = 1;
            }
            else if (found){
                found = 0;
                num[cur] = '\0';
                params[cur_param++] = atoi(num);               
                cur = 0;
                if (cur_param==3) break;
            }
        }
    }

    if (cur_param == 3){
        width =  params[0];
        height = params[1];

        if (ptr[1] == '5') 
            channels = 1;
        else if (ptr[2] == '6')
            channels = 3;


        temp = width * height *channels;
        if( len > temp ) {
            return Create( (UShort)(width), (UShort)(height), (Byte)(channels), &compressed[ len - temp ] );
        }
    }

    return JAUS_FAILURE;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Decompress' The buffer with JPEG (1 or 3 channels) compressed image 
///   data into internal structure of the image 
///
///   \param compressed The buffer with data
///
///   \param len The size of the buffer
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::DecompressJPEG(const Jaus::Byte* compressed, const unsigned int len)
{
    int result = JAUS_FAILURE;


    unsigned int width = 0;
    unsigned int height = 0;
    Jaus::Byte channels = 1;

    // Open Virtual File
    VirtualFile *vfPtr = ((VirtualFile*)(&mVirtualFile));
    vfPtr->Open((char *) compressed, len);



    //  libjpeg woodoo
    struct jpeg_decompress_struct cinfo;
    JSAMPARRAY buffer;        /* Output row buffer */
    int row_stride;        /* physical row width in output buffer */
    struct my_error_mgr {
        struct jpeg_error_mgr pub;    /* "public" fields */
        jmp_buf setjmp_buffer;    /* for return to caller */
    };
    struct my_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr.pub);


    /* Now we can initialize the JPEG decompression object. */
    jpeg_create_decompress(&cinfo);

    /* Step 2: specify data source (eg, a file) */
    jpeg_stdio_src(&cinfo, *vfPtr);

    /* Step 3: read file parameters with jpeg_read_header() */
    (void) jpeg_read_header(&cinfo, TRUE);

    /* Step 5: Start decompressor */
    (void) jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;

    /* Make a one-row-high sample array that will go away when done with image */
    buffer = (*cinfo.mem->alloc_sarray)
        ((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

    /* Here we use the library's state variable cinfo.output_scanline as the
    * loop counter, so that we don't have to keep track ourselves.
    */
    int i=0;

    width = cinfo.output_width;
    height = cinfo.output_height;

    if (cinfo.out_color_space == JCS_RGB) 
        channels = 3;
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
        channels = 1;


    Jaus::Byte * rawImage = new Jaus::Byte[height*width*channels+100];
    assert(rawImage);


    while (cinfo.output_scanline < cinfo.output_height) {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(rawImage+i, buffer[0], row_stride);
        i+=row_stride;
    }


    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    result = Create((UShort)(width), (UShort)(height), (Byte)(channels), rawImage);
    delete[] rawImage;

    return result;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Converts the input (image data with 3 channels) to 1 channel
///   the formula used: value = r*0.3 + g*0.59 + b*0.11
///
///   \param width The width of the image
///
///   \param height The height of the image
///
///   \param input The image data with 3 channels
///
///   \param output The converted image data
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::ConvertToGrayscale(const UShort width,
                               const UShort height,
                               const Jaus::Byte* input,
                               Jaus::Byte* output)
{
    if(width > 0 && height > 0 && input && output ) {
        Jaus::Byte *ptr1, *ptr2;
        ptr1 = (Jaus::Byte*)input;
        ptr2 = output;
        for(unsigned int i = 0; i < ((unsigned int)width)*height; i++ ) {
            ptr2[i] = (Jaus::Byte)( ptr1[i*3]*0.3 + ptr1[i*3+1]*.59 + ptr1[i*3+2]*.11 );
        }
        return JAUS_OK;
    }
    return JAUS_FAILURE;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Decompress' The buffer with PNG (1 or 3 channels) compressed image 
///   data into internal structure of the image 
///
///   \param compressed The buffer with data
///
///   \param len The size of the buffer
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::DecompressPNG(const Jaus::Byte* compressed, const unsigned int len)
{
    int result = JAUS_OK;

    unsigned int width;
    unsigned int height;
    int channels = 0;

    png_structp      png_ptr = NULL;
    png_infop        info_ptr = NULL;
    png_byte *ptr = (png_byte *)compressed;

    int is_png = !png_sig_cmp(ptr, 0, 8);

    if (result && is_png){
        png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    }
    else result = JAUS_FAILURE;


    if (result && png_ptr){
        info_ptr = png_create_info_struct(png_ptr);
    }
    else result = JAUS_FAILURE;


    if (result && info_ptr){

        VirtualFile *vfPtr = ((VirtualFile*)(&mVirtualFile));
        vfPtr->Open((char *) compressed, len);

        png_init_io(png_ptr, mVirtualFile);  

        png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16, NULL);


        width = info_ptr->width;
        height = info_ptr->height;

        png_bytep * row_pointers = NULL;
        row_pointers = NULL;


        if (info_ptr->color_type == PNG_COLOR_TYPE_RGB || 
            info_ptr->color_type == PNG_COLOR_TYPE_GRAY){

                if (info_ptr->color_type == PNG_COLOR_TYPE_RGB)
                    channels = 3;
                else if (info_ptr->color_type == PNG_COLOR_TYPE_GRAY)
                    channels = 1;


                row_pointers = png_get_rows(png_ptr, info_ptr);

                Jaus::Byte * rawImage = new Jaus::Byte[height*width*channels+100];

                for (unsigned int i=0; i< height; i++){
                    memcpy( rawImage + i*width*channels, row_pointers[i], sizeof(Jaus::Byte)*width*channels);
                    //    delete[] row_pointers[i];
                }

                Create((UShort)(width), (UShort)(height), (Byte)(channels), rawImage);


                delete[] rawImage;
                // delete[] row_pointers;
        }
        else 
            result = JAUS_FAILURE;

    }
    else 
        result = JAUS_FAILURE;

    if(png_ptr && info_ptr)
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
    else if(png_ptr)
        png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
    


    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Converts the image to grayscale, the image must have 3 channels
///
///   \return JAUS_OK on success, JAUS_JAILURE on failure
///
////////////////////////////////////////////////////////////////////////////////////
int Image::ConvertToGrayscale(){

    //      Generate a grayscale image.

    if (mWidth==0 || mHeight == 0 || !mpImage || mChannels != 3) return JAUS_FAILURE;

    Jaus::Byte * buff = new Jaus::Byte[mWidth*mHeight*3];

    ConvertToGrayscale(mWidth, mHeight, mpImage, buff);
    int result = Create(mWidth, mHeight, 1, buff); 

    delete[]buff;

    return result;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief If the image data contains 3 channels, the outer channels are flipped.
///   This will convert from RGB to BGR, etc.
///
////////////////////////////////////////////////////////////////////////////////////
void Image::FlipChannels()
{
    if(mChannels == 3)
    {
        unsigned char *ptr = mpImage;
        unsigned char temp;
        for(unsigned int i = 0; i < mDataSize; i+= 3)
        {
            temp = *ptr;
            *ptr = *(ptr + 2);
            *(ptr + 2) = temp;
            ptr += 3;
        }
    }
}

/* End of File */