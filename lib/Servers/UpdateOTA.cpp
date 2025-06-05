#include "UpdateOTA.h"
#include "esp_ota_ops.h"
#include "system.h"

static const char * TAG = "ELG" ;

UpdateOTAClass::UpdateOTAClass(){}

void UpdateOTAClass::begin(WebServer *server, const char * username, const char * password){
  _server = server;

  setAuth(username, password);


    _server->on("/update", HTTP_GET, [&](){
      if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
        return _server->requestAuthentication();
      }
      // size_t pagesize=1;
      // const char* page = getWebPage("update",&pagesize);
      // LOG_DEBUG("Update Page Size : %u",pagesize);
      // _server->send_P(200, "text/html", page, pagesize);
      sendContent(_server,"update");

    });

    _server->on("/ota/last", HTTP_GET, [&]() {
      if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
        return _server->requestAuthentication();
      }
      String ActiveOta = getActiveOtaLabel() ;
      String LastOta = ActiveOta=="app0" ? "app1":"app0";
      

      const esp_partition_t* part = esp_partition_find_first(
          ESP_PARTITION_TYPE_APP, ESP_PARTITION_SUBTYPE_ANY, LastOta.c_str());
      if (part) {
          esp_err_t err = esp_ota_set_boot_partition(part);
          if (err == ESP_OK) {
              LOG_INFO("Last boot partition %s will be used", LastOta.c_str());
              return _server->send(200, "text/plain", "Last boot partition will be used");
              ESP.restart();
          } else {
              LOG_ERROR("Failed to set boot partition: %d", LastOta.c_str());
              return _server->send(400, "text/plain", "Failed to set boot partition");
          }
      } else {
          LOG_ERROR("Partition not found: %s", LastOta.c_str());
          return _server->send(400, "text/plain", "Partition not found");
      }
    });

    _server->on("/ota/start", HTTP_GET, [&]() {
      if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
        return _server->requestAuthentication();
      }

      // Get header x-ota-mode value, if present
      OTA_Mode mode = OTA_MODE_FIRMWARE;
      // Get mode from arg
      if (_server->hasArg("mode")) {
        String argValue = _server->arg("mode");
        if (argValue == "fs") {
          LOG_INFO("OTA Mode: Filesystem");
          mode = OTA_MODE_FILESYSTEM;
        }else {
          if(argValue == "eeprom"){
            LOG_INFO("OTA Mode: EEPROM");
            mode = OTA_MODE_EEPROM;

          } else {
          LOG_INFO("OTA Mode: Firmware");
          mode = OTA_MODE_FIRMWARE;
          }
        }
      }
      
      size_t filesize = UPDATE_SIZE_UNKNOWN;
      _total_file_size = 0 ;
      // Get mode from arg
      if (_server->hasArg("size")) {
        try
        {
          String argValue = _server->arg("size");
          filesize=argValue.toInt();
          _total_file_size = filesize;
          LOG_INFO("update file size : %u",filesize);
        }
        catch(const std::exception& e)
        {
          LOG_ERROR("update file size : %s",_server->arg("size").c_str());
        }
        
      }
      else{
        if(_server->args()>1){
          try
          {
            String argValue = _server->arg(_server->argName(1));
            filesize=argValue.toInt();
            _total_file_size = filesize;
            LOG_INFO("update file i %s : %u",_server->argName(1).c_str(),filesize);
          }
          catch(const std::exception& e)
          {
            LOG_ERROR("update file i %s : %s",_server->argName(1).c_str(),_server->arg(_server->argName(1)).c_str());
          }
        }
          
        LOG_INFO("update args : %u",_server->args());
      }
      
      

      // Pre-OTA update callback
      if (preUpdateCallback != NULL) preUpdateCallback();

      if(mode != OTA_MODE_EEPROM) {
        if (!Update.begin(filesize, mode == OTA_MODE_FILESYSTEM ? U_SPIFFS : U_FLASH)) {
          LOG_ERROR("Failed to start update process");
          // Save error to string
          StreamString str;
          Update.printError(str);
          LOG_ERROR("%s",str.c_str());
        }
        return _server->send((Update.hasError()) ? 400 : 200, "text/plain", (Update.hasError()) ? _update_error_str.c_str() : "OK");
      }
      else{
        return _server->send(400, "text/plain", "EEPROM not implemented");
      }
    });
  
    _server->on("/ota/upload", HTTP_POST, [&](){
      if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
        return _server->requestAuthentication();
      }
      // Post-OTA update callback
      if (postUpdateCallback != NULL) postUpdateCallback(!Update.hasError());
      _server->sendHeader("Connection", "close");
      _server->send((Update.hasError()) ? 400 : 200, "text/plain", (Update.hasError()) ? _update_error_str.c_str() : "OK");
      // Set reboot flag
      if (!Update.hasError()) {
        if (_auto_reboot) {
          _reboot_request_millis = millis();
          _reboot = true;
        }
      }
    }, [&](){
      // Actual OTA Download
      HTTPUpload& upload = _server->upload();
      if (upload.status == UPLOAD_FILE_START) {
        // Check authentication
        if (_authenticate && !_server->authenticate(_username.c_str(), _password.c_str())) {
          LOG_WARN("Authentication Failed on UPLOAD_FILE_START");
          return;
        }
        LOG_INFO("Update Received: %s", upload.filename.c_str());
        _current_progress_size = 0;
      } else if (upload.status == UPLOAD_FILE_WRITE) {
          size_t write_size = Update.write(upload.buf, upload.currentSize);
          if (write_size != upload.currentSize) {
            StreamString err;
            Update.printError(err);
            LOG_ERROR("Not writed completly : %u writen from %u ->%s",write_size,upload.currentSize,err.c_str());
          }
          _current_progress_size += upload.currentSize;
          // Progress update callback
          if (progressUpdateCallback != NULL) progressUpdateCallback(_current_progress_size, _total_file_size);
      } else if (upload.status == UPLOAD_FILE_END) {
          if (Update.end(true)) {
              LOG_INFO("Update Success: %u",upload.totalSize);
          } else {
              LOG_ERROR("[!] Update Failed");
              // Store error to string
              StreamString str;
              Update.printError(str);
              LOG_ERROR("%s",str.c_str());
          }
      } else {
        LOG_ERROR("Update Failed Unexpectedly (likely broken connection): status=%u",upload.status);
      }
    });
}

void UpdateOTAClass::setAuth(const char * username, const char * password){
  _username = username;
  _password = password;
  _authenticate = _username.length() && _password.length();
}

void UpdateOTAClass::clearAuth(){
  _authenticate = false;
}

void UpdateOTAClass::setAutoReboot(bool enable){
  _auto_reboot = enable;
}

void UpdateOTAClass::loop() {
  // Check if 2 seconds have passed since _reboot_request_millis was set
  if (_reboot && millis() - _reboot_request_millis > 2000) {
    LOG_INFO("Rebooting...");
    ESP.restart();
    _reboot = false;
  }
}

void UpdateOTAClass::onStart(std::function<void()> callable){
    preUpdateCallback = callable;
}

void UpdateOTAClass::onProgress(std::function<void(size_t current, size_t final)> callable){
    progressUpdateCallback= callable;
}

void UpdateOTAClass::onEnd(std::function<void(bool success)> callable){
    postUpdateCallback = callable;
}


UpdateOTAClass UpdateOTA;
