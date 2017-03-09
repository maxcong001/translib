/*
 * tcpSocket.cpp
 *
 *  Created on: 2015年6月24日
 *      Author: 
 */

#include "translib/tcpSocket.h"
#include <event2/buffer.h>

namespace translib
{

#define INPUT_BUFFER bufferevent_get_input(_bev)
#define OUTPUT_BUFFER bufferevent_get_output(_bev)

TcpSocket::TcpSocket() :
		_bev(NULL),
		_isClosing(false)
{}

TcpSocket::~TcpSocket()
{
	if(_bev != NULL)
	{
		bufferevent_free(_bev);
		_bev = NULL;
	}
}

SocketFd TcpSocket::socket() const
{
	return (NULL == _bev) ? SOCKET_FD_INVALID : bufferevent_getfd(_bev);
}

void TcpSocket::getAddr(struct sockaddr_in * dest, uint32_t size) const
{
	uint32_t addrSize = sizeof(struct sockaddr_in);
	if ((NULL == _bev) || (size < addrSize))
	{
		return;
	}
	getpeername(socket(), (struct sockaddr *)dest, &addrSize);
}

bool TcpSocket::send(const char *data, uint32_t size)
{
	if((NULL == _bev) || _isClosing)
	{
		return false;
	}
	return (-1 != bufferevent_write(_bev, data, size));
}

void TcpSocket::close(bool waiting)
{
	if (NULL == _bev)
	{
		return;
	}

	if (waiting && (evbuffer_get_length(OUTPUT_BUFFER) > 0))
	{
		_isClosing = true;
		return;
	}

	closeImpl();
}

uint32_t TcpSocket::getInputBufferLength() const
{
	return (NULL != _bev) ? evbuffer_get_length(INPUT_BUFFER) : 0;
}

const uint8_t * TcpSocket::viewInputBuffer(uint32_t size) const
{
	return (NULL != _bev) ? evbuffer_pullup(INPUT_BUFFER, size) : NULL;
}

bool TcpSocket::readInputBuffer(uint8_t * dest, uint32_t size)
{
	return (NULL != _bev) ? (-1 != evbuffer_remove(INPUT_BUFFER, (void *)dest, size)) : false;
}

void TcpSocket::clearInputBuffer()
{
	if (NULL != _bev)
	{
		evbuffer_drain(INPUT_BUFFER, UINT32_MAX);
	}
}

void TcpSocket::checkClosing()
{
	if(_isClosing && (0 == evbuffer_get_length(OUTPUT_BUFFER)))
	{
		_isClosing = false;
		closeImpl();
	}
}

void TcpSocket::closeImpl()
{
	_isClosing = false;
	clearInputBuffer();
	bufferevent_disable(_bev, EV_WRITE);
	shutdown(socket(), 2);
}

} /* namespace translib */



