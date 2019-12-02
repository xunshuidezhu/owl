#ifndef FILE_H
#define FILE_H
#include <string.h>
#include <string>

class File {
private:
    File(const File&) {} //禁止拷贝

    File& operator=(const File&) {} // 禁止赋值

protected:
    File(){};

    ~File(){};
};

class AppendFile : private File {
public:
    explicit AppendFile(std::string fileName);

    void append(const char* log, const size_t len);

    void flush();

    ~AppendFile();

    size_t write(const char* log, const size_t len);

public:
    FILE* _fp;

    static const int _size = 64 * 1024;

    char _buffer[_size];
};
#endif
