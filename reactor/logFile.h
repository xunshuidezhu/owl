#ifndef LOG_FILE_H
#define LOG_FILE_H
#include "../base/mutex_RAII.h"
#include "../base/thread_mutex.h"
#include "file.h"

#include <mutex>
#include <string>
#include <unistd.h>

using namespace owl::base;

class LogFile : File {
public:
    LogFile(const std::string& filename, int flushFlag = 1000);

    ~LogFile();

    void append(const char*, int);

    void start();

    void stop();

private:
    void _append(const char*, int);

    int _flushFlag;

    std::string _filename;

    int _flushCount;

    Mutex _mutex;

    AppendFile* _file;
};

#endif