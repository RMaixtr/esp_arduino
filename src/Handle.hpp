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

    // 晃
    if (mpu6050.gx.rms > 4000 || mpu6050.gy.rms > 4000 || mpu6050.gz.rms > 4000 ) {
        digital &= 0xfb;
    }else{
        digital |= 0x04;
    }
    // 甩
    if (mpu6050.gx.abs_max > 7000 || mpu6050.gy.abs_max > 7000 || mpu6050.gz.abs_max > 7000 ) {
        digital &= 0xf7;
    }else{
        digital |= 0x08;
    }
    // 倒
    if (abs(mpu6050.attitude.roll) > 0.8 || abs(mpu6050.attitude.pitch) > 0.8 ) {
        digital &= 0xef;
    }else{
        digital |= 0x10;
    }
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

extern Buzzer buzzer;
extern Motor motor;

uint16_t vibrate_data[1024];
bool vibrate(const JsonObjectConst &p){
    JsonArrayConst melody = p["melody"];
    size_t len = melody.size();
    for (size_t i = 0; i < len; i++) {
        vibrate_data[i] = melody[i].as<uint16_t>();
    }
    motor.vibrate(vibrate_data);
    return true;
}

uint16_t play_data[1024];
bool play(const JsonObjectConst &p){
    JsonArrayConst melody = p["melody"];
    size_t len = melody.size();
    for (size_t i = 0; i < len; i++) {
        play_data[i] = melody[i].as<uint16_t>();
    }
    buzzer.playMelody(play_data);
    return true;
}


#define ADC_HISTORY_LEN 5

int  rawHistory[ADC_HISTORY_LEN];
uint8_t histIndex = 0;
bool initFlag = false;

// 仅返回趋势：+1=上升，0=持平，-1=下降，-2=未初始化
int trend() {
    if (!initFlag) return -2;
    int *h = rawHistory;
    int  s = 0;
    for (uint8_t i = 1; i < ADC_HISTORY_LEN; ++i)
        s += (h[(histIndex+i)%ADC_HISTORY_LEN] > h[(histIndex+i-1)%ADC_HISTORY_LEN]) ? 1
            :(h[(histIndex+i)%ADC_HISTORY_LEN] < h[(histIndex+i-1)%ADC_HISTORY_LEN]) ? -1 : 0;
    return (s > 0) - (s < 0);
}
