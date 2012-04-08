#include "logdispatcher.h"

LogDispatcher::LogDispatcher(uint32_t mask)
    :mMask(mask)
{
}

LogDispatcher::~LogDispatcher()
{
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
}

void LogDispatcher::infoMessage(const char *message, int len)
{
    fprintf(stdout, ".*%s", message);
}

void LogDispatcher::debugMessage(const char *message, int len)
{
}

void LogDispatcher::errorMessage(const char *message, int len)
{
}
