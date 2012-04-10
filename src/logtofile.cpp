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

#include "logtofile.h"
#include <string.h>

#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
#include <sys/fcntl.h>
#endif

namespace jacl
{

LogToFile::LogToFile(const char * filename)
    :mFilename(filename)
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(filename && filename[0])
        mFd = open(mFilename.c_str(), O_APPEND|O_WRONLY|O_CREAT);
    else
        mFd = -1;
#else
    if(filename && filename[0])
        mFile = fopen(mFilename.c_str(), "a");
    else
        mFile = 0;
#endif
}

LogToFile::LogToFile(const std::string & filename)
    :mFilename(filename)
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(!mFilename.empty() && mFilename.size())
        mFd = open(mFilename.c_str(), O_APPEND|O_WRONLY|O_CREAT);
    else
        mFd = -1;
#else
    if(!mFilename.empty() && mFilename.size())
        mFile = fopen(mFilename.c_str(), "a");
    else
        mFile = 0;
#endif
}

LogToFile &LogToFile::setFilename(const std::string &filename)
{
    mFilename = filename;
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(mFd != -1) ::close(mFd);
    mFd = open(mFilename.c_str(), O_APPEND|O_WRONLY|O_CREAT);
#else
    if(mFile) fclose(mFile);
    mFile = fopen(mFilename.c_str(), "a");
#endif
    return *this;
}

LogToFile &LogToFile::close()
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(mFd != -1)
    {
        ::close(mFd);
        mFd = -1;
    }
#else
    if(mFile)
    {
        fclose(mFile);
        mFile = 0;
    }
#endif
    return *this;
}

void LogToFile::infoMessage(const char *message, int len)
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(mFd != -1)
        write(mFd, message, len);
#else
    if(mFile)
    {
        fprintf(mFile, "%.*s", len, message);
        fflush(stdout);
    }
#endif
}

void LogToFile::debugMessage(const char *message, int len)
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(mFd != -1)
    {
        iovec vec[2] = {
            {const_cast<void *>(static_cast<const void *>(LOGGER_DEBUG_PREFIX)), sizeof(LOGGER_DEBUG_PREFIX) - 1},
            {const_cast<void *>(static_cast<const void *>(message)), len}
        };
        writev(mFd, vec, 2);
    }
#else
    if(mFile)
    {
        fprintf(mFile, LOGGER_DEBUG_PREFIX"%.*s", len, message);
        fflush(stdout);
    }
#endif
}

void LogToFile::errorMessage(const char *message, int len)
{
#if     defined(POSIX) || defined(_POSIX) || defined(_POSIX_SOURCE)
    if(mFd != -1)
    {
        iovec vec[2] = {
            {const_cast<void *>(static_cast<const void *>(LOGGER_ERROR_PREFIX)), sizeof(LOGGER_ERROR_PREFIX) - 1},
            {const_cast<void *>(static_cast<const void *>(message)), len}
        };
        writev(mFd, vec, 2);
    }
#else
    if(mFile)
    {
        fprintf(mFile, LOGGER_ERROR_PREFIX"%.*s", len, message);
        fflush(stdout);
    }
#endif
}

}
