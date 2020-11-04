#include <imgui.h>

#include "core/gamepad.hpp"
#include "core/gui/manager.hpp"

static void renderButton(int gamepadIndex, Gamepad::ButtonType btn) {
    auto s = Gamepad::buttonTypeToLabel.find(btn);
    if (s != Gamepad::buttonTypeToLabel.end()) {
        ImGui::Text(s->second.c_str());

        bool isActive = Gamepad::isEmuButtonActive(gamepadIndex, btn);
        bool isPressed = Gamepad::isEmuButtonPressed(gamepadIndex, btn);
        ImGui::SameLine();
        ImGui::Checkbox("", &isActive);
        ImGui::SameLine();
        ImGui::Checkbox("", &isPressed);
        Gamepad::setEmuButton(gamepadIndex, btn, isPressed, isActive);
    }
    
}

static void renderGamepad(int gamepadIndex) {
    char buf[32];

    snprintf(buf, 32, "Gamepad %i", gamepadIndex);
    ImGui::Text(buf);
    for (int i = Gamepad::ButtonTypeBegin; i < Gamepad::ButtonTypeEnd; i++) {
        Gamepad::ButtonType btn = (Gamepad::ButtonType) i;
        
    }
}

template<class EmuType>
static void render(Gui::Manager<EmuType>::Window& window, EmuType& emu) {
    if (*window.show()) {
        if (ImGui::Begin("Controls", window.show())) {


            ImGui::Text("Controller 1");
            ImGui::Checkbox("Up", &(input0.d_up));
            ImGui::SameLine();
            ImGui::Checkbox("Left", &(input0.d_left));
            ImGui::SameLine();
            ImGui::Checkbox("A", &(input0.btn_a));
            ImGui::SameLine();
            ImGui::Checkbox("Start", &(input0.start));

            ImGui::Checkbox("Down", &(input0.d_down));
            ImGui::SameLine();
            ImGui::Checkbox("Right", &(input0.d_right));
            ImGui::SameLine();
            ImGui::Checkbox("B", &(input0.btn_b));
            ImGui::SameLine();
            ImGui::Checkbox("Select", &(input0.select));

            ImGui::Separator();

            ImGui::Text("Controller 2");
            ImGui::Checkbox("Up", &(input1.d_up));
            ImGui::SameLine();
            ImGui::Checkbox("Left", &(input1.d_left));
            ImGui::SameLine();
            ImGui::Checkbox("A", &(input1.btn_a));
            ImGui::SameLine();
            ImGui::Checkbox("Start", &(input1.start));

            ImGui::Checkbox("Down", &(input1.d_down));
            ImGui::SameLine();
            ImGui::Checkbox("Right", &(input1.d_right));
            ImGui::SameLine();
            ImGui::Checkbox("B", &(input1.btn_b));
            ImGui::SameLine();
            ImGui::Checkbox("Select", &(input1.select));
        }
        ImGui::End();
    }
}

template<class EmuType>
void createGamepad(Gui::Manager<EmuType>& manager) {
    manager.window("debugger-view-controls", "Controls", render);
}
