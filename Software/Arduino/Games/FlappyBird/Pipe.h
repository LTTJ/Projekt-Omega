#ifndef MY_PIPE_H
#define MY_PIPE_H

#include <Arduino.h>
#include "Definitions.h"
#include <Adafruit_SSD1306_WEMOS.h>


class Pipe {
private:
  uint8_t x;
  uint8_t y;
  uint8_t width;
  uint8_t height;
  uint8_t heightBottom;
  const uint8_t SPACER = 25;
  void init();


public:
  static long score;
  Pipe(uint8_t x);
  void update();
  void show(Adafruit_SSD1306_WEMOS *display);

  uint8_t getX();
  uint8_t getWidth();
  uint8_t getHeight();
  uint8_t getHeightBottom();
  uint8_t getSpacer();
};
#endif