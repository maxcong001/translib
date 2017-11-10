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

#pragma once

#include <atomic>
#include <event2/bufferevent.h>
#include "translib/define.h"

namespace translib
{

/**
 * @brief TCP base 
 */
class TcpSocket
{
public:
  TcpSocket();
  virtual ~TcpSocket();

  /** get socket */
  SocketFd socket() const;
  /** get connection address */
  void getAddr(struct sockaddr_in *dest, uint32_t size) const;

  /**
	 * @brief send message
	 * @param data the address of data
	 * @param size 
	 * @return 
	 */
  bool send(const char *data, uint32_t size);

  /**
	 * @brief close connection
	 * @param waiting: close after sending the data
	 */
  void close(bool waiting = true);

  /** get data length in the catche */
  uint32_t getInputBufferLength() const;
  /** view input buffer */
  const uint8_t *viewInputBuffer(uint32_t size) const;
  /** read input buffer */
  bool readInputBuffer(uint8_t *dest, uint32_t size);
  /** clear input buffer */
  void clearInputBuffer();

protected:
  // son need to call this interface to check the closing connection
  void checkClosing();

private:
  void closeImpl();

protected:
  struct bufferevent *_bev;

private:
  std::atomic<bool> _isClosing;
};

} /* namespace translib */
