/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2012 Paul Letnyanko                                       *
 *                                                                         *
 * Permission is hereby granted, free of charge, to any person obtaining a *
 * copy of this software and associated documentation files (the           *
 * "Software"), to deal in the Software without restriction, including     *
 * without limitation the rights to use, copy, modify, merge, publish,     *
 * distribute, sublicense, and/or sell copies of the Software, and to      *
 * permit persons to whom the Software is furnished to do so, subject to   *
 * the following conditions:                                               *
 *                                                                         *
 * The above copyright notice and this permission notice shall be included *
 * in all copies or substantial portions of the Software.                  *
 *                                                                         *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS *
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF              *
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY    *
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR   *
 * CLAIM, OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE        *
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                  *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "logger.h"
#include "../port/mutex.h"
#include "logdispatcher.h"

#include <algorithm>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CSTRING_LEN(str)            (sizeof(str) -1)
#define DEFINE_CSTRING(name, value) static const char * name = value; static const uint32_t name##_len = CSTRING_LEN(value);
#define ALLOC_ERROR_STRING          "Logger: could not allocate space for log message.\n"
#ifndef LOGGER_DEBUG_PREFIX
#define LOGGER_DEBUG_PREFIX         "[debug] "
#endif
#ifndef LOGGER_ERROR_PREFIX
#define LOGGER_ERROR_PREFIX         "[error] "
#endif

namespace jacl
{

DEFINE_CSTRING(alloc_error_str, ALLOC_ERROR_STRING)

// Todo: move this thingy somewhere else
template <typename Poor>
void grimDestructor(Poor * thingy) { delete thingy;}

Logger::Logger()
{
    mMutex = getPlatfromSpecificMutex();
}

Logger::Logger(int)
{
    mMutex = getPlatfromSpecificMutex();
    mDestinations.push_back(new LogDispatcher);
}

Logger::~Logger()
{
    std::for_each(mDestinations.begin(), mDestinations.end(), grimDestructor<LogDispatcher>);
    mDestinations.clear();
    delete mMutex;
}

void Logger::info(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_INFO, fmt, vl);
    va_end(vl);
}

void Logger::debug(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
}

void Logger::debug(uint32_t level, const char *fmt, ...)
{
    if(level > mDebugLevel) return;
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
}

void Logger::error(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_ERROR, fmt, vl);
    va_end(vl);
}

void Logger::setPrefix(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    char sBuf[4096];
    uint32_t size;
    size = vsnprintf(sBuf, sizeof(sBuf), fmt, vl);
    if(size >= sizeof(sBuf))
    {
        size = vsnprintf(0, 0, fmt, vl);
        char * dBuf = (char*)malloc(size + 2);
        if(dBuf)
        {
            vsnprintf(dBuf, size + 2, fmt, vl);
            dBuf[size++] = '\n';
            mPrefix = dBuf;
        }
        // I believe it's better just to ignore malloc error rather than abort whole execution.
    }
    else
    {
        sBuf[size++] = '\n';
        mPrefix = sBuf;
    }
    va_end(vl);
}

void Logger::Debug(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
}

void Logger::Debug(uint32_t level, const char * fmt, ...)
{
    if(level > Default.mDebugLevel) return;
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
}

void Logger::Error(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_ERROR, fmt, vl);
    va_end(vl);
}

void Logger::Debug(const std::string & fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_DEBUG, fmt.c_str(), vl);
    va_end(vl);
}

void Logger::Debug(uint32_t level, const std::string & fmt, ...)
{
    if(level > Default.mDebugLevel) return;
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_DEBUG, fmt.c_str(), vl);
    va_end(vl);
}

void Logger::Error(const std::string & fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(LogDispatcher::LOG_ERROR, fmt.c_str(), vl);
    va_end(vl);
}

Logger Logger::Default(0);

void Logger::mRelay(LogDispatcher::LogType type, const char *fmt, va_list vl)
{
    char sBuf[4096];
    char * dBuf = sBuf;
    uint32_t prefixLen = mPrefix.size();
    uint32_t size = prefixLen;
    int bufSpace = sizeof(sBuf) - prefixLen;
    bufSpace = bufSpace < 0 ? 0 : bufSpace;
    size += vsnprintf(sBuf + prefixLen, bufSpace, fmt, vl);
    if(size >= sizeof(sBuf))
    {
        size = prefixLen + vsnprintf(0, 0, fmt, vl);
        dBuf = (char*)malloc(size + 2);
        if(dBuf)
        {
            vsnprintf(dBuf, size + 2, fmt, vl);
            memcpy(dBuf, mPrefix.c_str(), prefixLen);
            dBuf[size++] = '\n';
        }
        else
        {
            size = alloc_error_str_len;
            memcpy(sBuf, alloc_error_str, size);
            dBuf = sBuf;
        }
    }
    else
    {
        memcpy(sBuf, mPrefix.c_str(), prefixLen);
        sBuf[size++] = '\n';
    }
    {
        ScopedGuard(this->mMutex);
        for(DispatcherList::const_iterator it = mDestinations.begin();
            it != mDestinations.end(); ++it)
            (*it)->sink(type, dBuf, size);
    }
    if(dBuf != sBuf) free(dBuf);
}

}
