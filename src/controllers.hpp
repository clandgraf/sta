#pragma once

#include <cstdint>

#include "inputs.hpp"

class Port {
    virtual void update(Input::Controller controllerState) = 0;
    virtual void write(uint8_t v) = 0;
    virtual uint8_t read() = 0;
};

class Controller : public Port
{
    bool m_latched;
    uint8_t m_shiftButtons;

public:
    Controller() : m_latched(false), m_shiftButtons(0) {}

    void update(Input::Controller controllerState);
    void write(uint8_t v);
    uint8_t read();
};
