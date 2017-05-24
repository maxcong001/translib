#pragma once
#include "logger/logger.h"
#include "ringbuffer/buffer_message.hpp"
#include "tcpServer.h"

namespace translib {
class TcpServerAPP : public translib::TcpServer {
public:
  virtual void onListenError() { __LOG(debug, ""); }

  virtual void onSessionRead(translib::TcpSession *session) {
    char *buff = _ring_buffer.peek_head_p();
    if (!buff) {
      __LOG(error, "get invalid buffer ptr " << (void *)buff);
      return;
    }
    uint32_t length = session->getInputBufferLength();
    session->readInputBuffer((uint8_t *)buff, length);

    __LOG(error, "receive message with length : " << length);
    

    while (!_ring_buffer.add(length, buff)) {
      std::this_thread::yield();
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    __LOG(debug, "now ring buffer size " << _ring_buffer.size());
    

    __LOG(debug, "sesson id is : " << session->id()
                                   << " data:" << (void *)buff);
  }

  virtual void onSessionDisconnected(translib::TcpSession *session) {
    __LOG(debug, " ");
  }

  virtual void onNewSession(translib::TcpSession *session) {
    __LOG(debug, " ");
  }

public:
  ring_buffer _ring_buffer;
};

} // end of namespace translib