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
        readVoltage();
        // if (voltage.max - voltage.min > 0.06 && voltage.is_full){
        //     if (voltage.is_max_new){
        //         printf("voltage max\n");
        //     } else {
        //         printf("voltage min\n");
        //     }
        // }
        // printf("%f %f\n",voltage.max - voltage.min, readVoltage());
        
    }

private:
    uint8_t _pin;
    float   _vRef;
    uint8_t _bits;
    uint8_t _avgSamples;
    uint32_t _tick = 0;
};