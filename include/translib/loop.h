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
	inline operator event_base * () const
	{
		return _base;
	};

	/** @brief get event_base * pointer*/
	inline event_base * ev() const
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

	static Loop * curLoop();

    // ID to loop
	static Loop * get(uint32_t id);

private:
	static std::mutex _sMutex;
	static std::map<uint32_t, translib::Loop *> _sLoops;
	static uint32_t _sIdGenerater;
};

} /* namespace translib */

#endif /* INCLUDE_EC_LOOP_H_ */
