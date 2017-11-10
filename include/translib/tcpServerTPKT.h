#pragma once
#include "tcpServer.h"
#include "logger/logger.h"

namespace translib
{
class TcpServerTPKT : public translib::TcpServer
{
public:
  virtual void onListenError() { __LOG(debug, ""); }

  virtual void onSessionRead(translib::TcpSession *session)
  {
    do
    {
      uint32_t length = session->getInputBufferLength();
      const uint8_t *buf = session->viewInputBuffer(TPKT1_LENGTH);
      if (!buf)
      {
        // note: this could happen when there is no data int he bufferevent!
        __LOG(debug, "[TcpServerTPKT] there is no data in the buffer)");
        return;
      }

      if (buf[0] != TPKT_VERSION1_LOW_BYTE)
      {
        __LOG(error, "[TcpServerTPKT] error!!! TPKT magic number is lost!!");
        // read one byte and try again
        uint8_t tmp_buf;
        session->readInputBuffer(&tmp_buf, 1);
      }

      // 3-byte packet length
      unsigned int packet_length = ((*(buf + 1) & 0xff) << 16) |
                                   ((*(buf + 2) & 0xff) << 8) |
                                   (*(buf + 3) & 0xff);
      //std::shared_ptr<T> ptr(static_cast<T*>(malloc(sizeof(T))), free);

      //length = packet_length - TPKT1_LENGTH;
      if (packet_length > length)
      {
        // some message is still in the air
        break;
      }
      else
      {
        uint8_t *buffer_tmp = static_cast<uint8_t *>(malloc(packet_length * sizeof(uint8_t)));
        // std::shared_ptr<uint8_t> buffer(static_cast<uint8_t*>(malloc(sizeof(packet_length))), free);
        //uint8_t *buffer_tmp = buffer.get();
        if (session->readInputBuffer(buffer_tmp, packet_length))
        {
          __LOG(debug, "[TcpServerTPKT] now receive " << packet_length << " message");
          onRead(session, (((uint8_t *)buffer_tmp) + TPKT1_LENGTH), (packet_length - TPKT1_LENGTH));
        }
        else
        {
          __LOG(warn, "read from input buffer fail!");
        }
        free((void *)buffer_tmp);
      }

    } while (1);
  }
  void onRead(translib::TcpSession *session, const uint8_t *msg, uint32_t len)
  {
    // please do not do too much work here!
    __LOG(debug, "[TcpServerTPKT] receive " << len << " byte message");
  }
  virtual void onSessionDisconnected(translib::TcpSession *session)
  {
    __LOG(debug, " ");
  }

  virtual void onNewSession(translib::TcpSession *session)
  {
    __LOG(debug, " ");
  }

  bool sendWithTPKT(translib::TcpSession *session, const char *data, uint32_t length)
  {
    // note: here we can use usr memory and let user reserve 4 byte in the header, but this is a burden to user
    // here we choose to send the header first and then send the message.
    char tpkt_header[TPKT1_LENGTH];

    tpkt_header[0] = TPKT_VERSION1_LOW_BYTE;
    tpkt_header[1] = ((length + TPKT1_LENGTH) & 0xFFFFFF) >> 16U;
    tpkt_header[2] = ((length + TPKT1_LENGTH) & 0xFFFF) >> 8U;
    tpkt_header[3] = ((length + TPKT1_LENGTH) & 0xFF);

    bool ret = session->send(tpkt_header, sizeof(tpkt_header));
    if (!ret)
    {
      __LOG(error, "[TcpServerTPKT] send TPKT header fail!");
      return false;
    }
    return session->send(data, length);
  }
};

} // end of namespace translib