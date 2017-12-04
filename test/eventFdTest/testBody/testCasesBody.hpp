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
std::atomic<bool> case_success(false);
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
  //case_success = false;
  case_result_promise.set_value(true);

}

// name: dbw_001
// info: this field should record the case info
case_result event_FD_basic_body(void *arg)
{
  int ev_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
  translib::Loop loop;
  translib::EventFdServer EVFDServer(loop, ev_fd, onEVFDRead);
  translib::EventFdClient EVFDClient(ev_fd);

  translib::TimerManager::instance()->getTimer()->startOnce(1, [&] { EVFDClient.send(); });

  //sleep(1);
  loop.start(true);
  std::future<bool> fut = case_result_promise.get_future();
  //std::this_thread::sleep_for(std::chrono::milliseconds(5));
  return fut.get() ? CASE_SUCCESS : CASE_FAIL;
}
