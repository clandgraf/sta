#include <GLFW/glfw3.h>
#include <map>
#include <unordered_set>
#include <stdexcept>

#include "inputs.hpp"
#include "core/util.hpp"

namespace Input {
    static Input::State keyboardState;

    static Input::State outputState;

    static ControllerDef waitingFor = None;

    static std::map<Scancode, ControllerDef> keyConfig;

    static std::map<ControllerDef, std::unordered_set<Scancode>> reverseKeyConfig; 
}

void Input::dispatchInput(Scancode scancode, bool pressed) {
    if (waitingFor != ControllerDef::None) {
        setScancode(scancode, waitingFor);
        waitingFor = ControllerDef::None;
        return;
    }

    auto button = keyConfig.find(scancode);
    auto input = button == keyConfig.end() ? ControllerDef::None : button->second;

    switch (input) {
    case Menu:      keyboardState.openMenu       = pressed; break;    

    case Up_0:      keyboardState.input0.d_up    = pressed; break;
    case Down_0:    keyboardState.input0.d_down  = pressed; break;
    case Left_0:    keyboardState.input0.d_left  = pressed; break;
    case Right_0:   keyboardState.input0.d_right = pressed; break;
    case ButtonA_0: keyboardState.input0.btn_a   = pressed; break;
    case ButtonB_0: keyboardState.input0.btn_b   = pressed; break;
    case Start_0:   keyboardState.input0.start   = pressed; break;
    case Select_0:  keyboardState.input0.select  = pressed; break;

    case Up_1:      keyboardState.input1.d_up    = pressed; break;
    case Down_1:    keyboardState.input1.d_down  = pressed; break;
    case Left_1:    keyboardState.input1.d_left  = pressed; break;
    case Right_1:   keyboardState.input1.d_right = pressed; break;
    case ButtonA_1: keyboardState.input1.btn_a   = pressed; break;
    case ButtonB_1: keyboardState.input1.btn_b   = pressed; break;
    case Start_1:   keyboardState.input1.start   = pressed; break;
    case Select_1:  keyboardState.input1.select  = pressed; break;
    }
}

void Input::waitForInput(ControllerDef def) {
    waitingFor = def;
}

Input::ControllerDef Input::getWaitingForInput() {
    return waitingFor;
}

const Input::State& Input::getState() {
    outputState.input0 = keyboardState.input0;
    outputState.input1 = keyboardState.input1;
    outputState.openMenu = keyboardState.openMenu;

    if (glfwJoystickPresent(GLFW_JOYSTICK_1)) {
        GLFWgamepadstate state;
        glfwGetGamepadState(GLFW_JOYSTICK_1, &state);
        outputState.input0.btn_a = outputState.input0.btn_a || state.buttons[GLFW_GAMEPAD_BUTTON_A];
        outputState.input0.btn_b = outputState.input0.btn_b || state.buttons[GLFW_GAMEPAD_BUTTON_X];
        outputState.input0.start = outputState.input0.start || state.buttons[GLFW_GAMEPAD_BUTTON_START];
        outputState.input0.select = outputState.input0.select || state.buttons[GLFW_GAMEPAD_BUTTON_BACK];
        outputState.input0.d_up = outputState.input0.d_up || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_UP];
        outputState.input0.d_down = outputState.input0.d_down || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_DOWN];
        outputState.input0.d_left = outputState.input0.d_left || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_LEFT];
        outputState.input0.d_right = outputState.input0.d_right || state.buttons[GLFW_GAMEPAD_BUTTON_DPAD_RIGHT];
    }

    return outputState;
}

void Input::clearButton(ControllerDef def) {
    for (auto scancode: reverseKeyConfig[def]) {
        keyConfig.erase(scancode);
    }
    reverseKeyConfig[def].clear();
}

void Input::setScancode(Scancode scancode, ControllerDef def) {
    keyConfig[scancode] = def;
    
    reverseKeyConfig[def].insert(scancode);
}

const std::unordered_set<Input::Scancode>& Input::getScancodes(ControllerDef def) {
    return reverseKeyConfig[def];
}

void Input::resetMenuRequest() {
    keyboardState.openMenu = false;
}

void Input::loadSettings() {
    auto keys = Settings::object["keys"];
    for (auto entry: keys.items()) {
        std::string key = entry.key();
        std::string value = entry.value();
        
        try {
            if (key.find("scancode_", 0) == 0) {
                Scancode scancode = std::stoi(key.substr(9));
                auto cdef = Input::stringToDef.find(value.c_str());
                if (cdef == Input::stringToDef.end()) {
                    LOG_ERR << "Illegal ControllerDef: " << value << "\n";
                    continue;
                }
    
                setScancode(scancode, cdef->second);
            }
        } catch (std::logic_error& e) {
            LOG_ERR << "Error Parsing Scancode '" << key << "': " 
                    << e.what() << "\n";
        }
    }
}

void Input::writeSettings() {
    Settings::erase("keys");
    for (auto entry: keyConfig) {
        std::stringstream ss; 
        ss << "scancode_" << std::to_string(entry.first);
        Settings::setIn({"keys", ss.str()}, 
                        defToString.at(entry.second));
    }
}
