#include "WebServerCustom.h"
#include "ServerConfig.h"
#include "handleTasks.h"



static const char* TAG = "WebServer";


// FreeRTOS task handles
TaskHandle_t WebServerTaskHandle;

// Web server instance
WebServer server(WEB_SERVER_PORT);


ESPWatchdog ServerWDG(TAG); 

void AddRouteToServer(WebServer * _server ,const String& uri, HTTPMethod method, WebServer::THandlerFunction handler) {
    _server->on(uri, method, [handler, uri]() {
        LOG_DEBUG("Web Server WDG has been reset ");
        ServerWDG.reset();
        LOG_DEBUG("Handling '%s' route", uri.c_str());
        if (!authenticate()) {
            LOG_WARN("Authentication failed for '%s'", uri.c_str());
            return;
        }
        handler();
    });
}

bool authenticate(){

  if(server.authenticate(CONFIG.get(UserName_CK).c_str(), CONFIG.get(UserPass_CK).c_str())){
    return true;
  }
  server.requestAuthentication(DIGEST_AUTH) ;

  return false ;
}


/**
 * @brief Web server task function.
 *        Handles incoming HTTP requests and manages web server routes.
 *        Runs as a FreeRTOS task.
 * @param pvParameters Unused parameter.
 */
void WebServerTask(void *pvParameters) {

  LOG_INFO("Starting web server task");

  for (;;) {
    server.handleClient();
    UpdateOTA.loop();
    // delay(2);
  }
}

/**
 * @brief Loads default configuration values for the web server.
 */
static void loadDefaultConfig() {
  // LOG_INFO("Loading default configuration");
  // CONFIG.initDefault("ssid", WEB_SERVER_DEFAULT_WIFI_SSID);
  // CONFIG.initDefault("password", WEB_SERVER_DEFAULT_WIFI_PASSWORD);
  // CONFIG.initDefault("username", WEB_SERVER_DEFAULT_USERNAME);
  // CONFIG.initDefault("userpass", WEB_SERVER_DEFAULT_USER_PASSWORD);
  // CONFIG.save();
  // LOG_INFO("Default configuration loaded and saved");
}

/**
 * @brief Initializes the web server and starts the FreeRTOS task.
 *        Sets up the WiFi access point and web server routes.
 */
void InitWebServer() {
  LOG_INFO("Initializing web server");
  if (WiFi.getMode() != WIFI_OFF) {
    LOG_INFO("Wifi mode is %u", WiFi.getMode());
    return;
  }
  ServerWDG.begin(CONFIG.get(ServerWdgTimeout_CK).toInt() * 1000 , DeInitWebServer);
  loadDefaultConfig();
  // Set up access point
  WiFi.softAP((String("Optic_")+CONFIG.get(DeviceId_CK)).c_str(), "");
  LOG_INFO("Access Point started");
  LOG_INFO("SSID: %s", (String("Optic_")+CONFIG.get(DeviceId_CK)).c_str());
  
  // WiFi.begin("AliX6", "11112222");
  // WiFi.begin("OTH-CO", "A60246024a");
  
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   LOG_WARN("WiFi connecting, status: %u", WiFi.status());
  // }
  // LOG_INFO("WiFi connected");
  // LOG_INFO("Server address : http://%s:%u",WiFi.localIP().toString().c_str(),WEB_SERVER_PORT);
   




  // Configure web server routes
  // server.on("/", []() {
  //   LOG_DEBUG("Handling '/' route");
  //   if (!authenticate()) {
  //     LOG_WARN("Authentication failed for '/'");
  //     return;
  //   }
  //   server.send(200, "text/plain", "Login OK");
  // });

  // server.on("/exit", HTTP_GET, []() {
  //   LOG_DEBUG("Handling '/exit' route");
  //   if (!authenticate()) {
  //     LOG_WARN("Authentication failed for '/exit'");
  //     return;
  //   }
  //   server.send(200, "text/plain", "Exited");
  //   delay(200);
  //   LOG_INFO("Deinitializing web server");
  //   DeInitWebServer();
  // });
  // server.serveStatic("/",SPIFFS,"/");
  // // server.on("/", HTTP_GET, handleHomePage);
  // server.on("/", HTTP_GET, []() {
  //   handleFileRead("/home.html");
  // });
  // server.on("/config", HTTP_GET, []() {
  //   handleFileRead("/config.html");
  // });
  
  AddRouteToServer(&server,"/", HTTP_GET, handleRoot);
  AddRouteToServer(&server,"/home", HTTP_GET, handleRoot);
  AddRouteToServer(&server,"/Style.css", HTTP_GET, handleStyle);
  AddRouteToServer(&server,"/config", HTTP_GET, handleConfig);
  AddRouteToServer(&server,"/static", HTTP_GET, handleStatic);
  AddRouteToServer(&server,"/getconfig", HTTP_GET, handleGetConfig);
  AddRouteToServer(&server,"/setconfig", HTTP_POST, handleSetConfig);
  addUpdaterHandle();


  // server.send_P

  // AddRoute2Server("/save-config", HTTP_POST, handleSaveConfig);
  // AddRoute2Server("/save-config", HTTP_POST, handleSaveConfig);
  // AddRoute2Server("/save-config", HTTP_POST, handleSaveConfig);

  // server.on("/config", HTTP_GET, handlePanelConfig);
  // server.on("/save-config", HTTP_POST, handleSaveConfig);
  // server.on("/exit", HTTP_ANY, handleExit);
  // server.addHandler()
  // server.onNotFound(handleNotFound);
  
  server.onNotFound([]() {
      // handleFileRead(server.uri());
      handleNotFound();
  });

  
  // #if USE_INTERNAL_UPDATE_SERVER
  //   addUpdaterHandle();
  // #endif

  server.begin();
  LOG_INFO("Web server started on address: http://%s:%u", WiFi.softAPIP().toString().c_str(),WEB_SERVER_PORT);

  xTaskCreatePinnedToCore(WebServerTask,
    "WebServerTask",
    WEB_SERVER_TASK_STACK_SIZE,
    NULL,
    WEB_SERVER_TASK_PRIORITY,
    &WebServerTaskHandle,
    WEB_SERVER_TASK_CORE_ID);

  LOG_INFO("Web server task created and pinned to core");

}

/**
 * @brief Deinitializes the web server, stops the FreeRTOS task,
 *        and turns off WiFi.
 */
void DeInitWebServer() {
  if (WiFi.getMode() == WIFI_OFF) {
    LOG_INFO("Wifi mode is %u", WiFi.getMode());
    return;
  }

  ServerWDG.stop();
  LOG_INFO("Deinitializing web server");
  server.close();
  WiFi.mode(WIFI_OFF);
  WiFi.disconnect(true);
  LOG_INFO("Web server stopped and WiFi turned off");
  vTaskDelete(WebServerTaskHandle);
}
