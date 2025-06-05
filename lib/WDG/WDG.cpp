#include "WDG.h"
#include "esp_timer.h"
#include "logger.h"

// Constructor
ESPWatchdog::ESPWatchdog(const char* name)
    : timeout_us_(0), onTimeout_(nullptr), timer_(nullptr), name_(name) {}

void ESPWatchdog::begin(uint64_t timeout_ms, void (*onTimeout)()) {
    timeout_us_ = timeout_ms * 1000; // Convert ms to us
    onTimeout_ = onTimeout;
    if (timer_) {
        esp_timer_stop(timer_);
        esp_timer_delete(timer_);
    }
    esp_timer_create_args_t timer_args = {};
    timer_args.callback = &ESPWatchdog::timerCallback;
    timer_args.arg = this;
    timer_args.name = name_;
    esp_timer_create(&timer_args, &timer_);
    esp_timer_start_once(timer_, timeout_us_);
}

void ESPWatchdog::reset() {
    if (timer_) {
        esp_timer_stop(timer_);
        esp_timer_start_once(timer_, timeout_us_);
    }
}

void ESPWatchdog::stop() {
    if (timer_) {
        esp_timer_stop(timer_);
    }
}

ESPWatchdog::~ESPWatchdog() {
    if (timer_) {
        esp_timer_stop(timer_);
        esp_timer_delete(timer_);
    }
}

void ESPWatchdog::timerCallback(void* arg) {
    ESPWatchdog* self = static_cast<ESPWatchdog*>(arg);
    
    String _tag("WDG.");
    _tag += self->name_;

    const char * TAG = _tag.c_str();

    LOG_DEBUG("[WDG.%s] timerCallback triggered", self->name_);
    if (self->onTimeout_) {
        LOG_WARN("[WDG.%s] Timeout callback called", self->name_);
        self->onTimeout_();
    } else {
        LOG_ERROR("[WDG.%s] PANIC: Watchdog timeout and no callback set!", self->name_);
        abort();
    }
}