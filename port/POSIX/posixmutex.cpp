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

#include "posixmutex.h"

namespace jacl
{

PosixMutex::PosixMutex()
{
    pthread_mutex_init(&mMutex, 0);
}

PosixMutex::~PosixMutex()
{
    pthread_mutex_destroy(&mMutex);
}

bool PosixMutex::lock()
{
    return pthread_mutex_lock(&mMutex);
}

bool PosixMutex::lock(uint32_t usecs)
{
    timespec interval;
    interval.tv_sec = usecs / 1000000;
    usecs -= interval.tv_sec * 1000000;
    interval.tv_nsec = usecs * 1000;
    return pthread_mutex_timedlock(&mMutex, &interval);
}

bool PosixMutex::tryLock()
{
    return pthread_mutex_trylock(&mMutex);
}

bool PosixMutex::unlock()
{
    return pthread_mutex_unlock(&mMutex);
}

Mutex * getPlatfromSpecificMutex()
{
    return new PosixMutex;
}

}
