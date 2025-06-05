#ifndef F4CE048B_4595_44CC_B1DD_1BEF34669854
#define F4CE048B_4595_44CC_B1DD_1BEF34669854
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <SD.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include "Logger.h"
#include "system.h"
#include "Config.h"
#include "WDG.h"
#include "UpdateOTA.h"
#include "Utils.h"

extern ESPWatchdog ServerWDG;

//authenticate
bool authenticate();

//utils
// String getContentType(String filename, bool download) ;
void redirectTo(WebServer &server, String url) ;
bool ServerIsOn();
// void fileServe(FS& fs , String path, bool download) ;
void fileServe(FS &fs , String path , bool download ) ;

//WebServerCustom
void InitWebServer();
void DeInitWebServer();



#endif /* F4CE048B_4595_44CC_B1DD_1BEF34669854 */