/*
By Jonas Pohl
~ September 2021
*/

#include "Scroll.h"

uint16_t Scroll::calcLength(String s, uint8_t textSize) {
  return 6 * s.length() * textSize;
}

void Scroll::config() {
  this->minX = -1 * calcLength(this->rows[0], 1);

  for (uint8_t i = 1; i < this->ROWS; i++) {
    int val = -1 * calcLength(this->rows[i], 1);
    if (val < this->minX) {
      this->minX = val;
    }
  }
}

Scroll::Scroll() {
  this->x = 0;
  for (uint8_t i = 0; i < this->ROWS; i++) {
    rows[i] = "";
  }
}

void Scroll::clear(Adafruit_SSD1306_WEMOS *display) {
  this->x = 0;
  this->minX = 0;
  for (uint8_t i = 0; i < this->ROWS; i++) {
    rows[i] = "";
  }
}
void Scroll::update(void) {
  this->x = this->x - 1;  // change here to adjust speed of scroll!
  if (this->x < this->minX) {
    this->x = DISPLAY_WIDTH;
  }
}

void Scroll::setRow(uint8_t row, String s) {
  if (row >= this->ROWS || row < 0) {
    return;
  }
  this->rows[row] = s;
  config();
}
void Scroll::show(Adafruit_SSD1306_WEMOS *display) {
  display->clearDisplay();
  for (uint8_t i = 0; i < this->ROWS; i++) {

    if (calcLength(this->rows[i], 1) > DISPLAY_WIDTH) {
      display->setCursor(x, i * 12);
    } else {
      display->setCursor(0, i * 12);
    }
    display->print(this->rows[i]);
  }
  display->display();
}