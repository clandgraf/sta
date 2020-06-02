#pragma once

#include <vector>
#include <map>

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
    Disassembler(Emu& emu) : m_emu(emu) {}

    const char* disasmOpcode(uint16_t address, bool* end = nullptr, uint8_t* next = nullptr);
    const char* disasmNextOpcode(bool* end = nullptr, uint8_t* next = nullptr);
    std::shared_ptr<DisasmSegment> disasmSegment(uint16_t addr);

    bool m_showAbsoluteLabels = true;

private:
    Emu& m_emu;

    std::map<uint16_t, DisasmSegmentSptr> m_disassembly;

    DisasmSegmentSptr findSegment(uint16_t addr);
    void mergeSegments(DisasmSegmentSptr segment, DisasmSegmentSptr other);
};
