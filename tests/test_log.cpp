#include <iostream>
#include "src/log.h"
using namespace FbServer;
int main(int argc, char** argv){
    Logger::ptr logger(new Logger);
    logger->addAppender(LogAppender::ptr(new StdoutLogAppender));

    FileLogAppender::ptr file_appender(new FileLogAppender("./log.txt"));
    LogFormatter::ptr fmt(new LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(LogLevel::ERROR);

    logger->addAppender(file_appender);

    //sylar::LogEvent::ptr event(new sylar::LogEvent(__FILE__, __LINE__, 0, sylar::GetThreadId(), sylar::GetFiberId(), time(0)));
    //event->getSS() << "hello sylar log";
    //logger->log(sylar::LogLevel::DEBUG, event);
    std::cout << "hello sylar log" << std::endl;
    return 0;
}
