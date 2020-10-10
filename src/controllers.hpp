#pragma once

#include <cstdint>

#include "inputs.hpp"

bool buttonState[16];

const unsigned int BTN_LEFT_0   = 0;
const unsigned int BTN_UP_0     = 1;
const unsigned int BTN_RIGHT_0  = 2;
const unsigned int BTN_DOWN_0   = 3;
const unsigned int BTN_START_0  = 4;
const unsigned int BTN_SELECT_0 = 5;
const unsigned int BTN_A_0      = 6;
const unsigned int BTN_B_0      = 7;
const unsigned int BTN_LEFT_1   = 8;
const unsigned int BTN_UP_1     = 9;
const unsigned int BTN_RIGHT_1  = 10;
const unsigned int BTN_DOWN_1   = 11;
const unsigned int BTN_START_1  = 12;
const unsigned int BTN_SELECT_1 = 13;
const unsigned int BTN_A_1      = 14;
const unsigned int BTN_B_1      = 15;

class Port {
public:
    virtual void update() = 0;
    virtual void write(uint8_t v) = 0;
    virtual uint8_t read() = 0;
};

class Controller : public Port
{
    bool m_latched;
    uint8_t m_shiftButtons;

    const Input::Controller& m_state;

public:
    Controller(const Input::Controller& state) 
        : m_latched(false)
        , m_shiftButtons(0)
        , m_state(state) {}

    void update();
    void write(uint8_t v);
    uint8_t read();
};
