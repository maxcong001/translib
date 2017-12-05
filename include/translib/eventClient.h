#pragma once
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
#include "logger/logger.h"
#include "translib/define.h"
#include <unistd.h>
#include <sys/eventfd.h>
namespace translib
{

/**
 * @brief EventFdClient base 
 */
class EventFdClient
{
public:
  EventFdClient() = delete;
  // Note:!! please make sure your fd is non-blocking
  // for example: int ev_fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
  EventFdClient(int efd) : one(1)
  {
    eventFd = efd;
    __LOG(debug, "event fd is " << eventFd);
  }
  virtual ~EventFdClient() {}

  int send()
  {
    int ret = write(eventFd, &one, sizeof(one));
    if (ret != sizeof(one))
    {
      __LOG(error, "write event fd : " << eventFd << " fail");
    }
    return ret;
  }

private:
  int eventFd;
  uint64_t one;
};

} /* namespace translib */