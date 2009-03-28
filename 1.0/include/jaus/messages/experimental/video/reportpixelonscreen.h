////////////////////////////////////////////////////////////////////////////////////
///
///  \file reportpixelonscreen.h
///  \brief This file contains a custom message used to respond to a 
///  QueryPixelOnScreen message.
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
#ifndef __JAUS_EXPERIMENTAL_REPORT_PIXEL_ON_SCREEN__H
#define __JAUS_EXPERIMENTAL_REPORT_PIXEL_ON_SCREEN__H

#include "jaus/messages/message.h"
#include <vector>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class ReportPixelOnScreen
    ///   \brief This message is a respone to the Query Pixel On Screen message.
    ///   It attempts to report what type of data is at a specific pixel location
    ///   in a video screen image.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL ReportPixelOnScreen : public Message
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Data
        ///   \brief Data about was discovered at a pixel location.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Data
        {
        public:
            typedef std::vector<Data> List; ///<  List of Pixel Location Data.
            Data();
            Data(const Data& data);
            ~Data();
            Data& operator=(const Data& data);
            Float mX;                       ///<  Normalized X position in image (0 is left side, 1.0 is right side of image).
            Float mY;                       ///<  Normalized Y position in image (0 is bottom of image, 1.0 is top of image).
            std::string mModelType;         ///<  Name of the model (type of object).
            std::string mActorName;         ///<  Name of the actor (instance of an model/object).
            CxUtils::Point3D mOrientation;  ///<  X = roll, Y = pitch, Z = yaw of entity in radians.
            CxUtils::Wgs mPosition;         ///<  Position of data in environment.
            CxUtils::Wgs mVehiclePosition;  ///<  Position of the vehicle.
            Float mDistance;                ///<  Distance to target.
            Float mAngle;                   ///<  Angle relative to vehicle position that target is at.
        };

        ReportPixelOnScreen();
        ReportPixelOnScreen(const ReportPixelOnScreen& query);
        virtual ~ReportPixelOnScreen();

        void SetTimeStamp(const Time& time) { mTimeStamp = time; }
        void SetData(const Data::List& data) { mDataList = data; }

        Data::List* GetData() const { return (Data::List*)(&mDataList); }
        Time GetTimeStamp() const   { return mTimeStamp; }

        virtual int WriteMessageBody(Stream& msg,
                                     const UShort version) const;
        virtual int ReadMessageBody(const Stream& msg, 
                                    const UShort version);
        virtual Message* Clone() const { return new ReportPixelOnScreen(*this); }
        virtual UShort GetPresenceVectorSize(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual UInt GetPresenceVectorMask(const UShort version = JAUS_DEFAULT_VERSION) const { return 0; }
        virtual void ClearMessageBody();
        void PrintData() const;
        ReportPixelOnScreen& operator=(const ReportPixelOnScreen& query);

    protected:
        Time mTimeStamp;        ///< Time Stamp.
        Data::List mDataList;   ///< List of discovered data at a pixel location.
    };
}



#endif
/*  End of File */
