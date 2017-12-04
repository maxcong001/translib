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

#include "logger/logger.h"
static const char black[] = {0x1b, '[', '1', ';', '3', '0', 'm', 0};
static const char red[] = {0x1b, '[', '1', ';', '3', '1', 'm', 0};
static const char yellow[] = {0x1b, '[', '1', ';', '3', '3', 'm', 0};
static const char blue[] = {0x1b, '[', '1', ';', '3', '4', 'm', 0};
static const char normal[] = {0x1b, '[', '0', ';', '3', '9', 'm', 0};

std::unique_ptr<logger_iface> active_logger(new logger(logger_iface::log_level::error)); //nullptr;

logger::logger(logger_iface::log_level level)
    : m_level(level) {}
void logger::set_log_level(logger_iface::log_level level)
{
    m_level = level;
}
void logger::debug(const std::string &msg, const std::string &file, std::size_t line)
{
    if (m_level >= logger_iface::log_level::debug)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "[" << black << "DEBUG" << normal << "] [" << file << ":" << line << "] " << msg << std::endl;
    }
}

void logger::info(const std::string &msg, const std::string &file, std::size_t line)
{
    if (m_level >= logger_iface::log_level::info)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "[" << blue << "INFO " << normal << "] [" << file << ":" << line << "] " << msg << std::endl;
    }
}

void logger::warn(const std::string &msg, const std::string &file, std::size_t line)
{
    if (m_level >= logger_iface::log_level::warn)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cout << "[" << yellow << "WARN " << normal << "] [" << file << ":" << line << "] " << msg << std::endl;
    }
}

void logger::error(const std::string &msg, const std::string &file, std::size_t line)
{
    if (m_level >= logger_iface::log_level::error)
    {
        std::lock_guard<std::mutex> lock(m_mutex);
        std::cerr << "[" << red << "ERROR" << normal << "] [" << file << ":" << line << "] " << msg << std::endl;
    }
}

void debug(const std::string &msg, const std::string &file, std::size_t line)
{
    if (active_logger)
        active_logger->debug(msg, file, line);
}

void info(const std::string &msg, const std::string &file, std::size_t line)
{
    if (active_logger)
        active_logger->info(msg, file, line);
}

void warn(const std::string &msg, const std::string &file, std::size_t line)
{
    if (active_logger)
        active_logger->warn(msg, file, line);
}

void error(const std::string &msg, const std::string &file, std::size_t line)
{
    if (active_logger)
        active_logger->error(msg, file, line);
}

void set_log_level(logger_iface::log_level level)
{
    if (active_logger)
        active_logger->set_log_level(level);
}