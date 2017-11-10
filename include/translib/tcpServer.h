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

#include <string>
#include <map>
#include <vector>
#include <event2/listener.h>
#include "translib/define.h"
#include "translib/tcpServerDispatcher.h"

namespace translib
{

/**
 * @brief TCP server
 * @details one listen thread multi IO thread
 */
class TcpServer
{
	friend class TcpSession;
	friend class TcpServerDispatcher;

  public:
	TcpServer(translib::TcpSessionFactory *sessionFactory = NULL, uint16_t threads = 0);
	virtual ~TcpServer();

	/** set IO thread num. need to be called before listen*/
	void setThreads(uint16_t threads);

	inline translib::Loop &getMasterLoop() const
	{
		return *_master;
	}

	inline translib::TcpSessionFactory *sessionFactory() const
	{
		return _sessionFactory;
	}

	bool listen(const char *ip, uint16_t port);

	void stop();

	/** wait for each thread to exit */
	void wait();

  protected:
	virtual void onListenError(){};

	/**
	 * @brief when there is data ready.
	 * @note when there is data ready in TcpSession, report to TcpServer by default.over ride member function onRead of TcpSession
	 * @see TcpSession::onRead
	 */
	virtual void onSessionRead(translib::TcpSession *session){};

	virtual void onSessionDisconnected(translib::TcpSession *session){};

	virtual void onNewSession(translib::TcpSession *session){};

  private:
	translib::TcpServerDispatcher *getSessionDispatcher(translib::SessionId id);

	static void listenEventCallback(
		struct evconnlistener *listener,
		evutil_socket_t sock,
		struct sockaddr *addr,
		int len,
		void *ctx);

	static void listenErrorCallback(struct evconnlistener *listener, void *ctx);

  private:
	struct evconnlistener *_eventListener;
	translib::TcpServerDispatcher *_master;
	std::vector<translib::TcpServerDispatcher *> _slavers;
	translib::TcpSessionFactory *_sessionFactory;

  private:
	static translib::SessionId s_sessionIdGenerator;
};

} /* namespace translib */
