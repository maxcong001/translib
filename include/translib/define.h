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

#include <functional>
#include <algorithm>
#include <memory>
#include <stdint.h>
#include <string.h>

#include <event2/event.h>
#include <event2/thread.h>

#include <thread>
#include <mutex>
#include <atomic>

#include <exception>  

#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
#define PLATFORM_WINDOWS
#endif
#define TPKT1_LENGTH (4)
#define TPKT1_MAX_MSG_LENGTH (0xffffff)
#define TPKT_VERSION1_LOW_BYTE 0x1
// this is throw number
#define CREATE_EVENT_FAIL "CREATE_EVENT_FAIL"
#define EVENTFD_EVENT_NEW_ERROR "EVENTFD_EVENT_NEW_ERROR"
#define EVENTFD_EVENT_ADD_ERROR "EVENTFD_EVENT_ADD_ERROR"
#define TIMERMANAGER_EVENT_NEW_ERROR "TIMERMANAGER_EVENT_NEW_ERROR"
#define TIMERMANAGER_ADD_ERROR "TIMERMANAGER_ADD_ERROR"
namespace translib
{

typedef uint64_t SessionId;
const SessionId SESSION_ID_INVALID = 0;

typedef evutil_socket_t SocketFd;
const SocketFd SOCKET_FD_INVALID = -1;

typedef std::function<void()> Handler;

typedef std::recursive_mutex Mutex;
typedef std::lock_guard<std::recursive_mutex> MutexLock;

} /* namespace translib */
