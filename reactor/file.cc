#include "file.h"

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>

// mode "e" -- O_CLOEXEC, 子进程fork之后关闭fd再exec
AppendFile::AppendFile(std::string fileName)
    : _fp(fopen(fileName.c_str(), "w+"))
{
    setvbuf(_fp, _buffer, _IOFBF,_size);// 设置全缓冲区
}

AppendFile::~AppendFile()
{
    fclose(_fp);
}

void AppendFile::append(const char* log, const size_t len)
{
    size_t n = write(log, len);
    size_t rest = len - n;// 没写完
    while (rest > 0) {
        size_t tmp = write(log + n, rest);
        if (tmp == 0) {
            perror("append file");
            break;
        }
        n += tmp;
        rest = len - n;
    }
    
}

size_t AppendFile::write(const char* log, const size_t len)
{
    return fwrite(log, 1, len, _fp);
}

void AppendFile::flush()
{
    fflush(_fp);
}

#ifdef DEBUG
int main()
{
    AppendFile file("./append");
    char writebuffer[100] = "xunshuidezhu";
    file.append(writebuffer, strlen(writebuffer));
    file.flush();
    sleep(5);
    return 0;
}
#endif