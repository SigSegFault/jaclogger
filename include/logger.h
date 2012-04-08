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

#ifndef LOGGER_H
#define LOGGER_H

#include <list>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "logdispatcher.h"

namespace jacl
{

class Mutex;

class Logger
{
public:
    Logger();
    /// Autoinclude stdout/stderr to destinations
    Logger(int);
    ~Logger();

    void info(const char * fmt, ...);
    /// Assumed to be of current log level
    void debug(const char * fmt, ...);
    void debug(uint32_t level, const char * fmt, ...);
    void error(const char * fmt, ...);

    inline int debugLevel() {return mDebugLevel;}
    inline void setDebuglevel(int lvl) {mDebugLevel = lvl;}

    const std::string & prefix() {return mPrefix;}
    void setPrefix(const char * prefix) {mPrefix = prefix;}
    void setPrefix(const std::string & prefix) {mPrefix = prefix;}
    void setPrefix(const char * fmt, ...);


    /// Lazy programmer API. Executes on default logger.

    static void Debug(const char * fmt, ...);
    static void Debug(uint32_t level, const char * fmt, ...);
    static void Error(const char * fmt, ...);

    static void Debug(const std::string & fmt, ...);
    static void Debug(uint32_t level, const std::string & fmt, ...);
    static void Error(const std::string & fmt, ...);

    /// By default uses LogDispatcher
    static Logger Default;
private:
    Logger(const Logger &);
    void operator=(const Logger &);

    //enum LogType {LOG_DUMMY, LOG_STD_AUTO, LOG_STDOUT, LOG_STDERR, LOG_FILE, LOG_FD, LOG_UDP, LOG_SYSLOG};

    void mRelay(LogDispatcher::LogType type, const char * fmt, va_list vl);

    typedef std::list<LogDispatcher *> DispatcherList;
    DispatcherList  mDestinations;
    Mutex *         mMutex;
    uint32_t        mDebugLevel;
    std::string     mPrefix;
};

}

#endif // LOGGER_H
