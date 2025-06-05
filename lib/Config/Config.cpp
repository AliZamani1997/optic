#include "Arduino.h"
#include <EEPROM.h>
#include "Config.h"
#include "Logger.h" 
#include <FS.h>
#include <SPIFFS.h>
#include <system.h>

// Make TAG static to avoid multiple definition linker errors
static const char* TAG = "Config";

/**
 * @brief ConfigLib handles configuration storage in EEPROM or RAM, with JSON serialization.
 *        It supports loading, saving, clearing, and accessing configuration values.
 */
ConfigLib::ConfigLib(size_t eepromSize) : eepromAvailable(false), eepromSize(eepromSize) {}

/**
 * @brief Initializes the configuration system. Tries to use EEPROM if available.
 * @return true if EEPROM is used, false if only RAM is used.
 */
bool ConfigLib::begin() {
#ifdef USE_EEPROM
    if(CONFIGLIB_TOTAL_PARAMETERS_SIZE > eepromSize)
    {
        ERROR("EEPROM size %u is too small for %u parameters", eepromSize, CONFIGLIB_TOTAL_PARAMETERS_SIZE);
        return false;
    }
    eepromAvailable = EEPROM.begin(eepromSize);
    if (!eepromAvailable) {
        ERROR("EEPROM not available, cannot use configuration.");
    }
    initParamters(); // Initialize parameters
    return eepromAvailable;
#elif USE_SPIFFS

#endif
}

bool ConfigLib::checkAvailable() {
    if (!eepromAvailable) {
        ERROR("EEPROM not available, cannot use configuration.");
    }
    return eepromAvailable ;
}
/**
 * @brief Saves configuration to EEPROM or RAM.
 * @return true if saved to EEPROM, false if to RAM.
 */
bool ConfigLib::save() {
    checkAvailable();
    if (!EEPROM.commit())
    {
        LOG_ERROR("Save failed !");
        return false;
    }
    
    return true;
}
config_t * ConfigLib::getValidParameter(config_key_t key) {
    config_t * param = getParameter(key);
    if (!param) {
        ERROR("Invalid config key %u", (uint16_t)key);
    }
    return param;
}

const char * ConfigLib::getKeyName(config_key_t key) {
    return getValidParameter(key)->Name;
}


bool ConfigLib::validKey(config_key_t key) {
    config_t * param = getParameter(key);
    if (!param) {
        return false;
    }
    return true;
}
/**
 * @brief Calculates CRC-16 (IBM/ANSI) for a buffer.
 * @param data Pointer to the data buffer.
 * @param length Number of bytes to calculate CRC for (excluding CRC bytes).
 * @return 16-bit CRC value.
 */
uint16_t ConfigLib::calcCRC(config_key_t key , const void* data, size_t length) {
    const uint8_t* buf = static_cast<const uint8_t*>(data);
    uint16_t crc = (uint16_t)key & (uint16_t)length<<8; // Use key as part of CRC seed
    for (size_t i = 0; i < length - CONFIGLIB_CRC_LENGTH; ++i) {
        crc ^= buf[i];
        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 1)
                crc = (crc >> 1) ^ 0xA001;
            else
                crc = (crc >> 1);
        }
    }
    return crc;
}

/**
 * @brief Checks CRC for a buffer.
 * @param data Pointer to the data buffer.
 * @param length Number of bytes in the buffer (including CRC bytes).
 * @return true if CRC matches, false otherwise.
 */
bool ConfigLib::checkCRC(config_key_t key , const void* data, size_t length) {
    if (length < CONFIGLIB_CRC_LENGTH) return false;
    uint16_t storedCrc = (static_cast<const uint8_t*>(data))[length - 2] |
                         (static_cast<const uint8_t*>(data))[length - 1] << 8;
    uint16_t calcCrc = calcCRC(key , data, length-CONFIGLIB_CRC_LENGTH);
    return storedCrc == calcCrc;
}

