#include <Arduino.h>
#include <SPIFFS.h>
#include <SD_MMC.h>
#include <logger.h>
#include "resource_mutex.h"
#ifndef LOG_FILE_PATH 
#define LOG_FILE_PATH "/log.txt"
#endif


extern bool spiffsEnabled;
extern bool sdEnabled;

bool spiffsLogEnabled=false;
bool sdLogEnabled=false;

esp_log_level_t esp_log_local_level = (esp_log_level_t)MAXIMUM_DEBUG_LEVEL ;


#ifndef USE_SPIFFS
#undef FILE_LOG_SPIFFS
#endif

#ifndef USE_SD
#undef FILE_LOG_SD
#endif

void logPrint( char * log,size_t len){
    if(len == 0 || log == NULL){
        return;
    }
    #ifdef FILE_LOG_SPIFFS
        if (spiffsLogEnabled )
        {
            if (xSemaphoreTake(spiffs_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                File file = SPIFFS.open(LOG_FILE_PATH, FILE_APPEND, true);
                if(file){
                    file.write((const uint8_t*)log,len);
                    file.flush();
                    file.close();
                }
                xSemaphoreGive(spiffs_mutex_handle);
            }
        }
    #endif    

    #ifdef FILE_LOG_SD
        if (sdLogEnabled)
        {
            if (xSemaphoreTake(sd_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                File file = SD_MMC.open(LOG_FILE_PATH, FILE_APPEND, true);
                if(file){
                    file.write((const uint8_t*)log,len);
                    file.flush();
                    file.close();
                }
                xSemaphoreGive(sd_mutex_handle);
            }
        }
    #endif


    #ifdef SERIAL_LOG_UART0
        {
            if (xSemaphoreTake(serial_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                Serial.print(log);
                Serial.flush();
                xSemaphoreGive(serial_mutex_handle);
            }
        }
    #endif
}
void logPrint( const String &s){
    logPrint((char*)s.c_str(),s.length());
} 
void logPrint(const char *format, ...)
{
    int len;
    va_list arg;
    va_start(arg, format);
    static char loc_buf[256];
    char * temp = loc_buf;
    va_list copy;
    va_copy(copy, arg);
    len = vsnprintf(NULL, 0, format, copy);
    va_end(copy);
    if(len >= sizeof(loc_buf)){
        temp = (char*)malloc(len+1);
        if(temp == NULL) {
            return ;
        }
    }
    vsnprintf(temp, len+1, format, arg);
    logPrint(temp,len);

    if(len >= sizeof(loc_buf)){
        free(temp);
    }

    va_end(arg);
}




void ets_print_char(char c) {
  static char loc_buf[256];
  static int index = 0;
  loc_buf[index++] = c;
  if (c == '\n' || index >= (sizeof(loc_buf) - 1)) {
    loc_buf[index] = '\0'; // Null-terminate the string

    // logPrint((char*)"[Ard]", 5 );
    logPrint(loc_buf, index);
    index = 0; // Reset index for next line
  }
}


void LoggerInit(){

    #ifdef FILE_LOG_SPIFFS
        if (spiffsEnabled)
        {
            if (xSemaphoreTake(spiffs_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                File file = SD_MMC.open(LOG_FILE_PATH, FILE_WRITE, true);
                if(file){
                    file.print("Logger started\n");
                    file.flush();
                    file.close();
                    spiffsLogEnabled=true;
                }
                
                #ifdef SERIAL_LOG_UART0
                else{
                    if (xSemaphoreTake(serial_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                        Serial.println("Logger file couldn't started\n");
                        xSemaphoreGive(serial_mutex_handle);
                    }
                }
                #endif
                xSemaphoreGive(spiffs_mutex_handle);
            }
        }    
    #endif

    #ifdef FILE_LOG_SD
        if (sdEnabled)
        {
            if (xSemaphoreTake(sd_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
                File file = SD_MMC.open(LOG_FILE_PATH, FILE_APPEND, true);
                if(file){
                    file.print("Logger started\n");
                    file.flush();
                    file.close();
                    sdLogEnabled=true;
                }
                xSemaphoreGive(sd_mutex_handle);
            }
        }
    #endif

    #ifdef SERIAL_LOG_UART0
    if(SERIAL_LOG_UART0){
        if (xSemaphoreTake(serial_mutex_handle, 2000 / portTICK_PERIOD_MS) == pdTRUE) {
            Serial.flush();
            Serial.begin(115200);
            xSemaphoreGive(serial_mutex_handle);
        }
    }
    #endif

    ets_install_putc1(&ets_print_char);

}