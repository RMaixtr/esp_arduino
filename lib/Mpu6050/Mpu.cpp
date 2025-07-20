#include "Mpu.hpp"


/*---- 构造 ----*/
MPU6050_Class::MPU6050_Class(uint8_t addr, TwoWire &w): mpu(addr, &w), _addr(addr),  _wire(w) {}

bool MPU6050_Class::begin() {

    // _wire.begin();
    mpu.initialize();
    if (mpu.dmpInitialize() != 0) return false;
    else {
        printf("MPU6050: DMP init success!\n");
        mpu.setDMPEnabled(true);

        mpu.dmpGetFIFOPacketSize(); 
    }
    mpu.setMotionDetectionDuration(1);
    _ready = true;
    return true;
}

bool MPU6050_Class::isReady() const { return _ready; }

void MPU6050_Class::calibrate(uint8_t n) {
    mpu.CalibrateAccel(n);
    mpu.CalibrateGyro(n);
}

Raw6 MPU6050_Class::getRaw6(){
    Raw6 r;
    mpu.getMotion6(&r.ax, &r.ay, &r.az, &r.gx, &r.gy, &r.gz);
    return r;
}

/* 弧度接口：直接调用官方函数，零拷贝 */
AttitudeRad MPU6050_Class::getAttitudeRad() {

    Quaternion q;
    VectorFloat gravity;
    float ypr[3]{};                         // 官方顺序：yaw, pitch, roll

    if (mpu.dmpGetCurrentFIFOPacket(_fifo)) {
        mpu.dmpGetQuaternion(&q, _fifo);
        mpu.dmpGetGravity(&gravity, &q);
        // 官方签名：dmpGetYawPitchRoll(float *data, Quaternion *q, VectorFloat *gravity)
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    }
    return {ypr[0], ypr[1], ypr[2]};        // 弧度
}

/* 角度接口：弧度 × 180/π */
AttitudeDeg MPU6050_Class::getAttitudeDeg() {
    AttitudeRad r = getAttitudeRad();
    return {r.yaw   * RAD_TO_DEG,
            r.pitch * RAD_TO_DEG,
            r.roll  * RAD_TO_DEG};
}
