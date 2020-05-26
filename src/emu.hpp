#pragma once

#include <cstdint>

class mem;
class cart;

// Address where execution starts
uint16_t constexpr RESET_VECTOR = 0xfffc;

struct registers {
    
};

class emu {
public:
    mem* m_mem = nullptr;
    cart* m_cart = nullptr;

    uint16_t m_pc = 0x0000;
    uint8_t m_sp = 0x00;
    
    bool m_f_irq = false;
    bool m_f_decimal = false;

    emu() {}

    void init(cart* _cart);
    void reset();
    void step();

private:
    enum class mode {
        EXEC,
        RESET,
    };

    mode m_mode = mode::RESET;

    // How many Cycles does the current instruction still have
    int8_t m_cycles_left = 0;
    
    // CPU Initialization after RESET
    void exec_reset();

    void fetch();
};
