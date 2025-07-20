#pragma once
#include <Arduino.h>

class Motor {
public:
    // 构造函数可选
    Motor(uint8_t pin) : _pin(pin), _startTime(0), _duration(0), _intensity(0) {}

    // 初始化
    void begin() {
        pinMode(_pin, OUTPUT);
        analogWrite(_pin, 0);
    }

    // 立即设定强度 (0-255)
    void setIntensity(uint8_t intensity) {
        _intensity = constrain(intensity, 0, 255);
        analogWrite(_pin, _intensity);
    }

    // 非阻塞震动：给定强度与毫秒时长
    void vibrate(uint8_t intensity, uint32_t ms) {
        _intensity  = constrain(intensity, 0, 255);
        _duration   = ms;
        _startTime  = millis();
        analogWrite(_pin, _intensity);
    }

    // 立即停止
    void stop() {
        _duration = 0;
        analogWrite(_pin, 0);
    }

    void loop() {
        if (_duration && (millis() - _startTime >= _duration)) {
            stop();
        }
    }

    bool isRunning() const {
        return _duration && (millis() - _startTime < _duration);
    }

private:
    uint8_t _pin;
    uint32_t _tick;
    uint32_t _startTime;
    uint32_t _duration;
    uint8_t  _intensity;
};