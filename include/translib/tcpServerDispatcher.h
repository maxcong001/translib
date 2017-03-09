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
 * @brief TCP服务器会话调度管理器
 * @details TcpServer负责监听连接，然后分配给TcpServerDispatcher管理
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

	/** @brief 返回所在TCPServer */
	inline translib::TcpServer * server() const
	{
		return _server;
	}

	/** 根据id获取Session */
	translib::TcpSessionPtr getSession(translib::SessionId id);
	/** 添加Session */
	void addSession(translib::SessionId id, translib::SocketFd sock);
	/** 删除Session */
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
