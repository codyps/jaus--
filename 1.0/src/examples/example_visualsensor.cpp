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

int LoadNumbers(std::vector<Image>& numbers)
{
    int result = 0;
    Image img;
    numbers.clear();
    char buffer[256];

    for(int i = 1; i <= 9; i++)
    {
        sprintf(buffer, "images/%d.jpg", i);
        if(img.LoadFrame(buffer) == JAUS_FAILURE)
        {
            return 0;
        }
        numbers.push_back(img);
    }
    return 1;
}


void ApplyNumber(const unsigned int num, 
                 const std::vector<Image>& numbers, 
                 const Image& original, 
                 Image& output)
{
    Byte* ptr1,* ptr2;

    output = original;

    ptr1 = numbers[num].ImageData();
    ptr2 = output.ImageData();

    for(UShort i = 0; i < numbers[num].Height(); i++)
    {
        for(UShort j = 0; j < numbers[num].Width(); j++)
        {
            if(i < output.Width() && j < output.Height())
            {
                if(ptr1[i*numbers[num].Width()*3 + j*3] == 254 &&
                    ptr1[i*numbers[num].Width()*3 + j*3 + 1] == 0 &&
                    ptr1[i*numbers[num].Width()*3 + j*3 + 2] == 0)
                {
                }
                else
                {
                    ptr2[i*output.Width()*3 + j*3] = ptr1[i*numbers[num].Width()*3 + j*3];
                    ptr2[i*output.Width()*3 + j*3 + 1] = ptr1[i*numbers[num].Width()*3 + j*3 + 1];
                    ptr2[i*output.Width()*3 + j*3 + 2] = ptr1[i*numbers[num].Width()*3 + j*3 + 2];
                }
            }
        }
    }
}


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
    camera.SetFrameRate(30);
    // Load image data to simulate video.
    if(original.LoadFrame("images/calculon640.jpg") == 0 ||
       LoadNumbers(numbers) == 0)
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
        ApplyNumber(currentNumber++, numbers, original, output);
        if(currentNumber > 8)
            currentNumber = 0;

        camera.SetCurrentFrame(output);
        //output.SaveFrame("images/test.jpg");
        cout << "Frame Number: " << camera.GetFrameNumber() << endl;
        if(CxUtils::GetChar() == 27)
        {
            gExitFlag = true;
        }
        Sleep(10);
    }

    return 0;
}

/*  End of File */
