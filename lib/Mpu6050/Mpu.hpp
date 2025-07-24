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

#define IMU_LIMIT 32

template <typename T>
class calc_data
{
public:
    int index = 0;
    bool is_full = false;
    T data[IMU_LIMIT];
    T max = -65535;
    T min = 65535;
    T abs_max = 0;
    T sum = 0;
    double sum2 = 0;
    T mean;
    T rms;

    calc_data(){
    }

    void add (T d){
        if (d>max) max = d;
        if (d<min) min = d;
        sum += d;
        sum2 += d * d;
        T tmp = data[index];
        data[index++] = d;
        if (is_full){
            sum -= tmp;
            sum2 -= tmp * tmp;
            mean = sum / IMU_LIMIT;
            rms = sqrt(sum2 / IMU_LIMIT);
            if (tmp == max){
                max = - 32767;
                for (size_t i = 0; i < IMU_LIMIT; i++){
                    if (data[i] > max) max = data[i];
                }
            }
            if (tmp == min){
                min =  32767;
                for (size_t i = 0; i < IMU_LIMIT; i++){
                    if (data[i] < min) min = data[i];
                }
            }
        }
        abs_max = abs(max) > abs(min) ? abs(max) : abs(min);
        if (index >= IMU_LIMIT){index = 0; is_full = true;};
        
    }
};



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

    calc_data <int16_t> gx, gy, gz;
    calc_data <float> yaw, pitch, roll;

    explicit MPU6050_Class(uint8_t addr = 0x68, TwoWire &w = Wire): mpu(addr, &w), _addr(addr),  _wire(w) {}

    bool begin(){
        // _wire.begin();
        mpu.initialize();
        if (mpu.dmpInitialize() != 0) {return false; printf("mpu.dmpInitialize() != 0\n");};
        calibrate(6);
        mpu.setDMPEnabled(true);
        return true;
    }
    void calibrate(uint8_t n) {
        // mpu.CalibrateAccel(n);
        mpu.CalibrateGyro(n);
    }      // 加速度/陀螺仪校准

    /* 原始 6 轴 */
    Raw6 getRaw6(){
        mpu.getMotion6(&raw6.ax, &raw6.ay, &raw6.az, &raw6.gx, &raw6.gy, &raw6.gz);
        gx.add(raw6.gx);
        gy.add(raw6.gy);
        gz.add(raw6.gz);
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
            // 官方签名：dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity)
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        }
        yaw.add(ypr[0]);
        pitch.add(ypr[1]);
        roll.add(ypr[2]);
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

    void loop(){
        if(millis() - imu_tick < 20) return;
            imu_tick = millis();
            getRaw6();
            getAttitudeRad();
    }

};
