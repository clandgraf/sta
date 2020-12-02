#include <imgui.h>

#include "nes/emu.hpp"
#include "nes/ppu.hpp"
#include "core/gui/manager.hpp"
#include "IconsMaterialDesign.h"

static void ImGui_AttachTooltip(const char* str) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(str);
        ImGui::EndTooltip();
    }
}

static void renderStateControl(const char8_t* icon, const char* tooltip, std::function<void()> action) {
    if (ImGui::Button(reinterpret_cast<const char*>(icon))) {
        action();
    }
    ImGui_AttachTooltip(tooltip);
}

void render(Gui::Window<Emu>& window, Emu& emu) {
    if (emu.isInitialized() && *window.show()) {
        if (ImGui::Begin("Emu State", window.show())) {
            renderStateControl(ICON_MD_PLAY_ARROW, "Continue", [&emu] { emu.m_isStepping = false; });
            ImGui::SameLine();
            renderStateControl(ICON_MD_ARROW_FORWARD, "Step Operation", [&emu] { emu.stepOperation(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_ARROW_UPWARD, "Step Out", [&emu] { emu.stepOut(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_CAMERA, "Step Frame", [&emu] { emu.stepFrame(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_SKIP_NEXT, "Step Scanline", [&emu] { emu.stepScanline(); });

            window.manager.pushMonoFont();
            ImGui::Text("CPU Cycles: %d", emu.getCycleCount());
            ImGui::Text("PC: %04x  Carry:    %01x", emu.m_pc, emu.m_f_carry);
            ImGui::Text("SP: %02x    Zero:     %01x", emu.m_sp, emu.m_f_zero);
            ImGui::Text("A:  %02x    IRQ:      %01x", emu.m_r_a, emu.m_f_irq);
            ImGui::Text("X:  %02x    Decimal:  %01x", emu.m_r_x, emu.m_f_decimal);
            ImGui::Text("Y:  %02x    Overflow: %01x", emu.m_r_y, emu.m_f_overflow);
            ImGui::Text("          Negative: %01x", emu.m_f_negative);
            ImGui::Separator();
            ImGui::Text("PPU Cycles: %d", emu.m_ppu->getCycleCount());
            ImGui::Text("PPU Scanline: %d", emu.m_ppu->m_scanline);
            ImGui::PopFont();
        }
        ImGui::End();
    }
}

void createEmuState(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-state", "Emu State", render);
}