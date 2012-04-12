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

#ifndef LOGTOFILE_H
#define LOGTOFILE_H

#include "logdispatcher.h"
#include <string>

#if     !defined(POSIX) && !defined(_POSIX) && !defined(_POSIX_SOURCE)
#include <stdio.h>
#endif

namespace jacl
{

class LogToFile : public LogDispatcher
{
public:
    LogToFile(const char * filename = 0);
    LogToFile(const std::string & filename = std::string());

    LogToFile & setFilename(const std::string & filename);
    inline const std::string & filename() {return mFilename;}

    LogToFile & close();

protected:
    virtual void infoMessage(const char * message, int len);
    virtual void debugMessage(const char * message, int len);
    virtual void errorMessage(const char * message, int len);

private:
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    int         mFd;
#else
    FILE *      mFile;
#endif
    std::string mFilename;
};


}

#endif // LOGTOFILE_H
