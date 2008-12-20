////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportplatformspecifications.h
///  \brief This file contains the message class for 
///            Report Platform Operational Data
///
///  <br>Author(s): David Adams
///  <br>Created: 09 June 2008
///  <br>Last Modified: 10 June 2008
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
#ifndef __JAUS_REPORT_PLATFORM_OPERATIONAL_DATA__H
#define __JAUS_REPORT_PLATFORM_OPERATIONAL_DATA__H

#include "jaus/messages/message.h"

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportPlatformOperationalData
    ///   \brief This message provides the receiver with a variety of operational data 
    ///    from the platform including engine temperature, odometer reading, battery 
    ///    voltage, fuel level, and oil pressure if these are available from the platform.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportPlatformOperationalData : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorMask
        ///   \brief This class contains bit masks for bitwise operations on the
        ///          presence vector for this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorMask
        {
        public:
            enum Masks
            {
                EngineTemperature        = 0x0001,
                Odometer                = 0x0002,
                BatteryVoltage          = 0x0004,
                PercentageFuelLevel     = 0x0008,
                PercentageOilPressure   = 0x0010,        
            };
        };
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class VectorBit
        ///   \brief This class contains bit positions mappings for fields of the
        ///          presence vector of this structure.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL VectorBit
        {
        public:
            enum Bits
            {          
                EngineTemperature       = 0,    
                Odometer,       
                BatteryVoltage,      
                PercentageFuelLevel,     
                PercentageOilPressure,  
            };
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Limits
        ///   \brief Limit values for constants used by class.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Limits
        {
        public:
            const static double MinEngineTemperature;       ///<  Lower Limit = -75C
            const static double MaxEngineTemperature;       ///<  Upper Limit = 180 C.
            const static double MinBatteryVoltage;            ///<  Lower Limit = 0%.
            const static double MaxBatteryVoltage;            ///<  Upper Limit = 127%.
            const static double MinPercentageFuelLevel;        ///<  Lower Limit = 0%.
            const static double MaxPercentageFuelLevel;        ///<  Upper Limit = 100%.
            const static double MinPercentageOilPressure;    ///<  Lower Limit = 0%.
            const static double MaxPercentageOilPressure;    ///<  Upper Limit = 127%.
        };
    public:
        ReportPlatformOperationalData();
        ReportPlatformOperationalData(const ReportPlatformOperationalData& msg);
        virtual ~ReportPlatformOperationalData();

        int SetPresenceVector(const Byte pv);
        int SetEngineTemperature(const double val);
        int SetOdometer(const UInt val);
        int SetBatteryVoltage(const double val);
        int SetPercentageFuelLevel(const double val);
        int SetPercentageOilPressure(const double val);

        Byte   GetPresenceVector()const { return mPresenceVector; }       
        double GetEngineTemperature() const;    
        double GetOdometer()const;                
        double GetBatteryVoltage()const;        
        double GetPercentageFuelLevel()const;    
        double GetPercentageOilPressure()const;    

        // Clears only message body information.
        virtual void ClearMessageBody();

        // Writes message body data from current write position in stream for specific JAUS version.
        // Sets an error or returns -1 on failure.
        virtual int WriteMessageBody(Stream& msg,
                                     const UShort version) const;
        // Reads message body data from current read position in stream and for specific JAUS version.
        // Sets an error or returns -1 on failure.
        virtual int ReadMessageBody(const Stream& msg, 
                                    const UShort version);
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const;
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const;
        // Make a copy of the message and returns pointer to it.
        virtual Message* Clone() const { return new ReportPlatformOperationalData(*this); }
        ReportPlatformOperationalData& operator=(const ReportPlatformOperationalData& msg);
    protected:
        Byte mPresenceVector;            ///<  Presence vector associated with message.
        double mEngineTemperature;        ///<  Scaled Integer, Lower Limit = -75C, Upper Limit = 180 C.
        UInt mOdometer;                    ///<  Cumulative distance traveled by vehicle.
        double mBatteryVoltage;            ///<  Scaled Integer, Lower Limit = 0%, Upper Limit = 127%.
        double mPercentageFuelLevel;    ///<  Scaled Integer, Lower Limit = 0%, Upper Limit = 100%.
        double mPercentageOilPressure;    ///<  Scaled Integer, Lower Limit = 0%, Upper Limit = 127%.

    };
}

#endif
/*  End of File */
