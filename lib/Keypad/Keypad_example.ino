// #include "Keypad.h"

// KeypadPinConfig myPins[] = {
//     {15, true, -1}, // OK
//     {2, true, -1},  // Left
//     {0, true, -1},  // Right
//     {4, true, -1},  // Top
//     {16, true, -1}  // Bottom
// };

// Keypad keypad(myPins, 5);

// void setup() {
//     Serial.begin(115200);
//     keypad.begin();

//     // Register callbacks:
//     keypad.on(0, KEYPAD_EVENT_PRESS, [](const KeypadEvent& e){
//         Serial.println("OK Pressed!");
//     });
//     keypad.on(1, KEYPAD_EVENT_USER_0, [](const KeypadEvent& e){
//         Serial.println("Custom event for Left!");
//     });

//     keypad.onAny([](const KeypadEvent& e){
//         Serial.printf("Event: Btn %d, Type %d\n", e.buttonIdx, e.type);
//     });

//     #ifdef KEYPAD_ON_FREERTOS
//     // If using FreeRTOS, you can create a task to handle keypad events
//     xTaskCreate([](void*){
//         while (true) {
//             keypad.poll();
//             keypad.processEvents();
//             vTaskDelay(pdMS_TO_TICKS(10));
//         }
//     }, "KeypadTask", 2048, NULL, 2, NULL);
//     #endif
// }

// void loop() {
//     #ifndef KEYPAD_ON_FREERTOS
//     keypad.poll();
//     keypad.processEvents();

//     // Example: trigger custom event for Left button when a condition is met
//     if (/* some condition */ false) {
//         keypad.triggerCustomEvent(1, KEYPAD_EVENT_USER_0);
//     }

//     // Example: dynamically disable Top button
//     if (/* some condition */ false) {
//         keypad.disableButton(3);
//     }
//     #endif

//     delay(10);
// }


