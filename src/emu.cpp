#include <iostream>

#include "emu.hpp"
#include "mem.hpp"
#include "rom.hpp"
#include "ppu.hpp"
#include "util.hpp"

#include "cpu_opcodes.hpp"

Emu::Emu() {
    #ifdef LOG_EXECUTION
    logOut.open("cpu.log");
    #endif
}

Emu::~Emu() {
    #ifdef LOG_EXECUTION
    logOut.close();
    #endif
}

void Emu::init(Cart* cart) {
    if (m_mem) { delete m_mem; m_mem = nullptr; }
    if (m_cart) { delete m_cart; m_cart = nullptr; }
    if (m_ppu) { delete m_ppu; m_ppu = nullptr; }

    m_cart = cart;
    m_ppu = new PPU(this, m_cart);
    m_mem = new Memory(m_cart, m_ppu);

    reset();
}

bool Emu::toggleBreakpoint(uint16_t address) {
    auto bp = m_breakpoints.find(address);
    if (bp == m_breakpoints.end()) {
        m_breakpoints.insert(address);
        return true;
    } else {
        m_breakpoints.erase(bp);
        return false;
    }
}

bool Emu::isBreakpoint(uint16_t address) {
    return m_breakpoints.find(address) != m_breakpoints.end();
}

bool Emu::isInitialized() {
    return m_cart && m_mem;
}

uint8_t Emu::getOpcode() { return m_nextOpcode; }
uint8_t Emu::getOpcode(uint16_t addr) { return m_mem->readb(addr); }
uint16_t Emu::getOpcodeAddress() { return m_nextOpcodeAddress; }
uint8_t Emu::getImmediateArg(int offset) { return m_mem->readb(m_pc + offset); }
uint8_t Emu::getImmediateArg(uint16_t addr, int offset) { return m_mem->readb(addr + 1 + offset); }

uint8_t Emu::getProcStatus(bool setBrk) {
    uint8_t v = 0x16;  // Bit 5 is always set, see https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
    if (m_f_carry)    v |= 1;
    if (m_f_zero)     v |= 2;
    if (m_f_irq)      v |= 4;
    if (m_f_decimal)  v |= 8;
    if (setBrk)       v |= 32;
    if (m_f_overflow) v |= 64;
    if (m_f_negative) v |= 128;
    return v;
}

uint8_t Emu::setProcStatus(uint8_t v) {
    m_f_carry    = v & 1;
    m_f_zero     = v & 2;
    m_f_irq      = v & 4;
    m_f_decimal  = v & 8;
    m_f_overflow = v & 64;
    m_f_negative = v & 128;
    return v;
}

void Emu::reset() {
    // Enter Reset Mode
    m_mode = Mode::RESET;
    //m_cyclesLeft = 9;
    m_cyclesLeft = 7;

    // TODO do in exec_reset
    m_f_irq = false;
    m_f_decimal = false;

    // -- Non CPU Stuff
    m_cycleCount = 0;
    m_ppu->reset();

    // Reset Interrupt Lines
    m_nmi_request = false;
    m_irq_request = false;
    m_intVector = IRQ_VECTOR;
    m_isInterrupt = false;
}

uint8_t Emu::fetchArg() {
    return m_mem->readb(m_pc++);
}

void Emu::fetch() {
    m_nextOpcodeAddress = m_pc;
    m_nextOpcode = m_mem->readb(m_pc);
    m_cyclesLeft = OPC_CYCLES[m_nextOpcode];

    m_pc++;
    m_lastCycleFetched = true;

    #ifdef LOG_EXECUTION
    logOut << std::hex 
           << std::setfill('0') 
           << std::setw(4) << m_nextOpcodeAddress
           << " A:"  << std::setw(2) << int(m_r_a)
           << " X:"  << std::setw(2) << int(m_r_x)
           << " Y:"  << std::setw(2) << int(m_r_y)
           << " P:"  << std::setw(2) << int(getProcStatus(false))
           << " SP:" << std::setw(2) << int(m_sp)
           << std::dec
           << std::setfill(' ')
           << " PPU:" << std::setw(3) << int(m_ppu->m_sl_cycle) << "," << std::setw(3) << int(m_ppu->m_scanline)
           << " CYC:" << std::setw(0) << m_cycleCount
           << "\n";
    logOut.flush();
    #endif
}

