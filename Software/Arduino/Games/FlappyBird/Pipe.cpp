/*
By Jonas Pohl
~ September 2021
*/

#include "Pipe.h"

Pipe::Pipe(uint8_t x) {
  this->x = x;
  this->y = 0;
  this->width = 3;

  init();
}

void Pipe::update() {
  this->x--;
  if (this->x == 0) {
    init();
    score++;
    this->x = DISPLAY_WIDTH;
  }
}

long Pipe::score = 0;

void Pipe::show(Adafruit_SSD1306_WEMOS *display) {
  display->fillRect(this->x, this->y, this->width, this->height, WHITE);
  display->fillRect(this->x, this->heightBottom, this->width, DISPLAY_HEIGHT, WHITE);
}

void Pipe::init() {
  this->height = random(2, DISPLAY_HEIGHT-(DISPLAY_HEIGHT/2));
  this->heightBottom = this->height + this->SPACER;
}

uint8_t Pipe::getX() {
  return this->x;
}

uint8_t Pipe::getWidth() {
  return this->width;
}

uint8_t Pipe::getHeight() {
  return this->height;
}

uint8_t Pipe::getHeightBottom() {
  return this->heightBottom;
}

uint8_t Pipe::getSpacer() {
  return this->SPACER;
}