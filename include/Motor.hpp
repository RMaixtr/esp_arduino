#pragma once
#include <Arduino.h>

class Motor {
    public:
        explicit Motor(uint8_t pin) : _pin(pin), _start(0), _duration(0),
                                       _melody(nullptr), _idx(0) {}
    
        // 初始化
        void begin() { pinMode(_pin, OUTPUT); }
    
        // 非阻塞单音
        void setIntensity(uint8_t intensity, uint32_t ms) {
            _intensity = constrain(intensity, 0, 255);
            _duration = ms;
            _start    = millis();
            analogWrite(_pin, _intensity);
        }
    
        // 立即停止
        // 停止函数
        void stop() {
            // 将持续时间设置为0
            _duration = 0;
            // 将引脚的PWM输出设置为0
            analogWrite(_pin, 0);
        }
    
        // 非阻塞播放旋律
        // melody = {NOTE_C4,500, NOTE_E4,500, NOTE_G4,500, 0,0}
        void vibrate(const uint16_t *melody) {
            _melody = melody;
            _idx    = 0;
            nextNote();
        }
    
        // 必须在 loop() 中调用
        void loop() {
            if (_melody) {                 // 正在播放歌曲
                if (!isPlaying()) {
                    _idx += 2;             // NOTE,DURATION 两字节
                    nextNote();
                }
            } else {                       // 单音模式
                if (_duration && (millis() - _start >= _duration)) {
                    stop();
                }
            }
        }
    
        bool isPlaying() const {
            if (_melody) {
                return millis() - _start < _melody[_idx + 1];
            }
            return _duration && (millis() - _start < _duration);
        }
    
    private:
        void nextNote() {
            if (!_melody || (_melody[_idx] == 0 && _melody[_idx + 1] == 0)) { // 结束
                _melody = nullptr;
                stop();
                return;
            }
            _intensity = _melody[_idx];
            analogWrite(_pin, _intensity);
            _start    = millis();
            _duration = _melody[_idx + 1];
        }
    
        uint8_t  _pin;
        uint32_t _tick;
        uint32_t _start, _duration;
        uint8_t _intensity;
        const uint16_t *_melody;
        uint16_t _idx;
    };