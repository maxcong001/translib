/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author: 
 */

#include <iostream>
#include <functional>
#include <thread>
#include <unistd.h>
using namespace std;

#include "translib/loop.h"
#include "translib/tcpServer.h"
#include "translib/tcpClient.h"
#include "translib/tcpServerTPKT.h"
#include "translib/tcpClientTPKT.h"
#include "translib/timer.h"
#include "index.h"
#include "logger/logger.h"
#include "translib/timerManager.h"

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

        uint8_t buff[10240];

        session->readInputBuffer(buff, length);
        //        std::string str((char *)buff, length);

        __LOG(debug, "receive message with length : " << length); // << " data is " << str);

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
            //__LOG(debug, "not connected, round " << _timer.curRound());
            _client.connect("127.0.0.1", 4567);
            sleep(2);
        }
        if (!_client.isConnected())
        {
            __LOG(debug, "not connected, round " << _timer.curRound());
        }

        //std::string test("test");
        char test[1220] = {1};
        _client.send(test, 1220);
        //_client.send(test.c_str(), test.size()); //tmp_msg.get_len());

        if (_timer.curRound() >= 3000)
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

void TKPT_example()
{
    translib::TcpServerTPKT server;
    if (!server.listen("127.0.0.1", 2565))
    {
        __LOG(error, " server listen return fail!");
    }
    translib::Loop loop;
    translib::TcpClientWithTPKT client(loop);
    if (!client.connect("127.0.0.1", 2565))
    {
        __LOG(error, "connect to  server return fail!");
    }
    loop.start(true);
    // start a timer and send message to server
    auto timer = translib::TimerManager::instance()->getTimer();
    timer->startForever(1, [&]() {
        //std::string test("test!!!!");
        const char test[1000] = {1};

        //client.sendWithTPKT(test.c_str(), test.size());
        client.sendWithTPKT(test, sizeof(test));
    });
    getchar();

}

void tcpExample()
{
#if 0
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
#endif
    TKPT_example();
}
