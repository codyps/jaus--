////////////////////////////////////////////////////////////////////////////////////
///
///  \file commandmessages.h
///  \brief Main include file for all command messages.
///
///  <br>Author(s): Daniel Barber, Chris Hollander
///  <br>Created: 27 January 2007
///  <br>Last Modified: 8 August 2008
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
#ifndef __JAUS_COMMAND_MESSAGES_INCLUDE__H
#define __JAUS_COMMAND_MESSAGES_INCLUDE__H

#include "jaus/messages/command/core/activateserviceconnection.h"
#include "jaus/messages/command/core/clearemergency.h"
#include "jaus/messages/command/core/confirmcomponentcontrol.h"
#include "jaus/messages/command/core/confirmserviceconnection.h"
#include "jaus/messages/command/core/createserviceconnection.h"
#include "jaus/messages/command/core/rejectcomponentcontrol.h"
#include "jaus/messages/command/core/releasecomponentcontrol.h"
#include "jaus/messages/command/core/requestcomponentcontrol.h"
#include "jaus/messages/command/core/reset.h"
#include "jaus/messages/command/core/resume.h"
#include "jaus/messages/command/core/setcomponentauthority.h"
#include "jaus/messages/command/core/setemergency.h"
#include "jaus/messages/command/core/settime.h"
#include "jaus/messages/command/core/shutdown.h"
#include "jaus/messages/command/core/standby.h"
#include "jaus/messages/command/core/suspendserviceconnection.h"
#include "jaus/messages/command/core/terminateserviceconnection.h"

#include "jaus/messages/command/communications/setdatalinkselect.h"
#include "jaus/messages/command/communications/setdatalinkstate.h"
#include "jaus/messages/command/communications/setselecteddatalinkstate.h"

#include "jaus/messages/command/environment/setcamerapose.h"                //added by C. Hollander 8/8/08
#include "jaus/messages/command/environment/selectcamera.h"                    //added by C. Hollander 8/11/08
#include "jaus/messages/command/environment/setcameracapabilities.h"        //added by C. Hollander 8/11/08
#include "jaus/messages/command/environment/setcameraformatoptions.h"        //added by C. Hollander 8/13/08

#include "jaus/messages/command/events/createeventrequest.h"
#include "jaus/messages/command/events/updateevent.h"
#include "jaus/messages/command/events/cancelevent.h"
#include "jaus/messages/command/events/confirmeventrequest.h"
#include "jaus/messages/command/events/rejecteventrequest.h"

#include "jaus/messages/command/platform/setwrencheffort.h"
#include "jaus/messages/command/platform/setglobalvector.h"
#include "jaus/messages/command/platform/settravelspeed.h"
#include "jaus/messages/command/platform/setdiscretedevices.h"
#include "jaus/messages/command/platform/setlocalvector.h"                    //added by C. Hollander 8/21/08
#include "jaus/messages/command/platform/setglobalwaypoint.h"                    //added by C. Hollander 8/22/08
#include "jaus/messages/command/platform/setlocalwaypoint.h"                    //added by C. Hollander 8/22/08
#include "jaus/messages/command/platform/setglobalpathsegment.h"                    //added by C. Hollander 8/22/08
#include "jaus/messages/command/platform/setlocalpathsegment.h"                    //added by C. Hollander 8/22/08

#include "jaus/messages/command/world/createvectorknowledgestoreobjects.h"

#endif
/*  End of File */
