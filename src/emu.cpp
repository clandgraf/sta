#include "emu.hpp"
#include "mem.hpp"
#include "rom.hpp"

void emu::init(cart* _cart) {
    if (m_mem) { delete m_mem; m_mem = nullptr; }
    if (m_cart) { delete m_cart; m_cart = nullptr; }

    m_cart = _cart;
    m_mem = new mem(m_cart);
}

void emu::reset() {
    m_mode = mode::RESET;
    m_cycles_left = 9;

    // TODO do in exec_reset
    m_f_irq = false;
    m_f_decimal = false;
}

void emu::exec_reset() {
    switch(--m_cycles_left) {
    case 8: // C0
        m_sp = 0x00;
        break;
    case 7: // C1
        break;
    case 6: // C2
        break;
    case 5: // C3
        m_sp--;
        break;
    case 4: // C4
        m_sp--;
        break;
    case 3: // C5
        m_sp--;
        break;
    case 2: // C6
        m_pc = m_mem->readb(RESET_VECTOR);
        break;
    case 1: // C7
        m_pc = m_mem->readb(RESET_VECTOR + 1);
        break;
    case 0: // C8            
        m_mode = mode::EXEC;
        fetch();
        break;
    }
}

void emu::fetch() {
    
}

void emu::step() {

}