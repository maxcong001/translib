/*
 * timer.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
using namespace std;

#include "translib/loop.h"
#include "translib/timer.h"
#include "index.h"

class ExampleTimerServer
{
public:
	ExampleTimerServer():
		_timer(_loop)
	{
	}

	static ExampleTimerServer & instance()
	{
		static ExampleTimerServer ins;
		return ins;
	}

	void start()
	{
		_timer.startAfter(1000, 100, 2, [this]() {
			cout << "startAfter round " << _timer.curRound() << endl;
			if (_timer.isFinished())
			{
				_timer.startRounds(1000, 5, std::bind(&ExampleTimerServer::tick, this));
			}
		});
		cout << "before start" << endl;
		_loop.start();
		_loop.wait();
	}

	void stop()
	{
		_loop.stop();
	}

private:
	void tick()
	{
		uint64_t round = _timer.curRound();
		cout << "round " << round << " begin" << endl;
		if (_timer.isFinished())
		{
			stop();
		}
	}

private:
	translib::Loop _loop;
	translib::Timer _timer;
};

void timerExample()
{
	ExampleTimerServer::instance().start();
}
