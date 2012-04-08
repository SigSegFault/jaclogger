#ifndef LOGGER_H
#define LOGGER_H

#include <list>
#define LOGGER_USE_USE_UDP
#ifdef LOGGER_USE_USE_UDP
#include <netinet/in.h>
#endif
#include "logdispatcher.h"

class Logger
{
public:
    Logger();
    ~Logger();

    void log(const char * fmt, ...);

    /// Assumed to be of current log level
    void debug(const char * fmt, ...);
    void debug(int level, const char * fmt, ...);
    void error(const char * fmt, ...);

    inline int debugLevel() { return mDebugLevel;}
    inline void setDebuglevel(int lvl) {mDebugLevel = lvl;}

    /// Lazy programmer API. Executes on default logger.

    static void Debug(const char * fmt, ...);
    static void Debug(int level, const char * fmt, ...);
    static void Error(const char * fmt, ...);

    /// By default bound to LOG_STD_AUTO ie log() and debug() to stdout, error() to stderr
    static Logger Default;
private:

    enum LogType {LOG_DUMMY, LOG_STD_AUTO, LOG_STDOUT, LOG_STDERR, LOG_FILE, LOG_FD, LOG_UDP, LOG_SYSLOG};

    void mRelay(LogType type, const char * prefix, int prefix_len, const char * fmt, va_list vl);
    void mLog(FILE * dest, const char * str, int len);
    void mLog(int dest, const char * str, int len);
    void mLog(int udpSock, struct sockaddr * addr, const char * str, int len);

    LogType mType;
    int mDebugLevel;
    union
    {
        FILE *  mFile;
        int     mFd;
        struct
        {
            int             mUdpSock;
            struct sockaddr mAddr;
        };
    };
};

#endif // LOGGER_H
