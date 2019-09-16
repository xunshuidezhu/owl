#ifndef OWL_FILE_LOG_FILE_H
#define OWL_FILE_LOG_FILE_H

#include <syslog.h>

#include <cstdarg>

namespace owl {
namespace log {
    void openlog(const char* argv0, const char* log_dir, int priority);
    void closelog();

    void log(int priority, const char* format, ...) __attribute__((format(printf, 2, 3)));

    typedef void (*openlog_t)(const char*, const char*, int);
    typedef void (*closelog_t)();

    typedef void (*vlog_t)(int, const char*, va_list);
    void setvlog(vlog_t);

    void setlog(openlog_t, closelog_t, vlog_t);
    int set_priority(int mode);

} // namespace log
} // namespace owl

#endif // OWL_FILE_LOG_FILE_H
