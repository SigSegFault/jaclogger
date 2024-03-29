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
#include "logtostd.h"

#include <algorithm>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#ifdef THREAD_SAFE_LOGGER
#include "../port/mutex.h"
#endif

#define CSTRING_LEN(str)            (sizeof(str) -1)
#define DEFINE_CSTRING(name, value) static const char * name = value; static const uint32_t name##_len = CSTRING_LEN(value);
#define ALLOC_ERROR_STRING          "[Logger, internal]: could not allocate space for log message.\n"

namespace jacl
{

DEFINE_CSTRING(alloc_error_str, ALLOC_ERROR_STRING)

Logger::Logger()
{
#ifdef THREAD_SAFE_LOGGER
    mMutex = getPlatfromSpecificMutex();
#endif
}

Logger::Logger(ConstructOptions opts)
{
#ifdef THREAD_SAFE_LOGGER
    mMutex = (opts & ThreadUnsafe) ? 0 : getPlatfromSpecificMutex();
#endif
    if((opts & IncludeStd))
        mDispatchers["default"] = new LogToStd;
}

Logger::~Logger()
{
    DispatcherSet::iterator it = mDispatchers.begin();
    while(it != mDispatchers.end())
        delete (it++)->second;
    mDispatchers.clear();
#ifdef THREAD_SAFE_LOGGER
    delete mMutex;
#endif
}

Logger & Logger::info(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_INFO, fmt, vl);
    va_end(vl);
    return *this;
}

Logger & Logger::debug(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
    return *this;
}

Logger & Logger::debug(uint32_t level, const char *fmt, ...)
{
    {
#ifdef THREAD_SAFE_LOGGER
        ScopedGuard gandalf(this->mMutex);
#endif
        if(level > mDebugLevel) return *this;
    }
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
    return *this;
}

Logger & Logger::error(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(LogDispatcher::LOG_ERROR, fmt, vl);
    va_end(vl);
    return *this;
}

int Logger::debugLevel()
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    return mDebugLevel;
}

Logger & Logger::setDebuglevel(int lvl)
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    mDebugLevel = lvl;
    return *this;
}

const std::string Logger::prefix()
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    return mPrefix;
}

Logger & Logger::setPrefix(const std::string & prefix)
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    mPrefix = prefix;
    return *this;
}

Logger & Logger::setPrefix(const char * fmt, ...)
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
#ifdef THREAD_SAFE_LOGGER
            ScopedGuard gandalf(this->mMutex);
#endif
            mPrefix = dBuf;
        }
        // I believe it's better just to ignore malloc error rather than abort whole execution.
    }
    else
    {
#ifdef THREAD_SAFE_LOGGER
        ScopedGuard gandalf(this->mMutex);
#endif
        mPrefix = sBuf;
    }
    va_end(vl);
    return *this;
}

LogDispatcher * Logger::popDispatcher(const std::string &name)
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    DispatcherSet::iterator it = mDispatchers.find(name);
    if(it != mDispatchers.end())
    {
        LogDispatcher * out = it->second;
        mDispatchers.erase(it);
        return out;
    }
    return 0;
}

Logger & Logger::pushDispatcher(const std::string &name, LogDispatcher *dispatcher)
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    DispatcherSet::iterator it = mDispatchers.find(name);
    if(it != mDispatchers.end())
        mDispatchers.erase(it);
    if(dispatcher)
        mDispatchers[name] = dispatcher;
    return *this;
}

Logger &Logger::clear()
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
    DispatcherSet::iterator it = mDispatchers.begin();
    while(it != mDispatchers.end())
        delete (it++)->second;
    mDispatchers.clear();
    return * this;
}

Logger & Logger::Debug(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::Debug(uint32_t level, const char * fmt, ...)
{
    if(level > getDefault().mDebugLevel) return getDefault();
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_DEBUG, fmt, vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::Error(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_ERROR, fmt, vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::Debug(const std::string & fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_DEBUG, fmt.c_str(), vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::Debug(uint32_t level, const std::string & fmt, ...)
{
    if(level > getDefault().mDebugLevel) return getDefault();
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_DEBUG, fmt.c_str(), vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::Error(const std::string & fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    getDefault().mRelay(LogDispatcher::LOG_ERROR, fmt.c_str(), vl);
    va_end(vl);
    return getDefault();
}

Logger & Logger::getDefault()
{
    static Logger def(IncludeStd);
    return def;
}

void Logger::mRelay(LogDispatcher::LogType type, const char *fmt, va_list vl)
{
#ifdef THREAD_SAFE_LOGGER
    ScopedGuard gandalf(this->mMutex);
#endif
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

    for(DispatcherSet::const_iterator it = mDispatchers.begin();
        it != mDispatchers.end(); ++it)
        (*it).second->sink(type, dBuf, size);
    if(dBuf != sBuf) free(dBuf);
}

}
