#pragma once

#include <cstdint>

class Memory;
class Cart;
class PPU;

// Address where execution starts
uint16_t constexpr RESET_VECTOR = 0xfffc;

struct registers {
    
};

class Emu {
public:
    enum class Mode {
        EXEC,
        CYCLES,  // Additional cycles
        RESET,
    };

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

    Emu() {}

    void init(Cart* _cart);
    bool isInitialized();
    void reset();
    int8_t stepOperation();
    int8_t stepCycle();

    Mode getMode() { return m_mode; }
    uint8_t getOpcode();
    uint8_t getOpcode(uint16_t addr);
    uint16_t getOpcodeAddress();
    uint8_t getImmediateArg(int offset); 
    uint8_t getImmediateArg(uint16_t addr, int offset);

private:
    Mode m_mode = Mode::RESET;

    
    int8_t m_cyclesLeft = 0;  // How many Cycles does the current instruction still have
    uint8_t m_next_opcode = 0;  // The opcode that is now executed
    uint8_t m_last_cycle_fetched = false;  // Did a fetch occur in the last cycle, used to step by opcode
    
    // CPU Initialization after RESET
    void exec_opcode();
    void exec_reset();

    void fetch();
    uint8_t fetch_arg();
};
