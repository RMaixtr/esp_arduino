#include <Arduino.h>
#include "define.hpp"

void setup() {
  Serial.begin(115200);
  user::begin();
}

void loop() {
  user::loop();
}