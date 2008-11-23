////////////////////////////////////////////////////////////////////////////////////
///
///  \file jpeg.cpp
///  \brief This file contains methods for compression and decompression
///         of JPEG data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 21 November 2008
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
#include "jaus/video/jpeg/jpeg.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <jpeglib.h>
#include <setjmp.h>

#define JPEG_BUFFER_SIZE 4096

namespace Jaus
{
    namespace JPEG
    {
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \struct InputSource
        ///   \brief Input source for JPEG decompression manager.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        struct InputSource
        {
            struct
            {
                struct jpeg_error_mgr mManager;
                jmp_buf mSetJumpBuffer;
            } mErrorManager;

            unsigned char* mpImage;                       ///<  Pointer to JPEG image data.
            unsigned int mImageSize;                      ///<  Size of the JPEG image in bytes.
            unsigned int mNumBytesRead;                   ///<  Total number of bytes decompressed/read so far.
            unsigned char mpBuffer[JPEG_BUFFER_SIZE];     ///<  Buffer storing JPEG data to decompress.
        };

        // Initializes source (does nothing).
        void InitSource(j_decompress_ptr cinfo)
        {
        }


        // Reads data from the compressed image and puts it into 
        // a buffer for decompression by jpeg decompressor
        boolean FillInputBuffer(j_decompress_ptr cinfo)
        {
            struct InputSource* input = (struct InputSource*)cinfo->client_data;

            if(input->mImageSize <= 0)
            {
                // Insert fake EOI marker
                input->mpBuffer[0] = (JOCTET)0xFF;
                input->mpBuffer[1] = (JOCTET)JPEG_EOI;
                cinfo->src->bytes_in_buffer = 2;
            }
            if((unsigned int)(input->mImageSize - input->mNumBytesRead) < (unsigned int)JPEG_BUFFER_SIZE)
            {
                memcpy(input->mpBuffer, &input->mpImage[input->mNumBytesRead], input->mImageSize - input->mNumBytesRead);
                cinfo->src->bytes_in_buffer = input->mImageSize - input->mNumBytesRead;
            }
            else
            {
                memcpy(input->mpBuffer, &input->mpImage[input->mNumBytesRead], JPEG_BUFFER_SIZE);
                input->mNumBytesRead += JPEG_BUFFER_SIZE;
                cinfo->src->bytes_in_buffer = JPEG_BUFFER_SIZE;
            }
            cinfo->src->next_input_byte = input->mpBuffer;
            return TRUE;
        }

        // Handles skiped bytes.
        void SkipInputData(j_decompress_ptr cinfo, long num_bytes)
        {
            if(num_bytes > 0)
            {
                while(num_bytes > (long)cinfo->src->bytes_in_buffer)
                {
                    num_bytes -= (long)cinfo->src->bytes_in_buffer;
                    FillInputBuffer(cinfo);
                }
                cinfo->src->next_input_byte += (size_t)num_bytes;
                cinfo->src->bytes_in_buffer -= (size_t)num_bytes;
            }
        }

        // Terminates source data, does nothing for memory buffer.
        void TerminateSource(j_decompress_ptr cinfo)
        {
        }


