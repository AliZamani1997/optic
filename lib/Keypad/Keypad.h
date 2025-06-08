#ifndef KEYPAD_H
#define KEYPAD_H

#include <Arduino.h>
#include <functional>
#include <queue>
#include <vector>

#define KEYPAD_MAX_BUTTONS 10
#define KEYPAD_MAX_EVENT_TYPES 16
#define KEYPAD_EVENT_QUEUE_SIZE 10

enum KeypadEventType {
    KEYPAD_EVENT_PRESS = 0,
    KEYPAD_EVENT_DOUBLE_PRESS,
    KEYPAD_EVENT_LONG_PRESS,
    KEYPAD_EVENT_VERY_LONG_PRESS,
    KEYPAD_EVENT_REPEAT,
    // User types: KEYPAD_EVENT_USER_0 ... KEYPAD_EVENT_USER_9
    KEYPAD_EVENT_USER_0 = 5,
    KEYPAD_EVENT_USER_1,
    KEYPAD_EVENT_USER_2,
    KEYPAD_EVENT_USER_3,
    KEYPAD_EVENT_USER_4,
    KEYPAD_EVENT_USER_5,
    KEYPAD_EVENT_USER_6,
    KEYPAD_EVENT_USER_7,
    KEYPAD_EVENT_USER_8,
    KEYPAD_EVENT_USER_9
};

struct KeypadPinConfig {
    uint8_t pin;
    bool activeLow; // true: pressed = LOW
    int ledPin;     // -1 if not used
};

struct KeypadEvent {
    uint8_t buttonIdx;
    KeypadEventType type;
};

typedef std::function<void(const KeypadEvent&)> KeypadCallback;

class Keypad {
public:
    Keypad(const KeypadPinConfig* configs, uint8_t buttonCount);
    void begin();
    void poll();
    void processEvents();

    // Registration: per-button, per-event, or global
    void on(uint8_t buttonIdx, KeypadEventType evt, KeypadCallback cb);
    void onAny(KeypadCallback cb);
    void clearAllCallbacks();

    // Config
    void setDebounceMs(uint16_t ms);
    void setLongPressMs(uint16_t ms);
    void setVeryLongPressMs(uint16_t ms);
    void setDoubleTapMs(uint16_t ms);
    void setRepeatMs(uint16_t ms);

    // Custom event triggers (for user press types)
    void triggerCustomEvent(uint8_t buttonIdx, KeypadEventType customType);

    // Enable/disable buttons at runtime
    void enableButton(uint8_t buttonIdx);
    void disableButton(uint8_t buttonIdx);
    bool isButtonEnabled(uint8_t buttonIdx) const;

protected:
    void fireEvent(const KeypadEvent& evt);

private:
    struct ButtonState {
        bool lastPhysical;
        bool debounced;
        uint32_t lastDebounceTime;
        uint32_t pressStartTime;
        bool waitingDoubleTap;
        uint32_t lastReleaseTime;
        bool repeating;
        uint32_t lastRepeatTime;
        bool enabled;
    };

    KeypadPinConfig _configs[KEYPAD_MAX_BUTTONS];
    uint8_t _buttonCount;

    // Timing
    uint16_t _debounceMs;
    uint16_t _longPressMs;
    uint16_t _veryLongPressMs;
    uint16_t _doubleTapMs;
    uint16_t _repeatMs;

    // Button state
    ButtonState _states[KEYPAD_MAX_BUTTONS];

    // Event queue
    std::queue<KeypadEvent> _eventQueue;

    // Callbacks
    KeypadCallback _callbacks[KEYPAD_MAX_BUTTONS][KEYPAD_MAX_EVENT_TYPES]; // per button, per event
    KeypadCallback _anyCallback;

    // Internal
    void handleButton(uint8_t idx, uint32_t now);

    // LED feedback
    void setLed(uint8_t idx, bool on);
};

#endif