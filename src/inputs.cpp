#include <map>
#include <unordered_set>

#include "inputs.hpp"
#include "util.hpp"

namespace Input {
    static Input::State state;

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
    case Menu:      state.openMenu       = pressed; break;    

    case Up_0:      state.input0.d_up    = pressed; break;
    case Down_0:    state.input0.d_down  = pressed; break;
    case Left_0:    state.input0.d_left  = pressed; break;
    case Right_0:   state.input0.d_right = pressed; break;
    case ButtonA_0: state.input0.btn_a   = pressed; break;
    case ButtonB_0: state.input0.btn_b   = pressed; break;
    case Start_0:   state.input0.start   = pressed; break;
    case Select_0:  state.input0.select  = pressed; break;

    case Up_1:      state.input1.d_up    = pressed; break;
    case Down_1:    state.input1.d_down  = pressed; break;
    case Left_1:    state.input1.d_left  = pressed; break;
    case Right_1:   state.input1.d_right = pressed; break;
    case ButtonA_1: state.input1.btn_a   = pressed; break;
    case ButtonB_1: state.input1.btn_b   = pressed; break;
    case Start_1:   state.input1.start   = pressed; break;
    case Select_1:  state.input1.select  = pressed; break;
    }
}

void Input::waitForInput(ControllerDef def) {
    waitingFor = def;
}

Input::ControllerDef Input::getWaitingForInput() {
    return waitingFor;
}

const Input::State& Input::getState() {
    return Input::state;
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
    state.openMenu = false;
}

void Input::loadSettings() {
    
}

void Input::writeSettings() {
    Settings::erase("keys");
    for (auto entry: keyConfig) {
        // TODO entry.first is interpreted as array index
        Settings::setIn({"keys", std::to_string(entry.first)}, 
                        defToString.at(entry.second));
    }
}
