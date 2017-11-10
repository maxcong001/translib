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

#include "translib/tcpSession.h"
#include "translib/tcpServerDispatcher.h"
#include "translib/tcpServer.h"

namespace translib
{

TcpSession::TcpSession() : _dispatcher(NULL),
						   _id(0)
{
}

bool TcpSession::attach(
	translib::TcpServerDispatcher *dispatcher,
	translib::SessionId id,
	translib::SocketFd sock)
{
	_id = id;
	_dispatcher = dispatcher;

	if (SOCKET_FD_INVALID == sock)
	{
		return false;
	}

	_bev = bufferevent_socket_new(_dispatcher->ev(),
								  sock, BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS);
	if (NULL == _bev)
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
	_dispatcher->server()->onNewSession(this);

	return true;
}

void TcpSession::onRead()
{
	_dispatcher->server()->onSessionRead(this);
}

void TcpSession::handleEvent(short events)
{
	bufferevent_disable(_bev, EV_READ | EV_WRITE);
	evutil_closesocket(socket());
	_dispatcher->server()->onSessionDisconnected(this);
	_dispatcher->removeSession(id());
}

void TcpSession::readCallback(struct bufferevent *bev, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->onRead();
}

void TcpSession::writeCallback(struct bufferevent *bev, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->checkClosing();
}

void TcpSession::eventCallback(struct bufferevent *bev, short events, void *data)
{
	TcpSession *session = (TcpSession *)data;
	session->handleEvent(events);
}

} /* namespace translib */
