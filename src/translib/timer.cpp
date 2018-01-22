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

#include "translib/timer.h"
#include "translib/loop.h"

namespace translib
{

Timer::Timer(const translib::Loop &loop) : _loop(loop),
										   _event(NULL),
										   _interval(0),
										   _round(1),
										   _curRound(0),
										   _handler(NULL),
										   _userData(NULL),
										   _CBHandler(NULL),
										   _tid(0)

{
}

Timer::~Timer()
{
	reset();
}

bool Timer::startRounds(
	uint32_t interval,
	uint64_t round,
	translib::Timer::Handler handler)
{
	if (NULL != _event)
	{
		return false;
	}

	_event = event_new(_loop, -1, (round > 1) ? EV_PERSIST : 0, eventHandler, this);
	if (NULL == _event)
	{
		return false;
	}

	struct timeval tv = {};
	evutil_timerclear(&tv);
	tv.tv_sec = interval / 1000;
	tv.tv_usec = interval % 1000 * 1000;

	if (0 != event_add(_event, &tv))
	{
		reset();
		return false;
	}

	_interval = interval;
	_round = round;
	_curRound = 0;
	_handler = handler;

	return true;
}

bool Timer::startOnce(uint32_t interval, translib::Timer::Handler handler)
{
	return startRounds(interval, 1, handler);
}

bool Timer::startForever(uint32_t interval, translib::Timer::Handler handler)
{
	return startRounds(interval, uint32_t(-1), handler);
}

bool Timer::startAfter(
	uint32_t after,
	uint32_t interval,
	uint64_t round,
	translib::Timer::Handler handler)
{
	return startOnce(after, [=]() {
		startRounds(interval, round, handler);
	});
}

void Timer::reset()
{
	if (NULL != _event)
	{
		event_free(_event);
		_event = NULL;
		// make sure when we call isFinished, it is fininshed
		_curRound = _round + 1;
	}
}

void Timer::eventHandler(evutil_socket_t fd, short events, void *ctx)
{
	Timer *timer = (Timer *)ctx;
	translib::Timer::Handler handler = timer->_handler;

	timer->_curRound++;
	if (timer->_curRound >= timer->_round)
	{
		event_del(timer->_event);
		timer->reset();
	}

	handler();
}

} /* namespace translib */
