#pragma once
#include <ArduinoJson.h>
#include "SerialJson.hpp"
#include "define.hpp"

extern AnalogIn adc;
extern MPU6050_Class mpu6050;
extern Raw6 raw6;
extern AttitudeRad attitude;

#define UartInterval 200

uint32_t Uart_tick = 0;

SerialJson parser;

void uart_begin(void){
    
    parser.begin(Serial);
}

void uart_loop(void){
    parser.loop();
    if(millis() - Uart_tick < UartInterval) return;
    return;
    Uart_tick = millis();

    int adcValue = adc.readRaw();

    raw6     = mpu6050.getRaw6();
    attitude = mpu6050.getAttitudeRad();

    /* 2. 构造 JSON */
    JsonDocument doc;   // 256 字节静态 Buffer，足够用
    doc["adc"] = adcValue;

    JsonObject imu = doc["imu"].to<JsonObject>();
    imu["gx"] = raw6.gx;
    imu["gy"] = raw6.gy;
    imu["gz"] = raw6.gz;

    JsonObject att = imu["att"].to<JsonObject>();
    att["roll"]  = attitude.roll;
    att["pitch"] = attitude.pitch;
    att["yaw"]   = attitude.yaw;

    /* 3. 序列化并通过 UART 发送 */
    serializeJson(doc, Serial);   // 直接写到 UART
    Serial.println();             // 加换行方便上位机解析
    
}