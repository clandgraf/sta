#pragma once

#include <cstdint>
#include <set>

#include "inputs.hpp"

class Memory;
class Cart;
class PPU;

uint16_t constexpr NMI_VECTOR = 0xfffa;    // Address where NMI starts
uint16_t constexpr RESET_VECTOR = 0xfffc;  // Address where execution starts
uint16_t constexpr IRQ_VECTOR = 0xfffe;    // Address where IRQ/BRK starts

class Emu {
public:
    enum class Mode {
        EXEC,
        CYCLES,  // Additional cycles
        RESET,
        INTERRUPT,
    };

    bool m_isStepping = true;

    Memory* m_mem = nullptr;
    Cart* m_cart = nullptr;
    PPU* m_ppu = nullptr;

    uint16_t m_pc = 0x0000;
    uint8_t m_sp = 0x00;
    
    bool m_f_irq = false;
    bool m_f_decimal = false;
    bool m_f_carry = false;
    bool m_f_zero = false;
    bool m_f_overflow = false;
    bool m_f_negative = false;
    uint8_t m_r_a = 0x00;
    uint8_t m_r_x = 0x00;
    uint8_t m_r_y = 0x00;

    bool m_nmi_request = false;
    bool m_irq_request = false;

    Emu() {}

    bool toggleBreakpoint(uint16_t address);
    bool isBreakpoint(uint16_t address);

    void init(Cart* _cart);
    bool isInitialized();
    void reset();
    void stepOperation();
    void stepScanline();
    void stepFrame();
    bool stepCycle();

    unsigned long getCycleCount() const { return m_cycleCount; }
    Mode getMode() { return m_mode; }
    uint8_t getOpcode();
    uint8_t getOpcode(uint16_t addr);
    uint16_t getOpcodeAddress();
    uint8_t getImmediateArg(int offset); 
    uint8_t getImmediateArg(uint16_t addr, int offset);

private:
    Mode m_mode = Mode::RESET;

    std::set<uint16_t> m_breakpoints;
    
    int8_t m_cyclesLeft = 0;  // How many Cycles does the current instruction still have
    uint16_t m_nextOpcodeAddress = 0;
    uint8_t m_nextOpcode = 0;  // The opcode that is now executed
    uint8_t m_lastCycleFetched = false;  // Did a fetch occur in the last cycle, used to step by opcode
    unsigned long m_cycleCount = 0;

    uint16_t m_intVector = IRQ_VECTOR;  // When interrupt occurs, we store the vector here (either NMI or IRQ/BRK)
    bool m_isInterrupt = false;         // True, when BRK is executed from interrupt

    uint8_t getProcStatus(bool setBrk);
    uint8_t setProcStatus(uint8_t value);

    // CPU Initialization after RESET
    void execOpcode();
    void execReset();
    
    void requestInterrupt(uint16_t vector);

    void fetch();
    uint8_t fetchArg();
};
