////////////////////////////////////////////////////////////////////////////////////
///
///  \file videolibdll.h
///  \brief Main header file for defining library/DLL import/export 
///   options at compile time for the JAUS++ Video/Audio library.  All
///   headers in this library must include this file somehow to
///   correctly export/import data from the library to/from DLL's.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 10 June 2008
///  <br>Created: 10 June 2008
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
#ifndef _JAUS_VIDEO_LIBRARY_DLL__H
#define _JAUS_VIDEO_LIBRARY_DLL__H


//////////////////////////////////////////////////////////////////////////////////// 
///  
///  \def JAUS_VIDEO_DLL
///  \brief Pre-processor definition needed to export JAUS library to 
///  DLL's in Windows.
///
//////////////////////////////////////////////////////////////////////////////////// 
#if (defined(WIN32)) || (defined(_WIN32_WCE))
    #ifdef JAUS_VIDEO_DLL_EXPORT
        #define JAUS_VIDEO_DLL __declspec(dllexport)
        #pragma warning( disable : 4251)
    #elif (defined(JAUS_DLL_IMPORTS)) || (defined(JAUS_VIDEO_DLL_IMPORT))
        #define JAUS_VIDEO_DLL __declspec(dllimport)
        #pragma warning( disable : 4251)
        #ifdef _DEBUG
            #ifdef _WIN32_WCE
            #pragma comment(lib, "jvideolib_1.0_dll_wm5_d.lib" )
            #else
            #pragma comment(lib, "jvideolib_1.0_dll_d.lib" )
            #endif
        #else
            #ifdef _WIN32_WCE
            #pragma comment(lib, "jvideolib_1.0_dll_wm5.lib" )
            #else
            #pragma comment(lib, "jvideolib_1.0_dll.lib" )
            #endif
        #endif
    #else
        #define JAUS_VIDEO_DLL
        #ifdef _DEBUG
            #ifdef _WIN32_WCE
                #pragma comment(lib, "jvideolib_1.0_wm5_d.lib")
            #else
                #pragma comment(lib, "jvideolib_1.0_d.lib")
            #endif
        #else
            #ifdef _WIN32_WCE
                #pragma comment(lib, "jvideolib_1.0_wm5.lib")
            #else
                #pragma comment(lib, "jvideolib_1.0.lib")
            #endif
        #endif
    #endif
#else
#define JAUS_VIDEO_DLL
#endif


#endif
/*  End of File */
