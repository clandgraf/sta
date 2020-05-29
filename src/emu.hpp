#pragma once

#include <cstdint>

class mem;
class Cart;

// Address where execution starts
uint16_t constexpr RESET_VECTOR = 0xfffc;

struct registers {
    
};

class Emu {
public:
    enum class Mode {
        EXEC,
        RESET,
    };

    mem* m_mem = nullptr;
    Cart* m_cart = nullptr;

    uint16_t m_pc = 0x0000;
    uint8_t m_sp = 0x00;
    
    bool m_f_irq = false;
    bool m_f_decimal = false;

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

    // How many Cycles does the current instruction still have
    int8_t m_cycles_left = 0;
    uint8_t m_next_opcode = 0;
    uint8_t m_last_cycle_fetched = false;
    
    // CPU Initialization after RESET
    void exec_opcode();
    void exec_reset();

    void fetch();
};
