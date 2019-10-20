/**
 * Copyright (c) 2017 rxi
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `log.c` for details.
 */

#ifndef LOG_H
#define LOG_H

#include <stdarg.h>

#define LOG_VERSION "0.1.0"

typedef void (*log_LockFn)(void *udata, int lock);

enum
{
    KLOG_TRACE,
    KLOG_DEBUG,
    KLOG_INFO,
    KLOG_WARN,
    KLOG_ERROR,
    KLOG_FATAL
};

#define klog_trace(...) klog_log(KLOG_TRACE, __FILE__, __LINE__, __VA_ARGS__)
#define klog_dbg(...) klog_log(KLOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__)
#define klog_info(...) klog_log(KLOG_INFO, __FILE__, __LINE__, __VA_ARGS__)
#define klog_warn(...) klog_log(KLOG_WARN, __FILE__, __LINE__, __VA_ARGS__)
#define klog_err(...) klog_log(KLOG_ERROR, __FILE__, __LINE__, __VA_ARGS__)
#define klog_fatal(...) klog_log(KLOG_FATAL, __FILE__, __LINE__, __VA_ARGS__)

// void log_set_udata(void *udata);
// void log_set_lock(log_LockFn fn);
// void log_set_fp(FILE *fp);
void klog_set_level(int level);
void klog_set_quiet(int enable);

void klog_log(int level, const char *file, int line, const char *fmt, ...);

#endif
