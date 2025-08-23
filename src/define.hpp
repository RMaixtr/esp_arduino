#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#define SDA 4
#define SCL 5
#define KEY 0
#define EXIT 13
#define MOTOR 14
#define BACKLIGHT 2
#define POWER 16
#define ADC A0
#define BUZZER 12

// val 0-255
#define backlight_set(val) analogWrite(BACKLIGHT, val)

#define power_on() digitalWrite(POWER, HIGH);backlight_set(255)
#define power_off() digitalWrite(POWER, LOW);backlight_set(0)


bool power_off_flag = false;
uint32_t power_off_tick = 0;
uint16_t power_off_interval = 1000;
bool power_off_jsrpc(const JsonObjectConst &p){
    power_off_interval = p["val"] | 0;
    power_off_flag = true;
    power_off_tick = millis();
    return true;
}
void power_loop(void){
    if(!power_off_flag || millis() - power_off_tick < power_off_interval) return;
    power_off_flag = false;
    power_off();
}

#include "Mpu.hpp"
#include "AnalogIn.hpp"
#include "Buzzer.hpp"
#include "Melodies.hpp"
#include "Motor.hpp"
#include "SerialJson.hpp"

AnalogIn adc(ADC);

MPU6050_Class mpu6050(0x68, Wire);

Buzzer buzzer(BUZZER);
Motor motor(MOTOR);

SerialJson parser;

#include "Handle.hpp"

namespace user {

    void begin(void){
        pinMode(POWER,OUTPUT);
        pinMode(BACKLIGHT, OUTPUT);
        power_off();
        Wire.begin(SDA, SCL);
        mpu6050.begin();
        buzzer.begin();
        motor.begin();
        parser.begin(Serial);
        parser.add("vibrate", vibrate);
        parser.add("play", play);
        parser.add("power_off", power_off_jsrpc);
        key_begin();
        status_init();
        power_on();
        // buzzer.playMelody(Twinkle_Melody);
    }

    void loop(void){
        adc.loop();
        buzzer.loop();
        motor.loop();
        mpu6050.loop();
        parser.loop();
        key_loop();
        power_loop();
    }
}