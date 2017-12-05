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
 * @brief EventFdServer base 
 */
class EventFdServer
{
  public:
    typedef void (*evCb)(evutil_socket_t fd, short event, void *args);
    EventFdServer() = delete;
    // Note:!! please make sure your fd is non-blocking
    // for example: int ev_fd = eventfd(0, EFD_NONBLOCK|EFD_CLOEXEC);
    // Note:!! please make sure that you pass in a right envent base loop
    // event base is not changeable, if you want to change it, please kill this object and start a new one
    // will add some some check later.....
    EventFdServer(event_base *evBase_in, int efd, evCb cb) : eventFd(efd), one(1), evBase(evBase_in), eventCallback(cb)
    {
        _event = event_new(evBase, eventFd, EV_READ | EV_PERSIST, eventCallback, NULL);
        if (NULL == _event)
        {
            return;
        }
        if (0 != event_add(_event, NULL))
        {
            return;
        }
    }
    virtual ~EventFdServer()
    {
        if (NULL != _event)
        {
            event_free(_event);
            _event = NULL;
        }
    }

    bool init()
    {
        return true;
    }

    /** convert to event_base * pointer*/
    inline operator event_base *() const
    {
        return evBase;
    };

  private:
    int eventFd;
    uint64_t one;
    event_base *evBase;
    evCb eventCallback;
    struct event *_event;
};

} /* namespace translib */
