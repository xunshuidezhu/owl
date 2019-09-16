#include "log_file.h"
#include <unistd.h>

int main()
{
    owl::log::set_priority(LOG_DEBUG);
    owl::log::log(LOG_INFO, "owl log test %d", getpid());
}