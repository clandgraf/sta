#include "emu.hpp"
#include "mem.hpp"
#include "rom.hpp"

#include "cpu_cycles.hpp"
#include "cpu_opcodes.hpp"

void Emu::init(Cart* cart) {
    if (m_mem) { delete m_mem; m_mem = nullptr; }
    if (m_cart) { delete m_cart; m_cart = nullptr; }

    m_cart = cart;
    m_mem = new mem(m_cart);

    reset();
}

bool Emu::isInitialized() {
    return m_cart && m_mem;
}

uint8_t Emu::getOpcode() { return m_next_opcode; }
uint16_t Emu::getOpcodeAddress() { return m_pc - 1; }
uint8_t Emu::getImmediateArg(int offset) { return m_mem->readb(m_pc + offset); }

void Emu::reset() {
    m_mode = Mode::RESET;
    m_cycles_left = 9;

    // TODO do in exec_reset
    m_f_irq = false;
    m_f_decimal = false;
}

void Emu::exec_reset() {
    switch (--m_cycles_left) {
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
        m_pc |= m_mem->readb(RESET_VECTOR + 1) << 8;
        break;
    case 0: // C8            
        m_mode = Mode::EXEC;
        fetch();
        break;
    }
}

void Emu::exec_opcode() {
    switch (m_next_opcode) {
    case OPC_CLI:
        m_f_irq = false;
        break;
    case OPC_SEI:
        m_f_irq = true;
        break;
    case OPC_CLD:
        m_f_decimal = false;
        break;
    case OPC_SED:
        m_f_decimal = true;
        break;
    }
}


void Emu::fetch() {
    m_next_opcode = m_mem->readb(m_pc);
    m_cycles_left = OPC_CYCLES[m_next_opcode];

    m_pc++;
    m_last_cycle_fetched = true;
}

int8_t Emu::stepOperation() {
    do  {
        stepCycle();
    } while (!m_last_cycle_fetched);

    return m_cycles_left;
}

int8_t Emu::stepCycle() {
    m_last_cycle_fetched = false;

    switch (m_mode) {
    case Mode::EXEC:
        if (--m_cycles_left == 0) {
            exec_opcode();
            fetch();
        }

        break;
    case Mode::RESET:
        exec_reset();
        break;
    }

    return m_cycles_left;
}