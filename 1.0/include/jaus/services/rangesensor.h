////////////////////////////////////////////////////////////////////////////////////
///
///  \file rangesensor.h
///  \brief This file contains software for creating a JAUS Range
///  Sensor Component (ID 50).
///
///  <br>Author(s): David Adams
///  <br>Created: 06 October 2008
///  <br>Last Modified: 06 October 2008
///  <br>Copyright (c) 2008
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
///  <br>All rights reserved.
///  <br>Email: dadams@ist.ucf.edu
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
#ifndef __JAUS_RANGE_SENSOR__H
#define __JAUS_RANGE_SENSOR__H

#include "jaus/components/informcomponent.h"
#include "jaus/messages/experimental/sick/reportsicklidar.h"
#include "jaus/services/srvclibdll.h"
#include <cxutils/mutex.h>
#include <map>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class RangeSensor
    ///   \brief Range Sensor component structure. (Component ID 50).
    ///
    ///   This component reports range data to requesting components.  
    ///      The range data is measured relative to the platform coordinate 
    ///   system at the time reported.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_SRVC_DLL RangeSensor : public InformComponent
    {
    public:
        RangeSensor();
        ~RangeSensor();
        // Overloaded/simplified initialize method.
        virtual int Initialize(const Byte subsystem,
                               const Byte node,
                               const Byte instance = 0);
        // Sets up service for Range Sensor.
        virtual int SetupService();
        // Set a single relative object position.
        int SetRelativeObjectPosition(const ReportRelativeObjectPosition& state);
        // Sets a NON-STANDARD MESSAGE used for SICK LIDAR (use in conjuction with standard messages for interoperability).
        int SetSickLidarData(const ReportSickLidar& lidarScan);
        // Gets a specifc objects information.
        ReportRelativeObjectPosition GetRelativeObjectPosition() const;
        // Gets SICK LIDAR scan information if present.
        ReportSickLidar GetSickLidarData() const;
        // Responds to queries.
        virtual int ProcessQueryMessage(const Message* msg);
        // Generates events.
        virtual int GenerateEvent(const Event* eventInfo);
        // Method called when events are requested.
        virtual int ProcessEventRequest(const Jaus::CreateEventRequest& command,
                                        Byte& responseValue,
                                        double& confirmedRate,
                                        std::string& errorMessage) const;
    protected:
        ReportRelativeObjectPosition mRelativeObjectPosition;   ///< Range sensor data
        ReportSickLidar mSickLidarData;                         ///< SICK Lidar range data.
        CxUtils::Mutex mRangeSensorMutex;                       ///< This is the mutex for range sensor data
    };
}
#endif


/*  End of File */
