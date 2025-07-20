#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

struct Raw6 {
    int16_t ax, ay, az, gx, gy, gz;
};

struct AttitudeRad {
    float yaw, pitch, roll;   // 单位：弧度
};

struct AttitudeDeg {
    float yaw, pitch, roll;   // 单位：度
};

class MPU6050_Class {
public:
    explicit MPU6050_Class(uint8_t addr = 0x68,
                           TwoWire &wirePort = Wire);

    bool begin();                       // 初始化 + DMP
    bool isReady() const;               // DMP 是否就绪
    void calibrate(uint8_t n = 6);      // 加速度/陀螺仪校准

    /* 原始 6 轴 */
    Raw6 getRaw6();

    /* 姿态：弧度（直接来自 DMP，零拷贝） */
    AttitudeRad getAttitudeRad();

    /* 姿态：角度（弧度 × 180/π） */
    AttitudeDeg getAttitudeDeg();

private:
    MPU6050   mpu;
    uint8_t   _addr;
    TwoWire   &_wire;
    bool      _ready = false;
    uint8_t   _fifo[64]{};

};
