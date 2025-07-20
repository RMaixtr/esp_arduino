#pragma once
#include <Arduino.h>
#include "define.hpp"

#define KeyInterval 200

uint8_t Key_Val = 0xff;
uint8_t Key_Down = 0;
uint8_t Key_Up = 0;
uint8_t Key_Old = Key_Val;
uint32_t Key_tick = 0;

void key_begin(void){
    pinMode(KEY, INPUT_PULLUP);
}

uint8_t key_read(void){
    return digitalRead(KEY);
}

void key_loop(void){
    if(millis() - Key_tick < KeyInterval) return;
    Key_tick = millis();

    Key_Val = key_read();
    Key_Down = Key_Val & (Key_Old ^ Key_Val);
    Key_Up = ~Key_Val & (Key_Old ^ Key_Val);
    Key_Old = Key_Val;
}


IRAM_ATTR void touch_interrupt(){
    //Serial.println("touch_interrupt");
}