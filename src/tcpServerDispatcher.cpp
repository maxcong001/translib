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
#include "translib/tcpServerDispatcher.h"
#include "translib/tcpServer.h"

namespace translib
{

TcpServerDispatcher::TcpServerDispatcher(TcpServer *server) : _server(server)
{
}

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
		SessionAction &action = _actions.front();

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
