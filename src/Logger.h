#pragma once
#include <string>
#include "noncopyable.h"
#include <iostream>

//LOG_INFO("%s %d",arg1,arg2)
#define LOG_INFO(logmsgFormat,...) \
do{\
    Logger& logger = Logger::instance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::INFO)); \
    char buf[1024] = {0}; \
    snprintf(buf, sizeof(buf)-1, logmsgFormat, ##__VA_ARGS__); \
    logger.log(buf); \
}while(0)

#define LOG_ERROR(logmsgFormat,...) \
do{\
    Logger& logger = Logger::instance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::ERROR)); \
    char buf[1024] = {0}; \
    snprintf(buf, sizeof(buf)-1, logmsgFormat, ##__VA_ARGS__); \
    logger.log(buf); \
}while(0)

#define LOG_FATAL(logmsgFormat,...) \
do{\
    Logger& logger = Logger::instance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::FATAL)); \
    char buf[1024] = {0}; \
    snprintf(buf, sizeof(buf)-1, logmsgFormat, ##__VA_ARGS__); \
    logger.log(buf); \
}while(0)

#ifdef MUDEBUG
#define LOG_DEBUG(logmsgFormat,...) \
do{\
    Logger& logger = Logger::instance(); \
    logger.setLogLevel(static_cast<int>(LogLevel::DEBUG)); \
    char buf[1024] = {0}; \
    snprintf(buf, sizeof(buf)-1, logmsgFormat, ##__VA_ARGS__); \
    logger.log(buf); \
}while(0)
#else
#define LOG_DEBUG(logmsgFormat,...)
#endif
enum class LogLevel
{
    INFO,
    ERROR,
    FATAL,
    DEBUG,
};

class Logger : noncopyable
{
    public:

    static Logger& instance();
    
    void setLogLevel(int level);
    
    void log(std::string msg);

    private:

    int logLevel_;
    
    Logger(){}
};