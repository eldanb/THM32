#include "log.h"
#include <Arduino.h>

#define LOG_MAX_LOG_MESSAGE_SIZE 256

static int lastFreeHeap = 0;
static int logLevel = LOGLEVEL_DEBUG;

static void log_formatted_message(int debuglevel, char *messageBuffer);

void log(int debuglevel, const char *fmt, ...)
{
  if (debuglevel > logLevel)
    return;

  va_list va;

  va_start(va, fmt);
  char messageBuffer[LOG_MAX_LOG_MESSAGE_SIZE];
  int length = vsnprintf(messageBuffer, LOG_MAX_LOG_MESSAGE_SIZE - 3, fmt, va);

  va_end(va);

  if (length > LOG_MAX_LOG_MESSAGE_SIZE)
    strcat(messageBuffer, "...");

  log_formatted_message(debuglevel, messageBuffer);
}

void log_formatted_message(int debuglevel, char *messageBuffer)
{
  char print_time[20];

  sprintf(print_time, "[%010d]", (int)millis());

  int _freeHeap = ESP.getFreeHeap();
  int _maxFreeAlloc = ESP.getMaxAllocHeap();
  Serial.printf("\e[%d;%dm%8s  [%6d]  [%6d] [%6d] [%-1d]   %s\n\e[1;37m", 0, (31 + debuglevel), print_time, _freeHeap, _freeHeap - lastFreeHeap, _maxFreeAlloc, debuglevel, messageBuffer);

  lastFreeHeap = _freeHeap;
}
