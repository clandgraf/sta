#include "emu.hpp"
#include "mem.hpp"
#include "rom.hpp"
#include "ppu.hpp"

#include "cpu_cycles.hpp"
#include "cpu_opcodes.hpp"

void Emu::init(Cart* cart) {
    if (m_mem) { delete m_mem; m_mem = nullptr; }
    if (m_cart) { delete m_cart; m_cart = nullptr; }
    if (m_ppu) { delete m_ppu; m_ppu = nullptr; }

    m_cart = cart;
    m_ppu = new PPU(m_cart);
    m_mem = new Memory(m_cart, m_ppu);

    reset();
}

bool Emu::isInitialized() {
    return m_cart && m_mem;
}

uint8_t Emu::getOpcode() { return m_next_opcode; }
uint8_t Emu::getOpcode(uint16_t addr) { return m_mem->readb(addr); }
uint16_t Emu::getOpcodeAddress() { return m_pc - 1; }
uint8_t Emu::getImmediateArg(int offset) { return m_mem->readb(m_pc + offset); }
uint8_t Emu::getImmediateArg(uint16_t addr, int offset) { return m_mem->readb(addr + 1 + offset); }

void Emu::reset() {
    m_mode = Mode::RESET;
    m_cyclesLeft = 9;

    // TODO do in exec_reset
    m_f_irq = false;
    m_f_decimal = false;

    // -- Non CPU Stuff
    m_cycleCount = 0;
}

void Emu::exec_reset() {
    --m_cyclesLeft;
    m_cycleCount++;
    switch (m_cyclesLeft) {
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
    uint16_t hi;
    uint16_t lo;

    auto hilo = [this, &hi, &lo]{
        lo = fetch_arg();
        hi = fetch_arg();
        return hi << 8 | lo; 
    };

    auto branch = [this, &hi, &lo](bool flag) {
        hi = (int8_t) fetch_arg();
        lo = m_pc + hi;
        if (flag) {
            m_cyclesLeft++;
            // Check wether page boundary is crossed, PC still points to next instruction
            if ((lo & 0xf0) != (m_pc & 0xf0)) {
                m_cyclesLeft++;
            }
            m_pc = lo;
        }
    };

    auto updateNZ = [this](uint8_t value) {
        m_f_zero = value == 0;
        m_f_negative = value & 0b10000000;
    };

    switch (m_next_opcode) {

    /* Branching */
    case OPC_BPL:
        branch(!m_f_negative);
        break;
    case OPC_BMI:
        branch(m_f_negative);
        break;
    case OPC_BVC:
        branch(!m_f_overflow);
        break;
    case OPC_BVS:
        branch(m_f_overflow);
        break;
    case OPC_BCC:
        branch(!m_f_carry);
        break;
    case OPC_BCS:
        branch(m_f_carry);
        break;
    case OPC_BNE:
        branch(!m_f_zero);
        break;
    case OPC_BEQ:
        branch(m_f_zero);
        break;

    /* Jumps/Returns */

    /* Set/Reser Flags */
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

    /* Transfer */
    case OPC_TXS:
        m_sp = m_r_x;
        break;
    case OPC_TSX:
        updateNZ(m_r_x = m_sp);
        break;
    case OPC_TXA:
        updateNZ(m_r_a = m_r_x);
        break;
    case OPC_TYA:
        updateNZ(m_r_a = m_r_y);
        break;
    case OPC_TAX:
        updateNZ(m_r_x = m_r_a);
        break;
    case OPC_TAY:
        updateNZ(m_r_y = m_r_a);
        break;

    /* Load */
    case OPC_LDA_ABS:
        updateNZ(m_r_a = m_mem->readb(hilo()));
        break;
    case OPC_LDA_IMD:
        updateNZ(m_r_a = fetch_arg());
        break;

    case OPC_LDX_IMD:
        updateNZ(m_r_x = fetch_arg());
        break;
    
    /* Store */
    case OPC_STA_ABS:
        m_mem->writeb(hilo(), m_r_a);
        break;

    default:
        reset();
        break;
    }
}


uint8_t Emu::fetch_arg() {
    return m_mem->readb(m_pc++);
}


void Emu::fetch() {
    m_next_opcode = m_mem->readb(m_pc);
    m_cyclesLeft = OPC_CYCLES[m_next_opcode];

    m_pc++;
    m_last_cycle_fetched = true;
}

int8_t Emu::stepOperation() {
    do  {
        stepCycle();
    } while (!m_last_cycle_fetched);

    return m_cyclesLeft;
}

int8_t Emu::stepCycle() {
    m_last_cycle_fetched = false;

    switch (m_mode) {
    case Mode::EXEC:
        --m_cyclesLeft;
        m_cycleCount++;
        if (m_cyclesLeft == 0) {
            exec_opcode();
            if (m_cyclesLeft > 0) {
                if (m_mode != Mode::RESET) {
                    m_mode = Mode::CYCLES;
                }
            } else {
                fetch();
            }
        }

        break;

    case Mode::CYCLES:
        --m_cyclesLeft;
        m_cycleCount++;
        if (m_cyclesLeft == 0) {
            m_mode = Mode::EXEC;
            fetch();
        }
        break;

    case Mode::RESET:
        exec_reset();
        break;
    }

    return m_cyclesLeft;
}
