
#include <execinfo.h>

#include "owl_log.h"

#include <assert.h>
#include <climits>
#include <ctype.h>
#include <errno.h>
#include <fstream>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <time.h>
#include <vector>

using namespace owllog;

namespace owllog {
namespace detail {
    ThreadMutex s_writeLogLock;
    ThreadMutex& g_writeLogLock()
    {
        return s_writeLogLock;
    }

    std::string GetSystemDayTime2()
    {
        char timebuf[64] = { 0 };

        time_t timep;
        struct tm* t;
        time(&timep);
        t = localtime(&timep);
        snprintf(timebuf, sizeof(timebuf), "%4d-%02d-%02d %d:%02d:%02d",
            t->tm_year + 1900, t->tm_mon + 1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);
        return std::string(timebuf);
    }

    std::string GetSystemDay()
    {
        char Time[30] = { 0 };
        time_t timep;
        struct tm* p;
        time(&timep);
        p = localtime(&timep);
        snprintf(Time, sizeof(Time), "%4d-%02d-%02d", 1900 + p->tm_year, 1 + p->tm_mon, p->tm_mday);
        std::string SystemTime(Time);
        return SystemTime;
    }
    func_type dobackup(void* p)
    {
        owlLogTool tool;
        Log* pLog = (Log*)p;
        std::string oldfile(pLog->getLogFileName());
        int roll = pLog->getRoll();
        while (true) {
            if (roll && tool.TimeIs(23)) //23::00::00~23::59::59
            {
                try {
                    if (tool.isExist(oldfile) && !tool.isFileEmpty(oldfile)) {
                        ScopedLock<ThreadMutex> lock(g_writeLogLock());
                        rename(oldfile.c_str(), tool.getNewFileName(oldfile).c_str());
                        tool.newfile(oldfile);
                    }
                } catch (std::exception& ex) {
                    printf("[simpleLog::Log] [dobackup] %s\n", ex.what());
                }
                SleepSeconds(3600 * 2);
            }

            SleepSeconds(60 * 5);
        }
    }
}; // namespace detail

using namespace detail;
std::string owlLogTool::getNewFileName(const std::string& oldname)
{
    std::string tryname(oldname);

    std::string dateSuffix = "." + GetSystemDay();
    tryname += dateSuffix;
    std::string newname(tryname);
    char numSuffix[64 + 2] = { 0 };
    for (int i = 1; isExist(newname) && i < INT_MAX; i++) {
        newname = tryname;
        memset(numSuffix, 0, sizeof(numSuffix));
        snprintf(numSuffix, sizeof(numSuffix), ".%d", i);
        newname += numSuffix;
    }
    return newname;
}
bool owlLogTool::TimeIs(int hour)
{
    time_t timep;
    struct tm* t;
    time(&timep);
    t = localtime(&timep);
    return (/*t->tm_min == 0 &&*/ t->tm_hour == hour);
}
bool owlLogTool::isExist(const std::string& file)
{
    std::ifstream ifs(file.c_str());
    return ifs.good();
}
bool owlLogTool::isFileEmpty(const std::string& file)
{
    std::ifstream ifs(file.c_str());
    return ifs.peek() == std::ifstream::traits_type::eof();
}

void owlLogTool::newfile(const std::string& name)
{
    std::ofstream ofs(name.c_str());
    if (!ofs) {
        printf("[simpleLog::Log] Cannot open the output file: %s", name.c_str());
    }
    ofs.close();
}
};

Log::Log(const std::string& name, ConstructorMode mode)
{
    m_LogArgs.mode = mode;
    m_LogArgs.name = name;

    m_roll = 1;
}

Log::~Log()
{
    if (getRoll()) {
        rename(m_logfilename.c_str(), m_tool.getNewFileName(m_logfilename).c_str());
        m_tool.newfile(m_logfilename);
    }
}

void Log::start()
{
    if (m_LogArgs.name.empty()) {
        printf("[simpleLog::Log] log filename is empty\n");
        assert(false);
    }
    switch (m_LogArgs.mode) {
    case SET_NAME:
        m_logfilename = m_LogArgs.name;
        break;
    case READ_CONFIG:
        // TODO
        break;
    default:
        assert(false);
        break;
    }

    m_LogThread.createThread(dobackup, *this);
}

const char* Log::log_info[TOTLE_LOG_LELEVE_SIZE + 1] = { "FATAL", "ERROR", "WARNING", "INFO", "DEBUG", "" };

void Log::setRoll(int roll)
{
    m_roll = roll;
}

std::string Log::getLogFileName() const
{
    return m_logfilename;
}

int Log::checkLogLevel(int log_level)
{
    // TODO;
    return 0;
}

void Log::logImpl(FILE* stream, const char* fmt, va_list argList)
{
    int ret = fprintf(stream, "%s ", GetSystemDayTime2().c_str());
    if (ret < 0) {
        return;
    }

    ret = vfprintf(stream, fmt, argList);
    if (ret < 0) {
        return;
    }

    ret = fprintf(stream, "\n");
    if (ret < 0) {
        return;
    }
}

void Log::print_log(int log_level, const char* fmt, ...)
{
    if (checkLogLevel(log_level) != 0) {
        return;
    }

    ScopedLock<ThreadMutex> lock(g_writeLogLock());
    FILE* fp = fopen(m_logfilename.c_str(), "a");
    if (fp == NULL)
        return;

    va_list ap;
    va_start(ap, fmt);
    logImpl(fp, fmt, ap);
    va_end(ap);

    fclose(fp);
}