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

#include <iostream>
#include <functional>
using namespace std;

#include "translib/loop.h"
#include "translib/timer.h"
#include "index.h"

class ExampleTimerServer
{
  public:
	ExampleTimerServer() : _timer(_loop)
	{
	}

	static ExampleTimerServer &instance()
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
