/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "logger/logger.h"
#include <iostream>
#include <functional>
#include <unistd.h>
using namespace std;
//using namespace translib;

#include "translib/loop.h"
#include "translib/timerManager.h"
#include "index.h"

void timerManagerExample()
{

	{
		//////////
		//starCB test
		//////
		translib::Loop loop;
		translib::TimerManager TM(loop);
		int counter = 10;
		int timerIDCb001;
		__LOG(debug, "the address of timerIDCb001 is : " << (void *)(&timerIDCb001));
		{
			auto cbTimer = TM.getTimer(&timerIDCb001);
			cbTimer->startCB(500, [&](void *usrData) -> int {
				__LOG(debug, "user data is : " << (void *)usrData);
				return counter--;
			},
							 &timerIDCb001);
		}
		loop.start();
		sleep(2);
		TM.killTimer(timerIDCb001);
	}
	{
		int timerID001 = 100;
		translib::Timer::ptr_p timer001 = translib::TimerManager::instance()->getTimer(&timerID001);
		std::cout << "timer ID 001 is " << timerID001 << std::endl;
		timer001->startOnce(10, [] { std::cout << "timer001" << std::endl; });

		int timerID002 = 100;
		{
			// note!!!!!!, please make sure the shared_ptr is released!!! or in one cold block
			// or call timer002.reset();
			// or the timer will not stop
			translib::Timer::ptr_p timer002 = translib::TimerManager::instance()->getTimer(&timerID002);
			std::cout << "timer ID 002 is " << timerID002 << std::endl;
			timer002->startForever(1000, [=] { std::cout << "timer002, timer ID is : " << timerID002 << std::endl; });
		}

		int timerID003 = 100;
		{
			auto timer003 = translib::TimerManager::instance()->getTimer(&timerID003);
			std::cout << "timer ID 003 is " << timerID003 << std::endl;
			timer003->startForever(1000, [=] { std::cout << "timer003, timer ID is : " << timerID003 << std::endl; });
		}
		sleep(5);

		translib::TimerManager::instance()->killTimer(timerID002);
		sleep(2);

		timer001->stop();
		sleep(2);

		std::cout << "kill all the timer" << std::endl;
		translib::TimerManager::instance()->killTimer(timerID003);

		// note:!!!!!!!!!!!!!!!!!
		// please do not call timer001->stop(); if you do not use instance!!!!
		// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		translib::Loop loop;

		translib::TimerManager TM(loop);
		int timerID004 = 100;
		auto timer004 = TM.getTimer(&timerID004);
		timer004->startForever(1000, [] { std::cout << "timer004" << std::endl; });
		loop.start(true);
		int timerID005 = 100;
		auto timer005 = TM.getTimer(&timerID005);
		timer005->startForever(500, [] { std::cout << "timer005" << std::endl; });
		sleep(5);
		TM.killTimer(timerID004);
	}
	{
		//////////
		//get loop test
		//////

		auto TMM = translib::TimerManager::instance();

		translib::Loop &loop = TMM->getLoop();
		translib::TimerManager TM(loop);
		int counter = 10;
		int timerIDCb001;
		__LOG(debug, "the address of timerIDCb001 is : " << (void *)(&timerIDCb001));
		{
			auto cbTimer = TM.getTimer(&timerIDCb001);
			cbTimer->startCB(100, [&](void *usrData) -> int {
				__LOG(debug, "user data is : " << (void *)usrData << ". counter is : " << counter);
				return counter--;
			},
							 &timerIDCb001);
		}
		//loop.start();
		sleep(2);
		TM.killTimer(timerIDCb001);
	}
	sleep(5);
}
