/*
 * 日志系统 2021-06-19
 */

#ifndef _FBSERVER_LOG_H
#define _FBSERVER_LOG_H
#include <iostream>
#include <string.h>
#include <stdint.h>
#include <memory>
#include <list>
#include <map>
#include <vector>
#include <time.h>
#include <sstream>
#include <fstream>

namespace FbServer{

class Logger;

/*
* @brief 日志级别
*/
enum LogLevel {
    UNKNOW = 0,
    DEGBUG = 1,
    IFNO = 2,
    WARN = 3,
    ERROR = 4,
    FATAL = 5
};
// LogLevel TO 字符串
inline const char* ToString(LogLevel level) {
        switch(level) {
#define XX(name) \
    case LogLevel::name: \
        return #name; \
        break;

    XX(DEGBUG)
    XX(IFNO)
    XX(WARN)
    XX(ERROR)
    XX(FATAL)
#undef XX
            default:
                return "UNKNOW";
        }
        return "UNKNOW";
}


/*
* @brief 日志记录信息
*/
class LogEvent {
public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

    // 返回文件名
    const char* getFile() const { return m_file;}
    // 返回行号
    int32_t getLine() const { return m_line;}
    // 返回耗时
    uint32_t getElapse() const { return m_elapse;}
    // 返回线程ID
    uint32_t getThreadId() const { return m_threadId;}
    // 返回协程ID
    uint32_t getFiberId() const { return m_fiberId;}
    // 返回时间
    uint64_t getTime() const { return m_time;}
//    // 返回线程名称
//    const std::string& getThreadName() const { return m_threadName;}
    // 返回日志内容
    std::string getContent() const { return m_content;}

private:
    const char* m_file = nullptr;   // 文件名
    int32_t m_line = 0;             // 行号
    uint32_t m_threadId = 0;        // 线程号
    uint32_t m_fiberId = 0;         // 协程号
    uint64_t m_time = 0;            // 时间戳
    uint32_t m_elapse = 0;          // 程序启动到现在的ms
    std::string m_content;          // 内容
};

/*
* @brief 日志格式
*/
class LogFormatter {
    friend class Logger;
public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);
    // 格式化输出
    std::string format(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);

    // 输出样式
    class FormatItem {
    public:
        typedef std::shared_ptr<FormatItem> ptr;
        FormatItem(const std::string& fmt=""){};
        virtual ~FormatItem(){}
        virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;
};

private:
    void init();

private:
    /// 日志格式模板
    std::string m_pattern;
    /// 日志格式解析后格式
    std::vector<FormatItem::ptr> m_items;
    /// 是否有错误
    bool m_error = false;
};

/*
* @brief 日志输出地
*/
class LogAppender {
    friend class Logger;
public:
    typedef std::shared_ptr<LogAppender> ptr;
    virtual ~LogAppender(){}
    // 写入日志
    virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event) = 0;

    void setFormatter(LogFormatter::ptr val) {m_formatter = val;}
    LogFormatter::ptr getFormatter() {return m_formatter;}
    void setLevel(LogLevel val) { m_level = val;}

protected:
    LogLevel m_level = LogLevel::DEGBUG;
    LogFormatter::ptr m_formatter;
};

// 输出到控制台
    class StdoutLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<StdoutLogAppender> ptr;
        virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);

    };
// 输出到文件
    class FileLogAppender : public LogAppender {
    public:
        typedef std::shared_ptr<FileLogAppender> ptr;
        FileLogAppender(const std::string& filename);
        virtual void log(std::shared_ptr<Logger> logger, LogLevel level, LogEvent::ptr event);
        // 确保文件打开,成功返回true
        bool reopen();
    private:
        std::string m_filename;  // 文件路径
        std::ofstream m_filestream;   //  文件流
    };

/*
* @brief 日志生成器
*/
class Logger :public std::enable_shared_from_this<Logger>{
public:
    typedef std::shared_ptr<Logger> ptr;
    Logger(const std::string& name = "root");
    // 打印日志
    void log(LogLevel level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);

    LogLevel getLevel() const {return m_level;}
    void setLevel(LogLevel val) {m_level = val;}
    std::string getName() const {return m_name;}
private:
    const std::string &m_name;                // 日志名称
    LogLevel m_level;                         // 日志级别
    std::list<LogAppender::ptr> m_appenders;  // 日志输出地集合

};


}

#endif //_FBSERVER_LOG_H
