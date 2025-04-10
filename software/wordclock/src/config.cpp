#include "config.h"
#include <FFat.h>
#include "log.h"

static StaticJsonDocument<512> _configJson;

void setupConfig()
{
  auto configFile = FFat.open("/config.json");
  if (configFile)
  {
    if (auto serializationError = deserializeJson(_configJson, configFile))
    {
      log(LOGLEVEL_WARNING, "Could not deserialize config file: %d", serializationError);
    }
  }
  else
  {
    log(LOGLEVEL_WARNING, "Config file not found");
    _configJson.to<JsonObject>();
  }
}

ArduinoJson::JsonDocument &getConfig()
{
  return _configJson;
}

void saveConfig()
{
  auto configFile = FFat.open("/config.json", "w", true);
  if (configFile)
  {
    serializeJson(_configJson, configFile);
  }
  else
  {
    log(LOGLEVEL_WARNING, "Could not write config file");
  }
}
