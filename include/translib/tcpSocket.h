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

#endif /* INCLUDE_EC_TCPSOCKET_H_ */
