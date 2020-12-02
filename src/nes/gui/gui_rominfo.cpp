#include <imgui.h>

#include "emu.hpp"
#include "rom.hpp"
#include "mappers.hpp"
#include "core/gui/manager.hpp"

static void render(Gui::Window<Emu>& window, Emu& emu) {
    if (emu.isInitialized() && *window.show()) {
        if (ImGui::Begin("ROM Info", window.show())) {
            ImGui::Text("File: %s", emu.m_cart->m_name.c_str());
            ImGui::Text("Mapper: %d, %s", emu.m_cart->m_mapperId, mappers[emu.m_cart->m_mapperId]);
            ImGui::Text("PRG ROM #: %d", emu.m_cart->prgSize());
            ImGui::Text(emu.m_cart->m_useChrRam ? "CHR RAM #: %d" : "CHR ROM #: %d", emu.m_cart->chrSize());
        }
        ImGui::End();
    }
}

void createRomInfo(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-rominfo", "ROM Info", render);
}
