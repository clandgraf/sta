#include <iostream>

#include "nes/emu.hpp"
#include "nes/mem.hpp"
#include "nes/rom.hpp"
#include "nes/ppu.hpp"
#include "core/util.hpp"
#include "nes/cpu_opcodes.hpp"
#include "nes/disasm.hpp"
#include "nes/controllers.hpp"

namespace sm = StreamManipulators;

Emu::Emu() {
    m_breakOnInterrupt = Settings::get("emulator/break-on-interrupt", false);
    m_logOut.open("cpu.log");
    m_disassembler = std::make_unique<Disassembler>(*this);

    m_ports[0] = std::make_shared<Controller>(0);
    m_ports[1] = std::make_shared<Controller>(1);
}

Emu::~Emu() {
    m_logOut.close();
}

void Emu::setPixelFn(std::function<void(unsigned int, unsigned int, unsigned int)> fn) {
    m_setPixel = fn;
    if (m_ppu) {
        m_ppu->setPixelFn(fn);
    }
}

void Emu::writeSettings() {
    Settings::set("emulator/break-on-interrupt", m_breakOnInterrupt);
    m_disassembler->writeSettings();
}

bool Emu::init(const std::filesystem::path& path) {
    std::shared_ptr<Cart> cart = Cart::fromFile(path);
    if (cart) {
        init(cart);
        return true;
    }
    return false;
}

void Emu::init(std::shared_ptr<Cart> cart) {
    m_cart = cart;
    m_disassembler->clear();
    m_ppu = std::make_shared<PPU>(*this, m_cart);
    m_mem = std::make_unique<Memory>(*this, m_cart, m_ppu);
    m_mem->setPort0(m_ports[0]);
    m_mem->setPort1(m_ports[1]);

    if (m_setPixel) {
        m_ppu->setPixelFn(m_setPixel);
    }

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
    uint8_t v = 16;  // Bit 5 is always set, see https://wiki.nesdev.com/w/index.php/Status_flags#The_B_flag
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
#ifdef NESTEST_SETUP
    m_cyclesLeft = 7;
#else
    m_cyclesLeft = 9;
#endif

    // TODO do in exec_reset
    m_f_irq = true;
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

void Emu::startDMA(uint8_t page) {
    // TODO Additional Cycle on odd cpu cycles??
    m_dmaCycle = -1;
    m_dmaPage = page;
}

void Emu::execDma() {
    if (m_dmaCycle >= 512) {
        m_mode = Mode::EXEC;
        return;
    } 

    if (m_dmaCycle >= 0) {
        bool isWrite = m_dmaCycle % 2;  // Alternatingly ...
        if (isWrite) {
            m_mem->writeb(0x2000 | PPU::OAMDATA, uint8_t(_m_lo));   // ... write to OAM
        } else {
            uint8_t oamAddress = m_dmaCycle / 2;  // ... or read from DMA page
            _m_lo = m_mem->readb(uint16_t(m_dmaPage) << 8 | oamAddress);
        }
    }

    m_dmaCycle++;
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

    if (m_logState) {
        m_disassembler->logState(m_logOut);
        m_logOut.flush();
    }
}

void Emu::requestInterrupt(uint16_t vector) {
    m_interruptInCycle = true;

    m_nextOpcodeAddress = m_pc;
    m_nextOpcode = OPC_BRK;
    m_cyclesLeft = OPC_CYCLES[m_nextOpcode];
    m_intVector = vector;
    m_isInterrupt = true;

    // m_pc++;
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
        m_mode = Mode::EXEC;        

#ifdef NESTEST_SETUP
        m_pc = 0xc000;
#endif
      
        fetch();
        break;
    }
}

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
    bool currentFrame = m_ppu->isOddFrame();

    while (m_ppu->isOddFrame() == currentFrame) {
        if (stepCycle()) {
            break;
        }
    }
}

void Emu::stepOut() {
    m_breakOnRTS = true;

    while (true) {
        if (stepCycle()) {
            break;
        }
    }

    m_breakOnRTS = false;
}

