/*
 * tcpExample.cpp
 *
 *  Created on: 2015年6月29日
 *      Author:
 */

#include "translib/loop.h"
#include "translib/tcpServerApp.h"
#include "translib/tcpClient.h"
#include <functional>
#include <iostream>
#include <random>
#include <thread>
#include "index.h"
#include "logger/logger.h"
#include "translib/timer.h"
#include <chrono>
#include <string>
using namespace std;
using namespace translib;

void tcp_client_t() {
  Loop _loop;
  TcpClient _client(_loop);
  if (!_client.connect("127.0.0.1", 4567)) {
    __LOG(error, "connect failed");
    return;
  }
  _loop.start();



  while (1) {
    if (!_client.isConnected()) {
      _client.connect("127.0.0.1", 4567);
    }

    std::random_device rd;
    int dice_roll = (rd() % (MAX_MSG_LEN - sizeof(tcp_message_header) - 3));

    char tmp_str[MAX_MSG_LEN] = "test";

    tcp_message tmp_msg;

    tmp_msg.form_msg(tmp_str, dice_roll); // sizeof(tmp_str));
    __LOG(debug, "try to send out message with size : " << dice_roll);
    __LOG(debug, "payload size is : " << tmp_msg.get_len());

    char raw_buff[MAX_MSG_LEN];
    if (tmp_msg.raw_msg(raw_buff, sizeof(raw_buff))) {
    } else {
      __LOG(error, "get raw message fail");
    }
    __LOG(debug, "actually send message length is : " << tmp_msg.get_len());
    _client.send(raw_buff, tmp_msg.get_len()); // tmp_msg.get_len());

  }
}

void tcpExample() {
  TcpServerAPP tcpServer;
  if (!tcpServer.listen("127.0.0.1", 4567)) {
    __LOG(error, "listen failed");
    return;
  }
  __LOG(info, "server listen success!");

  tcpServer.wait();
}

void tcpClientExample() {
  // std::this_thread::sleep_for(std::chrono::milliseconds(100));
  std::thread client_thread(tcp_client_t);
  std::thread client_thread1(tcp_client_t);
  std::thread client_thread2(tcp_client_t);
  std::thread client_thread3(tcp_client_t);
  std::thread client_thread4(tcp_client_t);
  std::thread client_thread5(tcp_client_t);
  std::thread client_thread6(tcp_client_t);
  client_thread.join();
  client_thread1.join();
  client_thread2.join();
  client_thread3.join();
  client_thread4.join();
  client_thread5.join();
  client_thread6.join();

  // tcp_client_t();
}
