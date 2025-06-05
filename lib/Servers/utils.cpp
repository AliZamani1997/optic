#include "WebServerCustom.h"

static const char* TAG = "Server.Utils";

extern WebServer server;
extern ESPWatchdog ServerWDG;



static String _getContentType(String filename,bool download=false) {
  if (download) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm")) {
    return "text/html";
  } else if (filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".txt")) {
    return "text/plain";
  } else if (filename.endsWith(".json")) {
    return "application/json";
  } else if (filename.endsWith(".svg")) {
    return "image/svg+xml";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }
  return "application/octet-stream";
}

String convertSize(size_t s){
  if (s < 1024) return String(s) + " B";
  float size = s / 1024.0;
  if (size < 1) return String(size, 3) + " KB";
  if (size < 10) return String(size, 2) + " KB";
  if (size < 100) return String(size, 1) + " KB";
  if (size < 1000) return String(size, 0) + " KB";
  size /= 1024.0;
  if (size < 1) return String(size, 3) + " MB";
  if (size < 10) return String(size, 2) + " MB";
  if (size < 100) return String(size, 1) + " MB";
  if (size < 1000) return String(size, 0) + " MB";
  size /= 1024.0;
  return String(size, 2) + " GB";
}

void redirectTo(WebServer &server, String url) {
  ServerWDG.reset();
  LOG_DEBUG("redirectTo: %s", url.c_str());
  server.sendHeader("Location", url, true);
  server.send(302, "text/plain", "");
}

bool ServerIsOn(){
  return WiFi.getMode() == WIFI_MODE_AP ;
}


void fileServe(FS &fs , String path , bool download) {
    File f = fs.open(path);
    bool isFile = (f && (! f.isDirectory()));
    if (!isFile || !f.available()) {
      //Error
      LOG_ERROR("Cant serve file %s",path.c_str());
      f.close();
      return;
    }

    String contentType = _getContentType(path,download);

    // if (_cache_header.length() != 0)
    //     server.sendHeader("Cache-Control", _cache_header);

    server.streamFile(f, contentType);
    return ;
}
