// AnalogIn.hpp
#pragma once
#include <Arduino.h>
#include "Util.hpp"

class AnalogIn {
public:
    calc_data <float> voltage {5};

    explicit AnalogIn(uint8_t pin,
                      float vRef = 6.0f,
                      uint8_t bits = 10,
                      uint8_t avgSamples = 1)
        : _pin(pin), _vRef(vRef), _bits(bits), _avgSamples(avgSamples) {}

    // 基本读取接口
    inline int   readRaw() {
        long sum = 0;
        for (uint8_t i = 0; i < _avgSamples; ++i) {
            sum += analogRead(_pin);
        }
        return static_cast<int>(sum / _avgSamples);
    }

    inline float readVoltage() {
        int raw = readRaw();
        int maxRaw = (1 << _bits) - 1;
        float tmp = (raw * _vRef) / maxRaw;
        voltage.add(tmp);
        return tmp;
    }

    inline float readPercent() {
        int raw = readRaw();
        int maxRaw = (1 << _bits) - 1;
        return 100.0f * raw / maxRaw;
    }

    // 配置
    inline void setVRef(float vRef)           { _vRef = vRef; }
    inline void setResolution(uint8_t bits)   { _bits = bits; }
    inline void setAveraging(uint8_t samples) { _avgSamples = samples; }
    inline void setReference(uint8_t arAnalogRef) { analogReference(arAnalogRef); }

    void loop(){
        if(millis() - _tick < 200) return;
        _tick = millis();
        float tmp = readVoltage();
        if(millis() - _tick2 < 1000) return;
        _tick2 = millis();
        uint8_t life = 0;
        if (tmp >= 4.2) {
            life = 10;
        }else if (tmp <= 3.2){
            life = 0;
        }else{
            life = (tmp - 3.2) * 10;
        }
        Serial.printf("%d", life);
    }

    uint8_t _pin;
    float   _vRef;
    uint8_t _bits;
    uint8_t _avgSamples;
    uint32_t _tick = 0;
    uint32_t _tick2 = 0;
};