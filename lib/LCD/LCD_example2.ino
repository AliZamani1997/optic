// #include <Adafruit_SSD1306.h>
// #include <Wire.h>
// #include "LCD.h"

// // Dummy Keypad implementation for example
// #include "Keypad.h"

// // ----- Globals for editable variables -----
// int setPoint = 25;
// bool enableFeature = false;

// // ----- Forward declarations for menu pages -----
// MenuPage* mainMenu;
// MenuPage* settingsMenu;

// // ----- Example Action Function -----
// void actionHello() {
//     // Show a popup for 2 seconds
//     ui.showPopup("Hello, World!", 2000);
// }

// // ----- Status Bar Example -----
// void drawStatusBar(Adafruit_SSD1306& display) {
//     display.setTextSize(1);
//     display.setCursor(0, 54);
//     display.print("SP:");
//     display.print(setPoint);
//     display.print("  F:");
//     display.print(enableFeature ? "ON" : "OFF");
// }

// // ----- Keypad, Display, UI objects -----
// Adafruit_SSD1306 display(128, 64, &Wire, -1);
// Keypad keypad;
// IndustrialLcdUI ui(display, keypad);

// void setup() {
//     Wire.begin();
//     // Display + UI init
//     ui.begin();
//     ui.setStatusBarProvider(drawStatusBar);
//     ui.setSleepTimeout(30000); // 30s screen sleep

//     // ----------- Settings Submenu -----------
//     settingsMenu = new MenuPage("Settings", {
//         MenuItem("SetPoint", &setPoint, 10, 40),
//         MenuItem("Feature ON", &enableFeature),
//     });

//     // ----------- Main Menu -----------
//     mainMenu = new MenuPage("Main Menu", {
//         MenuItem("Say Hello", actionHello),
//         MenuItem("Settings...", settingsMenu),
//         MenuItem("Exit", []() { ui.showPopup("Bye!", 1000); }),
//     });

//     // Show main menu
//     ui.pushPage(mainMenu);
// }

// void loop() {
//     ui.loop();
// }