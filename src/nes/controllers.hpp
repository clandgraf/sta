#pragma once

#include <cstdint>

#include "core/gamepad.hpp"

class Port {
public:
    virtual void update() = 0;
    virtual void write(uint8_t v) = 0;
    virtual uint8_t read() = 0;
};

enum ButtonType {
    Up, Down, Left, Right, ButtonA, ButtonB, Start, Select
};

enum Button {
    Up_0, Down_0, Left_0, Right_0, ButtonA_0, ButtonB_0, Start_0, Select_0,
    Up_1, Down_1, Left_1, Right_1, ButtonA_1, ButtonB_1, Start_1, Select_1
};

class Controller : public Port
{
    bool m_latched;
    uint8_t m_shiftButtons;
    int m_gamepadIndex;

public:
    Controller(int gamepadIndex)
        : m_latched(false)
        , m_shiftButtons(0)
        , m_gamepadIndex(gamepadIndex) {}

    void update();
    void write(uint8_t v);
    uint8_t read();
};
