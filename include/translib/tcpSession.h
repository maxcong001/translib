/*
 * tcpSession.h
 *
 *  Created on: 2015年6月22日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPSESSION_H_
#define INCLUDE_EC_TCPSESSION_H_

#include "translib/tcpSocket.h"

namespace translib
{

class TcpServerDispatcher;

/**
 * @brief TCP server
 */
class TcpSession : public translib::TcpSocket, public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession();

	/** get TCP server dispatcher */
	inline translib::TcpServerDispatcher * dispatcher() const
	{
		return _dispatcher;
	}

	/** get session ID */
	inline translib::SessionId id() const
	{
		return _id;
	}

	/**
	 * @brief attach session dispatcher and session
	 * @return 
	 */
	bool attach(
			translib::TcpServerDispatcher * dispatcher,
			translib::SessionId id,
			translib::SocketFd sock);

protected:
	/**
	 * @brief 
	 * @details cal TcpServer::onSessionRead by default. 
	 * @see TcpServer::onSessionRead
	 */
	virtual void onRead();

private:
	void handleEvent(short events);

	static void readCallback(struct bufferevent *bev, void *data);
	static void writeCallback(struct bufferevent *bev, void *data);
	static void eventCallback(struct bufferevent *bev, short events, void *data);

private:
	translib::TcpServerDispatcher *_dispatcher;
	translib::SessionId _id;
};

typedef std::shared_ptr<TcpSession> TcpSessionPtr;

} /* namespace translib */


#endif /* INCLUDE_EC_TCPSESSION_H_ */
