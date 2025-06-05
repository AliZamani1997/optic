#ifndef config_param
#define config_param
#include <Arduino.h>

#define CONFIGLIB_PARAMETERS_DEFAULT_SIZE 16
#define CONFIGLIB_PARAMETERS_COUNT 10
#define CONFIGLIB_TOTAL_PARAMETERS_SIZE 0x0152

typedef enum {
	FirmwareVersion_CK=0, //lib:main , type:String
	DeviceId_CK=1, //lib:main , type:String
	LogLevel_CK=10, //lib:main , type:uint8_t
	UserName_CK=22, //lib:Server , type:String
	UserPass_CK=23, //lib:Server , type:String
	ServerWdgTimeout_CK=24, //lib:Server , type:uint32_t

    MAX_CK
} config_key_t;

typedef enum {
    LOCK_LEVEL_NONE = 0, // No lock, key can be modified freely
    LOCK_LEVEL_USER, // Key can be modified by user-level access
    LOCK_LEVEL_ADMIN, // Key can only be modified by admin-level access
    LOCK_LEVEL_SYSTEM, // Key can only be modified by system-level access
    LOCK_LEVEL_MAX // Maximum lock level, no modifications allowed
} config_lock_level_t;

typedef struct {
    uint8_t key;
    uint8_t writeLockLevel = (uint8_t)LOCK_LEVEL_USER; // Lock level for the key, can be used to restrict access or modification
    uint8_t readLockLevel = (uint8_t)LOCK_LEVEL_NONE; // Lock level for reading the key
    uint8_t size = CONFIGLIB_PARAMETERS_DEFAULT_SIZE; // Lock level for the key, can be used to restrict access or modification
    uint16_t address; // Address in EEPROM or SPIFFS where the value is stored
    const char* Name;
    const char* Description = "";
    const char* Default = "";
    bool (*validation)(const char* value)= nullptr; // Pointer to a validation function, can be nullptr if no validation is needed
} config_t;

void initParamters();

config_t * getParameter(uint8_t key);

extern config_t Parameters[CONFIGLIB_PARAMETERS_COUNT];

#endif /* config_param */
