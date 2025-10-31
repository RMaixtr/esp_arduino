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

extern SerialJson parser;
extern AnalogIn adc;

struct status
{
    uint8_t priority=0;
    uint8_t code=0;
    String msg="";
    uint32_t start_tick=0;
    uint32_t update_tick=0;
    uint8_t flag=0;
    JsonDocument data;
};

status status_now, status_fall, status_shake, status_up, status_down, status_charge, status_touch;

void status_init(void){
    status_fall.priority = 1; status_fall.code = 1; status_fall.msg = "fall"; status_fall.data["show_type"] = "fall";
    status_shake.priority = 2; status_shake.code = 2; status_shake.msg = "shake"; status_shake.data["show_type"] = "shake";
    status_up.priority = 3; status_up.code = 3; status_up.msg = "up"; status_up.data["show_type"] = "up";
    status_down.priority = 3; status_down.code = 4; status_down.msg = "down"; status_down.data["show_type"] = "down";
    status_charge.priority = 1; status_charge.code = 5; status_charge.msg = "charge"; status_charge.data["show_type"] = "charge";
    status_touch.priority = 1; status_touch.code = 6; status_touch.msg = "touch"; status_touch.data["show_type"] = "touch";
}

void status_add(status val={}){
    if (millis() - status_now.update_tick > 1000){
        // 恢复默认状态
        status_now.priority = 0;
        status_now.code = 0;
        status_now.msg = "";
        status_now.start_tick = 0;
        status_now.update_tick = 0;
        status_now.flag = 0;
    }
    if (status_now.code!=0 && status_now.flag==0 && millis() - val.start_tick > 500){
        // printf("status: %s \n", status_now.msg.c_str());
        status_now.flag = 1;
        // parser.send_json(status_now.data);
        Serial.printf("%s", status_now.msg.c_str());
        adc._tick2 = millis();
    }
    if (val.code == 0) return;
    if (val.priority < status_now.priority || status_now.code == 0){
        status_now.code = val.code;
        status_now.priority = val.priority;
        status_now.msg = val.msg;
        status_now.data = val.data;
        status_now.flag = 0;
        status_now.start_tick = millis();
        status_now.update_tick = millis();
    }else if (val.code == status_now.code){
        status_now.update_tick = millis();
    }
}

uint8_t take_flag = 0;
extern AnalogIn adc;
uint8_t digital = 0xff;

uint8_t key_read(void){
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

    if (Key_Down & 0x02){
        status_add(status_touch);
    }

    // if (abs(mpu6050.yaw.abs_max) > 1.3) {
    //     status_add(status_fall);
    // }else 
    if (mpu6050.gz.rms > 300) {
        status_add(status_shake);
    }else{
        if (take_flag == 1 && mpu6050.gv.rms < 15) {
            take_flag = 0;
            status_add(status_down);
        }else if (take_flag == 0 && mpu6050.gv.rms > 1000) {
            take_flag = 1;
            status_add(status_up);
        }
    }

    // if (adc.voltage.max - adc.voltage.min > 0.06 && adc.voltage.is_full){
    //     if (adc.voltage.is_max_new){
    //         status_add(status_charge);
    //     }
    // }
    status_add();
    // printf("key: %4d, %4d, %4d\n", mpu6050.gx.rms, mpu6050.gy.rms, mpu6050.gz.rms);
    // printf("key: %d, %d \n", mpu6050.gz.max, mpu6050.gz.min);
    // printf("rms %d \n", mpu6050.gv.rms);
    // < 5000 放下 > 10000 拿起
    // printf("adc %f \n", adc.readVoltage());
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
