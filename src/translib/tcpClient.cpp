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

#include "translib/tcpClient.h"
#include <netinet/tcp.h>
namespace translib
{

TcpClient::TcpClient(const translib::Loop &loop) : _loop(loop),
												   _isConnected(false)
{
}

bool TcpClient::connect(const char *ip, uint16_t port)
{
	if (NULL != _bev)
	{
		return false;
	}
	_bev = bufferevent_socket_new(_loop.ev(), SOCKET_FD_INVALID,
								  BEV_OPT_CLOSE_ON_FREE | BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS);
	if (NULL == _bev)
	{
		return false;
	}
#ifdef TRANS_TCP_NO_DELAY
	evutil_socket_t fd = bufferevent_getfd(_bev);
	int enable = 1;
	setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, (void *)&enable, sizeof(enable));
// IPPROTO_TCP SOL_SOCKET
#endif

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
