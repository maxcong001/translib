#pragma once
#include "logger/logger.h"
#include "ringbuffer/buffer_message.hpp"
#include "tcpServer.h"

namespace translib {

typedef std::function<void(std::shared_ptr<char>, size_t)> MSG_FUN;

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

    __LOG(debug, "receive message with length : " << length);

    while (!_ring_buffer.add(length, buff)) {
      __LOG(info, "add to ring buffer fail");
      std::this_thread::yield();
      // std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    __LOG(info, "now ring buffer size " << _ring_buffer.size());

    __LOG(debug, "sesson id is : " << session->id()
                                   << " data:" << (void *)buff);
  }

  virtual void onSessionDisconnected(translib::TcpSession *session) {
    __LOG(debug, " ");
  }

  virtual void onNewSession(translib::TcpSession *session) {
    __LOG(debug, " ");
  }

  void setMsgCallback(MSG_FUN cb) { msg_callback_func = cb; }
  MSG_FUN getMsgCallback() { return msg_callback_func; }

  void ringBuffer_f() {
    while (1) {
      int msg_len;
      // get the header pointer and check if it is valid
      tcp_message *msg_p = (tcp_message *)_ring_buffer.peek_tail_p();
      if (msg_p) {
        msg_len = msg_p->get_len();
      } else {
        __LOG(warn, "get message_p fail");
        continue;
      }
      if (msg_p->is_valid()) {
        std::shared_ptr<char> sp(new char[msg_len],
                                 [](char *p) { delete[] p; });
        _ring_buffer.get(msg_len, sp.get());
        if (msg_callback_func) {
          msg_callback_func(sp, msg_len);
        } else {
          __LOG(warn, "no callback function is called!");
        }
      } else {
        string error_magic((msg_p->header).magic_num, 5);
        __LOG(debug, " no valid message in the ring buffer! magic number is "
                        << error_magic);
        // Humm we do not get the header, pehaps there is no message in the ring
        // buffer
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::this_thread::yield();

        continue;
      }
      std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
  }

  // note this function is mainly to start another thread to read out
  // message in the ring buffer and call the message_callback set.
  bool init() {
    if (!msg_callback_func) {
      __LOG(error, " message callback function is not set! Please check! ");
      return false;
    }
    auto tmp_fun = std::bind(&TcpServerAPP::ringBuffer_f, this);
    ringBuffer_thread = new (std::nothrow) thread(tmp_fun);
    return true;
  }

  bool destory() {
    ringBuffer_thread->join();
    delete ringBuffer_thread;
    return true;
  }

 public:
  ring_buffer _ring_buffer;
  MSG_FUN msg_callback_func;
  std::thread *ringBuffer_thread;
};

}  // end of namespace translib