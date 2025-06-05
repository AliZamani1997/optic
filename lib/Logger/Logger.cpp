#include <Arduino.h>
#include <SPIFFS.h>
#include <SD.h>
// #include <FileSystem.h>
#include <logger.h>

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
        if (spiffsLogEnabled)
        {
            File file = SPIFFS.open(LOG_FILE_PATH, FILE_APPEND, true);
            if(file){
                file.write((const uint8_t*)log,len);
                file.flush();
                file.close();
            }
        }
    #endif    

    #ifdef FILE_LOG_SD
        if (sdLogEnabled)
        {
            File file = SD.open(LOG_FILE_PATH, FILE_APPEND, true);

            if(file){
                file.write((const uint8_t*)log,len);
                file.flush();
                file.close();
            }
        }
    #endif


    #ifdef SERIAL_LOG_UART0
        Serial.print(log);
        Serial.flush();
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
            File file = SPIFFS.open(LOG_FILE_PATH, FILE_APPEND, true);
            if(file){
                spiffsLogEnabled=true;
                file.close();
            }
        }    
    #endif

    #ifdef FILE_LOG_SD
        if (sdEnabled)
        {
            File file = SD.open(LOG_FILE_PATH, FILE_WRITE, true);
            if(file){
                sdLogEnabled=true;
                file.close();
            }
        }
    #endif

    #ifdef SERIAL_LOG_UART0
        Serial.flush();
        Serial.begin(115200);
    #endif

    ets_install_putc1(&ets_print_char);

}