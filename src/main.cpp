#include <Arduino.h>
#include "define.hpp"

void setup() {
  Serial.begin(9600);
  user::begin();
}

void loop() {
  user::loop();
}