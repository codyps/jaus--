////////////////////////////////////////////////////////////////////////////////////
///
///  \file example_rangesensor.cpp
///  \brief Example program showing how to use the RangeSensor service class to
///  create a Range Sensor.  This program simulates data that would come from
///  something like a SICK LIDAR.  Data be visualized with the wxrangesensorclient
///  program.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 19 December 2008
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
#include "jaus/services/rangesensor.h"
#include <vector>
#ifdef WIN32
#include <vld.h>
#endif

#include <math.h>

using namespace std;
using namespace CxUtils;
using namespace Jaus;

int main(int argc, char *argv[])
{
    Address nodeID;     // ID of the node manager.
    RangeSensor sensor;

    cout << "Looking for node manager...";
    while(true)
    {
        if(Component::IsNodeManagerPresent(&nodeID))
        {
            cout << "Success!\n";
            cout << "Node Manager ID is: ";
            nodeID.PrintID();
            break;
        }
        if(CxUtils::GetChar() == 27)
        {
            return 0;
        }
        Sleep(100);
    }
    if(nodeID.IsValid() == false)
    {
        cout << "Failure.\n";
        cout << "Exiting...";
        return 0;
    }

    cout << "Initializing Range Sensor Component...";

    // Initialize the component with any instance ID.
    if(sensor.Initialize(nodeID.mSubsystem, nodeID.mNode))
    {
        cout << "Success!\n";
    }
    else
    {
        cout << "Failure!\n";
        return 0;
    }

    sensor.SetPrimaryStatus(Component::Status::Ready);
    
    ReportRelativeObjectPosition object;
    double range = 0;;

    cout << "Press Escape to Exit\n";
    CxUtils::SleepMs(1500);
    unsigned int scanNumber = 0;
    // ReportSickLidar is a non-JAUS standard message, but 
    // is useful to represent data from a SICK LIDAR.  If you use this
    // in conjuction to the standard messages you will be OK.
    ReportSickLidar reportLidar;
    ReportSickLidar::Scan scan;
    while(true)
    {
        // Generate fake range sensor data.  Bearing is around the local
        // coordinate system, so a value of 0 is in front of the vehicle, a value
        // of 45 degrees to the front right, and -45 degrees front left, etc.
        // This simulates a 180 degree scan from left to right.

        range = 8.0;
        UShort objectID = 0;
        bool decreaseRange = true;
        scan.clear();
        for(double bearing = -90.0; bearing <= 90.0; bearing += 0.5)
        {
            // Simulate SICK data with MM accuracy 180 degree scan half degree res.
            scan.push_back(abs((UShort)(range*100.0))); 

            // Populate JAUS standard data (Report Relative Object Position).
            object.SetRange(range);
            object.SetBearing(JAUS_DEG2RAD(bearing));
            object.SetInclination(0);
            object.SetObjectID(objectID++);
            object.SetTimeStamp(Jaus::Time::GetUtcTime());
            sensor.SetRelativeObjectPosition(object);

            if(decreaseRange)
            {
                range -= 0.2;
                if(range < 5.0)
                {
                    decreaseRange = false;
                }
            }
            else
            {
                range += 0.2;
                if(range > 8.0)
                {
                    decreaseRange = true;
                }
            }
        }

        reportLidar.SetScanData(scan, Jaus::Time::GetUtcTime(), ReportSickLidar::Millimeter, ReportSickLidar::OneEightyDegreesHalfRes);
        // Save to sensor.
        sensor.SetSickLidarData(reportLidar);

        cout << "Scan: " << scanNumber++ << endl;

        if(CxUtils::GetChar() == 27)
        {
            break;
        }
        CxUtils::SleepMs(15);
    }

    return 0;
}


/*  End of File */
