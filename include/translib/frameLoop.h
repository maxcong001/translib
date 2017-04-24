/*
 * FrameLoop.h
 *
 *  Created on: 2016年7月11日
 *      Author: 
 */

#ifndef INCLUDE_EC_FRAMELOOP_H_
#define INCLUDE_EC_FRAMELOOP_H_

#include "translib/loop.h"

namespace translib
{

class Timer;

/**
 * @brief event loop with frame ttimer
 * @details as there is infinite timer, so it exist in the registerd event forever. default frame timer is 10ms
 */
class FrameLoop : public translib::Loop {
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
	Timer * _frameTimer;
	//frame interval in ms
	uint32_t _frameInterval;
};

} /* namespace translib */



#endif /* INCLUDE_EC_FRAMELOOP_H_ */