void Emu::requestInterrupt(uint16_t vector) {
    m_nextOpcodeAddress = m_pc;
    m_nextOpcode = OPC_BRK;
    m_cyclesLeft = OPC_CYCLES[m_nextOpcode];
    m_intVector = vector;
    m_isInterrupt = true;

    m_pc++;
    m_lastCycleFetched = true;
}

void Emu::execReset() {
    --m_cyclesLeft;
    m_cycleCount++;
    switch (m_cyclesLeft) {
    case 8: // C0
        m_sp = 0x00;
        break;
    //case 7: // C1
    //    break;
    //case 6: // C2
    //    break;
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
        //m_pc = 0xc000;          
        m_mode = Mode::EXEC;
        fetch();
        break;
    }
}

bool ADC_CARRY_LUT[8]    = { 0, 0, 0, 1, 0, 1, 1, 1 };
bool ADC_OVERFLOW_LUT[8] = { 0, 1, 0, 0, 0, 0, 1, 0 };

void Emu::stepOperation() {
    do  {
        if (stepCycle()) {
            break;
        }
    } while (!m_lastCycleFetched);
}

void Emu::stepScanline() {
    int currentScanline = m_ppu->m_scanline;
    while (m_ppu->m_scanline == currentScanline) {
        if (stepCycle()) {
            break;
        }
    }
}

void Emu::stepFrame() {
    bool currentFrame = m_ppu->m_f_odd_frame;
    while (m_ppu->m_f_odd_frame == currentFrame && m_mode != Mode::RESET) {
        if (stepCycle()) {
            break;
        }
    }
}

bool Emu::stepCycle() {
    bool breakpointHit = false;

    m_lastCycleFetched = false;

    switch (m_mode) {
    case Mode::EXEC:
        m_cycleCount++;
        if (--m_cyclesLeft == 0) {
            execOpcode();
            if (m_cyclesLeft > 0) {
                if (m_mode != Mode::RESET) {
                    // Opcode uses additional cycles
                    m_mode = Mode::CYCLES;
                }
            } else if (m_nmi_request) {
                requestInterrupt(NMI_VECTOR);
                m_nmi_request = false;
            } else if (m_irq_request) {
                requestInterrupt(IRQ_VECTOR);
                m_irq_request = false;
            } else {
                fetch();
            }
        }

        break;

    case Mode::CYCLES:
        m_cycleCount++;
        if (--m_cyclesLeft == 0) {
            m_mode = Mode::EXEC;
            if (m_nmi_request) {
                requestInterrupt(NMI_VECTOR);
                m_nmi_request = false;
            } else if (m_irq_request) {
                requestInterrupt(IRQ_VECTOR);
                m_irq_request = false;
            } else {
                fetch();
            }
        }
        break;

    case Mode::RESET:
        execReset();
        break;
    }

    m_ppu->run(3);

    // We are at the start of a new opcode and have hit a breakpoint
    if (m_lastCycleFetched && m_breakpoints.find(m_nextOpcodeAddress) != m_breakpoints.end()) {
        m_isStepping = true;
        breakpointHit = true;
    }

    return breakpointHit;
}

