// #include "Arduino.h"
// #include "WDG.h"
// #include "logger.h"

// static const char * TAG = "WDG_Example";

// void onWDTTimeout() {
//     LOG_ERROR("[WDT] Watchdog timeout! System should reset or handle error.");
//     // Optionally: esp_restart();
// }

// extern "C" void app_main() {
//     ESPWatchdog wdg("web_server_wdg"); // Named watchdog
//     wdg.begin(3000, onWDTTimeout);   // 3 seconds timeout
//     for (int i = 0; i < 10; ++i) {
//         LOG_INFO("Loop %d: Resetting watchdog...", i);
//         wdg.reset();
//         vTaskDelay(1000 / portTICK_PERIOD_MS); // Simulate work (1s)
//     }
//     wdg.stop();
//     LOG_INFO("Watchdog stopped.");
// }
