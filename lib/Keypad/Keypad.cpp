#include "Keypad.h"

Keypad::Keypad(const KeypadPinConfig* configs, uint8_t buttonCount)
    : _buttonCount(buttonCount),
      _debounceMs(30),
      _longPressMs(500),
      _veryLongPressMs(1500),
      _doubleTapMs(250),
      _repeatMs(300),
      _anyCallback(nullptr)
{
    for (uint8_t i = 0; i < _buttonCount; ++i) {
        _configs[i] = configs[i];
        _states[i] = {false, false, 0, 0, false, 0, false, 0, true};
        for (int e = 0; e < KEYPAD_MAX_EVENT_TYPES; ++e)
            _callbacks[i][e] = nullptr;
    }
}

void Keypad::begin() {
    for (uint8_t i = 0; i < _buttonCount; ++i) {
        pinMode(_configs[i].pin, _configs[i].activeLow ? INPUT_PULLUP : INPUT);
        if (_configs[i].ledPin >= 0) {
            pinMode(_configs[i].ledPin, OUTPUT);
            digitalWrite(_configs[i].ledPin, LOW);
        }
    }
}

void Keypad::poll() {
    uint32_t now = millis();
    for (uint8_t i = 0; i < _buttonCount; ++i) {
        if (!_states[i].enabled) continue;
        handleButton(i, now);
    }
}

void Keypad::handleButton(uint8_t idx, uint32_t now) {
    ButtonState& state = _states[idx];
    bool pressed = digitalRead(_configs[idx].pin);
    if (_configs[idx].activeLow) pressed = !pressed;

    // Debounce
    if (pressed != state.lastPhysical) {
        state.lastDebounceTime = now;
        state.lastPhysical = pressed;
    }
    if ((now - state.lastDebounceTime) > _debounceMs) {
        if (pressed != state.debounced) {
            state.debounced = pressed;
            if (pressed) {
                // Button down
                state.pressStartTime = now;
                state.repeating = false;
                setLed(idx, true);
            } else {
                // Button up
                setLed(idx, false);
                uint32_t held = now - state.pressStartTime;
                if (held >= _veryLongPressMs) {
                    fireEvent({idx, KEYPAD_EVENT_VERY_LONG_PRESS});
                } else if (held >= _longPressMs) {
                    fireEvent({idx, KEYPAD_EVENT_LONG_PRESS});
                } else {
                    if (state.waitingDoubleTap && (now - state.lastReleaseTime) < _doubleTapMs) {
                        fireEvent({idx, KEYPAD_EVENT_DOUBLE_PRESS});
                        state.waitingDoubleTap = false;
                    } else {
                        state.waitingDoubleTap = true;
                        state.lastReleaseTime = now;
                    }
                }
            }
        }
        // Handle repeat
        if (state.debounced && !state.repeating && (now - state.pressStartTime) > _repeatMs) {
            fireEvent({idx, KEYPAD_EVENT_REPEAT});
            state.lastRepeatTime = now;
            state.repeating = true;
        } else if (state.debounced && state.repeating && (now - state.lastRepeatTime) > _repeatMs) {
            fireEvent({idx, KEYPAD_EVENT_REPEAT});
            state.lastRepeatTime = now;
        }
    }
    // Single press if no double tap in time
    if (state.waitingDoubleTap && !state.debounced && (now - state.lastReleaseTime) > _doubleTapMs) {
        fireEvent({idx, KEYPAD_EVENT_PRESS});
        state.waitingDoubleTap = false;
    }
}

void Keypad::processEvents() {
    while (!_eventQueue.empty()) {
        KeypadEvent evt = _eventQueue.front();
        _eventQueue.pop();
        if (evt.buttonIdx < _buttonCount && evt.type < KEYPAD_MAX_EVENT_TYPES) {
            if (_callbacks[evt.buttonIdx][evt.type])
                _callbacks[evt.buttonIdx][evt.type](evt);
        }
        if (_anyCallback)
            _anyCallback(evt);
    }
}

void Keypad::on(uint8_t buttonIdx, KeypadEventType evt, KeypadCallback cb) {
    if (buttonIdx < _buttonCount && evt < KEYPAD_MAX_EVENT_TYPES)
        _callbacks[buttonIdx][evt] = cb;
}

void Keypad::onAny(KeypadCallback cb) {
    _anyCallback = cb;
}

void Keypad::clearAllCallbacks() {
    for (uint8_t i = 0; i < _buttonCount; ++i)
        for (uint8_t e = 0; e < KEYPAD_MAX_EVENT_TYPES; ++e)
            _callbacks[i][e] = nullptr;
    _anyCallback = nullptr;
}

void Keypad::fireEvent(const KeypadEvent& evt) {
    if (_eventQueue.size() < KEYPAD_EVENT_QUEUE_SIZE)
        _eventQueue.push(evt);
}

void Keypad::setDebounceMs(uint16_t ms) { _debounceMs = ms; }
void Keypad::setLongPressMs(uint16_t ms) { _longPressMs = ms; }
void Keypad::setVeryLongPressMs(uint16_t ms) { _veryLongPressMs = ms; }
void Keypad::setDoubleTapMs(uint16_t ms) { _doubleTapMs = ms; }
void Keypad::setRepeatMs(uint16_t ms) { _repeatMs = ms; }

// Custom event trigger
void Keypad::triggerCustomEvent(uint8_t buttonIdx, KeypadEventType customType) {
    if (buttonIdx < _buttonCount && customType >= KEYPAD_EVENT_USER_0 && customType < KEYPAD_MAX_EVENT_TYPES) {
        fireEvent({buttonIdx, customType});
    }
}

// Enable/disable buttons at runtime
void Keypad::enableButton(uint8_t buttonIdx) {
    if (buttonIdx < _buttonCount) _states[buttonIdx].enabled = true;
}
void Keypad::disableButton(uint8_t buttonIdx) {
    if (buttonIdx < _buttonCount) _states[buttonIdx].enabled = false;
}
bool Keypad::isButtonEnabled(uint8_t buttonIdx) const {
    if (buttonIdx < _buttonCount) return _states[buttonIdx].enabled;
    return false;
}

void Keypad::setLed(uint8_t idx, bool on) {
    if (_configs[idx].ledPin >= 0)
        digitalWrite(_configs[idx].ledPin, on ? HIGH : LOW);
}