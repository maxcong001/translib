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
#include <memory>

namespace translib
{

class Loop;
class Timer
{
  public:
	/** @brief callback fuction */
	typedef std::function<void()> Handler;
	typedef std::function<int(void *, int)> CBHandler;
	typedef std::shared_ptr<Timer> ptr_p;

  public:
	Timer(const translib::Loop &loop);
	virtual ~Timer();

	/**
	 * @brief start timer for several times
	 * @param interval ms
	 * @param round 
	 * @param handler callback function
	 */
	bool startRounds(uint32_t interval, uint64_t round, translib::Timer::Handler handler);

	/**
	 * @brief 
	 * @param interval ms
	 * @param handler callback fuction
	 */
	bool startOnce(uint32_t interval, translib::Timer::Handler handler);

	/**
	 * @brief 
	 * @param interval ms
	 * @param handler callback function
	 */
	bool startForever(uint32_t interval, translib::Timer::Handler handler);

	/**
	 * @brief 
	 * @param after ms
	 * @param interval ms
	 * @param round 
	 * @param handler callback function
	 */
	bool startAfter(
		uint32_t after,
		uint32_t interval,
		uint64_t round,
		translib::Timer::Handler handler);
	/**
	 * @brief this is for adapt some wrapper function, if the callback return -1, then stop the timer.
	 * @param interval ms
	 * @param handler callback function
	 * @param userData bring back while callback is called
	 */

	bool startCB(uint32_t interval, translib::Timer::CBHandler handler, void *userData, int tid)
	{
		_tid = tid;
		_userData = userData;
		_CBHandler = handler;
		return startForever(interval, [this] {
			int ret = _CBHandler(_userData, _tid);
			if (ret == -1)
			{
				this->stop();
			}
		});
	}
	/** get timer interval */
	inline uint32_t interval() const
	{
		return _interval;
	}

	/** get timer rounds */
	inline uint64_t round() const
	{
		return _round;
	}

	/** get current round */
	inline uint64_t curRound() const
	{
		return _curRound;
	}

	/** finished? */
	inline bool isFinished() const
	{
		return _curRound >= _round;
	}
	void stop()
	{
		reset();
	}

  private:
	void reset();
	static void eventHandler(evutil_socket_t fd, short events, void *ctx);

  private:
	const Loop &_loop;
	struct event *_event;
	uint32_t _interval; //ms
	uint64_t _round;
	uint64_t _curRound;
	translib::Timer::Handler _handler;
	// actually we could capture the userdata and callback, but just left it here,
	// in case we need to get the user data and callback
	void *_userData;
	translib::Timer::CBHandler _CBHandler;
	int _tid;
};

} /* namespace translib */
