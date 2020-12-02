#include "nes/mappers/mapper.hpp"
#include "core/util.hpp"

class Cart;

class Mapper001 : public Mapper {
public:
    PACK(union Control {
        struct {
            uint8_t mirroring : 2;
            uint8_t prgMode   : 2;
            uint8_t chrMode   : 2;
        };
        
        uint8_t value;
    });

    Mapper001(Cart&);
    ~Mapper001();

    virtual uint8_t readbCpu(uint16_t address);
    virtual void writebCpu(uint16_t address, uint8_t value);
    virtual void translateCpu(uint16_t addressIn, uint8_t& bankOut, uint16_t& addressOut);

    virtual uint8_t readbPpu(uint16_t address);
    virtual void writebPpu(uint16_t address, uint8_t value);

private:
    uint8_t* m_prgRam;

    prg_bank* m_prgBanks[2];

    unsigned int m_counter = 0;
    unsigned int m_shifter = 0;

    Control m_control;
    
    uint8_t m_prgBankSelect;
    bool m_prgRamEnable;

    void updatePrgBanks();
};
