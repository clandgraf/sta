#include <imgui.h>
#define _CRT_SECURE_NO_WARNINGS 1
#include <imgui_memory_editor.h>

#include "emu.hpp"
#include "mem.hpp"
#include "rom.hpp"
#include "core/gui/gui.hpp"
#include "core/gui/manager.hpp"

static MemoryEditor mem_edit;

static void render(Gui::Manager<Emu>::Window& window, Emu& emu) {
    if (emu.isInitialized() && *window.show()) {
        if (ImGui::Begin("Memory", window.show())) {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            char title[10];
            if (ImGui::BeginTabBar("Memory Tabbar", tab_bar_flags)) {
                if (ImGui::BeginTabItem("RAM")) {
                    window.manager.pushMonoFont();
                    mem_edit.DrawContents(emu.m_mem->m_internalRam, 0x800, 0x0000);
                    ImGui::PopFont();
                    ImGui::EndTabItem();
                }
                for (uint8_t i = 0; i < emu.m_cart->prgSize(); i++) {
                    snprintf(title, 10, "PRG %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        window.manager.pushMonoFont();
                        mem_edit.DrawContents(emu.m_cart->m_prgBanks[i], 0x4000, 0x8000 + 0x4000 * i);  // TODO only true for NROM
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                for (uint8_t i = 0; i < emu.m_cart->chrSize(); i++) {
                    snprintf(title, 10, "CHR %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        window.manager.pushMonoFont();
                        mem_edit.DrawContents(emu.m_cart->chr(i), 0x2000, 0x2000 * i);
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
    }
}

void createMemoryViewer(Gui::Manager<Emu>& manager) {
    manager.window("debugger-view-memory", "Memory", render);
}