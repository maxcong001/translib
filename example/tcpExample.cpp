/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
using namespace std;

#include "translib/loop.h"
#include "translib/tcpServer.h"
#include "translib/tcpClient.h"
#include "translib/timer.h"

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
		uint32_t length = session->getInputBufferLength();
		char * buff = new char[length + 1];
		session->readInputBuffer((uint8_t *)buff, length);
		buff[length] = '\0';
		cout << "ExampleTcpServer::" << __FUNCTION__
				<< " " << session->id() << " data:" << buff << endl;
		delete [] buff;
		session->close();
	}

	virtual void onSessionDisconnected(translib::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
	}

	virtual void onNewSession(translib::TcpSession *session)
	{
		cout << "ExampleTcpServer::" << __FUNCTION__ << session->id() << endl;
	}
};

class ExampleTcpClientManager : public translib::Loop
{
public:
	ExampleTcpClientManager():
		_client(*this),
		_timer(*this)
	{
		_timer.startForever(1000, std::bind(&ExampleTcpClientManager::tick, this));
	}

protected:
	void tick()
	{
		cout << "round " << _timer.curRound() << endl;
		if (!_client.isConnected())
		{
			_client.connect("127.0.0.1", 4567);
		}

		if (_timer.curRound() >= 30)
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


