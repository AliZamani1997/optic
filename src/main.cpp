#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <SD.h>
#include <WebServer.h>
#include <WebServerCustom.h>
#include <EEPROM.h>
#include "Logger.h"
#include "Config.h"
#include "freertos/task.h"





bool spiffsEnabled = false;
bool sdEnabled = false;
bool eepromEnabled = false;

// Make TAG static to avoid multiple definition linker errors
static const char * TAG = "Setup" ;


void setup() {

  Serial.flush();
  #ifdef SERIAL_LOG_UART0
    Serial.begin(115200);
    Serial.println(CORE_DEBUG_LEVEL);
  #else
    Serial.end();
  #endif

  #if CONFIG_ESP32_SPIRAM_SUPPORT

  #endif
  // LoggerInit();
  // esp_log_set_vprintf(_vprintf);
  // esp_log_level_set("*", (esp_log_level_t )CORE_DEBUG_LEVEL);
  // esp_log_write(ESP_log_e, "setup", "Starting setup...");
  // LoggerInit();

  #ifdef USE_SPIFFS
  // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
      LOG_ERROR("An error occurred while mounting SPIFFS");
      return;
    }
    spiffsEnabled=true;
  #endif


  #ifdef USE_SD
    // Initialize SD card
    if (!SD.begin()) {
      LOG_ERROR("An error occurred while initializing SD card");
      // return;
    }
    
    else{
      sdEnabled=true;
    }
  #endif

  // Initialize logger
  // LoggerInit();

  
  // Initialize configuration
  if (!CONFIG.begin())
  {
    LOG_ERROR("failed to initialise configuration");
  }


  // loadDefaultConfig();

  LOG_INFO("System initialized");
  LOG_INFO("Tasks start ");


  InitWebServer();

  // Create FreeRTOS tasks with adjusted stack sizes
  // xTaskCreatePinnedToCore(Task2Code, "Task2", 15000, NULL, 1, &Task2, 1); // Increased stack size
  // xTaskCreatePinnedToCore(ExportFiles, "ExportFiles", 15000, NULL, 2, &Task3, 1); // Increased stack size

}



void loop() {
  vTaskDelay(10000);
  // CONFIG.printAllConfigs(&Serial);
}
