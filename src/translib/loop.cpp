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

#include "translib/loop.h"

thread_local translib::Loop *curThreadLoop;

namespace translib
{
std::mutex Loop::_sMutex;
std::map<uint32_t, translib::Loop *> Loop::_sLoops;
uint32_t Loop::_sIdGenerater = 0;

Loop::Loop() : _id(0),
			   _base(NULL),
			   _thread(NULL),
			   _status(StatusInit)
{
	_sMutex.lock();
	_sIdGenerater++;
	_id = _sIdGenerater;

	if (_sLoops.empty())
	{
#ifdef PLATFORM_WINDOWS
		WSADATA wsadata;
		WSAStartup(MAKEWORD(2, 2), &wsadata);
		evthread_use_windows_threads();
#else
		// this may cause memory leak see
		// https://github.com/libevent/libevent/issues/55
		evthread_use_pthreads();
#endif //PLATFORM_WINDOWS
	}

	_sLoops[_id] = this;
	_sMutex.unlock();
	_base = event_base_new();
	if (!_base)
	{
		// note!!!! if you catch this exception
		// please remember call the distructure function
		// !!!!!!! this is important
		throw std::logic_error(CREATE_EVENT_FAIL);
	}
}

Loop::~Loop()
{
	if (NULL != _thread)
	{
		stop();
		wait();
		delete _thread;
	}

	if (NULL != _base)
	{
		event_base_free(_base);
		_base = NULL;
	}

	_sMutex.lock();
	_sLoops.erase(_id);
	if (_sLoops.empty())
	{
#ifdef PLATFORM_WINDOWS
		WSACleanup();
#endif //PLATFORM_WINDOWS
	}
	_sMutex.unlock();
}

bool Loop::start(bool newThread)
{
	if (_status != StatusInit)
	{
		return false;
	}

	if (!onBeforeStart())
	{
		return false;
	}

	if (newThread)
	{
		_thread = new std::thread(std::bind(&Loop::_run, this));
	}
	else
	{
		_run();
	}

	return true;
}

void Loop::wait()
{
	if (NULL != _thread && StatusFinished != _status)
	{
		_thread->join();
	}
}

void Loop::stop(bool waiting)
{
	if (StatusFinished == _status)
	{
		return;
	}

	waiting ? event_base_loopexit(_base, NULL) : event_base_loopbreak(_base);
	onAfterStop();
}

void Loop::_run()
{
	_status = StatusRunning;
	curThreadLoop = this;
	onBeforeLoop();
	event_base_loop(_base, 0);
	onAfterLoop();
	curThreadLoop = NULL;
	_status = StatusFinished;
}

bool Loop::onBeforeStart()
{
	return true;
}

void Loop::onBeforeLoop()
{
}

void Loop::onAfterLoop()
{
}

void Loop::onAfterStop()
{
}

Loop *Loop::curLoop()
{
	return curThreadLoop;
}

Loop *Loop::get(uint32_t id)
{
	std::unique_lock<std::mutex> lock(_sMutex);
	auto iter = _sLoops.find(id);
	return (iter != _sLoops.end()) ? iter->second : NULL;
}

} /* namespace translib */
