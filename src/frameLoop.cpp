/*
 * FrameLoop.cpp
 *
 *  Created on: 2016年7月11日
 *      Author: 
 */

#include "translib/frameLoop.h"
#include "translib/timer.h"

namespace translib
{
FrameLoop::FrameLoop() : Loop()
{
	_frameTimer = new Timer(*this);
	_frameInterval = 10;
}

FrameLoop::~FrameLoop()
{
	delete _frameTimer;
}

void FrameLoop::setFrameInterval(uint32_t interval)
{
	if (interval > 0)
	{
		_frameInterval = interval;
	}
}

uint64_t FrameLoop::curFrameRound() const
{
	return _frameTimer->curRound();
}

void FrameLoop::onBeforeLoop()
{
	Super::onBeforeLoop();
	_frameTimer->startForever(_frameInterval, std::bind(&FrameLoop::onFrame, this));
}

void FrameLoop::onFrame()
{
}

} /* namespace translib */



