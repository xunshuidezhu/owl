#include "log_file.h"
#include "file_util.h"
#include <cstdio>
#include <cstdlib>

namespace owl {
namespace log {
    static openlog_t global_openlog_ = NULL;
    static closelog_t global_closelog_ = ::closelog;
    static vlog_t global_vlog_ = vsyslog;
    static int global_priority_ = LOG_ERR;
    int set_priority(int mode)
    {
        int prev_prioity = mode;
        global_priority_ = mode;
        return prev_prioity;
    }
    //打开文件写入日志
    void openlog(const char* argv0, const char* log_dir, int priority)
    {

        char new_path[1024] = { 0 };
        //＇～＇转换成'HOME'
        if ('~' == log_dir[0]) {
            snprintf(new_path, sizeof(new_path), "%s%s", getenv("HOME"), log_dir + 1);
        } else {
            snprintf(new_path, sizeof(new_path), "%s", log_dir);
        }

        global_priority_ = priority;
        //如果设置了日志文件目录，则打开这个日志
        if (NULL != global_openlog_) {
            global_openlog_(argv0, new_path, priority);
        } else {
            //默认打开系统日志，当出错时记录并记录进程Id
            ::openlog(argv0, LOG_CONS | LOG_PID, priority);
        }
    }

    void closelog()
    {
        //关闭日志系统文件描述符
        global_closelog_();
    }

    void log(int priority, const char* format, ...)
    {

        if (priority > global_priority_)
            return;
        //用来处理可变参数，　va_list指向可变参数的指针
        va_list args;
        //让指针指向可变参数第一个参数，在format后面
        va_start(args, format);

        global_vlog_(priority, format, args);
        va_end(args);
    }

    void setvlog(vlog_t vlog)
    {
        global_vlog_ = vlog;
    }

    void setlog(openlog_t open_log, closelog_t close_log, vlog_t vlog)
    {
        global_openlog_ = open_log;
        global_closelog_ = close_log;
        global_vlog_ = vlog;
    }
}
}
