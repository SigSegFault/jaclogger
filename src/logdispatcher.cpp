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

#include "../include/logdispatcher.h"
#include "../port/mutex.h"

#ifndef LOGGER_DEBUG_PREFIX
#define LOGGER_DEBUG_PREFIX         "[debug] "
#endif
#ifndef LOGGER_ERROR_PREFIX
#define LOGGER_ERROR_PREFIX         "[error] "
#endif


namespace jacl
{

LogDispatcher::LogDispatcher(uint32_t mask)
    :mMask(mask)
{
}

LogDispatcher::~LogDispatcher()
{
}

void LogDispatcher::infoMessage(const char *message, int len)
{
    fprintf(stdout, ".*%s", message);
}

void LogDispatcher::debugMessage(const char *message, int len)
{
    fprintf(stdout, LOGGER_DEBUG_PREFIX".*%s", message);
}

void LogDispatcher::errorMessage(const char *message, int len)
{
    fprintf(stderr, LOGGER_ERROR_PREFIX".*%s", message);
}

void LogDispatcher::sink(uint32_t type, const char *message, int len)
{
    switch(type)
    {
    case LOG_INFO:
        if(mMask && LOG_INFO)
            infoMessage(message, len);
        break;
    case LOG_DEBUG:
        if(mMask && LOG_DEBUG)
            debugMessage(message, len);
        break;
    case LOG_ERROR:
        if(mMask && LOG_ERROR)
            errorMessage(message, len);
        break;
    default: break;
    }
}

void LogDispatcher::sink(uint32_t type, std::string &message)
{
    sink(type, message.c_str());
}

}
