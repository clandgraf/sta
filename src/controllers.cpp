#include "controllers.hpp"

void Controller::update(Input::Controller state) {
    if (m_latched) {
        m_shiftButtons = 0;
        if (state.btn_a)   { m_shiftButtons |= 0xb00000001; }
        if (state.btn_b)   { m_shiftButtons |= 0xb00000010; }
        if (state.select)  { m_shiftButtons |= 0xb00000100; }
        if (state.start)   { m_shiftButtons |= 0xb00001000; }
        if (state.d_up)    { m_shiftButtons |= 0xb00010000; }
        if (state.d_down)  { m_shiftButtons |= 0xb00100000; }
        if (state.d_left)  { m_shiftButtons |= 0xb01000000; }
        if (state.d_right) { m_shiftButtons |= 0xb10000000; }
    }
}

void Controller::write(uint8_t value) {
    m_latched = value & 0x01;
}

uint8_t Controller::read() {
    uint8_t v = m_shiftButtons & 0xb00000001;
    m_shiftButtons >>= 1;
    return v;
}
