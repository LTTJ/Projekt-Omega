#ifndef MY_BIRD_H
#define MY_BIRD_H

#include <Arduino.h>
#include "Definitions.h"
#include <Adafruit_SSD1306_WEMOS.h>
#include "Pipe.h"

class Bird {
private:
  const uint8_t x = 8;
  uint8_t y;
  int8_t speed;
  int8_t acc;
  const int8_t UP_FORCE = -3;
  const int8_t UP_CONSTRAIN = -8;
  const uint8_t DOWN_CONSTRAIN = 5;

  const uint8_t r = 2;

  bool dead = false;

public:
  Bird();
  void update();
  void show(Adafruit_SSD1306_WEMOS *display);
  void fly();
  bool isDead();
  void check(Pipe *pipe);
};
#endif