#ifndef OWL_BASE_CURRENT_THREAD_H
#define OWL_BASE_CURRENT_THREAD_H

#include <string>
#include <sys/types.h>
namespace CurrentThread {
extern __thread pthread_t cached_tid = 0;
__thread std::string cached_thread_name;

void get_cached_tid();
int tid();
}

#endif