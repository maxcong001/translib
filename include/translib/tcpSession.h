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
 * @brief 服务端TCP连接
 */
class TcpSession : public translib::TcpSocket, public std::enable_shared_from_this<TcpSession>
{
public:
	TcpSession();

	/** 获取管理此连接的调度器 */
	inline translib::TcpServerDispatcher * dispatcher() const
	{
		return _dispatcher;
	}

	/** 获取唯一会话id */
	inline translib::SessionId id() const
	{
		return _id;
	}

	/**
	 * @brief 关联调度器和套接字
	 * @return 是否成功
	 */
	bool attach(
			translib::TcpServerDispatcher * dispatcher,
			translib::SessionId id,
			translib::SocketFd sock);

protected:
	/**
	 * @brief 有数据可读时处理接口
	 * @details 默认行为就数据可读时转调TcpServer::onSessionRead，子类重载时如果没有调用
	 * 父类(TcpSession)的onRead，则TcpServer::onSessionRead也不会被调用，建议由Session
	 * 自己处理可读数据而通知到TcpServer处理
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
