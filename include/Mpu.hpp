#pragma once
#include <Arduino.h>
#include <Wire.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"
#include "Util.hpp"


struct Raw6 {
    int16_t ax, ay, az, gx, gy, gz;
};

struct AttitudeRad {
    float yaw, pitch, roll;   // 单位：弧度
};

struct AttitudeDeg {
    float yaw, pitch, roll;   // 单位：度
};

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


class MPU6050_Class {
public:

    MPU6050   mpu;
    uint8_t   _addr;
    TwoWire   &_wire;
    uint8_t   _fifo[64]{};
    
    Raw6 raw6;
    AttitudeRad attitude;
    
    uint32_t imu_tick = 0;
    
    int16_t raw_g[3];
    float   ypr[3];

    calc_data <int16_t> gx, gy, gz {IMU_LIMIT};
    calc_data <uint16_t> gv {IMU_LIMIT}; // 速度平方和
    calc_data <float> yaw, pitch, roll, beta {IMU_LIMIT};

    explicit MPU6050_Class(uint8_t addr = 0x68, TwoWire &w = Wire): mpu(addr, &w), _addr(addr),  _wire(w) {}

    bool begin(){
        // _wire.begin();
        while (mpu.testConnection()==0){
            I2C_ClearBus();
        }
        
        mpu.initialize();
        if (mpu.dmpInitialize() != 0) {return false; printf("mpu.dmpInitialize() != 0\n");};
        calibrate(6);
        mpu.setDMPEnabled(true);
        return true;
    }
    void calibrate(uint8_t n) {
        mpu.CalibrateAccel(n);
        mpu.CalibrateGyro(n);
    }      // 加速度/陀螺仪校准

    /* 原始 6 轴 */
    Raw6 getRaw6(){
        mpu.getMotion6(&raw6.ax, &raw6.ay, &raw6.az, &raw6.gx, &raw6.gy, &raw6.gz);
        // gx.add(raw6.gx);
        // gy.add(raw6.gy);
        gz.add(raw6.gz);

        gv.add(raw6.gx*raw6.gx + raw6.gy*raw6.gy + raw6.gz*raw6.gz);

        // tmp = acos(raw6.gz / tmp) * (180.0 / M_PI);
        // beta.add(tmp);
        // printf("%f \n", tmp);
        return raw6;
    }

    /* 姿态：弧度（直接来自 DMP，零拷贝） */
    AttitudeRad getAttitudeRad(){

        Quaternion q;
        VectorFloat gravity;
        float ypr[3]{};                         // 官方顺序：yaw, pitch, roll

        if (mpu.dmpGetCurrentFIFOPacket(_fifo)) {
            mpu.dmpGetQuaternion(&q, _fifo);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        }
        // ypr[2] -= 0.5; // roll 偏移，修正为 0
        yaw.add(ypr[0]);
        // pitch.add(ypr[1]);
        // roll.add(ypr[2]);
        attitude.yaw   = ypr[0];
        attitude.pitch = ypr[1];
        attitude.roll  = ypr[2];
        return {ypr[0], ypr[1], ypr[2]};        // 弧度
    }


    /* 姿态：角度（弧度 × 180/π） */
    AttitudeDeg getAttitudeDeg(){
        AttitudeRad r = getAttitudeRad();
        return {r.yaw   * RAD_TO_DEG,
                r.pitch * RAD_TO_DEG,
                r.roll  * RAD_TO_DEG};
    }

    // uint32_t test_tick;
    void loop(){
        if(millis() - imu_tick < 20) return;
        imu_tick = millis();
        getRaw6();
        getAttitudeRad();
        // printf("%f, %f, %f \n", attitude.yaw, attitude.pitch, attitude.roll);
        // printf("%d \n", gv.mean);
        // printf("%d \n", raw6.gz);
        // printf("%6d, %6d, %6d \n", raw6.ax, raw6.ay, raw6.az);
    }

};
