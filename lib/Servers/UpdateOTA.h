/**
 * 
 * @name UpdateOTA
 * @author Ali Zamani
 * @brief 
 * @version 3.0.0
 * @date 2023-08-30
 */

#ifndef UpdateOTA_h
#define UpdateOTA_h

#include "Arduino.h"
#include "stdlib_noniso.h"
#include "logger.h"
#include <functional>
#include "FS.h"
#include "Update.h"
#include "StreamString.h"
#include "WiFi.h"
#include "WiFiClient.h"
#include "WebServer.h"

#include "Pages.h"

#define UpdateOTA_DEBUG_MSG(...) LOG_INFO(##__VA_ARGS__)
#define UPDATE_DEBUG_MSG(...) LOG_INFO(##__VA_ARGS__) 

enum OTA_Mode {
    OTA_MODE_FIRMWARE = 0,
    OTA_MODE_FILESYSTEM = 1,
    OTA_MODE_EEPROM = 2
};

class UpdateOTAClass{
  public:
    UpdateOTAClass();

    void begin(WebServer *server, const char * username = "", const char * password = "");

    void setAuth(const char * username, const char * password);
    void clearAuth();
    void setAutoReboot(bool enable);
    void loop();

    void onStart(std::function<void()> callable);
    void onProgress(std::function<void(size_t current, size_t final)> callable);
    void onEnd(std::function<void(bool success)> callable);
    
  private:
    WebServer *_server;

    bool _authenticate;
    String _username;
    String _password;

    bool _auto_reboot = true;
    bool _reboot = false;
    unsigned long _reboot_request_millis = 0;

    String _update_error_str = "";
    unsigned long _current_progress_size;
    unsigned long _total_file_size;

    std::function<void()> preUpdateCallback = NULL;
    std::function<void(size_t current, size_t final)> progressUpdateCallback = NULL;
    std::function<void(bool success)> postUpdateCallback = NULL;
};

extern UpdateOTAClass UpdateOTA;
#endif
