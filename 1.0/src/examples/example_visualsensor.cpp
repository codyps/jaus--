////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_visualsensor.cpp
///  \brief Examples demonstrating/testing the Visual Sensor class.  The 
///  Visual Sensor class allows you to stream video data to other components.
///  As of this writing (June 12 2008) only image stream is supported, but full
///  audio video stream will be added at some point in the near future.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 13 March 2008
///  <br>Last Modified: 12 June 2008
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
#include <vector>
#include <vld.h>
#include "jaus/video/videolib.h"

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;


int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    VisualSensor camera;
    Image original, output;
    unsigned int currentNumber = 0;
    std::vector<Image> numbers;

    cout << "Looking for node manager...";
    while(gExitFlag == false)
    {
        if(Component::IsNodeManagerPresent(&nodeID))
        {
            cout << "Success!\n";
            cout << "Node Manager ID is: ";
            nodeID.PrintID();
            break;
        }
        Sleep(100);
    }
    if(nodeID.IsValid() == false)
    {
        cout << "Failure.\n";
        cout << "Exiting...";
        return 0;
    }


    // Set camera update rate.
    unsigned int frameNumber = 1;
    char fname[512];
    camera.SetFrameRate(30);
    // Load image data to simulate video.
    if(original.LoadFrame("video/mgs_frames/mgs_image_000.jpg") == 0)
    {
        cout << "Failed to load image data for streaming.\n";
        return 0;
    }

    cout << "Initializing Visual Sensor...";

    // Instance is set to 0, so the Initialize function will
    // use the first available instance number.
    if(camera.Initialize(nodeID.mSubsystem, nodeID.mNode, 0))
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        camera.PrintJausError();
        return 0;
    }

    Sleep(50);

    // Transition the sensor from the standy by state, 
    // which is default after initialization to 
    // a ready state.
    camera.SetPrimaryStatus(Component::Status::Ready);

    while(!gExitFlag)
    {
        // Load the next frame.
        sprintf(fname, "video/mgs_frames/mgs_image_%0.3d.jpg", frameNumber++);
        if(original.LoadFrame(fname) == FAILURE)
        {
            // Loop forever (start over at image 0).
            frameNumber = 0;
            sprintf(fname, "video/mgs_frames/mgs_image_%0.3d.jpg", frameNumber++);
            original.LoadFrame(fname);
        }

        camera.SetCurrentFrame(original);
        
        cout << "Frame Number: " << camera.GetFrameNumber() << endl;
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(33);
    }

    return 0;
}

/*  End of File */
