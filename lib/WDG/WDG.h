#ifndef FA1C7C61_A031_441F_B6CF_C08A0B6470FA
#define FA1C7C61_A031_441F_B6CF_C08A0B6470FA

#include "esp_timer.h"

class ESPWatchdog {
public:
    ESPWatchdog(const char* name = "esp_wdg");
    void begin(uint64_t timeout_ms, void (*onTimeout)() = nullptr);
    void reset();
    void stop();
    ~ESPWatchdog();
private:
    static void timerCallback(void* arg);
    uint64_t timeout_us_;
    void (*onTimeout_)();
    esp_timer_handle_t timer_;
    const char* name_;
};

#endif /* FA1C7C61_A031_441F_B6CF_C08A0B6470FA */
