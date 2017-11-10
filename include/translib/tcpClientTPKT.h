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
#include "logger/logger.h"
#include <malloc.h>
#include <stdlib.h>

namespace translib
{
class TcpClientTPKT : public translib::TcpClient
{
  public:
    TcpClientTPKT(const translib::Loop &loop) : translib::TcpClient(loop)
    {
    }
    bool connect_with_IP(std::string IP, std::string Port)
    {
        __LOG(debug, "[TCPProxyConnector] connect to " << IP << ":" << Port);
        return connect(IP.c_str(), uint16_t(std::stoi(Port)));
    }
    bool sendWithTPKT(const char *data, uint32_t length)
    {
        // note: here we can use usr memory and let user reserve 4 byte in the header, but this is a burden to user
        // here we choose to send the header first and then send the message.
        char tpkt_header[TPKT1_LENGTH];

        tpkt_header[0] = TPKT_VERSION1_LOW_BYTE;
        tpkt_header[1] = ((length + TPKT1_LENGTH) & 0xFFFFFF) >> 16U;
        tpkt_header[2] = ((length + TPKT1_LENGTH) & 0xFFFF) >> 8U;
        tpkt_header[3] = ((length + TPKT1_LENGTH) & 0xFF);

        bool ret = send(tpkt_header, sizeof(tpkt_header));
        if (!ret)
        {
            __LOG(error, "[TcpClientTPKT] send TPKT header fail!");
            return false;
        }
        return send(data, length);
    }

  protected:
    virtual void onRead()
    {
        do
        {
            uint32_t length = this->getInputBufferLength();
            if (length < 1)
            {
                __LOG(debug, "[TcpClientTPKT] read " << length << "message, it is less than 1 byte!");
                break;
            }
            const uint8_t *buf = this->viewInputBuffer(TPKT1_LENGTH);
            if (buf[0] != TPKT_VERSION1_LOW_BYTE)
            {
                __LOG(error, "[TcpClientTPKT] error!!! TPKT magic number is lost!!");
                // read one byte and try again
                uint8_t tmp_buf;
                this->readInputBuffer(&tmp_buf, 1);
            }

            // 3-byte packet length
            unsigned int packet_length = ((*(buf + 1) & 0xff) << 16) |
                                         ((*(buf + 2) & 0xff) << 8) |
                                         (*(buf + 3) & 0xff);
            //length = packet_length - TPKT1_LENGTH;
            if (packet_length > length)
            {
                // some message is still in the air
                break;
            }
            else
            {
                std::shared_ptr<uint8_t> buffer(static_cast<uint8_t *>(malloc(sizeof(packet_length))), free);
                uint8_t *buffer_tmp = buffer.get();
                this->readInputBuffer(buffer_tmp, packet_length);
                onRead((((uint8_t *)buffer_tmp) + TPKT1_LENGTH), (packet_length - TPKT1_LENGTH));
            }
            //  onRead((((uint8_t *)buff) + TPKT1_LENGTH), (packet_length - TPKT1_LENGTH));
        } while (1);
    }

    void onRead(uint8_t *msg, uint32_t len)
    {
        // please do not do too much work here!
        __LOG(debug, "[TcpClientTPKT] receive " << len << " byte message");
    }

    virtual void onDisconnected()
    {
        __LOG(error, "[TcpClientTPKT] disconnected");
    }

    virtual void onConnected(int errorr)
    {
        __LOG(debug, "[TcpClientTPKT] connected successfully");
    }

  private:
};

} /* namespace translib */