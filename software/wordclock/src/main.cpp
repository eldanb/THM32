#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>
#include <FFat.h>

#include "led_animations.h"
#include "word_board.h"
#include "clock_text.h"
#include "config.h"
#include "net.h"
#include "config_server.h"
#include "log.h"

#define NUM_LEDS 256
#define DATA_PIN 16
#define BOARD_SIZE 16

#define SSID "tazmania-guest"
#define PASSWORD "fragicali"

#define TZ_GMTOFFSET_SEC 7200
#define TZ_DST_OFFSET_SEC 3600
#define NTP_SERVER "europe.pool.ntp.org"

#ifdef SHACHAR_BOARD
static const char *boardLetters =
    "ITLISASTHPMAHAVE"
    "ACFIFTEENDOORFUN"
    "TWENTYFIVEXWUIAT"
    "THIRTYFTENOSSTHE"
    "MINUTESETOURMALL"
    "PASTORUFOURTRFLY"
    "SEVENOONMIDNIGHT"
    "NINEFIVECTWORZOO"
    "EIGHTFELEVENPARK"
    "SIXTHREEONEGGSKI"
    "TENSZOCLOCKOTOK?"
    "ITDISARAINYNTIME"
    "WINDYBSUNNYRKFOR"
    "STORMYRIKCDINNER"
    "CHILLYFWARMKTOMU"
    "GONLUNCHLBREAKDS";
#else
static const char *boardLetters =
    "LITASTHISPMAHAVE"
    "ACFIFTEENDOORFUN"
    "TWENTYWFIVEXUIAT"
    "FTHIRTYOSTENSTHE"
    "MINUTESETOURMALL"
    "ORUPASTRTFOURFLY"
    "SEVENOONMIDNIGHT"
    "NINEFIVECTWORZOO"
    "EIGHTFELEVENPARK"
    "SIXTHREEONEGGSKI"
    "TENSEZOCLOCKTOK?"
    "DITAISRAINYNTIME"
    "WINDYBSUNNYRKFOR"
    "STORMYRIKCDINNER"
    "CHILLYFWARMKTOMU"
    "GONLUNCHLBREAKDS";
#endif

CRGB leds[NUM_LEDS] = {0};
LedAnimationController ledAnimationController(leds, BOARD_SIZE, BOARD_SIZE);
WordBoard wordBoard(boardLetters, ledAnimationController);

int lastFrameMillis;
int refreshPeriod = 60 * 1000;

void clearBoardAndShowText(const char *text, int startAtMillis = -1)
{
  if (startAtMillis == -1)
  {
    startAtMillis = millis();
  }

  ledAnimationController.setAllToColor(CRGB{0, 0, 0}, startAtMillis + 500);
  wordBoard.displayText(text, startAtMillis + 500, NULL);
}

void syncTime()
{
  // init and get the time
  auto &config = getConfig();
  int timeZone = config.containsKey("timeZone") ? config["timeZone"] : 0;
  bool dstEnabled = config.containsKey("isDst") && config["isDst"];

  configTime(timeZone * 3600, dstEnabled ? 3600 : 0, NTP_SERVER);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 15000))
  {
    log(LOGLEVEL_ERROR, "Failed to obtain time");
    return;
  }

  char formattedTime[32];
  strftime(formattedTime, sizeof(formattedTime), "%H:%M:%S", &timeinfo);
  log(LOGLEVEL_DEBUG, "Loaded time %s (GMT %d, DST %s)", formattedTime, timeZone, dstEnabled ? "true" : "false");
}

void refreshBoardTime()
{
  int currentHour = 0;
  int currentMinute = 0;

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    log(LOGLEVEL_ERROR, "Failed to obtain time; will show random time");
    currentHour = rand() % 24;
    currentMinute = rand() % 60;
  }
  else
  {
    currentHour = timeinfo.tm_hour;
    currentMinute = timeinfo.tm_min;
  }

  log(LOGLEVEL_DEBUG, "Show time %02d:%02d", currentHour, currentMinute);

  clearBoardAndShowText(timeToString(currentHour, currentMinute).c_str());
}

void setup()
{
  Serial.begin(115200);
  FFat.begin();
  FastLED.addLeds<WS2812B, DATA_PIN, EOrder::GRB>(leds, NUM_LEDS);

  setupConfig();
  setupNetwork();
  syncTime();

  setupConfigServer();

  lastFrameMillis = millis() - 120000;
  refreshPeriod = getConfig().containsKey("refreshPeriod") ? (getConfig()["refreshPeriod"].as<int>() * 1000) : (60 * 1000);
  FastLED.setBrightness(getConfig().containsKey("brightness") ? (getConfig()["brightness"].as<int>() * 255 / 100) : 255);
}

void loop()
{
  int newFrameMillis = millis();

  if (newFrameMillis / (refreshPeriod) != lastFrameMillis / refreshPeriod)
  {
    refreshBoardTime();
  }

  ledAnimationController.animate(newFrameMillis);
  lastFrameMillis = newFrameMillis;
  FastLED.show();
}