void Emu::execOpcode() {
    switch (m_nextOpcode) {

    case OPC_NOP:
        break;

    case OPC_PHP: _push(getProcStatus(true)); break;
    case OPC_PHA: _push(m_r_a); break;
    case OPC_PLP: _updateNZ(setProcStatus(_pop())); break;
    case OPC_PLA: _updateNZ(m_r_a = _pop()); break;

        /* Branching */
    case OPC_BPL: _branch(!m_f_negative); break;
    case OPC_BMI: _branch(m_f_negative);  break;
    case OPC_BVC: _branch(!m_f_overflow); break;
    case OPC_BVS: _branch(m_f_overflow);  break;
    case OPC_BCC: _branch(!m_f_carry);    break;
    case OPC_BCS: _branch(m_f_carry);     break;
    case OPC_BNE: _branch(!m_f_zero);     break;
    case OPC_BEQ: _branch(m_f_zero);      break;

        /* Jumps/Returns */
    case OPC_BRK:
        fetchArg();
        _push(m_pc >> 8);
        _push(m_pc & 0xff);
        _push(getProcStatus(!m_isInterrupt));
        m_pc = m_mem->readb(m_intVector);
        if (m_isInterrupt) { m_f_irq = true; }
        // Reset Interrupt Variables
        m_intVector = IRQ_VECTOR;
        m_isInterrupt = false;
        break;
    case OPC_JMP:
        m_pc = _hilo();
        break;
    case OPC_JSR:
        _toHilo(m_pc + 1);
        _push((uint8_t)_m_hi);
        _push((uint8_t)_m_lo);
        m_pc = _hilo();
        break;
    case OPC_RTS:
        _m_lo = _pop();
        _m_hi = _pop();
        m_pc = _fromHilo() + 1;
        break;

    /* Arithmetic */
    case OPC_LSR:
        m_f_carry = m_r_a & 1;
        _updateNZ(m_r_a >>= 1);
        break;

    /* Set/Reset Flags */
    case OPC_CLC: m_f_carry = false;    break;
    case OPC_SEC: m_f_carry = true;     break;
    case OPC_CLI: m_f_irq = false;      break;
    case OPC_SEI: m_f_irq = true;       break;
    case OPC_CLD: m_f_decimal = false;  break;
    case OPC_SED: m_f_decimal = true;   break;
    case OPC_CLV: m_f_overflow = false; break;

    /* Transfer */
    case OPC_TXS: m_sp = m_r_x; break;
    case OPC_TSX: _updateNZ(m_r_x = m_sp);  break;
    case OPC_TXA: _updateNZ(m_r_a = m_r_x); break;
    case OPC_TYA: _updateNZ(m_r_a = m_r_y); break;
    case OPC_TAX: _updateNZ(m_r_x = m_r_a); break;
    case OPC_TAY: _updateNZ(m_r_y = m_r_a); break;

    /* Store */
    case OPC_STA_IND_Y:
        _m_lo = fetchArg();
        _m_hi = ((uint16_t(m_mem->readb(_m_lo + 1)) << 8) | m_mem->readb(_m_lo)) + m_r_y;
        m_mem->writeb(_m_hi, m_r_a);
        break;

    case OPC_STA_ZPG:   _redrZpg(); _storeReg(m_r_a); break;
    case OPC_STA_ABS:   _redrAbs(); _storeReg(m_r_a); break;

    case OPC_STX_ZPG:   _redrZpg(); _storeReg(m_r_x); break;
    case OPC_STX_ABS:   _redrAbs(); _storeReg(m_r_x); break;

    case OPC_STY_ZPG:   _redrZpg(); _storeReg(m_r_y); break;
    case OPC_STY_ABS:   _redrAbs(); _storeReg(m_r_y); break;

    case OPC_INC_ZPG:   _redmZpg();  _execInc(_m_lo); _storeMem(); break;
    case OPC_INC_ZPG_X: _redmZpgX(); _execInc(_m_lo); _storeMem(); break;
    case OPC_INC_ABS:   _redmAbs();  _execInc(_m_lo); _storeMem(); break;
    case OPC_INC_ABS_X: _redmAbsX(); _execInc(_m_lo); _storeMem(); break;

    case OPC_DEC_ZPG:   _redmZpg();  _execDec(_m_lo); _storeMem(); break;
    case OPC_DEC_ZPG_X: _redmZpgX(); _execDec(_m_lo); _storeMem(); break;
    case OPC_DEC_ABS:   _redmAbs();  _execDec(_m_lo); _storeMem(); break;
    case OPC_DEC_ABS_X: _redmAbsX(); _execDec(_m_lo); _storeMem(); break;

    case OPC_INX:       _execInc(m_r_x); break;
    case OPC_INY:       _execInc(m_r_y); break;

    case OPC_DEX:       _execDec(m_r_x); break;
    case OPC_DEY:       _execDec(m_r_y); break;

    case OPC_LDA_IMD:   _readImd();  _execLd(m_r_a); break;
    case OPC_LDA_ABS:   _readAbs();  _execLd(m_r_a); break;
    case OPC_LDA_ABS_X: _readAbsX(); _execLd(m_r_a); break;
    case OPC_LDA_ABS_Y: _readAbsY(); _execLd(m_r_a); break;
    case OPC_LDA_ZPG:   _readZpg();  _execLd(m_r_a); break;
    case OPC_LDA_ZPG_X: _readZpgX(); _execLd(m_r_a); break;
    case OPC_LDA_IND_X: _readIndX(); _execLd(m_r_a); break;
    case OPC_LDA_IND_Y: _readIndY(); _execLd(m_r_a); break;

    case OPC_LDX_IMD:   _readImd();  _execLd(m_r_x); break;
    case OPC_LDX_ABS:   _readAbs();  _execLd(m_r_x); break;
    case OPC_LDX_ABS_Y: _readAbsY(); _execLd(m_r_x); break;
    case OPC_LDX_ZPG:   _readZpg();  _execLd(m_r_x); break;
    case OPC_LDX_ZPG_Y: _readZpgY(); _execLd(m_r_x); break;

    case OPC_LDY_IMD:   _readImd();  _execLd(m_r_y); break;
    case OPC_LDY_ABS:   _readAbs();  _execLd(m_r_y); break;
    case OPC_LDY_ABS_X: _readAbsX(); _execLd(m_r_y); break;
    case OPC_LDY_ZPG:   _readZpg();  _execLd(m_r_y); break;
    case OPC_LDY_ZPG_X: _readZpgX(); _execLd(m_r_y); break;

    case OPC_CPX_IMD:   _readImd();  _compare(m_r_x); break;
    case OPC_CPX_ABS:   _readAbs();  _compare(m_r_x); break;
    case OPC_CPX_ZPG:   _readZpg();  _compare(m_r_x); break;
    case OPC_CPY_IMD:   _readImd();  _compare(m_r_y); break;
    case OPC_CPY_ABS:   _readAbs();  _compare(m_r_y); break;
    case OPC_CPY_ZPG:   _readZpg();  _compare(m_r_y); break;

    case OPC_CMP_IMD:   _readImd();  _execCmp(); break;
    case OPC_CMP_ABS:   _readAbs();  _execCmp(); break;
    case OPC_CMP_ABS_X: _readAbsX(); _execCmp(); break;
    case OPC_CMP_ABS_Y: _readAbsX(); _execCmp(); break;
    case OPC_CMP_ZPG:   _readZpg();  _execCmp(); break;
    case OPC_CMP_ZPG_X: _readZpgX(); _execCmp(); break;
    case OPC_CMP_IND_X: _readIndX(); _execCmp(); break;
    case OPC_CMP_IND_Y: _readIndY(); _execCmp(); break;

    case OPC_ADC_IMD:   _readImd();  _execAdc(); break;
    case OPC_ADC_ABS:   _readAbs();  _execAdc(); break;
    case OPC_ADC_ABS_X: _readAbsX(); _execAdc(); break;
    case OPC_ADC_ABS_Y: _readAbsY(); _execAdc(); break;
    case OPC_ADC_ZPG:   _readZpg();  _execAdc(); break;
    case OPC_ADC_ZPG_X: _readZpgX(); _execAdc(); break;
    case OPC_ADC_IND_X: _readIndX(); _execAdc(); break;
    case OPC_ADC_IND_Y: _readIndY(); _execAdc(); break;

    case OPC_AND_IMD:   _readImd();  _execAnd(); break;
    case OPC_AND_ABS:   _readAbs();  _execAnd(); break;
    case OPC_AND_ABS_X: _readAbsX(); _execAnd(); break;
    case OPC_AND_ABS_Y: _readAbsY(); _execAnd(); break;
    case OPC_AND_ZPG:   _readZpg();  _execAnd(); break;
    case OPC_AND_ZPG_X: _readZpgX(); _execAnd(); break;
    case OPC_AND_IND_X: _readIndX(); _execAnd(); break;
    case OPC_AND_IND_Y: _readIndY(); _execAnd(); break;

    case OPC_EOR_IMD:   _readImd();  _execEor(); break;
    case OPC_EOR_ABS:   _readAbs();  _execEor(); break;
    case OPC_EOR_ABS_X: _readAbsX(); _execEor(); break;
    case OPC_EOR_ABS_Y: _readAbsY(); _execEor(); break;
    case OPC_EOR_ZPG:   _readZpg();  _execEor(); break;
    case OPC_EOR_ZPG_X: _readZpgX(); _execEor(); break;
    case OPC_EOR_IND_X: _readIndX(); _execEor(); break;
    case OPC_EOR_IND_Y: _readIndY(); _execEor(); break;

    case OPC_ORA_IMD:   _readImd();  _execOra(); break;
    case OPC_ORA_ABS:   _readAbs();  _execOra(); break;
    case OPC_ORA_ABS_X: _readAbsX(); _execOra(); break;
    case OPC_ORA_ABS_Y: _readAbsY(); _execOra(); break;
    case OPC_ORA_ZPG:   _readZpg();  _execOra(); break;
    case OPC_ORA_ZPG_X: _readZpgX(); _execOra(); break;
    case OPC_ORA_IND_X: _readIndX(); _execOra(); break;
    case OPC_ORA_IND_Y: _readIndY(); _execOra(); break;

    default:
        LOG_ERR << "Unhandled opcode at " << std::hex << int(m_nextOpcodeAddress) << ": " << int(m_nextOpcode) << "\n";
        reset();
        break;
    }
}

