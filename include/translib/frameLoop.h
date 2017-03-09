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
 * @brief 拥有幀定时器的事件循环
 * @details 由于有无限循环定时器，所以一直存在已注册的事件，不会有自动结束。默认幀时长为10毫秒
 */
class FrameLoop : public translib::Loop {
	typedef Loop Super;
public:
	FrameLoop();
	virtual ~FrameLoop();

	/**
	 * @brief 设置幀时长(毫秒)
	 * @note 应该在启动事件循环调用
	 * @details 没调用此函数则默认幀时长为10毫秒
	 * @param interval 幀时长(毫秒)，为0不会生效
	 * @see Loop::start
	 */
	void setFrameInterval(uint32_t interval);

	/** @brief 获取当前幀周期 */
	uint64_t curFrameRound() const;

protected:
	/** @override */
	virtual void onBeforeLoop();

	/**
	 * @brief 每幀事件处理
	 */
	virtual void onFrame();

private:
	//幀定时器
	Timer * _frameTimer;
	//幀时长(毫秒)
	uint32_t _frameInterval;
};

} /* namespace translib */



#endif /* INCLUDE_EC_FRAMELOOP_H_ */
