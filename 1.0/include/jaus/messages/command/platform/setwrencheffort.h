////////////////////////////////////////////////////////////////////////////////////
///
///  \file setwrencheffort.h
///  \brief This file contains the message class for Set Wrench Effort.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 12 February 2007
///  <br>Last Modified: 30 April 2008
///  <br>Copyright (c) 2007
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
#ifndef __JAUS_SET_WRENCH_EFFORT__H
#define __JAUS_SET_WRENCH_EFFORT__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class SetWrenchEffort
    ///   \brief This message is used to control platform mobility actuators by mapping
    ///          the twelve command elements to the specific mobility controls of a
    ///          vehicle.  The command consists of a six propulsive wrench and a six
    ///          resistive wrench.  The six elements of each wrench break down into
    ///          linear elements and three rotational elements, which are mapped to
    ///          three axis orthogonal coordinate frame of the vehicle.
    ///
    ///          All elements of the Wrench message are not necessarily applicable
    ///          to a particular platform.  For example, a typical wheeled vehicle
    ///          can be controlled with only three elements of the wrench command:
    ///          Propulsive Linear Effort X (throttle), Propulsive Rotational Effort
    ///          Z (steering), and Resistive Linear Effort X (brake).
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL SetWrenchEffort : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                PropulsiveLinearEffortX = 0x0001,
                PropulsiveLinearEffortY = 0x0002,
                PropulsiveLinearEffortZ = 0x0004,
                PropulsiveRotationalEffortX = 0x0008,
                PropulsiveRotationalEffortY = 0x0010,
                PropulsiveRotationalEffortZ = 0x0020,
                ResistiveLinearEffortX = 0x0040,
                ResistiveLinearEffortY = 0x0080,
                ResistiveLinearEffortZ = 0x0100,
                ResistiveRotationalEffortX = 0x0200,
                ResistiveRotationalEffortY = 0x0400,
                ResistiveRotationalEffortZ = 0x0800,
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {
                PropulsiveLinearEffortX = 0,
                PropulsiveLinearEffortY,
                PropulsiveLinearEffortZ,
                PropulsiveRotationalEffortX,
                PropulsiveRotationalEffortY,
                PropulsiveRotationalEffortZ,
                ResistiveLinearEffortX,
                ResistiveLinearEffortY,
                ResistiveLinearEffortZ,
                ResistiveRotationalEffortX,
                ResistiveRotationalEffortY,
                ResistiveRotationalEffortZ,
            };
        };
        SetWrenchEffort();
        SetWrenchEffort(const SetWrenchEffort& msg);
        ~SetWrenchEffort();
        int SetPropulsiveLinearEffortX(const double percent);
        int SetPropulsiveLinearEffortY(const double percent);
        int SetPropulsiveLinearEffortZ(const double percent);
        int SetPropulsiveRotationalEffortX(const double percent);
        int SetPropulsiveRotationalEffortY(const double percent);
        int SetPropulsiveRotationalEffortZ(const double percent);
        int SetResistiveLinearEffortX(const double percent);
        int SetResistiveLinearEffortY(const double percent);
        int SetResistiveLinearEffortZ(const double percent);
        int SetResistiveRotationalEffortX(const double percent);
        int SetResistiveRotationalEffortY(const double percent);
        int SetResistiveRotationalEffortZ(const double percent);
        UShort GetPresenceVector() const { return mPresenceVector; }
        double GetPropulsiveLinearEffortX() const;
        double GetPropulsiveLinearEffortY() const;
        double GetPropulsiveLinearEffortZ() const;
        double GetPropulsiveRotationalEffortX() const;
        double GetPropulsiveRotationalEffortY() const;
        double GetPropulsiveRotationalEffortZ() const;
        double GetResistiveLinearEffortX() const;
        double GetResistiveLinearEffortY() const;
        double GetResistiveLinearEffortZ() const;
        double GetResistiveRotationalEffortX() const;
        double GetResistiveRotationalEffortY() const;
        double GetResistiveRotationalEffortZ() const;
        virtual void Print() const;
        virtual void PrintWrenchEffort() const;
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new SetWrenchEffort(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        SetWrenchEffort& operator=(const SetWrenchEffort& msg);
    protected:
        UShort mPresenceVector;                 ///<  Bit vector for fields present.
        double mPropulsiveLinearEffortX;        ///<  Propulsiveulsive Linearear Effort X [-100, 100].
        double mPropulsiveLinearEffortY;        ///<  Propulsiveulsive Linearear Effort Y [-100, 100].
        double mPropulsiveLinearEffortZ;        ///<  Propulsiveulsive Linearear Effort Z [-100, 100].
        double mPropulsiveRotationalEffortX;    ///<  Propulsiveulsive Rotationalational Effort X [-100, 100].
        double mPropulsiveRotationalEffortY;    ///<  Propulsiveulsive Rotationalational Effort Y [-100, 100].
        double mPropulsiveRotationalEffortZ;    ///<  Propulsiveulsive Rotationalational Effort Z [-100, 100].
        double mResistiveLinearEffortX;         ///<  Resistive Linearear Effort X [0, 100].
        double mResistiveLinearEffortY;         ///<  Resistive Linearear Effort Y [0, 100].
        double mResistiveLinearEffortZ;         ///<  Resistive Linearear Effort Z [0, 100].
        double mResistiveRotationalEffortX;     ///<  Resistive Rotationalational Effort X [0, 100].
        double mResistiveRotationalEffortY;     ///<  Resistive Rotationalational Effort Y [0, 100].
        double mResistiveRotationalEffortZ;     ///<  Resistive Rotationalational Effort Z [0, 100].
    };
}

#endif
/*  End of File */
