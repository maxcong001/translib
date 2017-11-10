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

#include "translib/loop.h"

namespace translib
{

class Timer;

/**
 * @brief event loop with frame ttimer
 * @details as there is infinite timer, so it exist in the registerd event forever. default frame timer is 10ms
 */
class FrameLoop : public translib::Loop
{
	typedef Loop Super;

  public:
	FrameLoop();
	virtual ~FrameLoop();

	/**
	 * set frame interval 
	 * @note should be called in the start event loop
	 * @details defalt is 10 ms if this function is not called
	 * @param interval  not usable if interval is set to 0
	 * @see Loop::start
	 */
	void setFrameInterval(uint32_t interval);

	/** get current round */
	uint64_t curFrameRound() const;

  protected:
	virtual void onBeforeLoop();

	virtual void onFrame();

  private:
	//timer of frame
	Timer *_frameTimer;
	//frame interval in ms
	uint32_t _frameInterval;
};

} /* namespace translib */
