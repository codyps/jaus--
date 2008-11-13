////////////////////////////////////////////////////////////////////////////////////
///
///  \file platform.h
///  \brief Simple structure for storing information about a platform including
///  configuration, identification, and pose.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 25 April 2008
///  <br>Last Modified: 25 April 2008
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
#include "jaus/messages/common/platform/platform.h"
#include <iostream>

using namespace std;
using namespace Jaus;


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Platform::Platform() : mpIdentification(NULL),
                       mpGlobalPose(NULL)
{
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Copy constructor.
///
////////////////////////////////////////////////////////////////////////////////////
Platform::Platform(const Platform& platform) : mpIdentification(NULL), 
                                               mpGlobalPose(NULL)
{
    *this = platform;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Destructor.
///
////////////////////////////////////////////////////////////////////////////////////
Platform::~Platform()
{
    if(mpGlobalPose) { delete mpGlobalPose; }
    if(mpIdentification) { delete mpIdentification; }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the global pose the platform.
///
///   \param pose Global pose of the platform.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Platform::SetGlobalPose(const GlobalPose& pose)
{
    if(mpGlobalPose == NULL)
    {
        mpGlobalPose = new GlobalPose(pose);
    }
    else
    {
        *mpGlobalPose = pose;
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Adds the global pose data to the existing global pose without
///   removing any values.
///
///   This function only adds the new data to the platforms global pose, and
///   overwrites old data.  If a field is not present in the pose parameter
///   passed, that data is not removed from the platforms pose structure.
///
///   \param pose Global pose information for platform to combine with
///               current global pose data.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Platform::AddGlobalPoseData(const GlobalPose& pose)
{
    if(mpGlobalPose == NULL)
    {
        mpGlobalPose = new GlobalPose(pose);
    }
    else
    {
        if(pose.HaveLatitude()) { mpGlobalPose->SetLatitude(pose.GetLatitude()); }
        if(pose.HaveLongitude()) { mpGlobalPose->SetLongitude(pose.GetLongitude()); }
        if(pose.HaveElevation()) { mpGlobalPose->SetElevation(pose.GetElevation()); }
        if(pose.HavePositionRms()) { mpGlobalPose->SetPositionRms(pose.GetPositionRms()); }
        if(pose.HaveRoll()) { mpGlobalPose->SetRoll(pose.GetRoll()); }
        if(pose.HavePitch()) { mpGlobalPose->SetPitch(pose.GetPitch()); }
        if(pose.HaveYaw()) { mpGlobalPose->SetYaw(pose.GetYaw()); }
        if(pose.HaveAttitudeRms()) { mpGlobalPose->SetAttitudeRms(pose.GetAttitudeRms()); }
        if(pose.HaveTimeStamp()) { mpGlobalPose->SetTimeStamp(pose.GetTimeStamp()); }        
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the subsystem identification of the platform.
///
///   \param identification Subsystem identification.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Platform::SetIdentification(const Identification& identification)
{
    if(mpIdentification == NULL)
    {
        mpIdentification = new Identification(identification);
    }
    else
    {
        *mpIdentification = identification;
    }
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Set the subsystem configuration and ID of the platform.
///
///   \param config Subsystem configuration information.
///
///   \return JAUS_OK on success, otherwise JAUS_FAILURE.
///
////////////////////////////////////////////////////////////////////////////////////
int Platform::SetConfiguration(const Configuration::Subsystem& config)
{
    mConfiguration = config;
    return JAUS_OK;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes data.
///
////////////////////////////////////////////////////////////////////////////////////
void Platform::ClearGlobalPose()
{
    if(mpGlobalPose)
    {
        delete mpGlobalPose;
        mpGlobalPose = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Deletes data.
///
////////////////////////////////////////////////////////////////////////////////////
void Platform::ClearIdentification()
{
    if(mpIdentification)
    {
        delete mpIdentification;
        mpIdentification = NULL;
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Prints platform information to the console.
///
////////////////////////////////////////////////////////////////////////////////////
void Platform::Print() const
{
    Configuration::Node::Map::const_iterator node;
    unsigned int total = 0;
    for(node = mConfiguration.mNodes.begin(); 
        node != mConfiguration.mNodes.end();
        node++)
    {
        total += (unsigned int)node->second.mComponents.size();
    }
    cout << "Subsystem [" << (int)mConfiguration.mSubsystemID << "] with " << total << " Component(s).\n";
    if(mpIdentification)
    {
        cout << "Identification: " << mpIdentification->GetIdentification() << endl;
    }    
    if(mpGlobalPose)
    {
        mpGlobalPose->PrintGlobalPose();
    }
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Comparison function for sorting platform information by subsystem ID.
///
////////////////////////////////////////////////////////////////////////////////////
bool Platform::operator <(const Platform& platform) const
{
    return mConfiguration.mSubsystemID < platform.mConfiguration.mSubsystemID;
}


////////////////////////////////////////////////////////////////////////////////////
///
///   \brief Sets equal to.
///
////////////////////////////////////////////////////////////////////////////////////
Platform& Platform::operator=(const Platform& platform)
{
    if(this != &platform)
    {
        mConfiguration = platform.mConfiguration;
        if(platform.mpIdentification)
        {
            if(mpIdentification == NULL)
            {
                mpIdentification = new Identification(*platform.mpIdentification);
            }
            else
            {
                *mpIdentification = *platform.mpIdentification;
            }               
        }
        else
        {
            if(mpIdentification)
            {
                delete mpIdentification;
                mpIdentification = NULL;
            }
        }
        if(platform.mpGlobalPose)
        {
            if(mpGlobalPose == NULL)
            {
                mpGlobalPose = new GlobalPose(*platform.mpGlobalPose);
            }
            else
            {
                *mpGlobalPose = *platform.mpGlobalPose;
            }               
        }
        else
        {
            if(mpGlobalPose)
            {
                delete mpGlobalPose;
                mpGlobalPose = NULL;
            }
        }
    }
    return *this;
}


/*  End of File */
