#include "stdint.h"
#include <sys/types.h>
#include <Adafruit_NeoPixel.h>

Adafruit_NeoPixel pixels(256, 32, NEO_GRB + NEO_KHZ800);

//time
unsigned int startTime = millis();

void PixelsOff() {
  pixels.begin();
  pixels.show();
  uint8_t red = 0;
  uint8_t green = 0;
  uint8_t blue = 0;
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.show();
}

void NeoConnect() {
  pixels.begin();
  pixels.show();

  if (WiFi.status() != WL_CONNECTED) {  //check if while works better
    for (int b = 0; b <= 255;) {
      uint8_t red = 0;
      uint8_t green = 150;
      uint8_t blue = 0;
      pixels.setPixelColor(0, pixels.Color(red, green, blue));
      pixels.setBrightness(b);
      pixels.show();
      b++;
    }
    for (int b = 255; b >= 0;) {
      uint8_t red = 0;
      uint8_t green = 150;
      uint8_t blue = 0;
      pixels.setPixelColor(0, pixels.Color(red, green, blue));
      pixels.setBrightness(b);
      pixels.show();
      b--;
    }
    switch (WiFi.status() == WL_CONNECTED) {
      case 1:
        PixelsOff();
        break;
      default:;
    }
  }
}

void NeoError() {
  PixelsOff();
  uint8_t red = 255;
  uint8_t green = 0;
  uint8_t blue = 0;
  pixels.setPixelColor(0, pixels.Color(red, green, blue));
  pixels.setBrightness(155);
  pixels.show();
  while (millis() - startTime < 5000) {
    PixelsOff();
  }
}
