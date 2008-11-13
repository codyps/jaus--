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
#include <cxutils/cxutils.h>
#include <vector>
#ifdef WIN32
#include <conio.h>
#endif


using namespace std;
using namespace CxUtils;
using namespace Jaus;

int main(int argc, char *argv[])
{
    Image frame;
    Image scaledFrame;

    // Load a saved file.
    frame.LoadFrame("images/calculon640.jpg");

    // Save the image in different formats.
    frame.SaveFrame("images/calculon_copy.jpg");
    frame.SaveFrame("images/calculon_copy.png");
    frame.SaveFrame("images/calculon_copy.ppm");

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

    return 0;
}


/*  End of File */
