#include "log.h"
#include <iostream>

owllog::Log* OWL = NULL;
int main()
{
    static owllog::Log mylog("./log/my.log");
    OWL = &mylog;
    mylog.start();

    debug_log("this is a(n) debug log");
    info_log("this is a(n) info log");
    warn_log("this is a(n) warn log");
    err_log("this is a(n) error log");
    fatal_log("this is a(n) fatal log");

    //bt_log("this is a(n) backtrace log");

    return 0;
}