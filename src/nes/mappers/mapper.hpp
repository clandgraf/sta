#include <cstdint>

class Cart;

class Mapper {
public:
    Mapper(Cart& cart);
    virtual ~Mapper();

    virtual uint8_t readbCpu(uint16_t address) = 0;
    virtual void writebCpu(uint16_t address, uint8_t value) = 0;
    virtual void translateCpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut) = 0;
    
    virtual uint8_t readbPpu(uint16_t address) = 0;
    virtual void writebPpu(uint16_t address, uint8_t value) = 0;

    virtual void reset();
protected:
    Cart& m_cart;
};
