/*
 * tcpClient.h
 *
 *  Created on: 2015年6月25日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPCLIENT_H_
#define INCLUDE_EC_TCPCLIENT_H_

#include "translib/tcpSocket.h"
#include "translib/loop.h"

namespace translib
{

/**
 * @brief 客户端TCP连接
 */
class TcpClient : public TcpSocket
{
public:
	TcpClient(const translib::Loop &loop);

	/** 是否已成功连接 */
	inline bool isConnected() const
	{
		return _isConnected;
	}

	/**
	 * @brief 连接服务器
	 * @param ip 服务器IP地址
	 * @param port 服务器监听端口
	 * @return 返回是否成功
	 */
	bool connect(const char *ip, uint16_t port);

protected:

	/** 有数据可读时处理接口 */
	virtual void onRead() {};

	/** 连接断开时处理接口 */
	virtual void onDisconnected() {};

	/**
	 * @brief 连接成功时处理接口
	 * @param error 连接错误码，0表示成功
	 */
	virtual void onConnected(int error) {};

private:
	void handleEvent(short events);

	static void readCallback(struct bufferevent *bev, void *ctx);
	static void writeCallback(struct bufferevent *bev, void *ctx);
	static void eventCallback(struct bufferevent *bev, short events, void *ctx);

private:
	const Loop &_loop;
	std::atomic<bool> _isConnected;
};

} /* namespace translib */


#endif /* INCLUDE_EC_TCPCLIENT_H_ */
