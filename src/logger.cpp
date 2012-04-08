#include "logger.h"

#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define CSTRING_LEN(str)            (sizeof(str) -1)
#define DEFINE_CSTRING(name, value) static const char * name = value; static const uint32_t name##_len = CSTRING_LEN(value);
#define ALLOC_ERROR_STRING          "Logger: could not allocate space for log message.\n"
#ifndef LOGGER_DEBUG_PREFIX
#define LOGGER_DEBUG_PREFIX         "[debug] "
#endif
#ifndef LOGGER_ERROR_PREFIX
#define LOGGER_ERROR_PREFIX         "[error] "
#endif

DEFINE_CSTRING(alloc_error_str, ALLOC_ERROR_STRING)
DEFINE_CSTRING(debug_pferix,    LOGGER_DEBUG_PREFIX)
DEFINE_CSTRING(error_pferix,    LOGGER_ERROR_PREFIX)

Logger::Logger()
{
    mType = LOG_STD_AUTO;
}

Logger::~Logger()
{
    switch(mType)
    {
    case LOG_FILE:
        fclose(mFile);
        break;
    case LOG_FD:
        close(mFd);
        break;
    case LOG_UDP:
        close(mUdpSock);
        break;
    default:
        break;
    }
}

void Logger::log(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(mType, 0, 0, fmt, vl);
    va_end(vl);
}

void Logger::debug(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    mRelay(mType, debug_pferix, debug_pferix_len, fmt, vl);
    va_end(vl);
}

void Logger::debug(int level, const char *fmt, ...)
{
    if(level > mDebugLevel) return;
    va_list vl;
    va_start(vl, fmt);
    mRelay(mType, debug_pferix, debug_pferix_len, fmt, vl);
    va_end(vl);
}

void Logger::error(const char *fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    LogType type = (mType == LOG_STD_AUTO) ? LOG_STDERR : mType;
    mRelay(type, error_pferix, error_pferix_len, fmt, vl);
    va_end(vl);
}

void Logger::Debug(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(Default.mType, debug_pferix, debug_pferix_len, fmt, vl);
    va_end(vl);
}

void Logger::Debug(int level, const char * fmt, ...)
{
    if(level > Default.mDebugLevel) return;
    va_list vl;
    va_start(vl, fmt);
    Default.mRelay(Default.mType, debug_pferix, debug_pferix_len, fmt, vl);
    va_end(vl);
}

void Logger::Error(const char * fmt, ...)
{
    va_list vl;
    va_start(vl, fmt);
    LogType type = (Default.mType == LOG_STD_AUTO) ? LOG_STDERR : Default.mType;
    Default.mRelay(type, error_pferix, error_pferix_len, fmt, vl);
    va_end(vl);
}

Logger Logger::Default;

void Logger::mRelay(LogType type, const char *prefix, int prefix_len, const char *fmt, va_list vl)
{
    char sBuf[4096];
    char * dBuf = sBuf;
    unsigned int size = prefix_len;
    int bufSpace = sizeof(sBuf) - prefix_len;
    bufSpace = bufSpace < 0 ? 0 : bufSpace;
    size += vsnprintf(sBuf + prefix_len, bufSpace, fmt, vl);
    if(size >= sizeof(sBuf))
    {
        size = prefix_len + vsnprintf(0, 0, fmt, vl);
        dBuf = (char*)malloc(size + 2);
        if(dBuf)
        {
            vsnprintf(dBuf, size + 2, fmt, vl);
            memcpy(dBuf, prefix, prefix_len);
            dBuf[size++] = '\n';
        }
        else
        {
            size = alloc_error_str_len;
            memcpy(sBuf, alloc_error_str, size);
            dBuf = sBuf;
        }
    }
    else
    {
        memcpy(sBuf, prefix, prefix_len);
        sBuf[size++] = '\n';
    }
    switch(type)
    {
    case LOG_STD_AUTO:
    case LOG_STDOUT:
        mLog(stdout, dBuf, size);
        break;
    case LOG_STDERR:
        mLog(stderr, dBuf, size);
        break;
    case LOG_FILE:
        mLog(mFile, dBuf, size);
        break;
    case LOG_FD:
        mLog(mFd, dBuf, size);
        break;
    case LOG_UDP:
        mLog(mUdpSock, &mAddr, dBuf, size);
        break;
    case LOG_DUMMY:
    default:
        break;
    }
    if(dBuf != sBuf) free(dBuf);
}

void Logger::mLog(FILE *file, const char * str, int len)
{
    fprintf(file, "%.*s", len, str);
    fflush(file);
}

void Logger::mLog(int dest, const char * str, int len)
{
    write(dest, str, len);
}

void Logger::mLog(int udpSock, struct sockaddr * addr, const char * str, int len)
{
    sendto(udpSock, str, len, 0, addr, sizeof(*addr));
}
