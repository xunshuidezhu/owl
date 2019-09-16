#ifndef LOG_H
#define LOG_H

#include "owl_log.h"

extern owllog::Log* g_p_default_Log;

#define debug_log(format, ...) g_p_default_Log->print_log(owllog::LOG_LELEVE_DEBUG, "%s:%d|%s <DEBUG> " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define info_log(format, ...) g_p_default_Log->print_log(owllog::LOG_LELEVE_INFO, "<INFO> " format, ##__VA_ARGS__)
#define warn_log(format, ...) g_p_default_Log->print_log(owllog::LOG_LELEVE_WARNING, "%s:%d|%s <WARN> " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define err_log(format, ...) g_p_default_Log->print_log(owllog::LOG_LELEVE_ERROR, "%s:%d|%s <ERR> " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define fatal_log(format, ...) g_p_default_Log->print_log(owllog::LOG_LELEVE_FATAL, "%s:%d|%s <FATAL> " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#define bt_log(format, ...) g_p_default_Log->backtrace_log(owllog::LOG_LELEVE_INFO, "%s:%d|%s <BT> " format, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)

#endif