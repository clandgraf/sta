#include <imgui.h>

#include "emu.hpp"
#include "ppu.hpp"
#include "core/gui/manager.hpp"

static int selectedIndex = -1;
static int selectedOamEntry = -1;

static void renderOamEntry(unsigned int index, const PPU::OamEntry& entry) {
    if (entry.y >= 0xef) {
        return;
    }

    char buffer[64];
    snprintf(buffer, 64, "%02X  Y: %02X  X: %02X  I: %02X", index, entry.y, entry.x, entry.tileIndex);
    if (ImGui::Selectable(buffer, selectedIndex == index)) {
        if (selectedIndex != index) {
            selectedIndex = index;
        } else {
            selectedIndex = -1;
        }
    }
    ImGui::Separator();
}

static void render(Gui::Manager<Emu>::Window& window, Emu& emu) {
    if (emu.isInitialized() && *window.show()) {
        if (ImGui::Begin("OAM Viewer", window.show())) {
            const PPU::OamEntry* entries = emu.m_ppu->getSprites();
            for (int i = 0; i < 64; i++) {
                renderOamEntry(i, entries[i]);
            }
        }
        ImGui::End();
    }
}

void createOamViewer(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-oam", "OAM Viewer", render);
}