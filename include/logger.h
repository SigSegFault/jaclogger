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

#include <map>
#include <netinet/in.h>
#include <stdarg.h>
#include <stdio.h>
#include <string>

#include "config.h"
#include "logdispatcher.h"

namespace jacl
{

class Mutex;

class Logger
{
public:
    Logger();


    enum ConstructOptions {
        /// Do not synchronize critical sections.
        /// Has no effect if library is compiled without thread safety.
        ThreadUnsafe            = 0x1,
        /// Autoinclude LogToStd destination.
        IncludeStd              = 0x2
    };
    /// Obviously options can be grouped using | operator.
    Logger(ConstructOptions opts);

    ~Logger();

    Logger & info(const char * fmt, ...);
    /// Assumed to be of current log level
    Logger & debug(const char * fmt, ...);
    Logger & debug(uint32_t level, const char * fmt, ...);
    Logger & error(const char * fmt, ...);

    int debugLevel();
    Logger & setDebuglevel(int lvl);

    const std::string prefix();
    Logger & setPrefix(const std::string & prefix);
    Logger & setPrefix(const char * fmt, ...);

    LogDispatcher * popDispatcher(const std::string & name);
    Logger & pushDispatcher(const std::string & name, LogDispatcher * dispatcher);
    Logger & clear();


    /// Lazy programmer API. Executes on default logger.

    static Logger & Debug(const char * fmt, ...);
    static Logger & Debug(uint32_t level, const char * fmt, ...);
    static Logger & Error(const char * fmt, ...);

    static Logger & Debug(const std::string & fmt, ...);
    static Logger & Debug(uint32_t level, const std::string & fmt, ...);
    static Logger & Error(const std::string & fmt, ...);

    /// By default uses one destination: LogDispatcher
    static Logger & getDefault();
private:
    Logger(const Logger &);
    void operator=(const Logger &);

    void mRelay(LogDispatcher::LogType type, const char * fmt, va_list vl);

    typedef std::map<std::string, LogDispatcher *> DispatcherSet;
    DispatcherSet   mDispatchers;
    Mutex *         mMutex;
    uint32_t        mDebugLevel;
    std::string     mPrefix;
};

#ifdef THREAD_SAFE_LOGGER
// * Here is where magic begins. Whenever nasty programmer tries to create
//   threads at static initialization time and includes this header, our magical
//   slave uses his magical 'order of initialization' power and always does
//   default Logger intialization before anyone else in this multithreaded world.
//
// * The only pitfall i can see is that Logger object will be always created,
//   even if it hadn't been used during program execution. But it's pretty light.
//
// * As long as default Logger is singleton this also guarantees correct
//   library initialization order.
class __MagicalInitializationSlave
{
public:
    __MagicalInitializationSlave() {Logger::getDefault();}
};
static const __MagicalInitializationSlave __magicalInitializerSlave;
#endif

}

#endif // LOGGER_H
