#pragma once
#include <Arduino.h>
#include "define.hpp"

#define KeyInterval 200

uint8_t Key_Val = 0xff;
uint8_t Key_Down = 0;
uint8_t Key_Up = 0;
uint8_t Key_Old = Key_Val;
uint32_t Key_tick = 0;

bool touch_check(void);
IRAM_ATTR void touch_interrupt();

void key_begin(void){
    pinMode(KEY, INPUT_PULLUP);
    pinMode(EXIT, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(EXIT), touch_interrupt, FALLING);
}

uint8_t key_read(void){
    uint8_t digital = digitalRead(KEY);
    if (touch_check())
        digital &= 0xfd;
    else
        digital |= 0x02;
    
    return digital;
}

void key_loop(void){
    if(millis() - Key_tick < KeyInterval) return;
    Key_tick = millis();

    Key_Val = key_read();
    Key_Up = Key_Val & (Key_Old ^ Key_Val);
    Key_Down = ~ Key_Val & (Key_Old ^ Key_Val);
    Key_Old = Key_Val;
}

uint32_t touch_tick = 0;
bool touch_check(void){
    return (millis() - touch_tick) < 20;
}
IRAM_ATTR void touch_interrupt(){
    touch_tick = millis();
    // printf("touch_interrupt\n");
}