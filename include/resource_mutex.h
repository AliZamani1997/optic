#ifndef DFB3D063_1A73_4378_B381_989CC4273C41
#define DFB3D063_1A73_4378_B381_989CC4273C41
#include "Arduino.h"

extern SemaphoreHandle_t serial_mutex_handle ;
extern SemaphoreHandle_t sd_mutex_handle ;
extern SemaphoreHandle_t spiffs_mutex_handle ;

void init_resource_mutex();
#endif /* DFB3D063_1A73_4378_B381_989CC4273C41 */
