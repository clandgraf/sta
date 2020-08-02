#include <imgui.h>

#include "inputs.hpp"
#include "gui/gui_window.hpp"

static Input::Controller input0;
static Input::Controller input1;

static void render(Gui::Window& window, Emu& emu) {
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

void createControls() {
    Gui::Window::createWindow("debugger-view-controls", "Controls", render);
}