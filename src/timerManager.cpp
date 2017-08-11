/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: 
 */

#include "translib/timerManager.h"
namespace translib
{

bool TimerManager::init()
{
#if 0
	auto work_fun = std::bind(&TimerManager::do_work, this);
	// actually we want the thread never exit.
	// So we do not kill the thread.
	// start once and leave it there.
	// suppose when the loop stop, the thread will exit then.
	std::thread timer_thread(work_fun);
#endif
	_loop.start(true);
	//start audit timer
	audit_timer = new Timer(_loop);
	audit_timer->startForever(AUDIT_TIMER, [this] {
		// is this right?
		for_each(t_map.begin(), t_map.end(), [&](std::pair<const int, Timer::ptr_p> &i) {
			if ((i.second)->isFinished())
			{
				t_map.erase(i.first);
			}
		});
	});
	return true;
}

Timer::ptr_p TimerManager::getTimer(int *timerID)
{
	int tid = getUniqueID();
	Timer::ptr_p tmp_ptr(new Timer(_loop));
	t_map.emplace(tid, tmp_ptr);
	*timerID = tid;
	return tmp_ptr;
}
// you can call timer.stop or pass in the ID and we will kill the timer
bool TimerManager::killTimer(int timerID)
{
	auto iter = t_map.find(timerID);
	if (iter != t_map.end())
	{
		(iter->second)->stop();
	}
	else
	{
	}
	return true;
}

} /* namespace translib */
