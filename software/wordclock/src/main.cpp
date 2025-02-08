#include <Arduino.h>
#include <FastLED.h>
#include "led_animations.h"

#define NUM_LEDS 256
#define DATA_PIN 16

CRGB leds[NUM_LEDS] = {0};
LedAnimationController ledAnimationController(leds, 16, 16);

int lastFrameMillis;

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, EOrder::GRB>(leds, NUM_LEDS);
  lastFrameMillis = millis();

#define LEDCORD 3, 3

  ledAnimationController.addAnimation(LEDCORD, {targetColor : CRGB{255, 255, 255}, lenFrames : 400});
  ledAnimationController.addAnimation(LEDCORD, {targetColor : CRGB{128, 0, 0}, lenFrames : 800});
  ledAnimationController.addAnimation(LEDCORD, {targetColor : CRGB{0, 128, 0}, lenFrames : 800});
  ledAnimationController.addAnimation(LEDCORD, {targetColor : CRGB{0, 0, 128}, lenFrames : 800});
  ledAnimationController.addAnimation(LEDCORD, {targetColor : CRGB{0, 0, 0}, lenFrames : 400});
}

void loop()
{
  int newFrameMillis = millis();

  if (random() % 100 == 1)
  {
    int x = random() % 16;
    int y = random() % 16;

    ledAnimationController.addAnimation(x, y, {targetColor : CRGB{255, 255, 255}, lenFrames : 400});
    ledAnimationController.addAnimation(x, y, {targetColor : CRGB{128, 0, 0}, lenFrames : 800});
    ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 128, 0}, lenFrames : 800});
    ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 0, 128}, lenFrames : 800});
    ledAnimationController.addAnimation(x, y, {targetColor : CRGB{0, 0, 0}, lenFrames : 400});
  }

  ledAnimationController.animate(newFrameMillis - lastFrameMillis);
  lastFrameMillis = newFrameMillis;
  FastLED.show();
}