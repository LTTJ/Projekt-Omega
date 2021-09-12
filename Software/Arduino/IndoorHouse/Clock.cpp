/*
By Jonas Pohl
~ September 2021
*/

#include "Clock.h"

double Clock::toRad(double deg) {
  return deg * PI / 180;
}

void Clock::displayAnalog(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  hours %= 12;
  minutes %= 60;
  seconds %= 60;
  display->drawCircle(this->x, this->y, this->r, WHITE);
  display->drawLine(this->x, this->y, cos(toRad((hours + (minutes / 60.0f)) * 30 - 90)) * this->hourPointer + this->x, sin(toRad((hours + minutes / 60.0f) * 30 - 90)) * this->hourPointer + this->y, WHITE);  // hour
  display->drawLine(this->x, this->y, cos(toRad(minutes * 6 - 90)) * this->minutePointer + this->x, sin(toRad(minutes * 6 - 90)) * this->minutePointer + this->y, WHITE);                                      // mintes
  display->drawLine(this->x, this->y, cos(toRad(seconds * 6 - 90)) * this->r + this->x, sin(toRad(seconds * 6 - 90)) * this->r + this->y, WHITE);                                                              // seconds
}

void Clock::displayAnalog(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes) {
  hours %= 12;
  minutes %= 60;
  display->drawCircle(this->x, this->y, this->r, WHITE);
  display->drawLine(this->x, this->y, cos(toRad((hours + (minutes / 60.0f)) * 30 - 90)) * this->hourPointer + this->x, sin(toRad((hours + minutes / 60.0f) * 30 - 90)) * this->hourPointer + this->y, WHITE);  // hour
  display->drawLine(this->x, this->y, cos(toRad(minutes * 6 - 90)) * this->minutePointer + this->x, sin(toRad(minutes * 6 - 90)) * this->minutePointer + this->y, WHITE);                                      // mintes
}

void Clock::displayDigital(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  hours %= 24;
  minutes %= 60;
  seconds %= 60;
  display->setCursor(0, 0);
  display->setTextSize(1);
  if (hours < 10) {
    display->print(0);
  }
  display->print(hours);
  display->print(F(":"));
  if (minutes < 10) {
    display->print(0);
  }
  display->print(minutes);
  display->print(F(":"));
  if (seconds < 10) {
    display->print(0);
  }
  display->println(seconds);
}

void Clock::displayMixed(Adafruit_SSD1306_WEMOS *display, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  hours %= 24;
  minutes %= 60;
  seconds %= 60;
  display->setCursor(DISPLAY_WIDTH / 2 - 12, DISPLAY_HEIGHT / 2);
  display->setTextSize(1);
  if (hours < 10) {
    display->print(0);
  }
  display->print(hours);
  display->print(F(":"));
  if (minutes < 10) {
    display->print(0);
  }
  display->print(minutes);

  // draw Points for seconds
  for (uint8_t i = 0; i < seconds; i++) {
    display->fillCircle(cos(toRad(i * 6 - 90)) * this->r + this->x, sin(toRad(i * 6 - 90)) * this->r + this->y, 1, WHITE);  // seconds
  }
}