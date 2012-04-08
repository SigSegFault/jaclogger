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

#ifndef MUTEX_H
#define MUTEX_H

#include <stdint.h>

namespace jacl
{

class Mutex
{
public:
    Mutex() {}
    virtual ~Mutex() {}

    virtual bool lock() = 0;
    virtual bool lock(uint32_t usecs) = 0;
    virtual bool tryLock() = 0;
    virtual bool unlock() = 0;
};

class ScopedGuard
{
public:
    inline ScopedGuard(Mutex * mutex)
        :mMutex(mutex), mReleased(false)
    {
        mMutex->lock();
    }

    inline ~ScopedGuard()
    {
        mReleased || mMutex->unlock();
    }

    inline void release()
    {
        !mReleased && mMutex->unlock();
        mReleased = true;
    }

private:
    Mutex * mMutex;
    bool mReleased;
};

Mutex * getPlatfromSpecificMutex();

#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)

#elif   defined(WIN32) || defined(_WIN32)

#else
#error Well... it is seems like you are having troubles here. You can send me (SigSegFaultGit@gmail.com) email about this error message if you want me to port things to your platform.

#endif

}

#endif  //MUTEX_H
