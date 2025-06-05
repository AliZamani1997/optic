#ifndef __logger__
#define __logger__

#include "Arduino.h"

extern esp_log_level_t esp_log_local_level;

void logPrint( char * log,size_t len);
void logPrint( const String &s);
void logPrint(const char *format, ...);
void LoggerInit();


#define SHORT_LOG_FORMAT(level,TAG, format)  "%s[%s] : " format "\r\n" , level, TAG 
#define MEDIUM_LOG_FORMAT(level,TAG, format)  "[%6u]%s[%s] : " format "\r\n", (unsigned long) (esp_timer_get_time() / 1000ULL), level, TAG
#define LONG_LOG_FORMAT(level,TAG, format)  "[%6u]%s[%s:%u] %s(): " format "\r\n", (unsigned long) (esp_timer_get_time() / 1000ULL), level, __FILENAME__ , __LINE__, __FUNCTION__



#if  DEBUG_FORMAT_TYPE == 1
    #define FORMAT SHORT_LOG_FORMAT
#elif DEBUG_FORMAT_TYPE == 2
    #define FORMAT MEDIUM_LOG_FORMAT
#elif DEBUG_FORMAT_TYPE == 3
    #define FORMAT LONG_LOG_FORMAT
#else
    #define FORMAT SHORT_LOG_FORMAT
#endif



#ifndef MAXIMUM_DEBUG_LEVEL
    #define MAXIMUM_DEBUG_LEVEL CORE_DEBUG_LEVEL
#endif

#define LOG_LOCAL(level, format, ...)  do { \
    if (esp_log_local_level >= level) { \
        logPrint(format, ##__VA_ARGS__); \
    } \
} while(0)


#define LOG_NONE(format, ...)  LOG_LOCAL( ESP_LOG_NONE ,FORMAT("",TAG,format), ##__VA_ARGS__)

#if MAXIMUM_DEBUG_LEVEL >= ESP_LOG_ERROR
    #define LOG_ERROR(format, ...)  LOG_LOCAL( ESP_LOG_ERROR ,FORMAT("[E]",TAG,format), ##__VA_ARGS__)
#else
    #define LOG_ERROR(format, ...)  do{}while(0)
#endif


#if MAXIMUM_DEBUG_LEVEL >= ESP_LOG_WARN
    #define LOG_WARN(format, ...)   LOG_LOCAL( ESP_LOG_WARN ,FORMAT("[W]",TAG,format), ##__VA_ARGS__)
#else
    #define LOG_WARN(format, ...)  do{}while(0)
#endif


#if MAXIMUM_DEBUG_LEVEL >= ESP_LOG_INFO
    #define LOG_INFO(format, ...)   LOG_LOCAL( ESP_LOG_INFO ,FORMAT("[I]",TAG,format), ##__VA_ARGS__)
#else
    #define LOG_INFO(format, ...)  do{}while(0)
#endif


#if MAXIMUM_DEBUG_LEVEL >= ESP_LOG_DEBUG
    #define LOG_DEBUG(format, ...)  LOG_LOCAL( ESP_LOG_DEBUG ,FORMAT("[D]",TAG,format), ##__VA_ARGS__)
#else
    #define LOG_DEBUG(format, ...)  do{}while(0)
#endif


#if MAXIMUM_DEBUG_LEVEL >= ESP_LOG_VERBOSE
    #define LOG_VERBOSE(format, ...)  LOG_LOCAL( ESP_LOG_VERBOSE ,FORMAT("[V]",TAG,format), ##__VA_ARGS__)
#else
    #define LOG_VERBOSE(format, ...)  do{}while(0)
#endif

#endif /* __logger__ */
