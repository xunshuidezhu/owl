#include <sys/timerfd.h>

#include <sys/time.h>

#include <time.h>

#include <unistd.h>

#include <stdlib.h>

#include <stdio.h>

#include <stdint.h> /* Definition of uint64_t */

#define handle_error(msg)   \
    do {                    \
        perror(msg);        \
        exit(EXIT_FAILURE); \
    } while (0)

void printTime()

{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    printf("printTime: current time:%ld.%ld ", tv.tv_sec, tv.tv_usec);
}

int main(int argc, char* argv[])

{

    struct timespec now;

    if (clock_gettime(CLOCK_REALTIME, &now) == -1)

        handle_error("clock_gettime");

    struct itimerspec new_value;

    new_value.it_value.tv_sec = now.tv_sec + 1;

    new_value.it_value.tv_nsec = now.tv_nsec;

    new_value.it_interval.tv_sec = 3;

    new_value.it_interval.tv_nsec = 0;

    int fd = timerfd_create(CLOCK_REALTIME, 0);

    if (fd == -1)

        handle_error("timerfd_create");

    if (timerfd_settime(fd, TFD_TIMER_ABSTIME, &new_value, NULL) == -1)

        handle_error("timerfd_settime");

    printTime();

    printf("timer started\n");

    for (uint64_t tot_exp = 0; tot_exp < 10;)

    {

        uint64_t exp;

        ssize_t s = read(fd, &exp, sizeof(uint64_t));

        if (s != sizeof(uint64_t))

            handle_error("read");

        tot_exp += exp;

        printTime();

        printf("read: %llu; total=%llu\n", exp, tot_exp);
    }

    exit(EXIT_SUCCESS);
}