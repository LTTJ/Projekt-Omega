#include "Menu.h"

Menu::Menu() {
}

void Menu::increment(void) {
  this->index++;
  this->index %= MODE_COUNT;
  this->menuChanged = true;
}

void Menu::decrement(void) {
  this->index--;
  this->index %= MODE_COUNT;
  this->menuChanged = true;
}

void Menu::show(Adafruit_SSD1306_WEMOS *display) {
  display->clearDisplay();
  // display->setCursor(55, 0);
  // display->print(this->index);
  // display->setCursor(55, 40);
  // display->print(this->selectedMode);
  for (int i = 0; i < 4; i++) {
    display->setCursor(0, i * 12);
    if (i == 0) {
      display->setTextColor(BLACK, WHITE);
    } else {
      display->setTextColor(WHITE);
    }
    display->print(MODES[(this->index + i) % MODE_COUNT]);
  }
  display->display();
  this->menuChanged = false;
}

uint8_t Menu::getIndex(void) {
  return this->index;
}

uint8_t Menu::getSelectedMode(void) {
  return this->selectedMode;
}

bool Menu::hasChanged(void) {
  return this->menuChanged;
}

void Menu::select(void) {
  this->selectedMode = this->index;
}

void Menu::setSelectedMode(uint8_t mode) {
  mode %= MODE_COUNT;
  this->index = mode;
  this->selectedMode = mode;
  this->menuChanged = true;
}
