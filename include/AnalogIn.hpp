// AnalogIn.hpp
#pragma once
#include <Arduino.h>

class AnalogIn {
public:
    explicit AnalogIn(uint8_t pin,
                      float vRef = 6.0f,
                      uint8_t bits = 10,
                      uint8_t avgSamples = 1)
        : _pin(pin), _vRef(vRef), _bits(bits), _avgSamples(avgSamples) {}

    // 基本读取接口
    inline int   readRaw() const {
        long sum = 0;
        for (uint8_t i = 0; i < _avgSamples; ++i) {
            sum += analogRead(_pin);
        }
        return static_cast<int>(sum / _avgSamples);
    }

    inline float readVoltage() const {
        int raw = readRaw();
        int maxRaw = (1 << _bits) - 1;
        return raw * _vRef / maxRaw;
    }

    inline float readPercent() const {
        int raw = readRaw();
        int maxRaw = (1 << _bits) - 1;
        return 100.0f * raw / maxRaw;
    }

    // 配置
    inline void setVRef(float vRef)           { _vRef = vRef; }
    inline void setResolution(uint8_t bits)   { _bits = bits; }
    inline void setAveraging(uint8_t samples) { _avgSamples = samples; }
    inline void setReference(uint8_t arAnalogRef) { analogReference(arAnalogRef); }

private:
    uint8_t _pin;
    float   _vRef;
    uint8_t _bits;
    uint8_t _avgSamples;
};