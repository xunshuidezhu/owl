#include "owl_log/log.h"
#include <iostream>

owllog::Log* OWL = NULL;
int main()
{
    owllog::Log log("./net.log");
    OWL = &log;
    log.start();
    debug_log("debug");
    return 0;
}
