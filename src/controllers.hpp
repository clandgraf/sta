#pragma once

#include <cstdint>

#include "inputs.hpp"

class Port {
public:
    virtual void update() = 0;
    virtual void write(uint8_t v) = 0;
    virtual uint8_t read() = 0;
};

class Controller : public Port
{
    bool m_latched;
    uint8_t m_shiftButtons;

    const Input::Controller& m_state;

public:
    Controller(const Input::Controller& state) 
        : m_latched(false)
        , m_shiftButtons(0)
        , m_state(state) {}

    void update();
    void write(uint8_t v);
    uint8_t read();
};
