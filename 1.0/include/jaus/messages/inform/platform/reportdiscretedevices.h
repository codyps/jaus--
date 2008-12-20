////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportdiscretedevices.h
///  \brief This file contains the message class for Report Discrete Devices.
///
///  <br>Author(s): David Adams
///  <br>Created: 12 June 2008
///  <br>Last Modified: 12 June 2008
///  <br>Copyright (c) 2007
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
#ifndef __JAUS_REPORT_DISCRETE_DEVICES__H
#define __JAUS_REPORT_DISCRETE_DEVICES__H

#include "jaus/messages/message.h"


namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportDiscreteDevices
    ///   \brief This message provides the receiver the current values of the commanded
    ///          discrete devices.  The data fields and presence vector mapping of this
    ///          message are identical to Code 0406h: Set Discrete Devices.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportDiscreteDevices : public Message
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
                MainPropulsion =        0x01,
                ParkingBrakeAndHorn =   0x02,
                Gear =                  0x04,
                TransferCase =          0x08
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
                MainPropulsion = 0,
                ParkingBrakeAndHorn,
                Gear,
                TransferCase
            };
        };
        ///////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class MainPropulsion
        ///   \brief This class contains bit positions mappings for fields of the
        ///          Main Propulsion field of Discrete Devices.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL MainPropulsion
        {
        public:
            enum Bits
            {
                OnOff = 0,
                MainEnergyFuelSupplyOnOff,
                AuxiliaryEnergyFuelSupplyOnOff,
                PowerToAuxiliaryDevicesOnOff,
                StartingDeviceOnOff,
                ColdStartTrueFalse,
                CommenceAutomaticStartSequence,
                CommenceAutomaticShutdownSequence,
            };
        };
        ///////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class ParkingBrakeAndHorn
        ///   \brief This class contains bit positions mappings for fields of the
        ///          Parking Brake and Horn field of Discrete Devices.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL ParkingBrakeAndHorn
        {
        public:
            enum Bits
            {
                ParkingBrakeSetRelease = 0,
                HornOnOff,
            };
        };
        ///////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Gear
        ///   \brief This class contains methods to determine type of gear based on
        ///   value of Gear Field in Discrete Devices message.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Gear
        {
        public:
            static bool IsPark(const Byte value)    { return value == 0; }
            static bool IsForward(const Byte value) { return (value >= 1 && value <= 127) ? true : false; }
            static bool IsNeutral(const Byte value) { return value == 128; }
            static bool IsReverse(const Byte value) { return value >= 129; }
        };
        ///////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class TransferCase
        ///   \brief This class contains methods to determine type of case based on
        ///   value of Gear Field in Discrete Devices message.
        ///
        ////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL TransferCase
        {
        public:
            static bool IsLow(const Byte value) {return (value <= 127) ? true : false; }
            static bool IsNeutral(const Byte value) { return value == 128; }
            static bool IsHigh(const Byte value) { return value >= 129; }
        };
        ReportDiscreteDevices();
        ReportDiscreteDevices(const ReportDiscreteDevices& msg);
        virtual ~ReportDiscreteDevices() {}
        int SetMainPropulsionField(const Byte value);
        int SetParkingBrakeAndHornField(const Byte value);
        int SetGearField(const Byte value);
        int SetTransferCaseField(const Byte value);
        inline Byte GetPresenceVector() const { return mPresenceVector; }
        inline Byte GetMainPropulsionField() const { return mMainPropulsion; }
        inline Byte GetParkingBrakeAndHornField() const { return mParkingBrakeAndHorn; }
        inline Byte GetGearField() const { return mGear; }
        inline Byte GetTransferCaseField() const { return mTransferCase; }
        virtual int WriteMessageBody(Stream& msg, const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual void ClearMessageBody();
        virtual Message* Clone() const { return new ReportDiscreteDevices(*this); }
        ReportDiscreteDevices& operator=(const ReportDiscreteDevices& msg);
    protected:
        Byte mPresenceVector;       ///<  Bit vector for fields present.
        Byte mMainPropulsion;       ///<  Main propulsion Field.
        Byte mParkingBrakeAndHorn;  ///<  Parking brake and Horn field.
        Byte mGear;                 ///<  Gear field.
        Byte mTransferCase;         ///<  Transfer case Field.
    };
}

#endif
/*  End of File */
