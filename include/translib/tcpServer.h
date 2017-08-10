/*
 * tcpServer.h
 *
 *  Created on: 2015年6月22日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPSERVER_H_
#define INCLUDE_EC_TCPSERVER_H_

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
	TcpServer(translib::TcpSessionFactory * sessionFactory = NULL, uint16_t threads = 0);
	virtual ~TcpServer();

	/** set IO thread num. need to be called before listen*/
	void setThreads(uint16_t threads);

	inline translib::Loop & getMasterLoop() const
	{
		return *_master;
	}

	inline translib::TcpSessionFactory * sessionFactory() const
	{
		return _sessionFactory;
	}


	bool listen(const char *ip, uint16_t port);

	void stop();

	/** wait for each thread to exit */
	void wait();

protected:

	virtual void onListenError() {};

	/**
	 * @brief when there is data ready.
	 * @note when there is data ready in TcpSession, report to TcpServer by default.over ride member function onRead of TcpSession
	 * @see TcpSession::onRead
	 */
	virtual void onSessionRead(translib::TcpSession *session) {};

	virtual void onSessionDisconnected(translib::TcpSession *session) {};

	virtual void onNewSession(translib::TcpSession *session) {};

private:
	translib::TcpServerDispatcher * getSessionDispatcher(translib::SessionId id);

	static void listenEventCallback(
		struct evconnlistener *listener,
		evutil_socket_t sock,
		struct sockaddr *addr,
		int len,
		void *ctx);

	static void listenErrorCallback(struct evconnlistener *listener, void *ctx);

private:
	struct evconnlistener *_eventListener;
	translib::TcpServerDispatcher * _master;
	std::vector<translib::TcpServerDispatcher *> _slavers;
	translib::TcpSessionFactory * _sessionFactory;

private:
	static translib::SessionId s_sessionIdGenerator;

};

} /* namespace translib */

#endif /* INCLUDE_EC_TCPSERVER_H_ */
