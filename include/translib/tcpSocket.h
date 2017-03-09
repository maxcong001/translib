/*
 * tcpSocket.h
 *
 *  Created on: 2015年6月24日
 *      Author: 
 */

#ifndef INCLUDE_EC_TCPSOCKET_H_
#define INCLUDE_EC_TCPSOCKET_H_

#include <atomic>
#include <event2/bufferevent.h>
#include "translib/define.h"

namespace translib
{

/**
 * @brief TCP连接基类
 */
class TcpSocket
{
public:
	TcpSocket();
	virtual ~TcpSocket();

	/** 获取socket */
	SocketFd socket() const;
	/** 获取连接地址 */
	void getAddr(struct sockaddr_in * dest, uint32_t size) const;

	/**
	 * @brief 发送数据
	 * @param data 数据地址
	 * @param size 数据字节大小
	 * @return 返回是否成功
	 */
	bool send(const char *data, uint32_t size);

	/**
	 * @brief 关闭连接
	 * @param waiting 是否等待数据发送完再关闭
	 */
	void close(bool waiting = true);

	/** 获取收数据缓冲区数据长度 */
	uint32_t getInputBufferLength() const;
	/** 查看收数据缓冲区 */
	const uint8_t * viewInputBuffer(uint32_t size) const;
	/** 读取收数据缓冲区 */
	bool readInputBuffer(uint8_t * dest, uint32_t size);
	/** 清空收数据缓冲区 */
	void clearInputBuffer();

protected:
	//子类需要调用此接口检查正在关闭的数据
	void checkClosing();

private:
	void closeImpl();

protected:
	struct bufferevent *_bev;

private:
	std::atomic<bool> _isClosing;
};

} /* namespace translib */



#endif /* INCLUDE_EC_TCPSOCKET_H_ */
