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

#pragma once

#include <queue>
#include "translib/frameLoop.h"
#include "translib/tcpSession.h"
#include "translib/tcpSessionFactory.h"

namespace translib
{

class TcpServer;

/**
 * @brief TCP dispatcher
 * @details TcpServer for listen. then assign to TcpServerDispatcher
 * @see translib::TcpServer
 */
class TcpServerDispatcher : public translib::FrameLoop
{
	typedef FrameLoop Super;

	struct SessionAction
	{
		translib::SessionId id;
		translib::SocketFd sock;
	};

  public:
	TcpServerDispatcher(TcpServer *server);

	inline translib::TcpServer *server() const
	{
		return _server;
	}

	translib::TcpSessionPtr getSession(translib::SessionId id);

	void addSession(translib::SessionId id, translib::SocketFd sock);

	void removeSession(translib::SessionId id);

  protected:
	/** @override */
	virtual void onFrame();

  private:
	TcpServer *_server;
	std::map<translib::SessionId, translib::TcpSessionPtr> _sessions;
	std::queue<SessionAction> _actions;
	Mutex _mutex;
};

} /* namespace translib */
