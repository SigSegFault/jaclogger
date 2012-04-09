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

#ifndef LOGGERDEST_H
#define LOGGERDEST_H

#include <stdio.h>
#include <stdint.h>
#include <string>

#define LOGGER_USE_USE_UDP
#ifdef LOGGER_USE_USE_UDP
#include <netinet/in.h>
#endif

namespace jacl
{

class Mutex;

class LogDispatcher
{
public:
    enum LogType {LOG_INFO = 0x1, LOG_DEBUG = 0x2, LOG_ERROR = 0x4};

    LogDispatcher(uint32_t mask = (LOG_INFO|LOG_DEBUG|LOG_ERROR));
    virtual ~LogDispatcher() = 0;

    uint32_t mask() {return mMask;}
    void setMask(uint32_t mask) {mMask = mask;}
    void block(LogType type) {mMask |= type;}
    void unblock(LogType type) {mMask = mMask & ~type;}

protected:
    virtual void infoMessage(const char * message, int len) = 0;
    virtual void debugMessage(const char * message, int len) = 0;
    virtual void errorMessage(const char * message, int len) = 0;

private:
    friend class Logger;
    void sink(uint32_t type, const char * message, int len = -1);
    void sink(uint32_t type, std::string & message);

    uint32_t    mMask;
};

}

#endif // LOGGERDEST_H
