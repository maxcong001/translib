/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <random>
using namespace std;

#include "translib/loop.h"
#include "translib/tcpServer.h"
#include "translib/tcpClient.h"
#include "translib/timer.h"
//#include "ringbuffer/ringbuffer.hpp"
#include "ringbuffer/buffer_message.hpp"
#include "index.h"

class ExampleTcpClient : public translib::TcpClient
{
  public:
    ExampleTcpClient(const translib::Loop &loop) : translib::TcpClient(loop)
    {
    }

  protected:
    virtual void onRead()
    {
    }

    virtual void onDisconnected()
    {
        cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
    }

    virtual void onConnected(int error)
    {
        cout << "ExampleTcpClient::" << __FUNCTION__ << endl;
    }
};

class ExampleTcpServer : public translib::TcpServer
{
  public:
    static ExampleTcpServer &instance()
    {
        static ExampleTcpServer ins;
        return ins;
    }

  protected:
    virtual void onListenError()
    {
        cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
    }

    virtual void onSessionRead(translib::TcpSession *session)
    {
        char *buff = _ring_buffer.peek_head_p();
        if (!buff)
        {
            cout << "get invalid buffer ptr " << (void *)buff << endl;
            return;
        }
        uint32_t length = session->getInputBufferLength();
        session->readInputBuffer((uint8_t *)buff, length);

        cout << "receive message with length : " << length << endl;

        while (!_ring_buffer.add(length, buff))
        {
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        cout << "ring buffer size " << _ring_buffer.size() << endl;

        char message_buf[MAX_MSG_LEN];

        tcp_message *tmp_msg = (tcp_message *)message_buf;
        //tcp_message tmp_tcp_message;
        while (tmp_msg->get_message(_ring_buffer, (char *)tmp_msg))
        {
            cout << "get_message() return success" << endl;
            std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

        cout << "ExampleTcpServer::" << __FUNCTION__
             << " " << session->id() << " data:" << (void *)buff << endl;
    }

    virtual void onSessionDisconnected(translib::TcpSession *session)
    {
        cout << "ExampleTcpServer::" << __FUNCTION__ << endl;
    }

    virtual void onNewSession(translib::TcpSession *session)
    {
        cout << "ExampleTcpServer::" << __FUNCTION__ << session->id() << endl;
    }

  public:
    ring_buffer _ring_buffer;
};

class ExampleTcpClientManager : public translib::Loop
{
  public:
    ExampleTcpClientManager() : _client(*this),
                                _timer(*this)
    {
        _timer.startForever(1, std::bind(&ExampleTcpClientManager::tick, this));
    }

  protected:
    void tick()
    {
        cout << "round " << _timer.curRound() << endl;
        if (!_client.isConnected())
        {
            _client.connect("127.0.0.1", 4567);
        }

        std::random_device rd;
        int dice_roll = (rd() % (MAX_MSG_LEN - sizeof(tcp_message_header) -3));

        char tmp_str[MAX_MSG_LEN] = "test";

        tcp_message tmp_msg;

        tmp_msg.form_msg(tmp_str, dice_roll); //sizeof(tmp_str));
        cout << "try to send out message with size : " << dice_roll << endl;
        cout << "payload size is : " << tmp_msg.get_len() << endl;

        char raw_buff[MAX_MSG_LEN];
        if (tmp_msg.raw_msg(raw_buff, sizeof(raw_buff)))
        {
        }
        else
        {
            cout << "get raw message fail" << endl;
        }
        cout << "actually send message length is : " << tmp_msg.get_len() << endl;
        _client.send(raw_buff, tmp_msg.get_len()); //tmp_msg.get_len());

        if (_timer.curRound() >= 300000)
        {
            ExampleTcpServer::instance().stop();
            stop();
        }
    }

  private:
    ExampleTcpClient _client;
    translib::Timer _timer;
};

void tcpExample()
{
    ExampleTcpServer &tcpServer = ExampleTcpServer::instance();
    if (!tcpServer.listen("127.0.0.1", 4567))
    {
        cout << "listen failed" << endl;
        return;
    }

    ExampleTcpClientManager clientManager;
    clientManager.start(false);
    clientManager.wait();
    tcpServer.wait();
}
