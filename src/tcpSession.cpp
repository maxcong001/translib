/*
 * tcpSession.cpp
 *
 *  Created on: 2015年6月22日
 *      Author: 
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
