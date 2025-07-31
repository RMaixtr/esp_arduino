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

AnalogIn adc(ADC);

MPU6050_Class mpu6050(0x68, Wire);

Buzzer buzzer(BUZZER);
Motor motor(MOTOR);

SerialJson parser;

#include "Handle.hpp"

void I2C_ClearBus() {
  pinMode(SDA, INPUT_PULLUP);
  pinMode(SCL, INPUT_PULLUP);
  
  // 发送9个时钟脉冲，强制MPU6050释放SDA
  for (int i = 0; i < 9; i++) {
    digitalWrite(SCL, LOW);
    pinMode(SCL, OUTPUT);
    delayMicroseconds(5);
    pinMode(SCL, INPUT_PULLUP);
    delayMicroseconds(5);
  }
  
  // 发送一个STOP信号
  digitalWrite(SDA, LOW);
  pinMode(SDA, OUTPUT);
  delayMicroseconds(5);
  pinMode(SCL, OUTPUT);
  delayMicroseconds(5);
  pinMode(SDA, INPUT_PULLUP);
  delayMicroseconds(5);
}


namespace user {

    void begin(void){
        I2C_ClearBus();  // 清除I2C总线，防止MPU6050卡住
        Wire.begin(SDA, SCL);
        mpu6050.begin();
        buzzer.begin();
        motor.begin();
        parser.begin(Serial);
        parser.add("vibrate", vibrate);
        parser.add("play", play);
        key_begin();
        pinMode(POWER,OUTPUT);
        pinMode(BACKLIGHT, OUTPUT);
        power_on();
        backlight_set(255);
    }

    void loop(void){
        buzzer.loop();
        motor.loop();
        mpu6050.loop();
        parser.loop();
        key_loop();
        power_loop();
    }
}