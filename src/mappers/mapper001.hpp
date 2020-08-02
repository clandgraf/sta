#include "mapper.hpp"

class Cart;

class Mapper001 : public Mapper {
public:
    Mapper001(Cart&);
    ~Mapper001();

    virtual uint8_t readbCpu(uint16_t address);
    virtual void writebCpu(uint16_t address, uint8_t value);
    virtual void translateCpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut);

    virtual uint8_t readbPpu(uint16_t address);
    virtual void writebPpu(uint16_t address, uint8_t value);
};
