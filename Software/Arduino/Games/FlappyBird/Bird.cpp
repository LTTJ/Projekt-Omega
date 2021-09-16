/*
By Jonas Pohl
~ September 2021
*/
#include "Bird.h"

Bird::Bird() {
  this->y = DISPLAY_HEIGHT / 2;
  this->speed = 0;
  this->acc = 0;
}

void Bird::update() {
  this->speed += this->acc;
  this->speed = constrain(this->speed, this->UP_CONSTRAIN, this->DOWN_CONSTRAIN);
  this->acc = 1;  // set to gravity
  this->y += speed;
}

void Bird::show(Adafruit_SSD1306_WEMOS *display) {
  display->drawCircle(this->x, this->y, this->r, WHITE);
}

void Bird::fly() {
  this->acc = this->UP_FORCE;
}

bool Bird::isDead() {
  return this->dead;
}

void Bird::check(Pipe *pipe) {
  if (this->x + this->r < pipe->getX() || this->x - this->r > pipe->getX() + pipe->getWidth())
    return;
  if (this->y + this->r > pipe->getHeightBottom() || this->y - this->r < pipe->getHeight()) {
    this->dead = true;
    return;
  }
}
