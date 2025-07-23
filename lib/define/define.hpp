#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>

#define SDA 4
#define SCL 5
#define KEY 0
#define EXIT 13
#define MOTOR 2
#define BACKLIGHT 14
#define POWER 16
#define ADC A0
#define BUZZER 12
int a = D1;
// val 0-255
#define backlight_set(val) analogWrite(BACKLIGHT, val)

#define POWER_OFF_INTERVAL 3000
#define power_on() digitalWrite(POWER, HIGH)
bool power_off_flag = false;
uint32_t power_off_tick = 0;
void power_off(void){
    power_off_flag = true;
    power_off_tick = millis();
}
void power_loop(void){
    if(!power_off_flag || millis() - power_off_tick < POWER_OFF_INTERVAL) return;
    power_off_flag = false;
    digitalWrite(POWER, LOW);
}

#include "Mpu.hpp"
#include "AnalogIn.hpp"
#include "Buzzer.hpp"
#include "Melodies.hpp"
#include "Motor.hpp"
#include "SerialJson.hpp"
#include "SerialHandle.hpp"
#include "Handle.hpp"

AnalogIn adc(ADC);

MPU6050_Class mpu6050(0x68, Wire);

Buzzer buzzer(BUZZER);
Motor motor(MOTOR);

namespace user {

    void begin(void){
        Wire.begin(SDA, SCL);
        mpu6050.begin();
        buzzer.begin();
        motor.begin();
        key_begin();
        uart_begin();
        pinMode(POWER,OUTPUT);
        pinMode(BACKLIGHT, OUTPUT);
        power_on();
        backlight_set(255);
    }

    void loop(void){
        buzzer.loop();
        motor.loop();
        mpu6050.loop();
        key_loop();
        uart_loop();
        power_loop();
    }
}