        // Handles exit on errors.
        void ErrorExit(j_common_ptr cinfo)
        {
            struct InputSource* inputSource = (struct InputSource*)(cinfo->client_data);
            longjmp(inputSource->mErrorManager.mSetJumpBuffer, 1);
        }
    }
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes JPEG data inside of a buffer and decompresses it to
///   another buffer which will store the raw image.
///
///   \param[in] jpeg Pointer to JPEG data.
///   \param[in] jpegSize The size in bytes of the JPEG data.
///   \param[out] image Pointer to buffer which will store the result.  If NULL
///                     or too small new memory will be allocated dynamically.
///   \param[out] width The width of the image decompressed.
///   \param[out] height The height of the image decompressed.
///   \param[out] channels The number of channels in the decompressed image.
///
///   \return 1 on success, 0 on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Jaus::JPEG::DecompressImage(const unsigned char* jpeg,
                                const unsigned int jpegSize,
                                unsigned char** image,
                                unsigned short* width,
                                unsigned short* height,
                                unsigned char* channels)
{
    int result = 0;

    *width = *height = *channels = 0;

    // Create an input source for the JPEG source manager.
    struct InputSource inputSource;
    inputSource.mpImage = (unsigned char*)jpeg;
    inputSource.mImageSize = jpegSize;
    inputSource.mNumBytesRead = 0;

    // Create an input source manager for JPEG decompressor.
    jpeg_source_mgr mgr;
    mgr.bytes_in_buffer = 0;
    mgr.next_input_byte = NULL;
    mgr.init_source             = InitSource;
    mgr.fill_input_buffer       = FillInputBuffer;
    mgr.skip_input_data         = SkipInputData;
    mgr.resync_to_restart       = jpeg_resync_to_restart; // Default from library
    mgr.term_source             = TerminateSource;

    // Create a JPEG compression structure.
    struct jpeg_decompress_struct cinfo;
    // Initialize it.
    jpeg_create_decompress(&cinfo);

    // Set the input data for decompression.
    cinfo.client_data = &inputSource;
    // Set the error handler.
    cinfo.err = jpeg_std_error(&inputSource.mErrorManager.mManager);
    inputSource.mErrorManager.mManager.error_exit = ErrorExit;
    // Set jump state on error for decompressor, if failed
    // exit.
    if(setjmp(inputSource.mErrorManager.mSetJumpBuffer))
    {
        jpeg_destroy_decompress(&cinfo);
        return result;
    }
    
    // Assign the source manager to the decompressor.
    cinfo.src = &mgr;

    // Read the JPEG header data.
    jpeg_read_header(&cinfo, TRUE);
    // Start decompression.
    jpeg_start_decompress(&cinfo);

     // Do we support the number of color components?
    // We only support grayscale or RGB images.
    if(cinfo.output_components != 1 && cinfo.output_components != 3)
    {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return result;
    }

    // Double check the color space that we support.
    if (cinfo.out_color_space == JCS_RGB)
        *channels = (unsigned char)3;
    else if (cinfo.out_color_space == JCS_GRAYSCALE)
        *channels = (unsigned char)1;
    else
    {
        jpeg_finish_decompress(&cinfo);
        jpeg_destroy_decompress(&cinfo);
        return result;
    }
    
    // Make a one-row high sample array that will be deleted when
    // done decompressing.  This will be used to store temp data.
    int row_stride = cinfo.output_width * cinfo.output_components;
    JSAMPARRAY buffer = NULL;
    buffer = (*cinfo.mem->alloc_sarray)
             ((j_common_ptr) &cinfo,
             JPOOL_IMAGE,
             row_stride,
             1);

    // Calculate size of output image in bytes.
    unsigned int outputSize;
    outputSize = cinfo.output_components*cinfo.output_width*cinfo.output_height;

    // Allocate memory to store the decompressed image.
    if(*image != NULL)
    {
        // See if we need to re-allocate. 
        if(outputSize >= (unsigned int)(*width)*(*height)*(*channels))
        {
            delete[] *image;
            *image = new unsigned char[outputSize + 100];
        }
    }
    else
    {
        *image = new unsigned char[outputSize + 100];
    }

    *width = (unsigned short)cinfo.output_width;
    *height = (unsigned short)cinfo.output_height;
    *channels = (unsigned char)cinfo.output_components;
    
    unsigned char* ptr = *image;
    // Now decompress each scanline in the image.
    while (cinfo.output_scanline < cinfo.output_height) 
    {
        (void) jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(ptr, buffer[0], row_stride);
        ptr += row_stride;
    }

    (void) jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    
    result = 1;

    return result;
}

