/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: 
 */

#ifndef EC_TIMER_H_
#define EC_TIMER_H_

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
};

} /* namespace translib */

#endif /* EC_TIMER_H_ */
