// #include <Arduino.h>
// #include <WiFi.h>
// #include <SPIFFS.h>
// #include <SD_MMC.h>
// #include <WebServer.h>
// #include <WebServerCustom.h>
// #include <EEPROM.h>
// #include "Logger.h"
// #include "Config.h"
// #include "freertos/task.h"
// #include "resource_mutex.h"
// #include "time.h"




// bool spiffsEnabled = false;
// bool sdEnabled = false;
// bool eepromEnabled = false;

// // Make TAG static to avoid multiple definition linker errors
// static const char * TAG = "Setup" ;


// void setup() {

//   init_resource_mutex();

//   Serial.flush();
//   #ifdef SERIAL_LOG_UART0
//     Serial.setDebugOutput(true);
//     Serial.begin(115200);
//   #else
//     Serial.setDebugOutput(false);
//     Serial.end();
//   #endif

//   #if CONFIG_ESP32_SPIRAM_SUPPORT

//   #endif
//   // LoggerInit();
//   // esp_log_set_vprintf(_vprintf);
//   // esp_log_level_set("*", (esp_log_level_t )CORE_DEBUG_LEVEL);
//   // esp_log_write(ESP_log_e, "setup", "Starting setup...");
//   // LoggerInit();

//   #ifdef USE_SPIFFS
//   // Initialize SPIFFS
//     if (!SPIFFS.begin(true)) {
//       LOG_ERROR("An error occurred while mounting SPIFFS");
//       return;
//     }
//     spiffsEnabled=true;
//   #endif


//   #ifdef USE_SD
    
//     // Start Micro SD card
//     if(SD_MMC.begin()){
//       uint8_t cardType = SD_MMC.cardType();
//       if(cardType != CARD_NONE){
//         sdEnabled = true;
//         LOG_INFO("SD Card initialized");
//       } else {
//         LOG_ERROR("No SD Card attached");
//       }
//     } else {
//       LOG_ERROR("SD Card Mount Failed");
//     }
//     // sdCardOk can be used for further logic if needed

//   #endif

//   // Initialize logger
//   LoggerInit();

  
//   // Initialize configuration
//   if (!CONFIG.begin())
//   {
//     LOG_ERROR("failed to initialise configuration");
//   }


//   // loadDefaultConfig();

//   LOG_INFO("System initialized");
//   LOG_INFO("Tasks start ");


//   InitWebServer();

//   // Create FreeRTOS tasks with adjusted stack sizes
//   // xTaskCreatePinnedToCore(Task2Code, "Task2", 15000, NULL, 1, &Task2, 1); // Increased stack size
//   // xTaskCreatePinnedToCore(ExportFiles, "ExportFiles", 15000, NULL, 2, &Task3, 1); // Increased stack size

// }


// static void exportFS(FS &fs , Stream &stream) {
//     File root = fs.open("/");
    
//     stream.println("export FS:");
//     if (!root || !root.isDirectory()) {
//         stream.println("Failed to open root directory");
//         return;
//     }

//     File file = root.openNextFile();
//     while (file) {
//         stream.print("File: ");
//         stream.println(file.name());

//         while (file.available()) {
//             stream.write(file.read());
//         }
//         stream.println();

//         file = root.openNextFile();
//     }
// }

// void loop() {
//   vTaskDelay(20000);
//   if (serial_mutex_handle && xSemaphoreTake(serial_mutex_handle, 1000) == pdTRUE) {
    
//     CONFIG.printAllConfigs(Serial);

//     if(spiffsEnabled && spiffs_mutex_handle && xSemaphoreTake(spiffs_mutex_handle, 1000) == pdTRUE){
//       exportFS(SPIFFS ,Serial);
//       xSemaphoreGive(spiffs_mutex_handle);
//     }

//     if(sdEnabled && sd_mutex_handle && xSemaphoreTake(sd_mutex_handle, 1000) == pdTRUE){
//       exportFS(SD_MMC ,Serial);
//       xSemaphoreGive(sd_mutex_handle);
//     }

//     Serial.flush();
//     xSemaphoreGive(serial_mutex_handle);
//   }
// }












// #include "Keypad.h"

// #include <Adafruit_GFX.h>
// #include <Adafruit_SSD1306.h>

// #define SCREEN_WIDTH 128 // OLED display width, in pixels
// #define SCREEN_HEIGHT 64 // OLED display height, in pixels

