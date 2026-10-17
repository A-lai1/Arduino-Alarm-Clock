#ifndef REMOTE_CODES_H
#define REMOTE_CODES_H

#include <Arduino.h>

namespace Remote {
    inline constexpr uint32_t ONE = 0xBA45FF00;
    inline constexpr uint32_t TWO = 0xB946FF00;
    inline constexpr uint32_t THREE = 0xB847FF00;
    inline constexpr uint32_t FOUR = 0xBB44FF00;
    inline constexpr uint32_t FIVE = 0xBF40FF00;
    inline constexpr uint32_t SIX = 0xBC43FF00;
    inline constexpr uint32_t SEVEN = 0xF807FF00;
    inline constexpr uint32_t EIGHT = 0xEA15FF00;
    inline constexpr uint32_t NINE = 0xF609FF00;
    inline constexpr uint32_t STAR = 0xE916FF00;
    inline constexpr uint32_t ZERO = 0xE619FF00;
    inline constexpr uint32_t POUND = 0xF20DFF00;
    inline constexpr uint32_t UP = 0xE718FF00;
    inline constexpr uint32_t LEFT = 0xF708FF00;
    inline constexpr uint32_t OK = 0xE31CFF00;
    inline constexpr uint32_t RIGHT = 0xA55AFF00;
    inline constexpr uint32_t DOWN = 0xAD52FF00;
}

#endif