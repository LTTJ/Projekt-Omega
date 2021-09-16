/*
By Jonas Pohl
~ September 2021
*/

#include <Wire.h>
#include "Definitions.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306_WEMOS.h>
#include "Pipe.h"
#include "Bird.h"

#define OLED_RESET 0
Adafruit_SSD1306_WEMOS display(OLED_RESET);
Pipe pipes[4] = { (64), (96), (128), (160) };

Bird bird;
uint64_t lastPress;
const long DEBOUNCE = 25;


void setup() {


  pinMode(BTN, INPUT);
  lastPress = millis();
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(WHITE);
  display.clearDisplay();
}



void loop() {
  if (bird.isDead()) {
    display.clearDisplay();
    display.setCursor(0, DISPLAY_HEIGHT/2);
    display.setTextSize(1);
    display.print("Game Over");
    display.display();
    return;
  }


  if (digitalRead(BTN) == LOW) {
    if (millis() - lastPress >= DEBOUNCE) {
      bird.fly();
      lastPress = millis();
    }
  }

  display.clearDisplay();

  display.setCursor(0, 0);
  display.print(Pipe::score);

  for (int i = 0; i < 4; i++) {
    pipes[i].update();
    pipes[i].show(&display);
    bird.check(&pipes[i]);
  }
  bird.update();
  bird.show(&display);

  display.display();
}