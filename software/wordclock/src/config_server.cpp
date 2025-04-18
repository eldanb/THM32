#include <Arduino.h>
#include <WiFi.h>
#include <aWOT.h>
#include <ESPmDNS.h>
#include <FFat.h>
#include <ArduinoJson.h>

#include "log.h"
#include "config.h"

using namespace ArduinoJson;

static TaskHandle_t _webserverTask;
static void runWebServerTask(void *webserverContext);

void setupConfigServer()
{
  if (!_webserverTask)
  {
    log(LOGLEVEL_DEBUG, "Starting web server");
    xTaskCreate(runWebServerTask, "Web Server", 8192, nullptr, 1, &_webserverTask);
  }
}

void serveConfigConsole(Request &req, Response &res)
{
  auto fileHandle = FFat.open("/index.html");
  uint8_t buff[256];
  int readLen = 0;

  res.set("Content-Type", "text/html");
  res.status(200);

  while (fileHandle.available())
  {
    readLen = fileHandle.readBytes((char *)buff, sizeof(buff));
    res.write(buff, readLen);
  }
}

void handleGetSsids(Request &req, Response &res)
{
  log(LOGLEVEL_DEBUG, "Start scanning networks");
  int scanRet = WiFi.scanNetworks(false, true);
  log(LOGLEVEL_DEBUG, "Scanning networks completed: %d", scanRet);

  StaticJsonDocument<2048> doc;
  auto ssids = doc["ssids"];

  for (int netIdx = 0; netIdx < 10; netIdx++)
  {
    String ssidName = WiFi.SSID(netIdx);
    if (ssidName.isEmpty())
    {
      break;
    }
    log(LOGLEVEL_DEBUG, "Adding SSID %s", ssidName.c_str());

    auto ssidObj = ssids.add();
    ssidObj["ssid"] = ssidName;
    ssidObj["rssi"] = WiFi.RSSI(netIdx);
    ssidObj["enc"] = WiFi.encryptionType(netIdx);
  }

  res.set("Content-Type", "application/json");
  res.status(200);

  serializeJson(doc, res);
}

void handleGetConfig(Request &req, Response &res)
{
  res.set("Content-Type", "application/json");
  res.status(200);

  serializeJson(getConfig(), res);
}

void handlePostConfig(Request &req, Response &res)
{
  auto jsonDeserializationError = deserializeJson(getConfig(), req);
  if (jsonDeserializationError)
  {
    log(LOGLEVEL_ERROR, "Invalid JSON input: %d", (int)jsonDeserializationError.code());
    res.sendStatus(500);
    return;
  }

  saveConfig();

  res.set("Access-Control-Allow-Origin", "*");
  res.set("Content-Type", "application/json");
  res.status(200);
  res.println("{\"result\": \"ok\"}");

  delay(500);
  ESP.restart();
}

void handleManualTime(Request &req, Response &res)
{
  DynamicJsonDocument requestBody(128);
  auto jsonDeserializationError = deserializeJson(requestBody, req);
  if (jsonDeserializationError)
  {
    log(LOGLEVEL_ERROR, "Invalid JSON input: %d", (int)jsonDeserializationError.code());
    res.sendStatus(500);
    return;
  }

  struct tm timeStruct{
    tm_sec : requestBody["second"],
    tm_min : requestBody["minute"],
    tm_hour : requestBody["hour"],

    tm_mday : requestBody["day"],
    tm_mon : requestBody["month"],
    tm_year : requestBody["year"],

    0
  };

  timeval timeVal{
    tv_sec : mktime(&timeStruct),
    0
  };

  settimeofday(&timeVal, nullptr);

  res.set("Access-Control-Allow-Origin", "*");
  res.set("Content-Type", "application/json");
  res.status(200);
  res.println("{\"result\": \"ok\"}");

  delay(500);
  ESP.restart();
}
void runWebServerTask(void *webserverContext)
{
  WiFiServer server(80);
  Application app;

  app.get("/", &serveConfigConsole);
  app.get("/ssids", &handleGetSsids);
  app.get("/config", &handleGetConfig);
  app.post("/config", &handlePostConfig);
  app.post("/manual-time", &handleManualTime);

  server.begin();

  for (;;)
  {
    WiFiClient client = server.available();

    if (client.connected())
    {
      app.process(&client);
    }

    taskYIELD();
  }
}