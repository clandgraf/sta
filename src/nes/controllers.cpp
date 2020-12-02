#include "nes/controllers.hpp"

const std::map<Button, Gamepad::Button> controllerMapping = {
    { Up_0, Gamepad::Up_0 }, { Down_0, Gamepad::Down_0 }, { Left_0, Gamepad::Left_0 }, { Right_0, Gamepad::Right_0 },
    { ButtonA_0, Gamepad::ButtonA_0 }, { ButtonB_0, Gamepad::ButtonX_0 }, { Start_0, Gamepad::Start_0 }, { Select_0, Gamepad::Select_0 },
    { Up_1, Gamepad::Up_1 }, { Down_1, Gamepad::Down_1 }, { Left_1, Gamepad::Left_1 }, { Right_1, Gamepad::Right_1 },
    { ButtonA_1, Gamepad::ButtonA_1 }, { ButtonB_1, Gamepad::ButtonX_1 }, { Start_1, Gamepad::Start_1 }, { Select_1, Gamepad::Select_1 },
};

static bool isPressed(ButtonType btn, int off) {
    Button b = (Button)(btn + off);
    return Gamepad::isPressed(controllerMapping.find(b)->second);
}

void Controller::update() {
    if (m_latched) {
        int off = (8 * m_gamepadIndex);

        m_shiftButtons = 0;
        if (isPressed(ButtonA, off)) { m_shiftButtons |= 0b00000001; }
        if (isPressed(ButtonB, off)) { m_shiftButtons |= 0b00000010; }
        if (isPressed(Select, off))  { m_shiftButtons |= 0b00000100; }
        if (isPressed(Start, off))   { m_shiftButtons |= 0b00001000; }
        if (isPressed(Up, off))      { m_shiftButtons |= 0b00010000; }
        if (isPressed(Down, off))    { m_shiftButtons |= 0b00100000; }
        if (isPressed(Left, off))    { m_shiftButtons |= 0b01000000; }
        if (isPressed(Right, off))   { m_shiftButtons |= 0b10000000; }
    }
}

void Controller::write(uint8_t value) {
    m_latched = value & 0x01;
    update();
}

uint8_t Controller::read() {
    update();

    uint8_t v = m_shiftButtons & 0b00000001;
    m_shiftButtons >>= 1;
    return v;
}
