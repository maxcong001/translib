/*
 * loop.h
 *
 *  Created on: 2015年5月30日
 *      Author: 
 */

#ifndef INCLUDE_EC_LOOP_H_
#define INCLUDE_EC_LOOP_H_


#include <map>
#include "translib/define.h"

namespace translib
{

/**
 * @brief 事件循环，对event_base的封装。
 * @details 类本身是线程安全的，然而事件循环只能运行在一个线程中。
 * 如果没有已注册的事件，事件则会自动结束
 */
class Loop
{
public:
	/** @brief 初始状态 */
	static const int StatusInit = 0;
	/** @brief 运行中 */
	static const int StatusRunning = 2;
	/** @brief 已结束 */
	static const int StatusFinished = 4;

	Loop();
	virtual ~Loop();

	/** @brief 转换为event_base *指针 */
	inline operator event_base * () const
	{
		return _base;
	};

	/** @brief 获取event_base *指针*/
	inline event_base * ev() const
	{
		return _base;
	}

	/** @brief 获得自动生成的Id号 */
	inline uint32_t id() const
	{
		return _id;
	}

	/** @brief 是否是以新线程在运行 */
	inline bool isThread() const
	{
		return (NULL != _thread);
	}

	/** @brief 当前运行状态 */
	inline int status() const
	{
		return _status;
	}

	/**
	 * @brief 启动事件循环
	 * @param newThread 是否启动新线程运行时间循环
	 * @details 在当前线程运行会阻塞当前线程直到时间循环结束，@see onBeforeStart
	 * @return 是否成功
	 */
	bool start(bool newThread = true);

	/**
	 * @brief 等待时间循环(线程)结束
	 * @details 不是以新线程运行的调用此函数不会有任何效果
	 * @see bool Loop::startThread()
	 */
	void wait();

	/**
	 * @brief 停止事件循环
	 * @details 如果以新线程运行的，则会停止新线程。
	 * @param waiting 是否直到运行所有激活事件的回调才退出
	 */
	void stop(bool waiting = true);

protected:
	/**
	 * @brief 启动前触发
	 * @details 运行在调用start时所在的线程，一般实现此函数处理启动前的准备工作。
	 * @return 返回false幀
	 */
	virtual bool onBeforeStart();

	/**
	 * @brief 事件循环运行前触发
	 * @details 运行在事件循环所在线程
	 */
	virtual void onBeforeLoop();

	/**
	 * @brief 事件循环结束后触发
	 * @details 运行在事件循环所在线程
	 */
	virtual void onAfterLoop();

	/**
	 * @brief 停止后触发
	 * @details 运行在调用stop时所在的线程，一般实现此函数处理启动前的准备工作
	 */
	virtual void onAfterStop();

private:
	void _run();

private:
	uint32_t _id;
	event_base *_base;
	//线程
	std::thread *_thread;
	//运行状态
	std::atomic<int> _status;

public:
	/**
	 * @brief 返回当前线程的Loop
	 * @return 当前线程Loop指针，没有Loop则NULL
	 */
	static Loop * curLoop();

	/**
	 * @brief 根据id查找Loop
	 * @return Loop指针
	 */
	static Loop * get(uint32_t id);

private:
	static std::mutex _sMutex;
	static std::map<uint32_t, translib::Loop *> _sLoops;
	static uint32_t _sIdGenerater;
};

} /* namespace translib */

#endif /* INCLUDE_EC_LOOP_H_ */
