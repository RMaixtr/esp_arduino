#pragma once
#include "Buzzer.hpp"

// 小星星（Twinkle Twinkle Little Star）
const uint16_t Twinkle_Melody[] = {
    NOTE_C4, 40,   0, 3,
    NOTE_C4, 40,   0, 3,
    NOTE_G4, 40,   0, 3,
    NOTE_G4, 40,   0, 3,
    NOTE_A4, 40,   0, 3,
    NOTE_A4, 40,   0, 3,
    NOTE_G4, 80,   0, 3, 0, 3,
    NOTE_F4, 40,   0, 3,
    NOTE_F4, 40,   0, 3,
    NOTE_E4, 40,   0, 3,
    NOTE_E4, 40,   0, 3,
    NOTE_D4, 40,   0, 3,
    NOTE_D4, 40,   0, 3,
    NOTE_C4, 80,   0, 3,0, 3,
    0, 0             // 结束标志
};

const uint16_t EMO_JOY_Melody[] = {
    NOTE_C5, 12,
    NOTE_E5, 12,
    NOTE_G5, 16,
    0,       6,   // 小停顿 (需改代码支持)
    NOTE_E5, 12,
    NOTE_G5, 12,
    NOTE_C6, 30,  // 拉长做尾
    0, 0
};

const uint16_t EMO_FEAR_Melody[] = {
    NOTE_C6, 10,   // 突然尖叫
    0,       5,    // 空白，制造惊吓后的僵住
    NOTE_DS5, 8,   // 紧张抖动（小二度）
    NOTE_E5,  8,
    NOTE_DS5, 8,
    NOTE_E5,  8,
    NOTE_A4,  30,  // 惊呼下坠
    NOTE_F4,  20,
    0, 0
};

const uint16_t EMO_TENSION_Melody[] = {
    NOTE_G4, 22,
    NOTE_GS4,20,
    NOTE_A4, 18,
    NOTE_AS4,16,
    NOTE_B4, 14,
    NOTE_C5, 40,  // 悬念拉住
    0, 0
};

const uint16_t EMO_SUCCESS_Melody[] = {
    NOTE_G5, 12,
    NOTE_B5, 12,
    NOTE_D6, 20,
    NOTE_G6, 35,
    0, 0
};

const uint16_t EMO_ERROR_Melody[] = {
    NOTE_DS5, 15,
    NOTE_C5,  35,
    0, 0
};

// 拟声字母 "V" 的滑音
const uint16_t V0_Melody[] = {
    NOTE_A6,  6,   // 爆破起点
    NOTE_GS6, 6,   // 快速下滑
    NOTE_G6,  7,
    0, 5,
    NOTE_A6,  6,   // 爆破起点
    NOTE_GS6, 6,   // 快速下滑
    NOTE_G6,  7,
    0, 0            // 结束
};


// 拟声字母 "V" 的滑音
const uint16_t V1_Melody[] = {
    NOTE_A6,  6,   // 爆破起点
    NOTE_GS6, 6,   // 快速下滑
    NOTE_G6,  7,
    0, 5,
    NOTE_G6,  6,   // 爆破起点
    NOTE_FS6, 6,   // 快速下滑
    NOTE_F6,  7,
    0, 0            // 结束
};