__forceinline void Emu::_toHilo(const uint16_t& value) {
    _m_lo = value & 0xff;
    _m_hi = value >> 8;
}

__forceinline uint16_t Emu::_fromHilo() { return _m_hi << 8 | _m_lo; }

__forceinline uint16_t Emu::_hilo() {
    _m_lo = fetchArg();
    _m_hi = fetchArg();
    return _fromHilo();
}

__forceinline void Emu::_branch(bool flag) {
    _m_hi = (int8_t)fetchArg();
    _m_lo = m_pc + _m_hi;
    if (flag) {
        m_cyclesLeft++;
        // Check wether page boundary is crossed, PC still points to next instruction
        if ((_m_lo & 0xff00) != (m_pc & 0xff00)) {
            m_cyclesLeft++;
        }
        m_pc = _m_lo;
    }
}

__forceinline uint8_t Emu::_updateNZ(uint8_t value) {
    m_f_zero = value == 0;
    m_f_negative = value & 0b10000000;
    return value;
}

__forceinline void Emu::_compare(const uint8_t& reg) {
    _m_lo = (0x0100 | reg) - _m_lo;
    _updateNZ(_m_lo & 0xff);
    m_f_carry = (_m_lo & 0x0100);
}

__forceinline void Emu::_push(const uint8_t& value) {
    m_mem->writeb(0x0100 | m_sp--, value);
}

