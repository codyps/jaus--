////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_images.cpp
///  \brief Example program showing how to use the Image class
///  of the Video Library to load/save and compress/decompress image/video
///  data.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 10 June 2008
///  <br>Last Modified: 10 June 2008
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
#include <iostream>
#include <iomanip>
#include "jaus/jaus++.h"
#include "jaus/video/image.h"
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <vld.h>
#endif


using namespace std;
using namespace CxUtils;
using namespace Jaus;

int main(int argc, char *argv[])
{
    Image frame, grayscale;
    Image scaledFrame;

    // Load a saved file.
    frame.LoadFrame("images/calculon640.jpg");

    // Save the image in different formats.
    frame.SaveFrame("images/calculon_copy.jpg");
    frame.SaveFrame("images/calculon_copy.png");
    frame.SaveFrame("images/calculon_copy.ppm");
    grayscale = frame;
    grayscale.ConvertToGrayscale();
    grayscale.SaveFrame("images/calculon_copy.pgm");

    scaledFrame.Create(frame.Width(), frame.Height(), frame.Channels(), frame.ImageData(), 0.5, false);
    scaledFrame.SaveFrame("images/calculon_small_copy.jpg");

    scaledFrame.Create(frame.Width(), frame.Height(), frame.Channels(), frame.ImageData(), 1.5, false);
    scaledFrame.SaveFrame("images/calculon_big_copy.jpg");

    // Make the original image fit inside a specific size
    scaledFrame.Create(frame.Width(), frame.Height(), frame.Channels(), frame.ImageData(), 200, 480, false);
    scaledFrame.SaveFrame("images/calculon_size_fit1.jpg");

    scaledFrame.Create(frame.Width(), frame.Height(), frame.Channels(), frame.ImageData(), 512, 512, false);
    scaledFrame.SaveFrame("images/calculon_size_fit2.jpg");

    scaledFrame.Create(frame.Width(), frame.Height(), frame.Channels(), frame.ImageData(), 320, 240, false);
    scaledFrame.SaveFrame("images/calculon_size_fit3.jpg");

    // Checking compression rates for JPEG
    Byte* compressedImage = 0;
    unsigned int compressedSize = 0;
    unsigned int compressedBufferSize = 0;
    
    frame.LoadFrame("images/calculon640.jpg");
    // Pre-allocate some memory.
    compressedImage = new Byte[frame.DataSize()];
    compressedBufferSize = frame.DataSize();

    
    cout << "Checking compression performance, press Escape to skip..\n";
    double startTimeMs = CxUtils::Timer::GetTimeMs(), endTimeMs = 0;
    unsigned int count = 0;
    for(unsigned int i = 0; i < 500; i++)
    {
        if(CxUtils::GetChar() == 27)
        {
            break;
        }

        frame.Compress(&compressedImage, compressedBufferSize, compressedSize, Image::JPEG);
        count++;
        if(i%100 == 0)
        {
            cout << "FPS = " << count*1000.0/(CxUtils::Timer::GetTimeMs() - startTimeMs) << endl;
        }
    }
    endTimeMs = CxUtils::Timer::GetTimeMs();

    cout << "FPS = " << count*1000.0/(endTimeMs - startTimeMs) << endl;
    
    frame.Compress(&compressedImage, compressedBufferSize, compressedSize, Image::JPEG);

    cout << "Checking decompression performance, press Escape to skip..\n";
    startTimeMs = CxUtils::Timer::GetTimeMs();
    count = 0;
    for(unsigned int j = 0; j < 500; j++)
    {
        if(CxUtils::GetChar() == 27)
        {
            break;
        }
        frame.Decompress(compressedImage, compressedSize, Image::JPEG);
        count++;
        if(j%100 == 0)
        {
            cout << "FPS = " << count*1000.0/(CxUtils::Timer::GetTimeMs() - startTimeMs) << endl;
        }
    }
    endTimeMs = CxUtils::Timer::GetTimeMs();

    cout << "FPS = " << count*1000.0/(endTimeMs - startTimeMs) << endl;
   

    if(compressedImage)
    {
        delete[] compressedImage;
    }
 
    return 0;
}


/*  End of File */
