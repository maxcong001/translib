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

#include "translib/tcpSocket.h"
#include <event2/buffer.h>

namespace translib
{

#define INPUT_BUFFER bufferevent_get_input(_bev)
#define OUTPUT_BUFFER bufferevent_get_output(_bev)

TcpSocket::TcpSocket() : _bev(NULL),
						 _isClosing(false)
{
}

TcpSocket::~TcpSocket()
{
	if (_bev != NULL)
	{
		bufferevent_free(_bev);
		_bev = NULL;
	}
}

SocketFd TcpSocket::socket() const
{
	return (NULL == _bev) ? SOCKET_FD_INVALID : bufferevent_getfd(_bev);
}

void TcpSocket::getAddr(struct sockaddr_in *dest, uint32_t size) const
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
	if ((NULL == _bev) || _isClosing)
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

const uint8_t *TcpSocket::viewInputBuffer(uint32_t size) const
{
	return (NULL != _bev) ? evbuffer_pullup(INPUT_BUFFER, size) : NULL;
}

bool TcpSocket::readInputBuffer(uint8_t *dest, uint32_t size)
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
	if (_isClosing && (0 == evbuffer_get_length(OUTPUT_BUFFER)))
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