__forceinline uint8_t Emu::_pop() {
    return m_mem->readb(0x0100 | ++m_sp);
}

__forceinline void Emu::_readImd()  { _m_lo = fetchArg(); };
__forceinline void Emu::_readZpg()  { _m_lo = m_mem->readb(fetchArg()); }
__forceinline void Emu::_readZpgX() { _m_lo = m_mem->readb(fetchArg() + m_r_x); }
__forceinline void Emu::_readZpgY() { _m_lo = m_mem->readb(fetchArg() + m_r_y); }
__forceinline void Emu::_readAbs()  { _m_lo = m_mem->readb(_hilo()); }
__forceinline void Emu::_readAbsX() {
    _m_lo = _hilo();
    _m_hi = _m_lo + m_r_x;
    if ((_m_lo & 0xff00) != (_m_hi & 0xff00)) {
        m_cyclesLeft++;
    }
    _m_lo = m_mem->readb(_m_hi);
}
__forceinline void Emu::_readAbsY() {
    _m_lo = _hilo();
    _m_hi = _m_lo + m_r_y;
    if ((_m_lo & 0xff00) != (_m_hi & 0xff00)) {
        m_cyclesLeft++;
    }
    _m_lo = m_mem->readb(_m_hi);
}
__forceinline void Emu::_readIndX() {
    _m_lo = fetchArg() + m_r_x;
    _m_hi = m_mem->readb(_m_lo + 1);
    _m_lo = m_mem->readb(_m_lo);
    _m_lo = m_mem->readb(_fromHilo());
}
__forceinline void Emu::_readIndY() {
    _m_lo = fetchArg();
    _m_hi = m_mem->readb(_m_lo + 1);
    _m_lo = m_mem->readb(_m_lo);
    _m_lo = _fromHilo();
    _m_hi = _m_lo + m_r_y;
    if ((_m_lo & 0xff00) != (_m_hi & 0xff00)) {
        m_cyclesLeft++;
    }
    _m_lo = m_mem->readb(_m_hi);
}

