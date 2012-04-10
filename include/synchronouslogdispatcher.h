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

#ifndef SYNCHRONOUSLOGDISPATCHER_H
#define SYNCHRONOUSLOGDISPATCHER_H

#include "logdispatcher.h"

namespace jacl
{

class Mutex;

class SynchronousLogDispatcher : public LogDispatcher
{
public:
    SynchronousLogDispatcher();
    virtual ~SynchronousLogDispatcher() = 0;

    inline SynchronousLogDispatcher & setSynchronous(bool yes = true) {mSync = yes; return *this;}
    inline bool isSynchronous() {return mSync;}

protected:
    virtual void infoMessage(const char * message, int len) = 0;
    virtual void debugMessage(const char * message, int len) = 0;
    virtual void errorMessage(const char * message, int len) = 0;

    virtual void sink(uint32_t type, const char * message, int len = -1);

private:
    bool        mSync;
    Mutex *     mMutex;
};

}

#endif // SYNCHRONOUSLOGDISPATCHER_H
