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
#include "translib/eventClient.h"
#include "translib/eventServer.h"
#include "translib/loop.h"
#include "logger/logger.h"
#include "translib/timerManager.h"
#include "index.h"
#include <unistd.h>
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
}

void eventFdExample()
{
    // create evnet fd
    int ev_fd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    // start server
    translib::Loop loop;
    //event_base *loop = event_base_new();

    translib::EventFdServer EVFDServer(loop, ev_fd, onEVFDRead);
    //event *ev_fd_event = event_new(loop, ev_fd, EV_READ | EV_PERSIST, &onEVFDRead, NULL);
    //event_add(ev_fd_event, NULL);
    //auto thread1 = std::thread([&]() -> void { event_base_dispatch(loop); });
    //thread1.detach();
#if 0
    {
        uint64_t one = 1;
        int ret = write(ev_fd, &one, sizeof one);
        if (ret != sizeof one)
        {
            std::cout << "write evfd fail: " << ret << std::endl;
        }
    }
#endif
    translib::EventFdClient EVFDClient(ev_fd);

    //EVFDClient.send();

    translib::TimerManager::instance()->getTimer()->startOnce(500, [&] { EVFDClient.send(); });
    //sleep(1);
    loop.start(false);
}
