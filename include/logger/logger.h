#pragma once
/*
 * Copyright (c) 2016-20017 Max Cong <savagecm@qq.com>
 * this code can be found at https://github.com/maxcong001/logger
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

#include <memory>
#include <mutex>
#include <string>   // std::string
#include <iostream> // std::cout
#include <sstream>  // std::ostringstream

typedef std::basic_ostream<char> tostream;
typedef std::basic_istream<char> tistream;
typedef std::basic_ostringstream<char> tostringstream;
typedef std::basic_istringstream<char> tistringstream;

class logger_iface
{
  public:
    //! log level
    enum class log_level
    {
        error = 0,
        warn = 1,
        info = 2,
        debug = 3
    };

  public:
    logger_iface(void) = default;
    virtual ~logger_iface(void) = default;

    logger_iface(const logger_iface &) = default;
    logger_iface &operator=(const logger_iface &) = default;

  public:
    virtual void set_log_level(logger_iface::log_level level) = 0;
    virtual void debug(const std::string &msg, const std::string &file, std::size_t line) = 0;
    virtual void info(const std::string &msg, const std::string &file, std::size_t line) = 0;
    virtual void warn(const std::string &msg, const std::string &file, std::size_t line) = 0;
    virtual void error(const std::string &msg, const std::string &file, std::size_t line) = 0;
};

class logger : public logger_iface
{
  public:
    logger(logger_iface::log_level level = logger_iface::log_level::info);
    ~logger(void) = default;

    logger(const logger &) = default;
    logger &operator=(const logger &) = default;

  public:
    void set_log_level(logger_iface::log_level level);
    void debug(const std::string &msg, const std::string &file, std::size_t line);
    void info(const std::string &msg, const std::string &file, std::size_t line);
    void warn(const std::string &msg, const std::string &file, std::size_t line);
    void error(const std::string &msg, const std::string &file, std::size_t line);

  private:
    logger_iface::log_level m_level;
    std::mutex m_mutex;
};

void debug(const std::string &msg, const std::string &file, std::size_t line);
void info(const std::string &msg, const std::string &file, std::size_t line);
void warn(const std::string &msg, const std::string &file, std::size_t line);
void error(const std::string &msg, const std::string &file, std::size_t line);
void set_log_level(logger_iface::log_level level);

#define __LOGGING_ENABLED

#ifdef __LOGGING_ENABLED
#define __LOG(level, msg)                              \
    \
{                                                 \
        tostringstream var;                            \
        var << "[fuction:" << __func__ << "] " << msg; \
        level(var.str(), __FILE__, __LINE__);          \
    \
}
#else
#define __LOG(level, msg)
#endif /* __LOGGING_ENABLED */