bool Emu::stepCycle() {
    bool breakExecution = false;

    m_errorInCycle = false;
    m_interruptInCycle = false;
    m_lastCycleFetched = false;

    switch (m_mode) {
    case Mode::EXEC:
        m_cycleCount++;
        if (--m_cyclesLeft == 0) {
            execOpcode();
            if (m_breakOnRTS && m_nextOpcode == OPC_RTS) {
                breakExecution = true;
            }

            if (m_cyclesLeft > 0) {
                if (m_mode != Mode::RESET) {
                    // Opcode uses additional cycles
                    m_mode = Mode::CYCLES;
                }
            } else if (m_dmaCycle < 0) {
                // Instruction's write has triggered OAMDMA
                m_mode = Mode::DMA;
                fetch();
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
            if (m_dmaCycle < 0) {
                m_mode = Mode::DMA;
                fetch();
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

    case Mode::DMA:
        execDma();
        break;

    case Mode::RESET:
        execReset();
        break;
    }

    // TODO Should start PPU after Reset?
#ifdef NESTEST_SETUP
    if (m_mode != Mode::RESET) 
        m_ppu->run(3);
#else
    m_ppu->run(3);
#endif

    // We are at the start of a new opcode and have hit a breakpoint
    if (m_lastCycleFetched && m_breakpoints.find(m_nextOpcodeAddress) != m_breakpoints.end()
        || (m_interruptInCycle && m_breakOnInterrupt)
        || m_errorInCycle) {
        
        m_isStepping = true;
        breakExecution = true;
    }

    return breakExecution;
}

void Emu::execOpcode() {
    switch (m_nextOpcode) {

    case OPC_NOP:
    case _OPC_NOP__0:
    case _OPC_NOP__1:
    case _OPC_NOP__2:
    case _OPC_NOP__3:
    case _OPC_NOP__4:
    case _OPC_NOP__5:       break;

    case _OPC_NOP_ABS__0:   _readAbs(); break;

    case _OPC_NOP_IMD__0:
    case _OPC_NOP_IMD__1:
    case _OPC_NOP_IMD__2:
    case _OPC_NOP_IMD__3:
    case _OPC_NOP_IMD__4:   _readImd(); break;

    case _OPC_NOP_ZPG__0:
    case _OPC_NOP_ZPG__1:
    case _OPC_NOP_ZPG__2:   _readZpg(); break;

    case _OPC_NOP_ZPG_X__0:
    case _OPC_NOP_ZPG_X__1:
    case _OPC_NOP_ZPG_X__2:
    case _OPC_NOP_ZPG_X__3:
    case _OPC_NOP_ZPG_X__4:
    case _OPC_NOP_ZPG_X__5: _readZpgX(); break;
    
    case _OPC_NOP_ABS_X__0:
    case _OPC_NOP_ABS_X__1:
    case _OPC_NOP_ABS_X__2:
    case _OPC_NOP_ABS_X__3:
    case _OPC_NOP_ABS_X__4:
    case _OPC_NOP_ABS_X__5: _readAbsX(); break;

    case OPC_PHP: _push(getProcStatus(true)); break;
    case OPC_PHA: _push(m_r_a); break;
    case OPC_PLP: setProcStatus(_pop()); break;
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
        if (!m_isInterrupt) { fetchArg(); }
        _push(m_pc >> 8);
        _push(m_pc & 0xff);
        _push(getProcStatus(!m_isInterrupt));
        m_pc = m_mem->readb(m_intVector);
        m_pc |= uint16_t(m_mem->readb(m_intVector + 1)) << 8;
        if (m_isInterrupt) { m_f_irq = true; }
        // Reset Interrupt Variables
        m_intVector = IRQ_VECTOR;
        m_isInterrupt = false;
        break;
    case OPC_JMP:
        m_pc = _hilo();
        break;
    case OPC_JMP_IND:
        m_pc = m_mem->readb(_hilo());
        _m_lo += 1;
        m_pc |= uint16_t(m_mem->readb(_fromHilo())) << 8;
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
    case OPC_RTI:
        setProcStatus(_pop());
        _m_lo = _pop();
        _m_hi = _pop();
        m_pc = _fromHilo();
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

    case OPC_BIT_ZPG:   _readZpg();  _execBit(); break;
    case OPC_BIT_ABS:   _readAbs();  _execBit(); break;

    /* Store */
    case OPC_STA_ZPG:   _redrZpg();  _storeReg(m_r_a); break;
    case OPC_STA_ZPG_X: _redrZpgX(); _storeReg(m_r_a); break;
    case OPC_STA_ABS:   _redrAbs();  _storeReg(m_r_a); break;
    case OPC_STA_ABS_X: _redrAbsX(); _storeReg(m_r_a); break;
    case OPC_STA_ABS_Y: _redrAbsY(); _storeReg(m_r_a); break;
    case OPC_STA_IND_X: _redrIndX(); _storeReg(m_r_a); break;
    case OPC_STA_IND_Y: _redrIndY(); _storeReg(m_r_a); break;

    case OPC_STX_ZPG:   _redrZpg();  _storeReg(m_r_x); break;
    case OPC_STX_ZPG_Y: _redrZpgY(); _storeReg(m_r_x); break;
    case OPC_STX_ABS:   _redrAbs();  _storeReg(m_r_x); break;

    case OPC_STY_ZPG:   _redrZpg();  _storeReg(m_r_y); break;
    case OPC_STY_ZPG_X: _redrZpgX(); _storeReg(m_r_y); break;
    case OPC_STY_ABS:   _redrAbs();  _storeReg(m_r_y); break;

    case _OPC_AXS_ZPG__0:   _redrZpg();  _storeReg(m_r_a & m_r_x); break;
    case _OPC_AXS_ZPG_Y__0: _redrZpgY(); _storeReg(m_r_a & m_r_x); break;
    case _OPC_AXS_ABS__0:   _redrAbs();  _storeReg(m_r_a & m_r_x); break;
    case _OPC_AXS_IND_X__0: _redrIndX(); _storeReg(m_r_a & m_r_x); break;

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

    case _OPC_DCM_ZPG:   _redmZpg();  _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_ZPG_X: _redmZpgX(); _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_ABS:   _redmAbs();  _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_ABS_X: _redmAbsX(); _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_ABS_Y: _redmAbsY(); _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_IND_X: _redmIndX(); _execDec(_m_lo); _storeMem(); _execCmp(); break;
    case _OPC_DCM_IND_Y: _redmIndY(); _execDec(_m_lo); _storeMem(); _execCmp(); break;

    case _OPC_INS_ZPG:   _redmZpg();  _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_ZPG_X: _redmZpgX(); _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_ABS:   _redmAbs();  _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_ABS_X: _redmAbsX(); _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_ABS_Y: _redmAbsY(); _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_IND_X: _redmIndX(); _execInc(_m_lo); _storeMem(); _execSbc(); break;
    case _OPC_INS_IND_Y: _redmIndY(); _execInc(_m_lo); _storeMem(); _execSbc(); break;

    case OPC_ASL_A:      _execAsl(m_r_a); break;
    case OPC_ASL_ZPG:    _redmZpg();  _execAsl(_m_lo); _storeMem(); break;
    case OPC_ASL_ZPG_X:  _redmZpgX(); _execAsl(_m_lo); _storeMem(); break;
    case OPC_ASL_ABS:    _redmAbs();  _execAsl(_m_lo); _storeMem(); break;
    case OPC_ASL_ABS_X:  _redmAbsX(); _execAsl(_m_lo); _storeMem(); break;

    case _OPC_SLO_ZPG:   _redmZpg();  _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_ZPG_X: _redmZpgX(); _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_ABS:   _redmAbs();  _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_ABS_X: _redmAbsX(); _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_ABS_Y: _redmAbsY(); _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_IND_X: _redmIndX(); _execAsl(_m_lo); _storeMem(); _execOra(); break;
    case _OPC_SLO_IND_Y: _redmIndY(); _execAsl(_m_lo); _storeMem(); _execOra(); break;

    case OPC_LSR_A:     _execLsr(m_r_a); break;
    case OPC_LSR_ZPG:   _redmZpg();  _execLsr(_m_lo); _storeMem(); break;
    case OPC_LSR_ZPG_X: _redmZpgX(); _execLsr(_m_lo); _storeMem(); break;
    case OPC_LSR_ABS:   _redmAbs();  _execLsr(_m_lo); _storeMem(); break;
    case OPC_LSR_ABS_X: _redmAbsX(); _execLsr(_m_lo); _storeMem(); break;

    case _OPC_SRE_ZPG:   _redmZpg();  _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_ZPG_X: _redmZpgX(); _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_ABS:   _redmAbs();  _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_ABS_X: _redmAbsX(); _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_ABS_Y: _redmAbsY(); _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_IND_X: _redmIndX(); _execLsr(_m_lo); _storeMem(); _execEor(); break;
    case _OPC_SRE_IND_Y: _redmIndY(); _execLsr(_m_lo); _storeMem(); _execEor(); break;

    case OPC_ROL_A:     _execRol(m_r_a); break;
    case OPC_ROL_ZPG:   _redmZpg();  _execRol(_m_lo); _storeMem(); break;
    case OPC_ROL_ZPG_X: _redmZpgX(); _execRol(_m_lo); _storeMem(); break;
    case OPC_ROL_ABS:   _redmAbs();  _execRol(_m_lo); _storeMem(); break;
    case OPC_ROL_ABS_X: _redmAbsX(); _execRol(_m_lo); _storeMem(); break;

    case _OPC_RLA_ZPG:   _redmZpg();  _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_ZPG_X: _redmZpgX(); _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_ABS:   _redmAbs();  _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_ABS_X: _redmAbsX(); _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_ABS_Y: _redmAbsY(); _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_IND_X: _redmIndX(); _execRol(_m_lo); _storeMem(); _execAnd(); break;
    case _OPC_RLA_IND_Y: _redmIndY(); _execRol(_m_lo); _storeMem(); _execAnd(); break;

    case OPC_ROR_A:     _execRor(m_r_a); break;
    case OPC_ROR_ZPG:   _redmZpg();  _execRor(_m_lo); _storeMem(); break;
    case OPC_ROR_ZPG_X: _redmZpgX(); _execRor(_m_lo); _storeMem(); break;
    case OPC_ROR_ABS:   _redmAbs();  _execRor(_m_lo); _storeMem(); break;
    case OPC_ROR_ABS_X: _redmAbsX(); _execRor(_m_lo); _storeMem(); break;

    case _OPC_RRA_ZPG:   _redmZpg();  _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_ZPG_X: _redmZpgX(); _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_ABS:   _redmAbs();  _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_ABS_X: _redmAbsX(); _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_ABS_Y: _redmAbsY(); _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_IND_X: _redmIndX(); _execRor(_m_lo); _storeMem(); _execAdc(); break;
    case _OPC_RRA_IND_Y: _redmIndY(); _execRor(_m_lo); _storeMem(); _execAdc(); break;

    case _OPC_LAX_ABS__0:   _readAbs();  _execLd(m_r_a); _execLd(m_r_x); break;
    case _OPC_LAX_ABS_Y__0: _readAbsY(); _execLd(m_r_a); _execLd(m_r_x); break;
    case _OPC_LAX_ZPG__0:   _readZpg();  _execLd(m_r_a); _execLd(m_r_x); break;
    case _OPC_LAX_ZPG_Y__0: _readZpgY(); _execLd(m_r_a); _execLd(m_r_x); break;
    case _OPC_LAX_IND_X__0: _readIndX(); _execLd(m_r_a); _execLd(m_r_x); break;
    case _OPC_LAX_IND_Y__0: _readIndY(); _execLd(m_r_a); _execLd(m_r_x); break;

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
    case OPC_CMP_ABS_Y: _readAbsY(); _execCmp(); break;
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

    case _OPC_SBC_IMD__0:
    case OPC_SBC_IMD:   _readImd();  _execSbc(); break;
    case OPC_SBC_ABS:   _readAbs();  _execSbc(); break;
    case OPC_SBC_ABS_X: _readAbsX(); _execSbc(); break;
    case OPC_SBC_ABS_Y: _readAbsY(); _execSbc(); break;
    case OPC_SBC_ZPG:   _readZpg();  _execSbc(); break;
    case OPC_SBC_ZPG_X: _readZpgX(); _execSbc(); break;
    case OPC_SBC_IND_X: _readIndX(); _execSbc(); break;
    case OPC_SBC_IND_Y: _readIndY(); _execSbc(); break;

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
        LOG_ERR << "Unhandled opcode at " 
                << sm::hex(m_nextOpcodeAddress) 
                << ":"
                << sm::hex(m_nextOpcode) << "\n";
        m_errorInCycle = true;
        break;
    }
}

__forceinline void Emu::_toHilo(const uint16_t& value) {
    _m_lo = value & 0xff;
    _m_hi = value >> 8;
}

__forceinline uint16_t Emu::_fromHilo() { return _m_hi << 8 | (_m_lo & 0x00ff); }

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
__forceinline void Emu::_readZpgX() { _m_lo = m_mem->readb((fetchArg() + m_r_x) & 0xff); }
__forceinline void Emu::_readZpgY() { _m_lo = m_mem->readb((fetchArg() + m_r_y) & 0xff); }
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
    _m_hi = m_mem->readb((_m_lo + 1) & 0xff);
    _m_lo = m_mem->readb(_m_lo & 0xff);
    _m_lo = m_mem->readb(_fromHilo());
}
__forceinline void Emu::_readIndY() {
    _m_lo = fetchArg();
    _m_hi = m_mem->readb((_m_lo + 1) & 0xff);
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
    //// See http://www.righto.com/2012/12/the-6502-overflow-flag-explained.html: Addition on the 6502
    _m_hi = _m_lo + m_r_a + (m_f_carry ? 1 : 0);
    m_f_carry    = _m_hi > 0xff;
    m_f_overflow = (_m_lo ^ _m_hi) & (m_r_a ^ _m_hi) & 0x80;
    _updateNZ(m_r_a = uint8_t(_m_hi));
};
__forceinline void Emu::_execSbc() {
    _m_lo = (~_m_lo) & 0xff;
    _execAdc();
}
__forceinline void Emu::_execAnd() { _updateNZ(m_r_a &= _m_lo); }
__forceinline void Emu::_execCmp() { _compare(m_r_a); };
__forceinline void Emu::_execEor() { _updateNZ(m_r_a ^= _m_lo); };
__forceinline void Emu::_execOra() { _updateNZ(m_r_a |= _m_lo); };
__forceinline void Emu::_execBit() { 
    m_f_negative = _m_lo & 0b10000000;
    m_f_overflow = _m_lo & 0b01000000;
    m_f_zero = (_m_lo & m_r_a) == 0;
}

template<typename T>
__forceinline void Emu::_execAsl(T& field) {
    m_f_carry = field & 0x80;
    field <<= 1;
    field &= 0xff;
    _updateNZ(uint8_t(field));
}
template<typename T>
__forceinline void Emu::_execLsr(T& field) {
    m_f_carry = field & 0x01;
    field >>= 1;
    field &= 0xff;
    _updateNZ(uint8_t(field));
}
template<typename T>
__forceinline void Emu::_execRol(T& field) {
    uint8_t b0 = m_f_carry ? 0x01 : 0;
    m_f_carry = field & 0x80;
    field = (field << 1) | b0;
    field &= 0xff;
    _updateNZ(uint8_t(field));
}
template<typename T>
__forceinline void Emu::_execRor(T& field) {
    uint8_t b7 = m_f_carry ? 0x80 : 0;
    m_f_carry = field & 0x01;
    field = (field >> 1) | b7;
    field &= 0xff;
    _updateNZ(uint8_t(field));
}

__forceinline void Emu::_execLd(uint8_t& reg) { _updateNZ(reg = (uint8_t)_m_lo); }

__forceinline void Emu::_redrZpg()  { _m_hi = fetchArg(); }
__forceinline void Emu::_redrZpgX() { _m_hi = (fetchArg() + m_r_x) & 0xff; }
__forceinline void Emu::_redrZpgY() { _m_hi = (fetchArg() + m_r_y) & 0xff; }
__forceinline void Emu::_redrAbs()  { _m_hi = _hilo(); }
__forceinline void Emu::_redrAbsX() { _m_hi = _hilo() + m_r_x; }
__forceinline void Emu::_redrAbsY() { _m_hi = _hilo() + m_r_y; }
__forceinline void Emu::_redrIndX() {
    _m_lo = fetchArg() + m_r_x;
    _m_hi = (uint16_t(m_mem->readb((_m_lo + 1) & 0xff)) << 8) 
          | m_mem->readb(_m_lo & 0xff);
}
__forceinline void Emu::_redrIndY() {
    _m_lo = fetchArg();
    _m_hi = ((uint16_t(m_mem->readb((_m_lo + 1) & 0xff)) << 8)
             | m_mem->readb(_m_lo))
          + m_r_y;
}
__forceinline void Emu::_storeReg(const uint8_t& reg) { m_mem->writeb(_m_hi, reg); }

__forceinline void Emu::_redmZpg()  { _redrZpg();  _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmZpgX() { _redrZpgX(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmAbs()  { _redrAbs();  _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmAbsX() { _redrAbsX(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmAbsY() { _redrAbsY(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmIndX() { _redrIndX(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_redmIndY() { _redrIndY(); _m_lo = m_mem->readb(_m_hi); }
__forceinline void Emu::_storeMem() { m_mem->writeb(_m_hi, (uint8_t)_m_lo); }
