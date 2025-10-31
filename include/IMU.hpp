#pragma once
#include <Arduino.h>
#include <Wire.h>
#include <MadgwickAHRS.h>
#include <QMI8658.h>
#include "Util.hpp"

struct AttitudeRad {
    float yaw, pitch, roll;   // 单位：弧度
};

struct AttitudeDeg {
    float yaw, pitch, roll;   // 单位：度
};


class IMU {
public:

    QMI8658 imu;
    uint8_t   _sda;
    uint8_t   _scl;

    float accelOffsetX = -1.814, accelOffsetY = 1036.370, accelOffsetZ = -23.222;
    float gyroOffsetX = -0.863, gyroOffsetY = -7.365, gyroOffsetZ = 0.050;
    
    Madgwick filter;

    QMI8658_Data sensorData;
    AttitudeRad attitude;

    calc_data <int16_t> gx, gy, gz {IMU_LIMIT};
    calc_data <uint16_t> gv {IMU_LIMIT}; // 速度平方和
    calc_data <float> yaw, pitch, roll, beta {IMU_LIMIT};

    uint32_t microsPerReading = 40;
    uint32_t microsPrevious;

    explicit IMU(uint8_t sda = 21, uint8_t scl = 22): _sda(sda), _scl(scl) {}

    bool begin(){
        imu.begin(_sda, _scl);
        
        while (imu.getWhoAmI()==0){
            I2C_ClearBus();
        }
        // calibrate();

        // Set accelerometer range (±8g)
        imu.setAccelRange(QMI8658_ACCEL_RANGE_8G);
        
        // Set accelerometer output data rate (1000Hz)
        imu.setAccelODR(QMI8658_ACCEL_ODR_125HZ);
        
        // Set gyroscope range (±512dps)
        imu.setGyroRange(QMI8658_GYRO_RANGE_512DPS);
        
        // Set gyroscope output data rate (1000Hz)
        imu.setGyroODR(QMI8658_GYRO_ODR_125HZ);

        // Set units (DEFAULT: mg for accel, dps for gyro - matches most IMU displays)
        imu.setAccelUnit_mg(true);      // Use mg (like your screen: ACC_X = -965.82)
        imu.setGyroUnit_dps(true);      // Use dps (degrees per second)
        imu.setDisplayPrecision(6);     // 6 decimal places (like your screen)

        // Enable sensors
        imu.enableSensors(QMI8658_ENABLE_ACCEL | QMI8658_ENABLE_GYRO);
        
        // start  filter
        filter.begin(25);
        microsPrevious = millis();
        return true;
    }
    void calibrate() {
        
        for (int i = 3; i > 0; i--) {
            delay(1000);
        }
        
        const int numSamples = 1000;
        float accelSumX = 0, accelSumY = 0, accelSumZ = 0;
        float gyroSumX = 0, gyroSumY = 0, gyroSumZ = 0;
        int validSamples = 0;
        
        for (int i = 0; i < numSamples; i++) {
            QMI8658_Data data;
            if (imu.readSensorData(data)) {
                accelSumX += data.accelX;
                accelSumY += data.accelY;
                accelSumZ += data.accelZ;
                
                gyroSumX += data.gyroX;
                gyroSumY += data.gyroY;
                gyroSumZ += data.gyroZ;
                
                validSamples++;
            }
            
            if (i % 100 == 0) {
                Serial.print(".");
            }
            
            delay(10);
        }
        
        if (validSamples > 0) {
            // Calculate offsets
            accelOffsetX = accelSumX / validSamples;
            accelOffsetY = accelSumY / validSamples;
            accelOffsetZ = (accelSumZ / validSamples) - 9.81; // Remove gravity
            
            gyroOffsetX = gyroSumX / validSamples;
            gyroOffsetY = gyroSumY / validSamples;
            gyroOffsetZ = gyroSumZ / validSamples;
            
            Serial.print("Accel offsets: ");
            Serial.print(accelOffsetX, 3); Serial.print(", ");
            Serial.print(accelOffsetY, 3); Serial.print(", ");
            Serial.println(accelOffsetZ, 3);
            
            Serial.print("Gyro offsets: ");
            Serial.print(gyroOffsetX, 3); Serial.print(", ");
            Serial.print(gyroOffsetY, 3); Serial.print(", ");
            Serial.println(gyroOffsetZ, 3);
            
        } else {
            Serial.println("\n❌ Calibration failed! No valid samples collected.");
        }
    }

    /* 原始 6 轴 */
    bool getRaw6(){
        if (imu.readSensorData(sensorData)) {
            sensorData.accelX -= accelOffsetX;
            sensorData.accelY -= accelOffsetY;
            sensorData.accelZ -= accelOffsetZ;

            sensorData.gyroX -= gyroOffsetX;
            sensorData.gyroY -= gyroOffsetY;
            sensorData.gyroZ -= gyroOffsetZ;
            // filter.updateIMU(sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ, sensorData.accelX, sensorData.accelY, sensorData.accelZ);
            // gx.add(raw6.gx);
            // gy.add(raw6.gy);
            gz.add(sensorData.gyroZ);
            gv.add(sensorData.gyroX*sensorData.gyroX + sensorData.gyroY*sensorData.gyroY + sensorData.gyroZ*sensorData.gyroZ);

            // tmp = acos(raw6.gz / tmp) * (180.0 / M_PI);
            // beta.add(tmp);
            // printf("%f \n", tmp);
            return true;
        }else{
            return false;
        }
    }

    /* 姿态：弧度（直接来自 DMP，零拷贝） */
    AttitudeRad getAttitudeRad(){

        attitude.yaw   = filter.getYaw();
        attitude.pitch = filter.getPitch();
        attitude.roll  = filter.getRoll();
        return attitude;
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
        if (millis() - microsPrevious < microsPerReading) return;
        microsPrevious += microsPerReading;
        getRaw6();
        // getAttitudeRad();
        // printf("%f, %f, %f \n", attitude.yaw, attitude.pitch, attitude.roll);
        // printf("%d, %d \n", gv.mean, gz.rms);

        // printf("%f, %f, %f, %f, %f, %f\n", sensorData.gyroX, sensorData.gyroY, sensorData.gyroZ, sensorData.accelX, sensorData.accelY, sensorData.accelZ);
    
        // printf("%6d, %6d, %6d \n", raw6.ax, raw6.ay, raw6.az);
    }

    void I2C_ClearBus() {
        pinMode(_sda, INPUT_PULLUP);
        pinMode(_scl, INPUT_PULLUP);
        
        // 发送9个时钟脉冲，强制MPU6050释放SDA
        for (int i = 0; i < 9; i++) {
            digitalWrite(_scl, LOW);
            pinMode(_scl, OUTPUT);
            delayMicroseconds(5);
            pinMode(_scl, INPUT_PULLUP);
            delayMicroseconds(5);
        }
        
        // 发送一个STOP信号
        digitalWrite(_sda, LOW);
        pinMode(_sda, OUTPUT);
        delayMicroseconds(5);
        pinMode(_scl, OUTPUT);
        delayMicroseconds(5);
        pinMode(_sda, INPUT_PULLUP);
        delayMicroseconds(5);
    }
};
