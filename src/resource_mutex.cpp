// #include "Arduino.h"
// #include "task_Priority.h"


// SemaphoreHandle_t serial_mutex_handle = nullptr;

// void SerialPrintTask(void * buf) {
//     if (buf == nullptr) {
//         vTaskDelete(NULL); // Delete the task if no buffer is provided
//         return;
//     }
//     if (xSemaphoreTake(serial_mutex_handle, 10000 / portTICK_PERIOD_MS) == pdTRUE) {
//         Serial.print((const char*)buf);
//         Serial.flush(); // Ensure all data is sent before releasing the mutex
//         xSemaphoreGive(serial_mutex_handle);
//     } 
//     free(buf); // Free the allocated memory after use
//     vTaskDelete(NULL); // Delete the task after completion
// }
// void SerialPrint(const char* str) {
//     if (serial_mutex_handle== nullptr) 
//         serial_mutex_handle = xSemaphoreCreateMutex();

//     void * buf =malloc(strlen(str) + 1);
//     if (buf == nullptr) {
//         return;
//     }
//     strcpy((char*)buf, str);
//     xTaskCreate(SerialPrintTask, "print", 2048, buf, Low_priority, NULL);
// }




// #include "Arduino.h"
// #include "task_Priority.h"
// #include "freertos/FreeRTOS.h"
// #include "freertos/queue.h"

// #define SERIAL_QUEUE_LENGTH 50
// #define SERIAL_MSG_MAX_LEN 64

// SemaphoreHandle_t serial_mutex_handle = nullptr;
// QueueHandle_t serial_queue = nullptr;

// // The print task: waits for messages and prints them
// void SerialPrintTask(void *pvParameters) {
//     char msg[SERIAL_MSG_MAX_LEN];
//     while (1) {
//         if (xQueueReceive(serial_queue, &msg, portMAX_DELAY) == pdTRUE) {
//             if (serial_mutex_handle && xSemaphoreTake(serial_mutex_handle, portMAX_DELAY) == pdTRUE) {
//                 Serial.print(msg);
//                 Serial.flush();
//                 xSemaphoreGive(serial_mutex_handle);
//             }
//         }
//     }
// }

// // Call this in setup() to initialize
// void SerialPrintInit() {
//     serial_mutex_handle = xSemaphoreCreateMutex();
//     serial_queue = xQueueCreate(SERIAL_QUEUE_LENGTH, SERIAL_MSG_MAX_LEN);
//     xTaskCreate(SerialPrintTask, "SerialPrintTask", 2048, NULL, Low_priority, NULL);
// }

// // Use this function to print
// void SerialPrint(const char* str) {
//     if (!serial_queue || !serial_mutex_handle) 
//         SerialPrintInit();
//     char msg[SERIAL_MSG_MAX_LEN];
//     size_t len = strlen(str);
//     size_t max_data_len = SERIAL_MSG_MAX_LEN - 1; // Reserve space for null terminator
//     for (size_t i = 0; i <= len/max_data_len; i++)
//     {
//         size_t start = i * max_data_len;
//         size_t to_copy = (i == len / max_data_len) ? (len % max_data_len) : max_data_len;
//         memcpy(msg, str + start, to_copy);
//         msg[to_copy] = '\0';
//         if(!xQueueSend(serial_queue, &msg, 50))
//             break;
//     }
// }


#include "Arduino.h"

SemaphoreHandle_t serial_mutex_handle = nullptr;
SemaphoreHandle_t sd_mutex_handle = nullptr;
SemaphoreHandle_t spiffs_mutex_handle = nullptr;
SemaphoreHandle_t config_mutex_handle = nullptr;

void init_resource_mutex(){
    serial_mutex_handle = xSemaphoreCreateMutex();
    sd_mutex_handle = xSemaphoreCreateMutex();
    spiffs_mutex_handle = xSemaphoreCreateMutex();
    config_mutex_handle = xSemaphoreCreateMutex();
}