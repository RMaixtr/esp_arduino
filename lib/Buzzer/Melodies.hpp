#include "Buzzer.hpp"

// 小星星（Twinkle Twinkle Little Star）
const uint16_t Twinkle_Melody[] = {
    NOTE_C4, 400,   0, 30,
    NOTE_C4, 400,   0, 30,
    NOTE_G4, 400,   0, 30,
    NOTE_G4, 400,   0, 30,
    NOTE_A4, 400,   0, 30,
    NOTE_A4, 400,   0, 30,
    NOTE_G4, 800,   0, 30,0, 30,
    NOTE_F4, 400,   0, 30,
    NOTE_F4, 400,   0, 30,
    NOTE_E4, 400,   0, 30,
    NOTE_E4, 400,   0, 30,
    NOTE_D4, 400,   0, 30,
    NOTE_D4, 400,   0, 30,
    NOTE_C4, 800,   0, 30,0, 30,
    0, 0             // 结束标志
};

const uint16_t EMO_JOY_Melody[] = {
    NOTE_C5, 120,
    NOTE_E5, 120,
    NOTE_G5, 160,
    0,       60,   // 小停顿 (需改代码支持)
    NOTE_E5, 120,
    NOTE_G5, 120,
    NOTE_C6, 300,  // 拉长做尾
    0, 0
};

const uint16_t EMO_FEAR_Melody[] = {
    NOTE_C6, 100,   // 突然尖叫
    0,       50,    // 空白，制造惊吓后的僵住
    NOTE_DS5, 80,   // 紧张抖动（小二度）
    NOTE_E5,  80,
    NOTE_DS5, 80,
    NOTE_E5,  80,
    NOTE_A4,  300,  // 惊呼下坠
    NOTE_F4,  200,
    0, 0
};

const uint16_t EMO_TENSION_Melody[] = {
    NOTE_G4, 220,
    NOTE_GS4,200,
    NOTE_A4, 180,
    NOTE_AS4,160,
    NOTE_B4, 140,
    NOTE_C5, 400,  // 悬念拉住
    0, 0
};

const uint16_t EMO_SUCCESS_Melody[] = {
    NOTE_G5, 120,
    NOTE_B5, 120,
    NOTE_D6, 200,
    NOTE_G6, 350,
    0, 0
};

const uint16_t EMO_ERROR_Melody[] = {
    NOTE_DS5, 150,
    NOTE_C5,  350,
    0, 0
};

