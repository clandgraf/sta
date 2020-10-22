#include "controllers.hpp"

bool buttonState[16];

void Controller::update() {
    if (m_latched) {
        m_shiftButtons = 0;
        if (m_state.btn_a)   { m_shiftButtons |= 0b00000001; }
        if (m_state.btn_b)   { m_shiftButtons |= 0b00000010; }
        if (m_state.select)  { m_shiftButtons |= 0b00000100; }
        if (m_state.start)   { m_shiftButtons |= 0b00001000; }
        if (m_state.d_up)    { m_shiftButtons |= 0b00010000; }
        if (m_state.d_down)  { m_shiftButtons |= 0b00100000; }
        if (m_state.d_left)  { m_shiftButtons |= 0b01000000; }
        if (m_state.d_right) { m_shiftButtons |= 0b10000000; }
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