template<typename T>
__forceinline void Emu::_execInc(T& field) {
    _updateNZ(uint8_t(field = (field + 1) & 0xff));
}

template<typename T>
__forceinline void Emu::_execDec(T& field) {
    _updateNZ(uint8_t(field = (field - 1) & 0xff));
}

__forceinline void Emu::_execAdc() {
    // See http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html: Addition on the 6502
    _m_hi = _m_lo >> 5                         // m_7
        | m_r_a & 0x80 >> 6               // n_7
        | ((_m_lo & m_r_a & 0x40) >> 6);     // c_6
    if (m_f_carry) {
        m_r_a += 1;
    }
    m_f_carry = ADC_CARRY_LUT[_m_hi];
    m_f_overflow = ADC_OVERFLOW_LUT[_m_hi];
    _updateNZ(m_r_a += _m_lo);
};
__forceinline void Emu::_execAnd() { _updateNZ(m_r_a &= _m_lo); }
__forceinline void Emu::_execCmp() { _compare(m_r_a); };
__forceinline void Emu::_execEor() { _updateNZ(m_r_a ^= _m_lo); };
__forceinline void Emu::_execOra() { _updateNZ(m_r_a |= _m_lo); };

__forceinline void Emu::_execLd(uint8_t& reg) { _updateNZ(reg = (uint8_t)_m_lo); }

__forceinline void Emu::_redrZpg() { _m_hi = fetchArg(); }
__forceinline void Emu::_redrAbs() { _m_hi = _hilo(); }
__forceinline void Emu::_storeReg(const uint8_t& reg) { m_mem->writeb(_m_hi, reg); }

__forceinline void Emu::_redmZpg()  { _redrZpg(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmZpgX() { _m_lo = m_mem->readb(_m_hi = (fetchArg() + m_r_x)); }
__forceinline void Emu::_redmAbs()  { _redrAbs(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmAbsX() { _m_lo = m_mem->readb(_m_hi = (_hilo() + m_r_x)); }
__forceinline void Emu::_storeMem() { m_mem->writeb(_m_hi, (uint8_t)_m_lo); }
