#pragma once

#include <ArduinoJson.h>

void setupConfig();
ArduinoJson::JsonDocument &getConfig();
void saveConfig();