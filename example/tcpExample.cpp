/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
using namespace std;

#include "translib/loop.h"
#include "translib/tcpServer.h"
#include "translib/tcpClient.h"
#include "translib/timer.h"
#include "ringbuffer/ringbuffer.hpp"
#include "index.h"

class ExampleTcpClient : public translib::TcpClient
{
public:
	ExampleTcpClient(const translib::Loop &loop):
		translib::TcpClient(loop)
	{}


protected:
	virtual void onRead()
	{
	}

	virtual void onDisconnected()
	{
		cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
	}

	virtual void onConnected(int error)
	{
		cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
		send("hello", 5);
	}
};

class ExampleTcpServer : public translib::TcpServer
{
public:
	static ExampleTcpServer & instance()
	{
		static ExampleTcpServer ins;
		return ins;
	}

protected:
	virtual void onListenError()
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onSessionRead(translib::TcpSession *session)
	{
		char* buff = _ring_buffer.peek_head_p();
		if (!buff)
		{
			cout << "get buffer ptr "<<(void*)buff<<endl;
			return;
		}
		uint32_t length = session->getInputBufferLength();
		session->readInputBuffer((uint8_t *)buff, length);

		while(!_ring_buffer.add(length, buff))
		{
			std::this_thread::yield();
			std::this_thread::sleep_for (std::chrono::milliseconds(1));
		}

		cout << "ring buffer size "<< _ring_buffer.size()<<endl;

		cout << "ExampleTcpServer::" << __FUNCTION__
				<< " " << session->id() << " data:" << (void*)buff << endl;
	}

	virtual void onSessionDisconnected(translib::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onNewSession(translib::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << session->id() << endl;
	}
public:
	ring_buffer _ring_buffer;
};

class ExampleTcpClientManager : public translib::Loop
{
public:
	ExampleTcpClientManager():
		_client(*this),
		_timer(*this)
	{
		_timer.startForever(10, std::bind(&ExampleTcpClientManager::tick, this));
	}

protected:
	void tick()
	{
		cout << "round " << _timer.curRound() << endl;
		if (!_client.isConnected())
		{
			_client.connect("127.0.0.1", 4567);
		}
		char buffer[10000];
		_client.send(buffer, 10000);

		if (_timer.curRound() >= 30000)
		{
			ExampleTcpServer::instance().stop();
			stop();
		}
	}
private:
	ExampleTcpClient _client;
	translib::Timer _timer;
};

void tcpExample()
{
	ExampleTcpServer &tcpServer = ExampleTcpServer::instance();
	if (!tcpServer.listen("127.0.0.1", 4567))
	{
		cout << "listen failed" << endl;
		return;
	}

	ExampleTcpClientManager clientManager;
	clientManager.start(false);
	clientManager.wait();
	tcpServer.wait();
}


