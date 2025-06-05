#include "Arduino.h"
#include "esp_ota_ops.h"
#include "system.h"
#include "Logger.h"
static const char* TAG = "System";
/**
 * @brief Aborts the program and enters an infinite loop.
 * @param tag The message to log before aborting.
 */
void sys_error_critical(const char * tag , const char * filename ,const char * function ,uint16_t line) {
    LOG_ERROR("Critical error on '%s'", tag);
    LOG_ERROR("Critical error source : [ %s:%u -> %s ]", filename,line,function);
    while (true);
}

String getActiveOtaLabel() {
    const esp_partition_t* running = esp_ota_get_running_partition();
    if (running) return String(running->label);
    return String("unknown");
}