String ConfigLib::get(config_key_t key) {
    checkAvailable();
    config_t * parameter = getValidParameter(key);    

    char buf[CONFIGLIB_VALUE_MAX_LENGTH];
    EEPROM.readBytes(parameter->address, buf, parameter->size + CONFIGLIB_CRC_LENGTH);

    if (checkCRC(key , buf, parameter->size + CONFIGLIB_CRC_LENGTH))
    {
        return String(buf,strlen(buf));
    }
    LOG_WARN("%s has not been set : using default value ." ,parameter->Name);
    LOG_DEBUG("%s default value : %s" ,parameter->Name,parameter->Default);
    return String(parameter->Default);
}

bool ConfigLib::set(config_key_t key , const String& value) {
    checkAvailable();
    config_t * parameter = getValidParameter(key);    

    if (value.length() > parameter->size)
    {
        LOG_ERROR("'%s' for key %s is too large , must be less than %u character !",value.c_str(), parameter->Name, parameter->size);
        return false;
    }
    if(parameter->validation){
        if(parameter->validation(value.c_str())){
            LOG_ERROR("'%s' for key %s is invalid !",value.c_str(), parameter->Name);
            return false;
        }
    }

    char buf[CONFIGLIB_VALUE_MAX_LENGTH];
    memset(buf,0,CONFIGLIB_VALUE_MAX_LENGTH);
    configChangesLog(key, value);
    // Write value to buffer
    memcpy(buf, value.c_str(), value.length());
    uint16_t crc = calcCRC(key , buf, parameter->size);
    buf[parameter->size] = crc & 0xFF;
    buf[parameter->size + 1] = (crc >> 8) & 0xFF;

    EEPROM.writeBytes(parameter->address, buf, parameter->size+CONFIGLIB_CRC_LENGTH);
    return true;
}

/**
 * @brief Print all configuration key-value pairs to the specified UART stream.
 * @param uart Pointer to a Stream (e.g., &Serial).
 */
void ConfigLib::printAllConfigs(Stream* uart) {
    if (!uart) return;

    char buf[CONFIGLIB_VALUE_MAX_LENGTH];
    for (size_t i = 0; i < CONFIGLIB_PARAMETERS_COUNT; i++) {
        
        EEPROM.readBytes(Parameters[i].address, buf, Parameters[i].size + CONFIGLIB_CRC_LENGTH);

        if (checkCRC(static_cast<config_key_t>(Parameters[i].key) , buf, Parameters[i].size + CONFIGLIB_CRC_LENGTH))
        {        
            uart->printf("Config Key: %s, Value : %s  (%s)\n", Parameters[i].Name,String(buf, Parameters[i].size).c_str(),Parameters[i].Description);
        }
        else{
            uart->printf("Config Key: %s, Value(default): %s  (%s)\n", Parameters[i].Name,Parameters[i].Default,Parameters[i].Description);
        }
    }
    LOG_INFO("All configs printed to UART.");
}

/**
 * @brief Log configuration changes to SPIFFS and output info to logger.
 *        Only logs if the value actually changed.
 * @param key The configuration key being changed.
 * @param newValue The new value to set.
 */
void ConfigLib::configChangesLog(config_key_t key, const String& newValue) {
    const char* TAG = "Config";
    String oldValue = get(key);
    if (oldValue != newValue) {
        File logFile = CONFIGLIB_LOG_FILE(FILE_APPEND,true);
        if (logFile) {
            // Get current date and time string
            String dateTime = "1970-01-01 00:00:00"; // TODO: Replace with actual date/time function
            logFile.printf("Key: %s , Old: %s , New: %s , date time : %s\n", getKeyName(key), oldValue.c_str(), newValue.c_str(), dateTime.c_str());
            logFile.close();
        } else {
            LOG_WARN("Failed to open config_changes.log for appending");
        }
        LOG_INFO("Config key '%s' changed from '%s' to '%s'", getKeyName(key), oldValue.c_str(), newValue.c_str());
    } else {
        LOG_DEBUG("No change for config key '%s', value remains '%s'", getKeyName(key), oldValue.c_str());
    }
}

ConfigLib CONFIG;