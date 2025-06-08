#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>
#include <EEPROM.h>
#include <FS.h>
#include <SD_MMC.h>
#include <SPIFFS.h>
#include "config_param.h"
// Maximum length for config values and CRC
// #define CONFIGLIB_VALUE_LENGTH 32
#define CONFIGLIB_CRC_LENGTH 2
#define CONFIGLIB_VALUE_MAX_LENGTH  (64 + CONFIGLIB_CRC_LENGTH)
#define CONFIGLIB_LOG_PATH "/config_changes.log"

// #define CONFIGLIB_LOG_FILESYSTEM SPIFFS

#define CONFIGLIB_LOG_FILESYSTEM SD_MMC





class ConfigLib {
public:
    // Constructor: specify EEPROM size
    ConfigLib(size_t eepromSize = 512);

    // Initialize configuration system (EEPROM or SPIFFS)
    bool begin();


    // Get value for a key (with/without default)
    String get(config_key_t key);

    // Set value for a key
    bool set(config_key_t key, const String& value);

    // Save all configuration to EEPROM
    bool save();

    // Print all configuration key-value pairs to a stream
    void printAllConfigs(Stream & uart) ;

    // Validate key
    bool validKey(config_key_t key);
    
    // Get the name of a configuration key
    const char* getKeyName(config_key_t key);

private:
    bool eepromAvailable;
    size_t eepromSize;
    SemaphoreHandle_t config_mutex_handle;


    // Check if configuration storage is available
    bool checkAvailable();
    
    // Get valid configuration parameter by key
    config_t * getValidParameter(config_key_t key) ;

    // CRC calculation and checking
    uint16_t calcCRC(config_key_t key, const void* data, size_t length);
    bool checkCRC(config_key_t key, const void* data, size_t length);

    // Log configuration changes
    void configChangesLog(config_key_t key, const String& newValue);

};

extern ConfigLib CONFIG;

#endif // CONFIG_H
