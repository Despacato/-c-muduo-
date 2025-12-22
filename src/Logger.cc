#include "Logger.h"

Logger& Logger::instance()
{
    static Logger loggerInstance;
    return loggerInstance;
}

void Logger::setLogLevel(int level)
{
    logLevel_ = level;
}       



void Logger::log(std::string msg)
{
    switch(logLevel_)
    {
        case static_cast<int>(LogLevel::INFO):
            std::cout << "[INFO]: " << msg << std::endl;
            break;
        case static_cast<int>(LogLevel::ERROR):
            std::cerr << "[ERROR]: " << msg << std::endl;
            break;
        case static_cast<int>(LogLevel::FATAL):
            std::cerr << "[FATAL]: " << msg << std::endl;
            exit(EXIT_FAILURE);
            break;
        case static_cast<int>(LogLevel::DEBUG):
            std::cout << "[DEBUG]: " << msg << std::endl;
            break;
        default:
            std::cout << "[UNKNOWN LEVEL]: " << msg << std::endl;
            break;
    }
    std::cout << "print time" << " : " << msg << std::endl;
}