#ifndef OWL_LOG_H
#define OWL_LOG_H

#include "thread_group.h"
#include <stdio.h>
#include <string>
#include <vector>

namespace owllog {
enum {
    LOG_LELEVE_FATAL = 0,
    LOG_LELEVE_ERROR = 1,
    LOG_LELEVE_WARNING = 2,
    LOG_LELEVE_INFO = 3,
    LOG_LELEVE_DEBUG = 4,
    TOTLE_LOG_LELEVE_SIZE = 5
};

enum ConstructorMode {
    SET_NAME = 0,
    READ_CONFIG = 1 // Not implemented
};

class owlLogTool {
public:
    std::string getNewFileName(const std::string& oldname);
    bool TimeIs(int hour);
    bool isExist(const std::string& file);
    bool isFileEmpty(const std::string& file);
    void newfile(const std::string& name);
};

class Log {
public:
    Log(const std::string& name, ConstructorMode mode = SET_NAME);
    ~Log();
    void start();

public:
    void setRoll(int roll); //是否按日期滚动文件名，1滚动，0不滚动
    int getRoll() const { return m_roll; }
    void print_log(int log_level, const char* fmt, ...);

    std::string getLogFileName() const;

private:
    Log(const Log&);

    void logImpl(FILE* stream, const char* fmt, va_list argList);
    int checkLogLevel(int log_level);
    static const char* log_info[TOTLE_LOG_LELEVE_SIZE + 1];
    struct LogArgs {
        std::string name;
        ConstructorMode mode;
    };

    std::string m_logfilename;
    ThreadGroup m_LogThread;

    owlLogTool m_tool;
    LogArgs m_LogArgs;
    int m_roll;
};
} // namespace owllog

#endif // OWL_LOG_H