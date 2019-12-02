#include "logFile.h"

using namespace owl::base;

LogFile::LogFile(const std::string& filename, int flushFlag)
    : _filename(filename)
    , _flushFlag(flushFlag)
    , _flushCount(0)
    , _mutex(" ")

{
    _file = new AppendFile(_filename.c_str());
}

LogFile::~LogFile()
{
    delete _file;
}

void LogFile::append(const char* log, int len)
{
    Mutex_Guard guard(_mutex);
    _append(log, len);
}

void LogFile::_append(const char* log, int len)
{
    _file->append(log, len);
    ++_flushCount;
    if (_flushCount >= _flushFlag) {
        _flushCount = 0;
        _file->flush();
    }
}

#ifdef DEBUG
int main()
{
    LogFile log("./logfile.log", 10);
    char buffer[100] = "log file test\r\n";
    for (int i = 0; i < 11; i++) {
        log.append(buffer, strlen(buffer));
    }
    sleep(10);
}
#endif