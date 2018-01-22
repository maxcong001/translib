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

#pragma once

#include "loop.h"
#include "timer.h"
#include <unordered_map>
#include <atomic>
#include <thread>
#include <algorithm>
#include "logger/logger.h"
namespace translib
{
class Loop;
class Timer;
#define AUDIT_TIMER 5000
class TimerManager
{
  public:
	TimerManager() : uniqueID_atomic(0), t_map(), audit_timer(NULL), _loop(loop), loop()
	{
		init(true);
		audit_timer = new Timer(_loop);
		audit_timer->startForever(AUDIT_TIMER, [] {
			translib::TimerManager::instance()->auditTimer();
		});
	}
	TimerManager(translib::Loop &loop_in) : uniqueID_atomic(0), t_map(), audit_timer(NULL), _loop(loop_in)
	{
		init(false);
		audit_timer = new Timer(_loop);
		audit_timer->startForever(AUDIT_TIMER, [this] {
			auditTimer();
		});
	}
	~TimerManager()
	{
		//stop audit timer
		if (audit_timer)
		{
			delete audit_timer;
		}
		audit_timer = NULL;
	}
	// if the loop is passed outside or we start ourselves
	bool init(bool start);
	Timer::ptr_p getTimer(int *timerID = NULL);
	bool killTimer(int timerID);
	bool auditTimer();
	static TimerManager *instance()
	{
		static TimerManager *ins = new TimerManager();
		return ins;
	}
	std::mutex mtx;

  protected:
  private:
	int getUniqueID()
	{
		return (uniqueID_atomic++);
	}

	//std::lock_guard<std::mutex> lck(mtx);

	std::atomic<int> uniqueID_atomic;
	std::unordered_map<int, Timer::ptr_p> t_map;
	Timer *audit_timer;
	Loop &_loop;
	Loop loop;
};

} /* namespace translib */
