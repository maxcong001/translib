/*
 * tcpServerDispatcher.cpp
 *
 *  Created on: 2015年6月25日
 *      Author: 
 */

#include "translib/tcpServerDispatcher.h"
#include "translib/tcpServer.h"

namespace translib
{

TcpServerDispatcher::TcpServerDispatcher(TcpServer *server) :
		_server(server)
{}

translib::TcpSessionPtr TcpServerDispatcher::getSession(translib::SessionId id)
{
	MutexLock lock(_mutex);
	auto iter = _sessions.find(id);
	return (iter != _sessions.end()) ? iter->second : NULL;
}

void TcpServerDispatcher::addSession(translib::SessionId id, translib::SocketFd sock)
{
	MutexLock lock(_mutex);
	_actions.push({id, sock});
}

void TcpServerDispatcher::removeSession(translib::SessionId id)
{
	MutexLock lock(_mutex);
	_actions.push({id, SOCKET_FD_INVALID});
}

void TcpServerDispatcher::onFrame()
{
	if (_actions.empty())
	{
		return;
	}

	MutexLock lock(_mutex);
	while (!_actions.empty())
	{
		SessionAction & action = _actions.front();

		if (action.sock != SOCKET_FD_INVALID) //add session
		{
			TcpSessionPtr session = server()->sessionFactory()->create();
			if (session->attach(this, action.id, action.sock))
			{
				_sessions[session->id()] = session;
			}
		}
		else //delete session
		{
			_sessions.erase(action.id);
		}

		_actions.pop();
	}
}


} /* namespace translib */