namespace Jaus
{
    namespace JPEG
    {

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \struct OutputSource
        ///   \brief Output source for JPEG compression manager.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        struct OutputSource
        {
            struct jpeg_destination_mgr mDestinationManager;    ///<  Destination manager.
            unsigned char mOutputBuffer[JPEG_BUFFER_SIZE];      ///<  Output buffer to pull data for compression from.
            unsigned char* mpCompressedJPEG;                    ///<  Where to write compressed data to.
            unsigned int mCompressionBufferSize;                ///<  Total memory available for compressed data.
            unsigned int mNumBytesCompressed;                   ///<  Size of compressed JPEG inside of mpCompressedJPEG.
        };


        // Initializes destination manager.
        void InitDestination (j_compress_ptr cinfo)
        {
          struct OutputSource *dest = (struct OutputSource *) cinfo->dest;
          dest->mDestinationManager.next_output_byte = dest->mOutputBuffer;
          dest->mDestinationManager.free_in_buffer = JPEG_BUFFER_SIZE;
        }


        // Takes data from raw image and writes to temp buffer for compression.
        boolean EmptyOutputBuffer (j_compress_ptr cinfo)
        {
          struct OutputSource *dest = (struct OutputSource *) cinfo->dest;

          // See if we need to allocate more memory.
          if(dest->mpCompressedJPEG == NULL || dest->mNumBytesCompressed + JPEG_BUFFER_SIZE >= dest->mCompressionBufferSize)
          {
              unsigned char *tempOutput = new unsigned char[dest->mCompressionBufferSize + JPEG_BUFFER_SIZE*2];
              if(dest->mpCompressedJPEG)
              {
                  memcpy(tempOutput, dest->mpCompressedJPEG, dest->mCompressionBufferSize);
                  delete[] dest->mpCompressedJPEG;
              }
              dest->mpCompressedJPEG = tempOutput;
              dest->mCompressionBufferSize += JPEG_BUFFER_SIZE*2;
          }
          memcpy(&dest->mpCompressedJPEG[dest->mNumBytesCompressed], dest->mOutputBuffer, JPEG_BUFFER_SIZE);
          dest->mNumBytesCompressed += JPEG_BUFFER_SIZE;
          dest->mDestinationManager.next_output_byte = dest->mOutputBuffer;
          dest->mDestinationManager.free_in_buffer = JPEG_BUFFER_SIZE;

          return TRUE;
        }


