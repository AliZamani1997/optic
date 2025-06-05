#ifndef _system_
#define _system_
#include "Logger.h"

void sys_error_critical(const char * tag , const char * filename ,const char * function ,uint16_t line) ;
String getActiveOtaLabel() ;

#define ERROR(format,...)  do { \
    LOG_ERROR(format, ##__VA_ARGS__); \
    sys_error_critical(TAG,__FILE__,__FUNCTION__,__LINE__); \
} while(0)
#endif /* _system_ */
