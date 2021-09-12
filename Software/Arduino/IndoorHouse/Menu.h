#ifndef MY_MENU_H
#define MY_MENU_H

#include <Arduino.h>
#include <Adafruit_SSD1306_WEMOS.h>
#include "Definitions.h"


class Menu {
private:
  uint8_t index = 0;
  uint8_t selectedMode = 0;
  bool menuChanged = true;

public:
  Menu();
  void increment(void);
  void decrement(void);
  void show(Adafruit_SSD1306_WEMOS *display);
  uint8_t getSelectedMode(void);
  void setSelectedMode(uint8_t mode);
  uint8_t getIndex(void);
  void select(void);
  bool hasChanged(void);
};

#endif