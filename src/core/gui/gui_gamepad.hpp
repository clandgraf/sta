#pragma once

#include <imgui.h>

#include "core/gui/manager.hpp"

void renderButton(int gamepadIndex, Gamepad::ButtonType btn);
void renderGamepad(int gamepadIndex);

template<class EmuType>
void render(Gui::Window<EmuType>& window, EmuType& emu) {
    if (*window.show()) {
        if (ImGui::Begin("Controls", window.show())) {
            for (int i = 0; i < Gamepad::GlfwGamepads.size(); i++) {
                renderGamepad(i);
            }
        }
        ImGui::End();
    }
}

template<class EmuType>
void createGamepad(Gui::Manager<EmuType>& manager) {
    manager.window("debugger-view-controls", "Controls", render<EmuType>);
}
