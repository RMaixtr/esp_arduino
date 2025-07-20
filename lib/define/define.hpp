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

#define power_enable() digitalWrite(POWER, HIGH)
#define power_disable() digitalWrite(POWER, LOW)

// val 0-255
#define backlight_set(val) analogWrite(BACKLIGHT, val)

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
Raw6 raw6;
AttitudeRad attitude;

Buzzer buzzer(BUZZER);
Motor motor(MOTOR);

namespace user {

    void begin(void){
        Wire.begin(SDA, SCL);
        mpu6050.begin();
        // mpu6050.calibrate(6);
        buzzer.begin();
        motor.begin();
        key_begin();
        uart_begin();
        pinMode(POWER,OUTPUT);
        pinMode(BACKLIGHT, OUTPUT);
        pinMode(EXIT, INPUT_PULLUP);
        attachInterrupt(digitalPinToInterrupt(EXIT), touch_interrupt, FALLING);
    }

    void loop(void){
        buzzer.loop();
        motor.loop();
        key_loop();
        uart_loop();
    }
}