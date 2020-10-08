#include <imgui.h>

#include "emu.hpp"
#include "rom.hpp"
#include "core/gui/manager.hpp"
#include "core/gui/opengl_surface.hpp"

static std::shared_ptr<Gui::Surface> patternTableSurface;

static Palette::Color colors[4] = {
    { 0xff, 0xff, 0xff },
    { 0xbb, 0xbb, 0xbb },
    { 0x77, 0x77, 0x77 },
    { 0x00, 0x00, 0x00 },
};

static void render(Gui::Manager<Emu>::Window& window, Emu&) {
    if (*window.show()) {
        if (ImGui::Begin("Pattern Table", window.show())) {
            ImGui::Image((void*)(intptr_t)patternTableSurface->getTexture(), ImVec2(512, 256));
        }
        ImGui::End();
    }
}

static void refreshPatternTable(Emu& emu) {
    for (int table = 0; table < 2; table++) {
        for (int tile = 0; tile < 256; tile++) {
            uint16_t offset = (table ? 0x1000 : 0) + tile * 0x10;
            for (int row = 0; row < 8; row++) {
                uint8_t p0 = emu.m_cart->readb_ppu(offset + row + 0);
                uint16_t p1 = emu.m_cart->readb_ppu(offset + row + 8);
                for (int col = 0; col < 8; col++) {
                    uint8_t pxl = ((p0 >> (7 - col)) & 1) | (((p1 >> (7 - col)) & 1) << 1);

                    size_t textureX = ((table ? 128 : 0) + (tile % 16) * 8 + col);
                    size_t textureY = ((tile / 16) * 8 + row);

                    patternTableSurface->setPixel(textureX, textureY, colors[pxl]);
                }
            }
        }
    }

    patternTableSurface->upload();
}

static void init(Gui::Manager<Emu>::Window& window, Emu&) {
    patternTableSurface = std::make_shared<Gui::Surface>(2 * 128, 128);
    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 2 * 128; x++) {
            patternTableSurface->setPixel(x, y, { 0, 0, 0 });
        }
    }

    patternTableSurface->upload();
}

static void teardown(Gui::Manager<Emu>::Window& window, Emu&) {
    patternTableSurface = nullptr;
}

void createPatternTable(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-patterntbl", "Pattern Table", render, init, teardown);
    manager.action("Debugger", "Refresh Pattern Table", refreshPatternTable);
}
