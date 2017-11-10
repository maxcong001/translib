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

#include <map>
#include "translib/define.h"

namespace translib
{

/**
 * @brief on top of event_base
 * @details this class is thread safe, but event loop is sigle thread
 * if there is no event registered, event will stop auto
 */
class Loop
{
  public:
	static const int StatusInit = 0;

	static const int StatusRunning = 2;

	static const int StatusFinished = 4;

	Loop();
	virtual ~Loop();

	/** convert to event_base * pointer*/
	inline operator event_base *() const
	{
		return _base;
	};

	/** @brief get event_base * pointer*/
	inline event_base *ev() const
	{
		return _base;
	}

	inline uint32_t id() const
	{
		return _id;
	}

	/** run in a new thread? */
	inline bool isThread() const
	{
		return (NULL != _thread);
	}

	/** get status */
	inline int status() const
	{
		return _status;
	}

	/**
	 * @brief start event loop
	 * @details if this is called in the current thread. it will block current thread until the end of time loop
	 * see onBeforeStart
	 */
	bool start(bool newThread = true);

	void wait();

	/** if run with new thread, will stop the new thread
	 * waiting: if call the active callback before exit
	 */
	void stop(bool waiting = true);

  protected:
	virtual bool onBeforeStart();

	virtual void onBeforeLoop();

	virtual void onAfterLoop();

	virtual void onAfterStop();

  private:
	void _run();

  private:
	uint32_t _id;
	event_base *_base;

	std::thread *_thread;

	std::atomic<int> _status;

  public:
	static Loop *curLoop();

	// ID to loop
	static Loop *get(uint32_t id);

  private:
	static std::mutex _sMutex;
	static std::map<uint32_t, translib::Loop *> _sLoops;
	static uint32_t _sIdGenerater;
};

} /* namespace translib */