// // Declaration for SSD1306 display connected using software SPI (default case):
// #define OLED_MOSI   26
// #define OLED_CLK   27
// #define OLED_DC    33
// #define OLED_CS    32
// #define OLED_RESET 25

// Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
//   OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


// KeypadPinConfig myPins[] = {
//     {13, true, -1}, // OK
//     {12, true, -1},  // Left
//     {14, true, -1},  // Right
// };

// Keypad keypad(myPins, 3);

// void setup() {
//     Serial.begin(115200);



//   // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
//   if(!display.begin(SSD1306_SWITCHCAPVCC)) {
//     Serial.println(F("SSD1306 allocation failed"));
//     for(;;); // Don't proceed, loop forever
//   }


//   // Show initial display buffer contents on the screen --
//   // the library initializes this with an Adafruit splash screen.
//   display.display();
//   delay(2000); // Pause for 2 seconds

//   // Clear the buffer
//   display.clearDisplay();

//   // Draw a single pixel in white
//   display.drawPixel(10, 10, SSD1306_WHITE);

//   // Show the display buffer on the screen. You MUST call display() after
//   // drawing commands to make them visible on screen!
//   display.display();
//   delay(2000);


//   display.clearDisplay();

//   for(int16_t i=0; i<display.height()/2; i+=2) {
//     display.drawRect(i, i, display.width()-2*i, display.height()-2*i, SSD1306_WHITE);
//     display.display(); // Update screen with each newly-drawn rectangle
//     delay(1);
//   }

//   delay(2000);





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
    
//     pinMode(2, OUTPUT);
//     digitalWrite(2, LOW);
//     xTaskCreate([](void*){
//         while (true) {
//             digitalWrite(2, LOW);
//             vTaskDelay(pdMS_TO_TICKS(500));
//             digitalWrite(2, HIGH);
//             vTaskDelay(pdMS_TO_TICKS(500));
//         }
//     }, "LED", 2048, NULL, 2, NULL);
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







#include <Adafruit_SSD1306.h>
#include "Keypad.h"
#include "LCD.h"

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using software SPI (default case):
#define OLED_MOSI   26
#define OLED_CLK   27
#define OLED_DC    33
#define OLED_CS    32
#define OLED_RESET 25

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


KeypadPinConfig myPins[] = {
    {13, true, -1}, // OK
    {12, true, -1},  // Left
    {14, true, -1},  // Right
};

Keypad keypad(myPins, 3);
IndustrialLcdUI ui(display, keypad);

#define EXAMPLE_2


#ifdef EXAMPLE_1
int myInt = 5;
bool myBool = false;

MenuPage mainMenu("Main Menu", {
    MenuItem("Start", [](){ /* start action */ }),
    MenuItem("Settings", new MenuPage("Settings", {
        MenuItem("Edit Value", &myInt, 0, 10),
        MenuItem("Enable X", &myBool)
    })),
    MenuItem("Reboot", [](){ ESP.restart();})
});

#endif // EXAMPLE_1


#ifdef EXAMPLE_2

// Global variables
int someValue = 42;
bool someFlag = false;

// Submenu
std::vector<MenuItem> settingsItems = {
    MenuItem("Back", [](){ ui.popPage(); }),
    MenuItem("Flag", &someFlag),
    MenuItem("Value", &someValue, 0, 100)
};
MenuPage settingsMenu("Settings", settingsItems);

// Root menu
std::vector<MenuItem> mainItems = {
    MenuItem("Settings", &settingsMenu),
    MenuItem("Reset", [](){
        ui.pushPage(new ConfirmationPopup("Reset all?", [](bool yes){
            if (yes) ui.showPopup("Reset OK!");
        }));
    }),
    MenuItem("About", [](){
        ui.showPopup("V1.0 by OpenAI", 1500);
    })
};
MenuPage mainMenu("Main Menu", mainItems);


#endif // EXAMPLE_2

void setup() {
    Serial.begin(115200);
    if (!ui.begin()) {
        Serial.println("Failed to initialize UI!");
        return;
    }
    ui.pushPage(&mainMenu);

    // Optional: status bar for battery, WiFi, etc.
    ui.setStatusBarProvider([](Adafruit_SSD1306& d){
        d.setTextSize(1);
        d.setTextColor(SSD1306_WHITE);
        d.setCursor(80, 0);
        d.print("WiFi"); // Example
    });

    ui.setSleepTimeout(30000); // 30s sleep
}
uint32_t mmm=0;
void loop() {
    mmm=millis();
    ui.loop();
    Serial.println(millis()-mmm);
    delay(10);
}