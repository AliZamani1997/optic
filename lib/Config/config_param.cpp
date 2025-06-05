#include "config_param.h"

config_t Parameters[CONFIGLIB_PARAMETERS_COUNT];

void initParamters(){
    uint16_t index=0;
	/*Firmware Version;*/
	Parameters[index].Name = "Firmware Version" ;
	Parameters[index].Default = "1.0.2" ;
	Parameters[index].key = (uint8_t)FirmwareVersion_CK ;
	Parameters[index].writeLockLevel = (uint8_t)LOCK_LEVEL_SYSTEM ;
	Parameters[index].readLockLevel = (uint8_t)LOCK_LEVEL_SYSTEM ;
	Parameters[index].size = 8 ;
	Parameters[index].address = 0x0000 ;
	index++ ;

	/*Device Id;*/
	Parameters[index].Name = "Device Id" ;
	Parameters[index].Default = "100001" ;
	Parameters[index].key = (uint8_t)DeviceId_CK ;
	Parameters[index].writeLockLevel = (uint8_t)LOCK_LEVEL_SYSTEM ;
	Parameters[index].readLockLevel = (uint8_t)LOCK_LEVEL_SYSTEM ;
	Parameters[index].address = 0x000A ;
	index++ ;

	/*Log Level;*/
	Parameters[index].Name = "Log Level" ;
	Parameters[index].Default = "3" ;
	Parameters[index].key = (uint8_t)LogLevel_CK ;
	Parameters[index].size = 2 ;
	Parameters[index].address = 0x00C0 ;
	index++ ;

	/*User Name;*/
	Parameters[index].Name = "User Name" ;
	Parameters[index].Default = "user" ;
	Parameters[index].key = (uint8_t)UserName_CK ;
	Parameters[index].address = 0x0124 ;
	index++ ;

	/*User Pass;*/
	Parameters[index].Name = "User Pass" ;
	Parameters[index].Default = "user" ;
	Parameters[index].key = (uint8_t)UserPass_CK ;
	Parameters[index].address = 0x0136 ;
	index++ ;

	/*Server Wdg Timeout;*/
	Parameters[index].Name = "Server Wdg Timeout" ;
	Parameters[index].Default = "12000" ;
	Parameters[index].key = (uint8_t)ServerWdgTimeout_CK ;
	Parameters[index].size = 8 ;
	Parameters[index].address = 0x0148 ;
	index++ ;


}

config_t * getParameter(uint8_t key){
    for(uint8_t i=0; i<CONFIGLIB_PARAMETERS_COUNT; i++){
        if(Parameters[i].key == key){
            return &Parameters[i];
        }
    }
    return nullptr;
}

