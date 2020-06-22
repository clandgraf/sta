#pragma once

#include <vector>
#include <map>
#include <ostream>

class Emu;

struct DisasmLine {
    uint16_t offset;
    std::string repr;
};

struct DisasmSegment {
    uint16_t m_start;
    uint16_t m_length = 0;

    std::map<uint16_t, DisasmLine> m_lines;

    DisasmSegment(uint16_t start) : m_start(start) {}
};

class Disassembler {

    using DisasmSegmentSptr = std::shared_ptr<DisasmSegment>;

public:
    Disassembler(Emu& emu);

    void writeSettings();

    const char* disasmOpcode(uint16_t address, bool* end = nullptr, uint8_t* next = nullptr);

    void logState(std::ostream& os);

    const char* disasmNextOpcode(bool* end = nullptr, uint8_t* next = nullptr);
    DisasmSegmentSptr disasmSegment(uint16_t addr);
    DisasmSegmentSptr continueSegment(DisasmSegmentSptr segment);

    void refresh();

    bool m_translateCartSpace = true;       // Translate Addresses into Cartridge Space if applicable
    bool m_showAbsoluteLabels = true;       // Show Labels for Absolute Addressing
    bool m_absoluteBranchAddresses = true;  // Display absolute Branch Addresses

private:
    Emu& m_emu;

    std::map<uint16_t, DisasmSegmentSptr> m_disassembly;

    DisasmSegmentSptr findSegment(uint16_t addr, bool& adjacent);
    void mergeSegments(DisasmSegmentSptr segment, DisasmSegmentSptr other);

    bool translateToCartSpace(uint16_t address) const;
};
