#pragma once
/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001/CPP_test_env
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
#include "include/testUtil.hpp"
#include "translib/loop.h"
#include <atomic>
#include <sys/eventfd.h>
#include "translib/eventServer.h"
#include "translib/eventClient.h"
#include <future>
#include "logger/logger.h"
// Note!!!!!!!! : sure we can share the promise, but we choose to not share in case the cases run in different threads
/******************************************************************************************/
// basic test
std::promise<bool> case_result_promise;
void onEVFDRead(evutil_socket_t fd, short event, void *args)
{
  std::cout << "recv event: " << event << " from fd: " << fd << std::endl;
  uint64_t one;
  int ret = read(fd, &one, sizeof one);
  if (ret != sizeof one)
  {
    std::cout << "read evfd fail: " << ret << std::endl;
    return;
  }
  std::cout << "read from evfd: " << one << std::endl;
  case_result_promise.set_value(true);
}
// name: event_FD_basic_body
// info: this field should record the case info
case_result event_FD_basic_body(void *arg)
{
  int ev_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  translib::Loop loop;
  translib::EventFdServer EVFDServer(loop, ev_fd, onEVFDRead);
  translib::EventFdClient EVFDClient(ev_fd);
  translib::TimerManager::instance()->getTimer()->startOnce(1, [&] { EVFDClient.send(); });
  loop.start(true);
  std::future<bool> fut = case_result_promise.get_future();
  fut.wait();
  case_result ret = fut.get() ? CASE_SUCCESS : CASE_FAIL;

  if (ev_fd >= 0)
  {
    close(ev_fd);
    ev_fd = -1;
  }

  return ret;
}
/******************************************************************************************/
// test if we got the number of test count
std::promise<bool> case_result_promise_001;
std::atomic<int> fd_count(0);
#define FD_TIMES 100
#define FD_INTERVAL 1
void onEVFDRead_001(evutil_socket_t fd, short event, void *args)
{
  //  std::cout << "recv event: " << event << " from fd: " << fd << std::endl;
  uint64_t one;
  int ret = read(fd, &one, sizeof one);
  if (ret != sizeof one)
  {
    std::cout << "read evfd fail: " << ret << std::endl;
    return;
  }
  //  std::cout << "read from evfd: " << one << std::endl;
  fd_count += one;
  if (fd_count == FD_TIMES)
  {
    __LOG(warn, "case onEVFDRead_001 success!!!!");
    case_result_promise_001.set_value(true);
    fd_count = 0;
  }
}
case_result event_FD_count_test_body(void *arg)
{
  __LOG(warn, "NOTE: we will send " << FD_TIMES << " fd message at speed " << (1000 / FD_INTERVAL) << " msg/sec. This case will run " << ((FD_TIMES * FD_INTERVAL) / 1000) << " seconds");
  int ev_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  translib::Loop loop;
  translib::EventFdServer EVFDServer(loop, ev_fd, onEVFDRead_001);
  translib::EventFdClient EVFDClient(ev_fd);
  translib::TimerManager::instance()->getTimer()->startRounds(FD_INTERVAL, FD_TIMES, [&] { EVFDClient.send(); });
  loop.start(true);
  std::future<bool> fut = case_result_promise_001.get_future();
  std::chrono::milliseconds span(100); //((FD_TIMES * FD_INTERVAL) / 1000) * 2);
  __LOG(warn, "NOTE: we will send " << FD_TIMES << " fd message at speed " << (1000 / FD_INTERVAL) << " msg/sec. This case will run " << ((FD_TIMES * FD_INTERVAL) / 1000) << " seconds");
  __LOG(warn, " case is running");
  while (fut.wait_for(span) == std::future_status::timeout)
  {
    std::cout << '.' << std::flush;
    ;
  }

  if (ev_fd >= 0)
  {
    close(ev_fd);
    ev_fd = -1;
  }

  if (fut.valid())
  {
    case_result ret = fut.get() ? CASE_SUCCESS : CASE_FAIL;
    return ret;
  }
  else
  {
    return CASE_FAIL;
  }
}
/******************************************************************************************/
// test if the server side does not got the chance to process the message
std::promise<bool> case_result_promise_002;
std::atomic<int> fd_count_002(0);

void onEVFDRead_002(evutil_socket_t fd, short event, void *args)
{
  // used to check if the received counter is more than 1
  static bool flag = false;
  std::this_thread::sleep_for(std::chrono::seconds(1));
  //  std::cout << "recv event: " << event << " from fd: " << fd << std::endl;
  uint64_t one;
  int ret = read(fd, &one, sizeof one);
  if (ret != sizeof one)
  {
    std::cout << "read evfd fail: " << ret << std::endl;
    return;
  }
  if (one > 1)
  {
    flag = true;
  }
  std::cout << "read from evfd: " << one << std::endl;
  fd_count += one;
  if (fd_count == FD_TIMES && flag)
  {
    __LOG(warn, "case onEVFDRead_001 success!!!!");
    case_result_promise_002.set_value(true);
    fd_count = 0;
  }
}
case_result event_FD_queue_counter_test_body(void *arg)
{
  __LOG(warn, "NOTE: we will send " << FD_TIMES << " fd message at speed " << (1000 / FD_INTERVAL) << " msg/sec. This case will run " << ((FD_TIMES * FD_INTERVAL) / 1000) << " seconds");
  int ev_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  translib::Loop loop;
  translib::EventFdServer EVFDServer(loop, ev_fd, onEVFDRead_002);
  translib::EventFdClient EVFDClient(ev_fd);
  loop.start(true);
  translib::TimerManager::instance()->getTimer()->startRounds(FD_INTERVAL, FD_TIMES, [&] { 
    //__LOG(warn, "before send event fd");
    //__LOG(warn, " send return : "<<(EVFDClient.send()));
    EVFDClient.send(); 
    });

  std::future<bool> fut = case_result_promise_002.get_future();
  std::chrono::milliseconds span(100); //((FD_TIMES * FD_INTERVAL) / 1000) * 2);
  __LOG(warn, "NOTE: we will send " << FD_TIMES << " fd message at speed " << (1000 / FD_INTERVAL) << " msg/sec. This case will run " << ((FD_TIMES * FD_INTERVAL) / 1000) << " seconds");
  __LOG(warn, " case is running");
  while (fut.wait_for(span) == std::future_status::timeout)
  {
    std::cout << '.' << std::flush;
    ;
  }

  if (ev_fd >= 0)
  {
    close(ev_fd);
    ev_fd = -1;
  }

  if (fut.valid())
  {
    case_result ret = fut.get() ? CASE_SUCCESS : CASE_FAIL;
    return ret;
  }
  else
  {
    return CASE_FAIL;
  }
}