/*
 * tcpServerDispatcher.h
 *
 *  Created on: 2015年6月25日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPSERVERDISPATCHER_H_
#define INCLUDE_EC_TCPSERVERDISPATCHER_H_

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

	inline translib::TcpServer * server() const
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
	TcpServer * _server;
	std::map<translib::SessionId, translib::TcpSessionPtr> _sessions;
	std::queue<SessionAction> _actions;
	Mutex _mutex;
};

} /* namespace translib */



#endif /* INCLUDE_EC_TCPSERVERDISPATCHER_H_ */
