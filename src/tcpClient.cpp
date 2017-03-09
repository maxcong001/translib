/*
 * tcpClient.cpp
 *
 *  Created on: 2015年6月25日
 *      Author: 
 */


#include "translib/tcpClient.h"

namespace translib
{

TcpClient::TcpClient(const translib::Loop &loop) :
		_loop(loop),
		_isConnected(false)
{}

bool TcpClient::connect(const char *ip, uint16_t port)
{
	if (NULL != _bev)
	{
		return false;
	}

	_bev = bufferevent_socket_new(_loop.ev(), SOCKET_FD_INVALID,
			BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS);
	if(NULL == _bev)
	{
		return false;
	}

	bufferevent_setcb(_bev, readCallback, writeCallback, eventCallback, this);
	if (-1 == bufferevent_enable(_bev, EV_READ | EV_WRITE))
	{
		bufferevent_free(_bev);
		_bev = NULL;
		return false;
	}


	struct sockaddr_in serverAddr;
	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	evutil_inet_pton(AF_INET, ip, &serverAddr.sin_addr);
	serverAddr.sin_port = htons(port);

	if (-1 == bufferevent_socket_connect(_bev, (struct sockaddr *)&serverAddr, sizeof(serverAddr)))
	{
		bufferevent_free(_bev);
		_bev = NULL;
		return false;
	}

	evutil_make_socket_nonblocking(socket());
	return true;
}

void TcpClient::handleEvent(short events)
{
	if (events & (BEV_EVENT_EOF | BEV_EVENT_ERROR | BEV_EVENT_TIMEOUT))
	{
		bufferevent_disable(_bev, EV_READ | EV_WRITE);
		evutil_closesocket(socket());
		_isConnected = false;
		onDisconnected();
		bufferevent_free(_bev);
		_bev = NULL;
		return;
	}

	if (events & BEV_EVENT_CONNECTED)
	{
		int socketError = EVUTIL_SOCKET_ERROR();
		if ((socketError != ECONNREFUSED) && (socketError != ETIMEDOUT))
		{
			_isConnected = true;
			onConnected(0);
		}
		else
		{
			onConnected(socketError);
		}
	}
}

void TcpClient::readCallback(struct bufferevent *bev, void *ctx)
{
	TcpClient *socket = (TcpClient *)ctx;
	socket->onRead();
}

void TcpClient::writeCallback(struct bufferevent *bev, void *ctx)
{
	TcpClient *socket = (TcpClient *)ctx;
	socket->checkClosing();
}

void TcpClient::eventCallback(struct bufferevent *bev, short events, void *ctx)
{
	TcpClient *socket = (TcpClient *)ctx;
	socket->handleEvent(events);
}


} /* namespace translib */