        // Writes the last remaining bytes of the image for compression.
        void TerminateDestination(j_compress_ptr cinfo)
        {
            OutputSource *dest = (OutputSource *) cinfo->dest;
            unsigned int datacount = JPEG_BUFFER_SIZE - dest->mDestinationManager.free_in_buffer;
            // Write any remaining data.

            // See if we need to allocate more memory.
            if(dest->mpCompressedJPEG == NULL || dest->mNumBytesCompressed + datacount >= dest->mCompressionBufferSize)
            {
                unsigned char *tempOutput = new unsigned char[dest->mCompressionBufferSize + datacount];
                if(dest->mpCompressedJPEG)
                {
                    memcpy(tempOutput, dest->mpCompressedJPEG, dest->mCompressionBufferSize);
                    delete[] dest->mpCompressedJPEG;
                }
                dest->mpCompressedJPEG = tempOutput;
                dest->mCompressionBufferSize += datacount;
            }

            memcpy(&dest->mpCompressedJPEG[dest->mNumBytesCompressed], dest->mOutputBuffer, datacount);
            dest->mNumBytesCompressed += datacount;
            dest->mDestinationManager.next_output_byte = dest->mOutputBuffer;
            dest->mDestinationManager.free_in_buffer = JPEG_BUFFER_SIZE;
        }
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compresses RAW image data to JPEG format.
///
///   \param[in] image Pointer to raw image data that needs to be compressed.
///   \param[in] width The width of the image.
///   \param[in] height The height of the image.
///   \param[in] channels The number of channels in the image.
///   \param[out] jpeg Pointer to buffer which will store resulting JPEG
///                    data.  If NULL or too small this buffer will grow
///                    dynamically.
///   \param[out] jpegBufferSize The size of the buffer in bytes holding the
///                              compressed JPEG.
///   \param[out] jpegSize The size of the JPEG data inside the buffer in bytes.
///   \param[in] quality The compression quality.  Set to -1 for default, 0 worst
///                      and 100 best (but larger size).
///
///   \return 1 on success, 0 on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Jaus::JPEG::CompressImage(const unsigned short width,
                              const unsigned short height,
                              const unsigned char channels,
                              const unsigned char* image,
                              unsigned char** jpeg,
                              unsigned int* jpegBufferSize,
                              unsigned int* jpegSize,
                              const int quality)
{
    int result = 0;

    if(width > 0 && 
        height > 0 && 
        (channels == 1 || channels == 3) &&
        image != 0 && 
        jpeg != 0)
    {
        unsigned int imageSize = width*height*channels*sizeof(unsigned char);
        // Reset the compressed JPEG size to 0.
        *jpegSize = 0;

        // If necessary, allocate some memory to store
        // the resulting compressed image.
        if(*jpeg == NULL || jpegBufferSize == 0)
        {
            if(*jpeg)
            {
                delete[] *jpeg;
                *jpeg = 0;
            }
            *jpeg = new unsigned char[imageSize];
            assert( *jpeg );
            *jpegBufferSize = imageSize;
        }

        // Create a JPEG compression structure and an error manager.
        struct jpeg_compress_struct cinfo;
        struct jpeg_error_mgr jerr;

        // Create a compression object.
        jpeg_create_compress(&cinfo);
        cinfo.err = jpeg_std_error(&jerr);
        // Create a compression output destination object.
        OutputSource dest;

        // Initialize output destination manager.
        dest.mpCompressedJPEG = *jpeg;
        dest.mCompressionBufferSize = *jpegBufferSize;
        dest.mNumBytesCompressed = 0;
        dest.mDestinationManager.init_destination = InitDestination;
        dest.mDestinationManager.empty_output_buffer = EmptyOutputBuffer;
        dest.mDestinationManager.term_destination = TerminateDestination;

        cinfo.dest = (struct jpeg_destination_mgr *)&dest;

        cinfo.image_width = width;    
        cinfo.image_height = height;
        cinfo.input_components = channels; 
        cinfo.in_color_space = channels > 1 ? JCS_RGB : JCS_GRAYSCALE;

        // Sets default values.
        jpeg_set_defaults(&cinfo);

        // Adjust quality if set.
        if(quality >= 0)
        {
            jpeg_set_quality(&cinfo, quality, TRUE);
        }

        // Start compression process
        jpeg_start_compress(&cinfo, TRUE);

        JSAMPROW row_pointer[1];    /// JSAMPLE row to store a single compressed row.
        int row_stride;             /// Physical row width in image buffer (width*channels).
        row_stride = width*channels;
        
        while (cinfo.next_scanline < cinfo.image_height)
        {
            // jpeg_write_scanlines expects an array of pointers to scanlines.
            // Here the array is only one element long, but you could pass
            // more than one scanline at a time if that's more convenient.
            row_pointer[0] = & ((unsigned char *)(image)) [cinfo.next_scanline * row_stride];
            (void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

        }

        // Finish compression (finalize).
        jpeg_finish_compress(&cinfo);

        // Save results
        *jpegSize = dest.mNumBytesCompressed;
        *jpegBufferSize = dest.mCompressionBufferSize;
        *jpeg = dest.mpCompressedJPEG; // Make sure pointer is saved.

        // Destroy the compression object.
        jpeg_destroy_compress(&cinfo);

        result = 1;
    }

    return result;
}


/* End of File */
