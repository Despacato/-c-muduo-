#include "Timestamp.h"
#include <time.h>
#include <iostream>


Timestamp::Timestamp()
    : microSecondsSinceEpoch_(0)
{
}


Timestamp::Timestamp(int64_t microSecondsSinceEpoch)
    : microSecondsSinceEpoch_(microSecondsSinceEpoch)
{
}

Timestamp Timestamp::now()
{
    return Timestamp(static_cast<int64_t>(time(nullptr)*1000000));
}

std::string Timestamp::toString() const
{
    char buf[128] = {0};
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch_ / 1000000); // 转换为秒
    tm *tm_time = localtime(&seconds); // 传入 time_t 指针
    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d",
             tm_time->tm_year + 1900,
             tm_time->tm_mon + 1,   
             tm_time->tm_mday,
             tm_time->tm_hour,
             tm_time->tm_min,
             tm_time->tm_sec);
    return buf;
}

int main()
{
    Timestamp ts = Timestamp::now();
    std::cout << "Current time: " << ts.toString() << std::endl;
    return 0;
}

