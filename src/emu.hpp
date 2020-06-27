#pragma once

#include <cstdint>
#include <set>
#include <memory>

#ifdef LOG_EXECUTION
#include <fstream>
#endif

#include "inputs.hpp"

class Memory;
class Cart;
class PPU;
#ifdef LOG_EXECUTION
class Disassembler;
#endif

uint16_t constexpr NMI_VECTOR = 0xfffa;    // Address where NMI starts
uint16_t constexpr RESET_VECTOR = 0xfffc;  // Address where execution starts
uint16_t constexpr IRQ_VECTOR = 0xfffe;    // Address where IRQ/BRK starts

class Emu {
public:
    std::unique_ptr<Disassembler> m_disassembler;

    bool m_logState = false;

    enum class Mode {
        EXEC,
        CYCLES,  // Additional cycles
        RESET,
        INTERRUPT,
        DMA,
    };

    bool m_isStepping = true;

    std::unique_ptr<Memory> m_mem = nullptr;
    std::shared_ptr<Cart> m_cart = nullptr;
    std::shared_ptr<PPU> m_ppu = nullptr;

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

    bool m_breakOnInterrupt = false;

    Emu();
    ~Emu();

    void writeSettings();

    bool toggleBreakpoint(uint16_t address);
    bool isBreakpoint(uint16_t address);

    void init(std::shared_ptr<Cart> _cart);
    bool isInitialized();
    void reset();
    void startDMA(uint8_t page);
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

    uint8_t getProcStatus(bool setBrk);

private:
    std::ofstream m_logOut;

    Mode m_mode = Mode::RESET;

    std::set<uint16_t> m_breakpoints;
    
    /* Cycles and Opcodes */
    int8_t m_cyclesLeft = 0;  // How many Cycles does the current instruction still have
    uint16_t m_nextOpcodeAddress = 0;
    uint8_t m_nextOpcode = 0;  // The opcode that is now executed
    uint8_t m_lastCycleFetched = false;  // Did a fetch occur in the last cycle, used to step by opcode
    unsigned long m_cycleCount = 0;

    /* Interrupts */
    uint16_t m_intVector = IRQ_VECTOR;  // When interrupt occurs, we store the vector here (either NMI or IRQ/BRK)
    bool m_isInterrupt = false;         // True, when BRK is executed from interrupt

    /* DMA */
    int16_t m_dmaCycle = 0;
    uint16_t m_dmaPage = 0;
    void execDma();

    /* Emulator Flow Control */
    bool m_errorInCycle = false;  // Set if error occurs in cycle. Will go into stepping mode.
    bool m_interruptInCycle = false;  // Set if interrupt occurs in cycle. Will go into stepping mode if break on interrupt is set.

    uint8_t setProcStatus(uint8_t value);

    // CPU Initialization after RESET
    void execOpcode();
    void execReset();
    
    void requestInterrupt(uint16_t vector);

    void fetch();
    uint8_t fetchArg();

    // ------------------- cycle primitives ------------------

    uint16_t _m_hi = 0;
    uint16_t _m_lo = 0;

    __forceinline void     _toHilo(const uint16_t& value);
    __forceinline uint16_t _fromHilo();
    __forceinline uint16_t _hilo();
    __forceinline void     _branch(bool flag);
    __forceinline uint8_t  _updateNZ(uint8_t value);
    __forceinline void     _compare(const uint8_t& reg);
    __forceinline void     _push(const uint8_t& value);
    __forceinline uint8_t  _pop();

    /* Helpers that modify Accumulator */
    __forceinline void     _readImd();
    __forceinline void     _readZpg();
    __forceinline void     _readZpgX();
    __forceinline void     _readZpgY();
    __forceinline void     _readAbs();
    __forceinline void     _readAbsX();
    __forceinline void     _readAbsY();
    __forceinline void     _readIndX();
    __forceinline void     _readIndY();
    __forceinline void     _execAdc();
    __forceinline void     _execSbc();
    __forceinline void     _execAnd();
    __forceinline void     _execCmp();
    __forceinline void     _execEor();
    __forceinline void     _execOra();
    __forceinline void     _execBit();
    __forceinline void     _execLd(uint8_t& reg);

    /* Helpers that modify memory inplace */
    __forceinline void     _redmZpg();
    __forceinline void     _redmZpgX();
    __forceinline void     _redmAbs();
    __forceinline void     _redmAbsX();
    __forceinline void     _storeMem();

    template<typename T>
    __forceinline void     _execAsl(T& field);
    template<typename T>
    __forceinline void     _execLsr(T& field);
    template<typename T>
    __forceinline void     _execRol(T& field);
    template<typename T>
    __forceinline void     _execRor(T& field);
    template<typename T>
    __forceinline void     _execInc(T& field);
    template<typename T>
    __forceinline void     _execDec(T& field);

    /* Helpers that store register contents */
    __forceinline void     _redrZpg();
    __forceinline void     _redrZpgX();
    __forceinline void     _redrZpgY();
    __forceinline void     _redrAbs();
    __forceinline void     _redrAbsX();
    __forceinline void     _redrAbsY();
    __forceinline void     _redrIndX();
    __forceinline void     _redrIndY();
    __forceinline void     _storeReg(const uint8_t& reg);
};
