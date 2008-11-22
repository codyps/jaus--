////////////////////////////////////////////////////////////////////////////////////
///
///  \file pgm.cpp
///  \brief This file contains methods for compression and decompression
///         of PGM Image data.
///
///  <br>Author(s): Sergey Leontyev
///  <br>Created: 21 November 2008
///  <br>Copyright (c) 2008
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
#include "jaus/video/pgm/pgm.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Takes PGM data inside of a buffer and decompresses it to
///   another buffer which will store the raw image.
///
///   \param[in] pgm Pointer to PGM data.
///   \param[in] pgmSize The size in bytes of the PGM data.
///   \param[out] image Pointer to buffer which will store the result.  If NULL
///                     or too small new memory will be allocated dynamically.
///   \param[out] width The width of the image decompressed.
///   \param[out] height The height of the image decompressed.
///   \param[out] channels The number of channels in the decompressed image.
///
///   \return 1 on success, 0 on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Jaus::PGM::DecompressImage(const unsigned char* pgm,
                               const unsigned int pgmSize,
                               unsigned char** image,
                               unsigned short* width,
                               unsigned short* height,
                               unsigned char* channels)
{
    int params[3];
    int cur_param=0;
    char num[100];
    
    unsigned int pgmWidth;
    unsigned int pgmHeight;
    unsigned char pgmChannels = 0;
    unsigned int size;

    memset(params, 0, sizeof(int)*3);

    if (pgm[0] == 'P' && (pgm[1] == '5'))
    {
        int cur = 0;
        int found = 0;
        for (int i=3; i < 2000; i++)
        {
            if (pgm[i] == '#')
            {
                while( pgm[i++] != '\n');
            }

            if (isdigit(pgm[i]))
            {
                num[cur++] = pgm[i];
                found = 1;
            }
            else if (found)
            {
                found = 0;
                num[cur] = '\0';
                params[cur_param++] = atoi(num);
                cur = 0;
                if (cur_param == 3) 
                {   
                    break;
                }
            }
        }
    }

    if (cur_param == 3)
    {
        pgmWidth =  params[0];
        pgmHeight = params[1];

        if (pgm[1] == '5')
            pgmChannels = 1;
        else if (pgm[2] == '6')
            pgmChannels = 3;

        size = pgmWidth * pgmHeight *pgmChannels;
        if( pgmSize > size ) 
        {
            if(*image == NULL || 
               pgmWidth != *width  ||
               pgmHeight != *height ||
               pgmChannels != *channels)
            {
                if(*image)
                {
                    delete[] *image;
                    *image = new unsigned char[size];
                }
            }
            *width = (unsigned short)pgmWidth;
            *height = (unsigned short)pgmHeight;
            *channels = (unsigned char)pgmChannels;
            memcpy(*image, &pgm[pgmSize - size], pgmSize - size);
            return 1;
        }
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Compresses RAW image data to PGM format.
///
///   \param[in] image Pointer to raw image data that needs to be compressed.
///   \param[in] width The width of the image.
///   \param[in] height The height of the image.
///   \param[in] channels The number of channels in the image.
///   \param[out] pgm Pointer to buffer which will store resulting PGM
///                    data.  If NULL or too small this buffer will grow
///                    dynamically.
///   \param[out] pgmBufferSize The size of the buffer in bytes holding the
///                              compressed PGM.
///   \param[out] pgmSize The size of the PGM data inside the buffer in bytes.
///   
///   \return 1 on success, 0 on failure.
///
////////////////////////////////////////////////////////////////////////////////////
int Jaus::PGM::CompressImage(const unsigned short width,
                             const unsigned short height,
                             const unsigned char channels,
                             const unsigned char* image,
                             unsigned char** pgm,
                             unsigned int* pgmBufferSize,
                             unsigned int* pgmSize)
{
    int result = 0;

    if( width == 0 || 
        height == 0 || 
        !(channels == 1) ||
        image == NULL || 
        pgm == NULL ) 
    {
        return result;
    }

    unsigned int bytes = width*height*channels*sizeof(unsigned char);
    *pgmSize = 0;

    if( channels == 1) 
    {
        int characters = 0;
        if( *pgmBufferSize < bytes + 100 || *pgm == NULL )
        {
            //  If pgm is already pointing to allocated
            //  memory, make sure we delete it before asking
            //  for more.
            if( (*pgm) ) 
            {
                delete[] (*pgm);
                (*pgm) = NULL;
            }
            (*pgm) = new unsigned char[bytes + 100];
            assert( (*pgm) );
            *pgmBufferSize = bytes + 100;
        }

        characters = sprintf( ( (char *) (*pgm) ) , "P5 %d %d %d\n", width, height, 255);
        memcpy( (*pgm) + characters , image, bytes );
        *pgmSize = bytes + characters;
        result = 1;
    }

    return result;
}

/*  End of File */
