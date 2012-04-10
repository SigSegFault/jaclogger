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

#include "synchronouslogdispatcher.h"
#include "config.h"

#ifdef THREAD_SAFE_LOGGER
#include "../port/mutex.h"
#endif

namespace jacl
{

SynchronousLogDispatcher::SynchronousLogDispatcher()
    :LogDispatcher()
{
#ifdef THREAD_SAFE_LOGGER
    mMutex = getPlatfromSpecificMutex();
#endif
}

SynchronousLogDispatcher::~SynchronousLogDispatcher()
{
#ifdef THREAD_SAFE_LOGGER
    delete mMutex;
#endif
}

void SynchronousLogDispatcher::sink(uint32_t type, const char *message, int len)
{
#ifdef THREAD_SAFE_LOGGER
        ScopedGuard gandalf(mSync ? this->mMutex : 0);
#endif
    LogDispatcher::sink(type, message, len);
}

}
