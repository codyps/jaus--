////////////////////////////////////////////////////////////////////////////////////
///
///  \file spoolmission.h
///  \brief This file contains data structure for the JAUS_SPOOL_MISSION message.
///
///  <br>Author(s): Daniel Barber, Cathy Yen
///  <br>Created: 25 March 2009
///  <br>Last Modified: 12 March 2009
///  <br>Copyright (c) 2009
///  <br>Applied Cognition and Training in Immersive Virtual Environments
///  <br>(ACTIVE) Laboratory
///  <br>Institute for Simulation and Training (IST)
///  <br>University of Central Florida (UCF)
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
#ifndef __JAUS_SPOOL_MISSION_H
#define __JAUS_SPOOL_MISSION_H

#include "jaus/messages/message.h"
#include "jaus/messages/common/planning/mission.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SpoolMission
    ///   \brief Implements Spool Mission (0E00h), one of the Command set of messages 
    ///   under the Planning Subgroup in JAUS.    
    ///   
    ///   The Spool Mission message is used to send complex missions to lower level
    ///   components (Mission Spoolers, Drivers, etc.). The amount of data in one
    ///   Spool Mission message is determined by the receiving component's spooling
    ///   preferences. For example, if a component can handle (10) JAUS messages
    ///   at a time, then the spooling component shall send a Spool Message with
    ///   no more than ten (10) messages.  Spool Mission messages to lower
    ///   level components (i.e. drivers) shall only contain JAUS messages that
    ///   are specifically addressed to that lower level component.  Hence a
    ///   Spool Mission message to a lower level component is reduced to a list or
    ///   queue, of JAUS commands address to that component.
    ///
    ///   The Mission ID field shall be a unique ID to allow for spooling and
    ///   manipulating multiple missions at a time.
    ///   The Append Flag field indicates whether the mission is a new mission that
    ///   shall replace the existing mission or if the mission shall be
    ///   appended to the existing machine.
    ///
    ///   An empty Spool Mission message can be used to indicate a paused, aborted,
    ///   or completed mission.  This technique can be used rather than the Paus
    ///   and Abort messages to reduce the number of Mission Spooler messages a
    ///   lower level component must implement.  The Mission Spooler can determine
    ///   which method to use by looking at the services the lower level component
    ///   support.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SpoolMission : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \enum AppendFlag
        ///   \brief flag options of replace or append to a mission.
        ///               <br>0 = Replace the current mission. 
        ///               <br>1 = Append to current mission. 
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        enum AppendFlag       
        {
            Replace = 0,
            Append,
        }; 
        SpoolMission();
        SpoolMission(const SpoolMission &msg);
        ~SpoolMission();
        void SetAppendFlag(const AppendFlag flag);
        AppendFlag GetAppendFlag() const;
        void SetMission(const Mission& mission);
        const Mission* GetMission() const;
        Mission* GetMission();

        virtual int WriteMessageBody(Stream &msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream &msg, const UShort version);
        virtual void ClearMessageBody();
        virtual void Print() const;
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual Message* Clone() const;
        SpoolMission& operator=(const SpoolMission &msg);
        virtual int RunTestCase() const;  ///< Runs test program to show usage.
    protected:
        AppendFlag mAppendFlag; ///< 0 = Replace current mission, 1 = append to current mission.
        Mission mMission;       ///< Contains mission object
    };
}

#endif

