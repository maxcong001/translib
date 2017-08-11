/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: 
 */

#pragma once

#include "loop.h"
#include "timer.h"
#include <unordered_map>
#include <atomic>
#include <thread>
#include <algorithm>

namespace translib
{

class Loop;
class Timer;
#define AUDIT_TIMER 5000
class TimerManager
{
  public:
	TimerManager() : _loop()
	{
		init();
	}
	~TimerManager()
	{
		//stop audit timer
		if (!audit_timer)
		{
			delete audit_timer;
		}
		audit_timer = NULL;
	}
	bool init();
	Timer::ptr_p getTimer(int *timerID = NULL);
	bool killTimer(int timerID);

  protected:
  private:
	int getUniqueID()
	{
		return (uniqueID_atomic++);
	}

	//std::lock_guard<std::mutex> lck(mtx);

	std::atomic<int> uniqueID_atomic;
	std::unordered_map<int, Timer::ptr_p> t_map;
	std::mutex mtx;
	Loop _loop;
	Timer *audit_timer;
};

} /* namespace translib */
