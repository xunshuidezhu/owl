#include "file_util.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

using namespace owl;
using namespace std;

file::WriteFile::WriteFile(string filename)
    : fd(open(filename.c_str(), O_APPEND | O_RDWR | O_CLOEXEC))
{
}

file::WriteFile::~WriteFile()
{
    close(fd);
}

void file::WriteFile::write(char* log, size_t log_len)
{
    size_t write_length = write_(log, log_len);
    //没有写完
    size_t unused_buf = log_len - write_length;
    //继续在没有写完的Buffer里写
    while (unused_buf > 0) {
        size_t temp = write_(log + write_length, unused_buf);
        write_length += temp;
        unused_buf = log_len - write_length;
    }
}

size_t file::WriteFile::write_(char* log, size_t log_len)
{
    ::write(fd, log, log_len);
}
