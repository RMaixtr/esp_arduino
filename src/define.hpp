#pragma once
#include <Arduino.h>
#include <ArduinoJson.h>
#include "user_interface.h"
#include <ESP8266WiFi.h>

extern "C" {
#include "cont.h"          // 声明 cont_run 和 g_cont
}

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
    WiFi.mode(WIFI_OFF);
    wifi_station_disconnect();
    wifi_set_opmode(NULL_MODE); // set WiFi mode to null mode.
    *((volatile uint32_t*) 0x60000900) &= ~(1);
    yield();
    // pinMode(SDA,INPUT);
    // pinMode(SCL,INPUT);
    // pinMode(KEY,INPUT);
    // pinMode(EXIT,INPUT);
    // pinMode(MOTOR,INPUT);
    // pinMode(BACKLIGHT,INPUT);
    // pinMode(POWER,INPUT);
    // pinMode(ADC,INPUT);
    // pinMode(BUZZER,INPUT);
    // cont_run(g_pcont, NULL);   // 清空 Arduino 调度器队列
    // delay(100);
    // printf("sleep\n");
    Serial.flush();                // 等待 TX 完成

    wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); // set sleep type
    wifi_fpm_open(); // Enables force sleep
    
    gpio_pin_wakeup_enable(GPIO_ID_PIN(KEY), GPIO_PIN_INTR_LOLEVEL); //set wakeup pin
    int i = wifi_fpm_do_sleep(0xFFFFFFF); // Sleep for longest possible time
    delay(5);
    // printf("wake up %d \n", i);
    wifi_fpm_close();
    gpio_pin_wakeup_disable();

    // ESP.deepSleep(0xFFFFFFF); 
    *((volatile uint32_t*) 0x60000900) |= 1;
    power_on();
}

#include "IMU.hpp"
#include "AnalogIn.hpp"
#include "Buzzer.hpp"
#include "Melodies.hpp"
#include "Motor.hpp"
#include "SerialJson.hpp"

AnalogIn adc(ADC);

IMU mpu6050(SDA, SCL);

Buzzer buzzer(BUZZER);
Motor motor(MOTOR);

SerialJson parser;

#include "Handle.hpp"

namespace user {

    void begin(void){
        pinMode(POWER,OUTPUT);
        pinMode(BACKLIGHT, OUTPUT);
        power_off();
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
        // power_off_interval = 5000;
        // power_off_flag = true;
        // power_off_tick = millis();
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