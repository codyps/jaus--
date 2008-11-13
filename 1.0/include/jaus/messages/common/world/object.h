////////////////////////////////////////////////////////////////////////////////////
///
///  \file object.h
///  \brief This file contains a data structure for describing objects, and is
///  used by the World Modeling Subgroup messages such as Create Vector
///  Knowledge Store Objects.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 28 July 2008
///  <br>Last Modified: 28 July 2008
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
#ifndef __JAUS_COMMON_WORLD_MODEL_OBJECT__H
#define __JAUS_COMMON_WORLD_MODEL_OBJECT__H

#include "jaus/messages/types.h"
#include "jaus/messages/vartype.h"
#include <vector>

namespace Jaus
{
    ////////////////////////////////////////////////////////////////////////////////////
    ///
    ///   \class Object
    ///   \brief Data structure for describing an object in World Modeling 
    ///   Subgroup messages like Create Vector Knowledge Store Objects.
    ///
    ///   All objects are described by a primitive type (point, line, polygon) and
    ///   can have a buffer.  The feature class type is user defined per application.
    ///
    ////////////////////////////////////////////////////////////////////////////////////
    class JAUS_MSG_DLL Object
    {
    public:
        /*! Enumerations of differnt object types. */
        enum Type
        {
            Point = 0,
            Line, 
            Polygon
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Position
        ///   \brief Point structure for describing points on an object.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Position
        {
        public:
            Position() {}
            Position(const Position& point) : mLatitude(point.mLatitude), mLongitude(point.mLongitude) {}
            ~Position() {}
            Position& operator=(const Position& point)
            {
                mLatitude = point.mLatitude;
                mLongitude = point.mLongitude;
                return *this;
            }
            double mLatitude;  ///<  Latitude of point [-90, 90].
            double mLongitude; ///<  Longitude of point [-180, 180].
            typedef std::vector<Position> List; ///<  Type definition for vector of Position.
        };

        ////////////////////////////////////////////////////////////////////////////////////
        ///
        ///   \class Feature
        ///   \brief Simple data structure for storing Feature Class information for an
        ///          Object.  The class type is user defined per application, and the
        ///          data is variable and is whatever it needs to be based on the user
        ///          defined class type.
        ///
        ////////////////////////////////////////////////////////////////////////////////////
        class JAUS_MSG_DLL Feature
        {
        public:
            Feature() : mClass(0)
            {
            }
            Feature(const Feature& feature)
            {
                mClass = feature.mClass;
                mData = feature.mData;
            }
            ~Feature(){}
            /*! Compares class type of feature. */
            bool operator<(const Feature& feature) const
            {
                return mClass < feature.mClass;
            }
            /*! Compares class type of feature. */
            bool operator<=(const Feature& feature) const
            {
                return mClass <= feature.mClass;
            }
            Feature& operator=(const Feature& feature)
            {
                mClass = feature.mClass; mData = feature.mData;
                return *this;
            }
            UShort mClass;      ///<  Feature class (user defined per application).
            VarType mData;      ///<  Data associated with the feature class.
            typedef std::vector<Feature> List; ///<  Type definition for vector of Feature.
        };
        
        Object() : mpBuffer(0) {}
        Object(const Object& obj) 
        {
            mType = obj.mType;
            if(obj.mpBuffer)
            {
                mpBuffer = new Float(*obj.mpBuffer);
            }
            else
            {
                mpBuffer = NULL;
            }
            mFeatures = obj.mFeatures;
            mPoints = obj.mPoints;
        }
        Object(const Type& type,
               const Position::List points,
               const Float* buffer = NULL,
               const Feature::List* features = NULL) : mpBuffer(0)
        {
            mType = type;
            mPoints = points;
            if(buffer) { SetBuffer(*buffer); }
            if(features) { SetFeatures(*features); }
        }
        ~Object() {}
        Object& operator=(const Object& obj)
        {
            mType = obj.mType;
            if(mpBuffer)
            {
                delete mpBuffer; mpBuffer = NULL;
            }
            if(obj.mpBuffer)
            {
                mpBuffer = new Float(*obj.mpBuffer);
            }
            else
            {
                mpBuffer = NULL;
            }
            mFeatures = obj.mFeatures;
            mPoints = obj.mPoints;
            return *this;
        }
        int SetType(const Type& type);
        int SetFeatures(const Feature::List& features);
        int SetPoints(const Position::List& points);
        int SetBuffer(const Float& buffer);        
        bool HaveBuffer() const { return mpBuffer != NULL; }
        Type GetType() const;
        Float GetBuffer() const;
        Feature::List* GetFeatures() const;
        Position::List* GetPoints() const;
        typedef std::vector<Object> List; ///<  Type definition for a vector of Object.
    protected:
        Type mType;      ///<  Object type.
        Float* mpBuffer; ///<  Object buffer in meters.
        Feature::List mFeatures;    ///<  Feature classes for object.
        Position::List mPoints;     ///<  Points related to object.
    };  
}

#endif
/*  End of File */
