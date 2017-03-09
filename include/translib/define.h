/*
 * define.h
 *
 *  Created on: 2015年5月30日
 *      Author: 
 */

#ifndef INCLUDE_EC_DEFINE_H_
#define INCLUDE_EC_DEFINE_H_

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

#if (defined _WIN32) || (defined WIN32) || (defined _WIN64) || (defined WIN64)
#define PLATFORM_WINDOWS
#endif

namespace translib
{

typedef uint64_t SessionId;
const SessionId SESSION_ID_INVALID = 0;

typedef evutil_socket_t SocketFd;
const SocketFd SOCKET_FD_INVALID = -1;

typedef std::function<void ()> Handler;

typedef std::recursive_mutex Mutex;
typedef std::lock_guard<std::recursive_mutex> MutexLock;

} /* namespace translib */

#endif /* INCLUDE_EC_DEFINE_H_ */
