#pragma once

#include <map>
#include <array>
#include <string>
#include <GLFW/glfw3.h>

namespace Gamepad {
    using Scancode = int;

    extern const std::array<int, 5> GlfwGamepads;
    
    extern const int ButtonTypeBegin;
    extern const int ButtonTypeEnd;
    
    enum ButtonType {
        Up, Down, Left, Right, ButtonA, ButtonB, ButtonX, ButtonY, ButtonL1, ButtonR1, ButtonL2, ButtonR2, ButtonL3, ButtonR3, Start, Select,
    };
    
    extern const std::map<ButtonType, std::string> buttonTypeToLabel;

    enum Button {
        Up_0, Down_0, Left_0, Right_0, ButtonA_0, ButtonB_0, ButtonX_0, ButtonY_0, ButtonL1_0, ButtonR1_0, ButtonL2_0, ButtonR2_0, ButtonL3_0, ButtonR3_0, Start_0, Select_0,
        Up_1, Down_1, Left_1, Right_1, ButtonA_1, ButtonB_1, ButtonX_1, ButtonY_1, ButtonL1_1, ButtonR1_1, ButtonL2_1, ButtonR2_1, ButtonL3_1, ButtonR3_1, Start_1, Select_1,
        Up_2, Down_2, Left_2, Right_2, ButtonA_2, ButtonB_2, ButtonX_2, ButtonY_2, ButtonL1_2, ButtonR1_2, ButtonL2_2, ButtonR2_2, ButtonL3_2, ButtonR3_2, Start_2, Select_2,
        Up_3, Down_3, Left_3, Right_3, ButtonA_3, ButtonB_3, ButtonX_3, ButtonY_3, ButtonL1_3, ButtonR1_3, ButtonL2_3, ButtonR2_3, ButtonL3_3, ButtonR3_3, Start_3, Select_3,
        Up_4, Down_4, Left_4, Right_4, ButtonA_4, ButtonB_4, ButtonX_4, ButtonY_4, ButtonL1_4, ButtonR1_4, ButtonL2_4, ButtonR2_4, ButtonL3_4, ButtonR3_4, Start_4, Select_4,

        None,
    };

    extern void loadSettings();
    extern void writeSettings();

    extern bool readGamepadState(Gamepad::Button btn);
    extern void invalidate();
    extern void invalidate(int gamepadId);
    extern bool isEmuButtonActive(int gamepadIndex, ButtonType);
    extern bool isEmuButtonPressed(int gamepadIndex, ButtonType); 
    extern void setEmuButton(int gamepadIndex, Gamepad::ButtonType btn, bool pressed, bool active);
    extern bool isPressed(Gamepad::Button btn);
    extern bool isEscapePressed();
    extern void reset();
    extern void onKeydown(Scancode, bool pressed);
    extern void setScancode(Scancode, Button);
    extern void onWaitForInput(Button button);
}
