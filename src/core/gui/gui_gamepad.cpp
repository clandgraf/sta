#include "core/gui/gui_gamepad.hpp"

static void renderButton(int gamepadIndex, Gamepad::ButtonType btn) {
    auto s = Gamepad::buttonTypeToLabel.find(btn);
    if (s != Gamepad::buttonTypeToLabel.end()) {
        ImGui::SameLine();
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

void renderGamepad(int gamepadIndex) {
    ImGui::Text("Gamepad %d", gamepadIndex);
    for (int i = Gamepad::ButtonTypeBegin; i < Gamepad::ButtonTypeEnd; i++) {
        Gamepad::ButtonType btn = (Gamepad::ButtonType)i;
        renderButton(gamepadIndex, btn);
    }
}