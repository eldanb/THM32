#include <Arduino.h>
#include <FastLED.h>
#include <WiFi.h>

#include "led_animations.h"
#include "word_board.h"
#include "clock_text.h"

#define NUM_LEDS 256
#define DATA_PIN 16
#define BOARD_SIZE 16

#define SSID "tazmania-guest"
#define PASSWORD "fragicali"

#define TZ_GMTOFFSET_SEC 7200
#define TZ_DST_OFFSET_SEC 0
#define NTP_SERVER "europe.pool.ntp.org"

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

CRGB leds[NUM_LEDS] = {0};
LedAnimationController ledAnimationController(leds, BOARD_SIZE, BOARD_SIZE);
WordBoard wordBoard(boardLetters, ledAnimationController);

void clearBoardAndShowText(const char *text, int startAtMillis = -1)
{
  if (startAtMillis == -1)
  {
    startAtMillis = millis();
  }

  ledAnimationController.setAllToColor(CRGB{0, 0, 0}, startAtMillis + 500);
  wordBoard.displayText(text, startAtMillis + 500, NULL);
}

int lastFrameMillis;

void syncTime()
{
  WiFi.begin(SSID, PASSWORD);
  int progress = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    leds[progress++] = CRGB{32, 32, 32};
    FastLED.show();
  }

  // init and get the time
  configTime(TZ_GMTOFFSET_SEC, TZ_DST_OFFSET_SEC, NTP_SERVER);

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, EOrder::GRB>(leds, NUM_LEDS);
  lastFrameMillis = millis() - 120000;

  syncTime();
}

void loop()
{
  int newFrameMillis = millis();

  if (newFrameMillis / (60 * 1000) != lastFrameMillis / (60 * 1000))
  {
    int hr = rand() % 24;
    int mt = rand() % 60;

    struct tm timeinfo;
    if (!getLocalTime(&timeinfo))
    {
      Serial.println("Failed to obtain time");
    }
    else
    {
      hr = timeinfo.tm_hour;
      mt = timeinfo.tm_min;
    }

    Serial.printf("Show time %d:%d\n", hr, mt);
    clearBoardAndShowText(timeToString(hr, mt).c_str());
  }
  /*
    if ((newFrameMillis / 10000) % 2 == 0)
    {
      if (random() % 100 == 1)
      {
        int x = random() % 16;
        int y = random() % 16;

        ledAnimationController.addAnimation(x, y, {targetColor : CRGB{255, 255, 255}, targetFrame : newFrameMillis + 400});
        ledAnimationController.addAnimation(x, y, {targetColor : CRGB{128, 0, 0}, targetFrame : newFrameMillis + 800});
        ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 128, 0}, targetFrame : newFrameMillis + 1200});
        // ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 0, 128}, targetFrame : newFrameMillis + 1600});
        // ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 0, 0}, targetFrame : newFrameMillis + 2000});
      }
    }*/

  ledAnimationController.animate(newFrameMillis);
  lastFrameMillis = newFrameMillis;
  FastLED.show();
}