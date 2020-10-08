#include <imgui.h>

#include "core/gui/gui.hpp"
#include "core/gui/manager.hpp"
#include "disasm.hpp"
#include "emu.hpp"

static void render(Gui::Manager<Emu>::Window& window, Emu& emu) {
    using DisasmSegmentSptr = std::shared_ptr<DisasmSegment>;

    if (emu.getMode() != Emu::Mode::RESET && *window.show()) {
        uint16_t address = emu.getOpcodeAddress();
        emu.m_disassembler->disasmSegment(address);

        if (ImGui::Begin("Disassembly", window.show())) {
            window.manager.pushMonoFont();
            for (auto& segment : *emu.m_disassembler) {
                for (auto& entry : segment.second->m_lines) {
                    auto& line = entry.second;

                    bool isBreakpoint = emu.isBreakpoint(line.offset);
                    static char buffer[0xff];
                    snprintf(buffer, 0xff, "%s###%04x_brk", isBreakpoint ? ">" : " ", line.offset);
                    if (ImGui::Selectable(buffer, isBreakpoint)) {
                        emu.toggleBreakpoint(line.offset);
                    }
                    ImGui::SameLine();
                    if (line.offset == address) {
                        window.manager.pushHighlightText();
                        ImGui::Text(line.repr.c_str());
                        ImGui::PopStyleColor();
                    }
                    else {
                        ImGui::Text(line.repr.c_str());
                    }
                }
                if (ImGui::Button("continue...")) {
                    emu.m_disassembler->continueSegment(segment.second);
                    break;
                }
                ImGui::Separator();
            }
            ImGui::PopFont();

        }
        ImGui::End();
    }
}

void createDisassembly(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-disassembly", "Disassembly", render);
}
