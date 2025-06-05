#include "WebServerCustom.h"
#include "ServerConfig.h"
#include "Pages.h"

static const char* TAG = "Server.Tasks";

extern WebServer server;
extern ESPWatchdog ServerWDG;


void handleExit(){
  LOG_DEBUG("Handling '/exit' route");
  if (!authenticate()) {
    LOG_WARN("Authentication failed for '/exit'");
    return;
  }
  server.send(200, "text/plain", "Exited");
  LOG_INFO("Exit message sent to client, deinitializing web server");
  delay(200);
  LOG_INFO("Deinitializing web server");
  DeInitWebServer();
}

void handleNotFound() {
  String message = "Not Detected\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " NAME:" + server.argName(i) + "\n VALUE:" + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  LOG_WARN("Not fount page : %s",server.uri().c_str());
}

// void handleFileRead(String path) {
  
//   LOG_DEBUG("Handling '%s' route",server.uri().c_str());

//   if (!authenticate()) {
//     LOG_WARN("Authentication failed for '%s'",server.uri().c_str());
//     return;
//   }

//   LOG_INFO("handleFileRead: %s", path.c_str());
//   // if (path.endsWith("/")) {
//   //   path += "index.htm";
//   // }
//   String contentType = getContentType(path);

//   if ( SPIFFS.exists(path)) {
//     File file = SPIFFS.open(path, "r");
//     server.streamFile(file, contentType);
//     file.close();
//     return ;
//   }
//   handleNotFound();
//   return ;
// }


void addUpdaterHandle() {
  UpdateOTA.begin(&server);    // Start UpdateOTA
  UpdateOTA.setAuth(CONFIG.get(UserName_CK).c_str(), CONFIG.get(UserPass_CK).c_str());
  UpdateOTA.setAutoReboot(true);
  UpdateOTA.onProgress([](size_t current, size_t final) {
    ServerWDG.reset();
    LOG_DEBUG("OTA Progress : %u%%", (long) 100 * current / final);
  });
  UpdateOTA.onStart([]() {
    ServerWDG.reset();
    LOG_INFO("OTA Update Started :");
  });
  UpdateOTA.onEnd([](bool success) {
    ServerWDG.reset();
    if (success) {
      LOG_INFO("OTA Update Finished Successfully .");
    } else {
      LOG_ERROR("There was an error during OTA update!");
    }
    LOG_INFO("OTA Update Finished .");
  });
}


void handleRoot(){
    sendContent(&server,"home");

}
void handleStyle(){
    sendContent(&server,"style");
}

void handleStatic(){
  LOG_INFO("Handling '/static' route args: %d , uri : %s",server.args(),server.uri().c_str());
  for (size_t i = 0; i < server.args(); i++)
  {
    LOG_INFO("args : %s",server.arg(i).c_str());
  }
  // fileServe(SPIFFS,"/static/index.html",false);
}
void handleConfig(){
    sendContent(&server,"config");
}

String GetDynamicConfig(String name){
  if (name=="activeOta")
    return getActiveOtaLabel();
  
  return String();
}
void handleGetConfig(){
  /*
  example:
  for GET request (/getconfig?key=1,3)
  return json :
    {
      "key1":"value",
      "key3":"value",
    } 
  */

  // String message = "Arguments: {";
  // message += server.args();
  // for (uint8_t i = 0; i < server.args(); i++) {
  //   message += server.argName(i) + ":" + server.arg(i) + "," ;
  // }
  // message += "}";
  // LOG_DEBUG("/getconfig -> %s",message.c_str());

  
  StaticJsonDocument<512> json;
  if (server.hasArg("key")) {
    String keyArg = server.arg("key");
    std::vector<String> keys=String2Vector(keyArg);
    for (size_t i = 0; i < keys.size(); i++)
    {
      json[String("key")+keys[i]]=CONFIG.get((config_key_t)keys[i].toInt());
    }
  }
  
  if (server.hasArg("name")) {
    String nameArg = server.arg("name");
    std::vector<String> names=String2Vector(nameArg);
    for (size_t i = 0; i < names.size(); i++)
    {
      json[names[i]]=GetDynamicConfig(names[i]);
    }
  }
  String tempJson;
  serializeJson(json,tempJson);
  LOG_DEBUG("/getconfig -> %s",tempJson.c_str());
  server.send(200, "application/json",tempJson);
  return;
}


void handleSetConfig(){
  
    // Read the POST body
    String body = server.arg("plain");
    StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
        server.send(400, "text/plain", "Invalid JSON");
        return;
    }

    for (JsonPair kv : doc.as<JsonObject>()) {
    String key = kv.key().c_str();   // e.g., "key0"
    if (key.startsWith("key"))
    {
      key = key.substring(3);
      String value = kv.value().as<String>();
      CONFIG.set((config_key_t)key.toInt(), value);
    }
    
    // Now you can process key and value
    }

    CONFIG.save(); // If you have a save method
    server.send(200, "text/plain", "OK");
    delay(2000);
    ESP.restart();
}