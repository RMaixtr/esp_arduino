#pragma once
#include <ArduinoJson.h>
#include "SerialJson.hpp"
#include "define.hpp"
#include "Melodies.hpp"

extern AnalogIn adc;
extern MPU6050_Class mpu6050;
extern Raw6 raw6;
extern AttitudeRad attitude;
extern Buzzer buzzer;
extern Motor motor;
extern uint8_t Key_Val;
extern uint8_t Key_Down;
extern uint8_t Key_Up;

#define UartInterval 200
#define ADC_HISTORY_LEN 5

uint32_t Uart_tick = 0;

SerialJson parser;

bool play(const JsonObjectConst &p){
    int val = p["val"] | 0;
    switch (val)
    {
    case 0:
        buzzer.playMelody(V0_Melody);
        break;
    case 1:
        buzzer.playMelody(V1_Melody);
        break;
    default:
        return false;
    }
    return true;
}

bool vibrate(const JsonObjectConst &p){
    int val = p["val"] | 0;
    int ms = p["ms"] | 0;
    motor.vibrate(val, ms);
    return true;
}

void uart_begin(void){
    
    parser.begin(Serial);
    parser.add("vibrate", vibrate);
    parser.add("play", play);
}

int  rawHistory[ADC_HISTORY_LEN];
uint8_t histIndex = 0;

// 仅返回趋势：+1=上升，-1=下降，0=持平
int trend() {
  int *h = rawHistory;
  int  s = 0;
  for (uint8_t i = 1; i < ADC_HISTORY_LEN; ++i)
      s += (h[(histIndex+i)%ADC_HISTORY_LEN] > h[(histIndex+i-1)%ADC_HISTORY_LEN]) ? 1
          :(h[(histIndex+i)%ADC_HISTORY_LEN] < h[(histIndex+i-1)%ADC_HISTORY_LEN]) ? -1 : 0;
  return (s > 0) - (s < 0);
}

void uart_loop(void){
    parser.loop();
    if(millis() - Uart_tick < UartInterval) return;
    return;
    Uart_tick = millis();

    int adcValue = adc.readRaw();
    rawHistory[histIndex] = adcValue;
    histIndex = (histIndex + 1) % ADC_HISTORY_LEN;

    raw6     = mpu6050.getRaw6();
    attitude = mpu6050.getAttitudeRad();

    /* 2. 构造 JSON */
    JsonDocument doc;   // 256 字节静态 Buffer，足够用
    doc["adc"] = adcValue;
    doc["charging"] = trend();

    JsonObject key = doc["key"].to<JsonObject>();
    key["val"] = Key_Val;
    key["down"] = Key_Down;
    key["up"] = Key_Up;

    // 解析 IMU 数据 获取 晃动 倒下 事件
    // JsonObject imu = doc["imu"].to<JsonObject>();
    // imu["gx"] = raw6.gx;
    // imu["gy"] = raw6.gy;
    // imu["gz"] = raw6.gz;

    // JsonObject att = imu["att"].to<JsonObject>();
    // att["roll"]  = attitude.roll;
    // att["pitch"] = attitude.pitch;
    // att["yaw"]   = attitude.yaw;

    /* 3. 序列化并通过 UART 发送 */
    serializeJson(doc, Serial);   // 直接写到 UART
    Serial.println();             // 加换行方便上位机解析
    
}