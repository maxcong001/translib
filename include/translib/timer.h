/*
 * timer.h
 *
 *  Created on: 2015年5月31日
 *      Author: 
 */

#ifndef EC_TIMER_H_
#define EC_TIMER_H_

#include "loop.h"

namespace translib
{

class Loop;

/**
 * @brief 定时器类。
 * @details 封装了libevent的定时器，
 */
class Timer
{
public:
	/** @brief 定时器回调函数 */
	typedef std::function<void ()> Handler;

public:
	Timer(const translib::Loop &loop);
	virtual ~Timer();

	/**
	 * @brief 启动多轮循环定时器
	 * @param interval 定时时长，以毫秒为单位
	 * @param round 总周期
	 * @param handler 定时回调函数
	 */
	bool startRounds(uint32_t interval, uint64_t round, translib::Timer::Handler handler);

	/**
	 * @brief 启动单次定时器
	 * @param interval 定时时长，以毫秒为单位
	 * @param handler 定时回调函数
	 */
	bool startOnce(uint32_t interval, translib::Timer::Handler handler);

	/**
	 * @brief 启动无限循环定时器
	 * @param interval 定时时长，以毫秒为单位
	 * @param handler 定时回调函数
	 */
	bool startForever(uint32_t interval, translib::Timer::Handler handler);

	/**
	 * @brief 在指定事件后启动定时器
	 * @param after 在此时长后开始定时，以毫秒为单位
	 * @param interval 定时时长，以毫秒为单位
	 * @param round 总周期
	 * @param handler 定时回调函数
	 */
	bool startAfter(
			uint32_t after,
			uint32_t interval,
			uint64_t round,
			translib::Timer::Handler handler);

	/** 获取定时时长 */
	inline uint32_t interval() const
	{
		return _interval;
	}

	/** 获取总周期 */
	inline uint64_t round() const
	{
		return _round;
	}

	/** 获取当前周期 */
	inline uint64_t curRound() const
	{
		return _curRound;
	}

	/** 是否已经结束 */
	inline bool isFinished() const
	{
		return _curRound >= _round;
	}

private:
	void reset();
	static void eventHandler(evutil_socket_t fd, short events, void *ctx);

private:
	const Loop &_loop;
	struct event *_event;
	uint32_t _interval; //定时时长，以毫秒为单位
	uint64_t _round; //总周期
	uint64_t _curRound; //当前周期
	translib::Timer::Handler _handler;
};

} /* namespace translib */

#endif /* EC_TIMER_H_ */
