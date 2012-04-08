#ifndef LOGGERDEST_H
#define LOGGERDEST_H

#include <stdio.h>
#include <stdint.h>
#include <string>

class LogDispatcher
{
public:
    enum LogType {LOG_INFO = 0x1, LOG_DEBUG = 0x2, LOG_ERROR = 0x4};

    LogDispatcher(uint32_t mask = (LOG_INFO|LOG_DEBUG|LOG_ERROR));
    virtual ~LogDispatcher();

    uint32_t mask() {return mMask;}
    void setMask(uint32_t mask) {mMask = mask;}
    void block(LogType type) {mMask |= type;}
    void unblock(LogType type) {mMask = mMask & ~type;}

    void sink(uint32_t type, const char * message, int len = -1);
    void sink(uint32_t type, std::string & message);

protected:
    virtual void infoMessage(const char * message, int len);
    virtual void debugMessage(const char * message, int len);
    virtual void errorMessage(const char * message, int len);

private:
    uint32_t    mMask;
};

#endif // LOGGERDEST_H
