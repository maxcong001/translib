/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
#include <thread>
using namespace std;

#include "translib/loop.h"
#include "translib/tcpServer.h"
#include "translib/tcpClient.h"
#include "translib/timer.h"
#include "index.h"
#include "logger/logger.h"

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
        __LOG(debug, " run here");
    }

    virtual void onConnected(int error)
    {
        __LOG(debug, " run here");
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
        __LOG(debug, " run here");
    }

    virtual void onSessionRead(translib::TcpSession *session)
    {

        uint32_t length = session->getInputBufferLength();
        std::string buff;
        buff.reserve(length);

        session->readInputBuffer((uint8_t *)(buff.c_str()), length);

        __LOG(debug, "receive message with length : " << length << " buffer is " << buff);

        __LOG(debug, "sesson id is : " << session->id());
    }

    virtual void onSessionDisconnected(translib::TcpSession *session)
    {
        __LOG(debug, " run here");
    }

    virtual void onNewSession(translib::TcpSession *session)
    {
        __LOG(debug, " run here");
    }
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
        __LOG(debug, "round " << _timer.curRound());
        if (!_client.isConnected())
        {
            _client.connect("127.0.0.1", 4567);
        }

        std::string test("test");

        _client.send(test.c_str(), test.size()); //tmp_msg.get_len());

        if (_timer.curRound() >= 30)
        {
            _client.close(false);
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
        __LOG(error, "listen failed");
        return;
    }

    ExampleTcpClientManager clientManager;
    clientManager.start(false);
    clientManager.wait();
    tcpServer.wait();
}
