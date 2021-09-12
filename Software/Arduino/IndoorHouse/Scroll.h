#ifndef MY_SCROLL_H
#define MY_SCROLL_H

#include <Arduino.h>
#include "Definitions.h"
#include <Adafruit_SSD1306_WEMOS.h>

class Scroll {
private:
  static const uint8_t ROWS = 4;
  String rows[ROWS];

  int x;
  int minX;
  uint16_t calcLength(String s, uint8_t textSize);
  void config();

public:
  Scroll();
  void clear(Adafruit_SSD1306_WEMOS *display);
  void update(void);
  void setRow(uint8_t row, String s);
  void show(Adafruit_SSD1306_WEMOS *display);
};

#endif