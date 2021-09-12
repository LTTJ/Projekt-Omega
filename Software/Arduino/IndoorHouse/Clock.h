#ifndef MY_CLOCK_H
#define MY_CLOCK_H

#include <Arduino.h>
#include "Definitions.h"
#include <Adafruit_SSD1306_WEMOS.h>


class Clock {
private:
  uint8_t x = 32;
  uint8_t y = 24;
  uint8_t r = 23;
  uint8_t hourPointer = r * 0.75;
  uint8_t minutePointer = r;
  double toRad(double deg);

public:
  void displayAnalog(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds);
  void displayAnalog(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes);
  void displayDigital(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds);
  void displayMixed(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds);
};
#endif