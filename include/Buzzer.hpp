#pragma once
#include <Arduino.h>


class Buzzer {
public:
    explicit Buzzer(uint8_t pin) : _pin(pin), _start(0), _duration(0),
                                   _melody(nullptr), _idx(0) {}

    // 初始化
    void begin() { pinMode(_pin, OUTPUT); digitalWrite(_pin, HIGH);}

    // 非阻塞单音
    void tone(uint16_t freqHz, uint32_t ms) {
        _freq     = freqHz;
        _duration = ms;
        _start    = millis();
        ::tone(_pin, _freq);
    }

    // 立即停止
    void noTone() {
        _duration = 0;
        ::noTone(_pin);
        digitalWrite(_pin, HIGH);
    }

    // 非阻塞播放旋律
    // melody = {NOTE_C4,500, NOTE_E4,500, NOTE_G4,500, 0,0}
    void playMelody(const uint16_t *melody) {
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
                noTone();
            }
        }
    }

    bool isPlaying() const {
        if (_melody) {
            return millis() - _start < _melody[_idx+1];
        }
        return _duration && (millis() - _start < _duration);
    }

private:
    void nextNote() {
        if (!_melody || (_melody[_idx] == 0 && _melody[_idx + 1] == 0)) { // 结束
            _melody = nullptr;
            ::noTone(_pin);
            return;
        }
        uint16_t note = _melody[_idx];
        uint16_t len  = _melody[_idx + 1];
        ::tone(_pin, note);
        _start    = millis();
        _duration = len;
    }

    uint8_t  _pin;
    uint32_t _tick;
    uint32_t _start, _duration;
    uint16_t _freq;
    const uint16_t *_melody;
    uint16_t _idx;
};

/* 常用音调宏（频率） */
#define NOTE_B0  31
#define NOTE_C1  33
#define NOTE_CS1 35
#define NOTE_D1  37
#define NOTE_DS1 39
#define NOTE_E1  41
#define NOTE_F1  44
#define NOTE_FS1 46
#define NOTE_G1  49
#define NOTE_GS1 52
#define NOTE_A1  55
#define NOTE_AS1 58
#define NOTE_B1  62
#define NOTE_C2  65
#define NOTE_CS2 69
#define NOTE_D2  73
#define NOTE_DS2 78
#define NOTE_E2  82
#define NOTE_F2  87
#define NOTE_FS2 93
#define NOTE_G2  98
#define NOTE_GS2 104
#define NOTE_A2  110
#define NOTE_AS2 117
#define NOTE_B2  123
#define NOTE_C3  131
#define NOTE_CS3 139
#define NOTE_D3  147
#define NOTE_DS3 156
#define NOTE_E3  165
#define NOTE_F3  175
#define NOTE_FS3 185
#define NOTE_G3  196
#define NOTE_GS3 208
#define NOTE_A3  220
#define NOTE_AS3 233
#define NOTE_B3  247
#define NOTE_C4  262
#define NOTE_CS4 277
#define NOTE_D4  294
#define NOTE_DS4 311
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_FS4 370
#define NOTE_G4  392
#define NOTE_GS4 415
#define NOTE_A4  440
#define NOTE_AS4 466
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_CS5 554
#define NOTE_D5  587
#define NOTE_DS5 622
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_FS5 740
#define NOTE_G5  784
#define NOTE_GS5 831
#define NOTE_A5  880
#define NOTE_AS5 932
#define NOTE_B5  988
#define NOTE_C6  1047
#define NOTE_CS6 1109
#define NOTE_D6  1175
#define NOTE_DS6 1245
#define NOTE_E6  1319
#define NOTE_F6  1397
#define NOTE_FS6 1480
#define NOTE_G6  1568
#define NOTE_GS6 1661
#define NOTE_A6  1760
#define NOTE_AS6 1865
#define NOTE_B6  1976
#define NOTE_C7  2093
#define NOTE_CS7 2217
#define NOTE_D7  2349
#define NOTE_DS7 2489
#define NOTE_E7  2637
#define NOTE_F7  2794
#define NOTE_FS7 2960
#define NOTE_G7  3136
#define NOTE_GS7 3322
#define NOTE_A7  3520
#define NOTE_AS7 3729
#define NOTE_B7  3951
#define NOTE_C8  4186
#define NOTE_CS8 4435
#define NOTE_D8  4699
#define NOTE_DS8 4978