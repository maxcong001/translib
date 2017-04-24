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

class TcpClient : public TcpSocket
{
public:
	TcpClient(const translib::Loop &loop);

	inline bool isConnected() const
	{
		return _isConnected;
	}

	bool connect(const char *ip, uint16_t port);

protected:

	virtual void onRead() {};

	virtual void onDisconnected() {};

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
