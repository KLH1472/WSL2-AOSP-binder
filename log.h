#pragma once
#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <unistd.h>

#ifndef LOG_TAG
#define LOG_TAG "binder"
#endif

#define LOG(fmt, ...) do { \
    struct timeval _tv; gettimeofday(&_tv, NULL); \
    struct tm *_tm = localtime(&_tv.tv_sec); \
    printf("%02d:%02d:%02d.%06ld %5d %5d %s: " fmt "\n", \
        _tm->tm_hour, _tm->tm_min, _tm->tm_sec, (long)_tv.tv_usec, \
        getpid(), (int)syscall(SYS_gettid), LOG_TAG, \
        ##__VA_ARGS__); \
} while(0)
