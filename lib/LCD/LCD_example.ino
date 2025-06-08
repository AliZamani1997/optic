// #include <Adafruit_SSD1306.h>
// #include "Keypad.h"
// #include "LCD.h"

// #define BTN_OK_PIN    26
// #define BTN_LEFT_PIN  27
// #define BTN_RIGHT_PIN 14

// #define LCD_MOSI_PIN    33
// #define LCD_SCLK_PIN    25
// #define LCD_DC_PIN      35
// #define LCD_RST_PIN     32
// #define LCD_CS_PIN      34

// KeypadPinConfig kpConfigs[] = {
//     {BTN_OK_PIN,    true, -1}, // 0
//     {BTN_LEFT_PIN,  true, -1}, // 1
//     {BTN_RIGHT_PIN, true, -1}, // 2
// };
// const uint8_t BTN_OK_IDX    = 0;
// const uint8_t BTN_LEFT_IDX  = 1;
// const uint8_t BTN_RIGHT_IDX = 2;
// Keypad keypad(kpConfigs, 3);


// Adafruit_SSD1306 display(LCD_WIDTH, LCD_HEIGHT, LCD_MOSI_PIN, LCD_SCLK_PIN, LCD_DC_PIN, LCD_RST_PIN, LCD_CS_PIN);

// IndustrialLcdUI ui(display, keypad);

// #define EXAMPLE_1


// #ifdef EXAMPLE_1
// int myInt = 5;
// bool myBool = false;

// MenuPage mainMenu("Main Menu", {
//     MenuItem("Start", [](){ /* start action */ }),
//     MenuItem("Settings", new MenuPage("Settings", {
//         MenuItem("Edit Value", &myInt, 0, 10),
//         MenuItem("Enable X", &myBool)
//     })),
//     MenuItem("Reboot", [](){ ESP.restart(); })
// });

// #endif // EXAMPLE_1


// #ifdef EXAMPLE_2

// // Global variables
// int someValue = 42;
// bool someFlag = false;

// // Submenu
// std::vector<MenuItem> settingsItems = {
//     MenuItem("Back", [](){ ui.popPage(); }),
//     MenuItem("Flag", &someFlag),
//     MenuItem("Value", &someValue, 0, 100)
// };
// MenuPage settingsMenu("Settings", settingsItems);

// // Root menu
// std::vector<MenuItem> mainItems = {
//     MenuItem("Settings", &settingsMenu),
//     MenuItem("Reset", [](){
//         ui.pushPage(new ConfirmationPopup("Reset all?", [](bool yes){
//             if (yes) ui.showPopup("Reset OK!");
//         }));
//     }),
//     MenuItem("About", [](){
//         ui.showPopup("V1.0 by OpenAI", 1500);
//     })
// };
// MenuPage mainMenu("Main Menu", mainItems);


// #endif // EXAMPLE_2

// void setup() {
//     Serial.begin(115200);
//     keypad.begin();
//     ui.begin();
//     ui.pushPage(&mainMenu);

//     // Optional: status bar for battery, WiFi, etc.
//     ui.setStatusBarProvider([](Adafruit_SSD1306& d){
//         d.setTextSize(1);
//         d.setTextColor(SSD1306_WHITE);
//         d.setCursor(80, 0);
//         d.print("WiFi"); // Example
//     });

//     ui.setSleepTimeout(30000); // 30s sleep
// }

// void loop() {
//     ui.loop();
//     delay(10);
// }