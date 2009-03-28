////////////////////////////////////////////////////////////////////////////////////
///
///  \file smtest.cpp
///  \brief Test program to ensure communication through shared memory works 
///         correctly.  There are test cases which just create shared 
///         memory and put messages in, and others which test the
///         maximum communication speed between threads using
///         shared memory.
///
///  <br>Author(s): Daniel Barber
///  <br>Created: 11 March 2008
///  <br>Last Modified: 11 March 2008
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
#include <iostream>
#include <algorithm>
#ifdef WIN32
#include <vld.h>
#endif
#include "jaus/components/cmplib.h"
#include <cxutils/thread.h>
#include <cxutils/timer.h>

#ifdef WIN32
#include <conio.h>
#endif

using namespace std;
using namespace Jaus;

bool gExitFlag = false;

class StreamReader : public StreamCallback
{
public:
    StreamReader()
    {
        mStartTimeMs = 0;
        mLastReceiveTimeMs = 0;
        mTotalReceived = 0;
        mFPS = 0;
    }
    virtual void ProcessStreamCallback(const Stream& msg, 
                                       const Header* info,
                                       const Transport transport)
    {
        if(mTotalReceived == 0)
        {
            mStartTimeMs = CxUtils::Timer::GetTimeMs();
        }
        mTotalReceived++;
        mLastReceiveTimeMs = CxUtils::Timer::GetTimeMs();
        // Re-calculate FPS
        if(mTotalReceived%10 == 0)
        {
            mFPS = mTotalReceived*1000.0/(mLastReceiveTimeMs - mStartTimeMs);
        }
    }
    volatile double mFPS;               ///< Streams/Second
    double mStartTimeMs;                ///< Time of first message received in Ms.
    double mLastReceiveTimeMs;          ///< Time of last message received in Ms.
    volatile unsigned int mTotalReceived;   ///< Total number of messages received.
};

void TestInbox(void *args);
void TestOutbox(void *args);
void TestReadingInbox(void *args);

int main(int argc, char *argv[])
{   
    cout << "Program Arugments are:\n";
    cout << "\t1 to Test Inbox.\n";
    cout << "\t2 to Test Inbox/Outbox.\n";
    cout << "\t3 to Test Opening View of Inbox and Reading from it.\n";
    cout << "\tDefault: Outbox (no program arugments).\n";

    if(argc == 1)
    {
        TestOutbox(NULL);
    }
    else if(argc > 1 && atoi(argv[1]) == 1)
    {
        TestInbox(NULL);
    }
    else if(argc > 1 && atoi(argv[1]) == 3)
    {
        TestReadingInbox(NULL);
    }
    else
    {
        CxUtils::Thread inThread;
        CxUtils::Thread outThread;
        inThread.CreateThread(TestInbox, NULL);
        Sleep(100);
        outThread.CreateThread(TestOutbox, NULL);

        while(!gExitFlag)
        {
#ifdef WIN32
            if(_kbhit())
            {
                if(_getch() == 27)
                {
                    gExitFlag = true;
                }
            }
#endif
            Sleep(50);
        }
    }

    return 0;
}


void TestInbox(void *args)
{
    JSharedMemory messageBox;
    StreamReader readerCallback;
    unsigned int printTime = 0;
    // Create an inbox that can hold up to 10 full size messages.
    if( messageBox.CreateInbox(Address(254, 254, 254, 254), 
                               &readerCallback,
                               JAUS_MAX_PACKET_SIZE*10) )
    {
        unsigned int loopCount = 0;
        while(!gExitFlag)
        {
            if(Time::GetUtcTimeMs() - printTime > 500)
            {
                cout << "FPS: " << readerCallback.mFPS << endl;
                cout << "Message Box Size: " << messageBox.Size() << endl;
                printTime = Time::GetUtcTimeMs();
            }
            if(++loopCount%1000 == 0)
            {
                Sleep(1);
            }
        }
    }
}

void TestOutbox(void *args)
{
    JSharedMemory messageBox;
    Header header;
    Stream message;

    if( messageBox.OpenInbox(Address(254, 254, 254, 254)) )
    {
        unsigned int loopCount = 0;
        header.mSourceID = Address(254, 254, 254, 1);
        header.mDestinationID = Address(254, 254, 254, 254);
        header.mCommandCode = JAUS_REPORT_HEARTBEAT_PULSE;
        message.Write(header);
        while(!gExitFlag)
        {
            if(messageBox.EnqueueMessage(message) == JAUS_FAILURE)
            {
                //cout << "Message Box is Full!\n";
                while(messageBox.Size() > 10 && !gExitFlag)
                {
                    Sleep(100);
                }
            }
            if(++loopCount%100 == 0)
            {
                Sleep(1);
            }
        }
    }
}

void TestReadingInbox(void *args)
{
    JSharedMemory messageBox;
    Header header;
    Stream message;

    if( messageBox.OpenInbox(Address(254, 254, 254, 254)) )
    {
        unsigned int loopCount = 0;
        while(!gExitFlag)
        {
            if(messageBox.DequeueMessage(message) == JAUS_OK )
            {
                Message *rmessage = MessageCreator::CreateMessage(message, NULL);
                if( rmessage )
                {
                    rmessage->Print();
                    delete rmessage;
                }
            }
            if(++loopCount%100 == 0)
            {
                Sleep(1);
            }
        }
    }
}


/*  End of File */
