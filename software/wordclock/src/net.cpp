#include "net.h"

#include <WiFi.h>
#include <ESPmDNS.h>

#include "config.h"
#include "log.h"

static bool waitForWifiStatus(int status)
{
  int startMillis = millis();
  while (WiFi.status() != status)
  {
    delay(500);
    if (millis() - startMillis > 10000)
    {
      break;
    }
  }

  return WiFi.status() == status;
}

bool setupNetwork()
{
  auto &config = getConfig();
  String ssid = config["ssid"].isNull() ? "" : (const char *)config["ssid"];
  String password = config["password"].isNull() ? "" : (const char *)config["password"];
  bool apMode = false;

  if (!ssid.isEmpty())
  {
    log(LOGLEVEL_DEBUG, "Attempting to connect to WiFi from config: %s (pass len = %d)", ssid.c_str(), password.length());
    if (password.isEmpty())
    {
      WiFi.begin(ssid);
    }
    else
    {
      WiFi.begin(ssid, password);
    }

    waitForWifiStatus(WL_CONNECTED);
  }

  String macStr = WiFi.macAddress();
  macStr.replace(":", "-");
  String baseHostName = String("wrdclk-") + macStr;

  if (WiFi.status() != WL_CONNECTED)
  {
    log(LOGLEVEL_DEBUG, "Starting AP %s", baseHostName.c_str());
    if (!WiFi.softAP(baseHostName))
    {
      log(LOGLEVEL_ERROR, "Could not start SoftAP!");
      return false;
    }

    apMode = true;
  }

  log(LOGLEVEL_DEBUG, "Starting MDNS for host %s, ip %s", baseHostName.c_str(), (apMode ? WiFi.softAPIP() : WiFi.localIP()).toString());

  if (!MDNS.begin(baseHostName))
  {
    log(LOGLEVEL_ERROR, "Could not start MDNS!");
    return false;
  }

  return true;
}