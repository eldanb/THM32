#include <Arduino.h>
#include <FastLED.h>

#define NUM_LEDS 256
#define DATA_PIN 16

CRGB leds[NUM_LEDS];

void setup()
{
  Serial.begin(115200);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
}

int frame = 0;
void loop()
{
  // Serial.println("Ok");

  for (int i = 0; i < 256; i++)
  {
    int base = (50 - min(abs(frame - i * 12), 50)) * 2;
    leds[i].r = base * ((i % 10) > 3 ? 1 : 0);
    leds[i].g = base * ((i % 10) < 7 ? 1 : 0);
    leds[i].b = base * ((i % 10) > 2 ? 1 : 0);
  }
  //  leds[0] = CRGB::White;
  FastLED.show();
  //  leds[0] = CRGB::Black;
  //  FastLED.show();
  //  delay(500);
  frame = frame + 1;
  if (frame > 256 * 12)
  {
    frame = 0;
  }
}