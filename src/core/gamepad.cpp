#include "core/gamepad.hpp"
#include "core/util.hpp"

namespace Gamepad {
    const std::array<int, 5> GlfwGamepads = {
        GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, GLFW_JOYSTICK_3, GLFW_JOYSTICK_4, GLFW_JOYSTICK_5
    };

    const std::array<const std::map<int, Button>, 5> GlfwButtonMap = {
        std::map<int, Button>({
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    Left_0     },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   Right_0    },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    Down_0     },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP,      Up_0       },
            { GLFW_GAMEPAD_BUTTON_A,            ButtonA_0  },
            { GLFW_GAMEPAD_BUTTON_B,            ButtonB_0  },
            { GLFW_GAMEPAD_BUTTON_X,            ButtonX_0  },
            { GLFW_GAMEPAD_BUTTON_Y,            ButtonY_0  },
            { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  ButtonL1_0 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ButtonR1_0 },
            { GLFW_GAMEPAD_BUTTON_START,        Start_0    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_0   }
        }),
        std::map<int, Button>({
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    Left_1     },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   Right_1    },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    Down_1     },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP,      Up_1       },
            { GLFW_GAMEPAD_BUTTON_A,            ButtonA_1  },
            { GLFW_GAMEPAD_BUTTON_B,            ButtonB_1  },
            { GLFW_GAMEPAD_BUTTON_X,            ButtonX_1  },
            { GLFW_GAMEPAD_BUTTON_Y,            ButtonY_1  },
            { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  ButtonL1_1 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ButtonR1_1 },
            { GLFW_GAMEPAD_BUTTON_LEFT_THUMB,   ButtonL3_1 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,  ButtonR3_1 },
            { GLFW_GAMEPAD_BUTTON_START,        Start_1    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_1   }
        }),
        std::map<int, Button>({
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    Left_2     },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   Right_2    },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    Down_2     },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP,      Up_2       },
            { GLFW_GAMEPAD_BUTTON_A,            ButtonA_2  },
            { GLFW_GAMEPAD_BUTTON_B,            ButtonB_2  },
            { GLFW_GAMEPAD_BUTTON_X,            ButtonX_2  },
            { GLFW_GAMEPAD_BUTTON_Y,            ButtonY_2  },
            { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  ButtonL1_2 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ButtonR1_2 },
            { GLFW_GAMEPAD_BUTTON_LEFT_THUMB,   ButtonL3_2 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,  ButtonR3_2 },
            { GLFW_GAMEPAD_BUTTON_START,        Start_2    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_2   }
        }),
        std::map<int, Button>({
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    Left_3     },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   Right_3    },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    Down_3     },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP,      Up_3       },
            { GLFW_GAMEPAD_BUTTON_A,            ButtonA_3  },
            { GLFW_GAMEPAD_BUTTON_B,            ButtonB_3  },
            { GLFW_GAMEPAD_BUTTON_X,            ButtonX_3  },
            { GLFW_GAMEPAD_BUTTON_Y,            ButtonY_3  },
            { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  ButtonL1_3 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ButtonR1_3 },
            { GLFW_GAMEPAD_BUTTON_LEFT_THUMB,   ButtonL3_3 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,  ButtonR3_3 },
            { GLFW_GAMEPAD_BUTTON_START,        Start_3    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_3   }
        }),
        std::map<int, Button>({
            { GLFW_GAMEPAD_BUTTON_DPAD_LEFT,    Left_4     },
            { GLFW_GAMEPAD_BUTTON_DPAD_RIGHT,   Right_4    },
            { GLFW_GAMEPAD_BUTTON_DPAD_DOWN,    Down_4     },
            { GLFW_GAMEPAD_BUTTON_DPAD_UP,      Up_4       },
            { GLFW_GAMEPAD_BUTTON_A,            ButtonA_4  },
            { GLFW_GAMEPAD_BUTTON_B,            ButtonB_4  },
            { GLFW_GAMEPAD_BUTTON_X,            ButtonX_4  },
            { GLFW_GAMEPAD_BUTTON_Y,            ButtonY_4  },
            { GLFW_GAMEPAD_BUTTON_LEFT_BUMPER,  ButtonL1_4 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_BUMPER, ButtonR1_4 },
            { GLFW_GAMEPAD_BUTTON_LEFT_THUMB,   ButtonL3_4 },
            { GLFW_GAMEPAD_BUTTON_RIGHT_THUMB,  ButtonR3_4 },
            { GLFW_GAMEPAD_BUTTON_START,        Start_4    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_4   },
        })
    };

    bool readGamepadState(Button btn);
    void refreshGamepadState(int controllerId);

    Button getButton(int gamepadIndex, ButtonType buttonType);

    static std::array<bool, 5> isGamepadStateValid;

    static Button waitingFor = None;

    static std::map<Scancode, Button> kbdMap;
    static std::map<Button, Scancode> revKbdMap;

    static std::map<Button, bool> kbdState;

    static std::map<Button, bool> padState;

    static std::map<Button, TriStateBool> emuState;
}

Gamepad::Button Gamepad::getButton(int gamepadIndex, Gamepad::ButtonType buttonType) {
    return (Button)(gamepadIndex * buttonType);
}

void Gamepad::reset() {
    for (int b = ButtonBegin; b <= ButtonEnd; b++) {
        Button btn = (Button)b;
        kbdState[btn] = false;
        padState[btn] = false;
        emuState[btn] = TriStateBool();
    }
}

bool Gamepad::isEmuButtonPressed(int gamepadIndex, Gamepad::ButtonType btn) {
    return emuState[getButton(gamepadIndex, btn)];
}

bool Gamepad::isEmuButtonActive(int gamepadIndex, Gamepad::ButtonType btn) {
    return !emuState[getButton(gamepadIndex, btn)].isUndefined();
}

void Gamepad::setEmuButton(int gamepadIndex, Gamepad::ButtonType btn, bool pressed, bool active) {
    emuState[getButton(gamepadIndex, btn)].set(pressed, active);
}

void Gamepad::invalidate() {
    for (int i = 0; i < 5; i++) {
        invalidate(i);
    }
}

void Gamepad::invalidate(int controllerId) {
    isGamepadStateValid[controllerId] = false;
}

void Gamepad::refreshGamepadState(int controllerId) {
    int glfwGamepadId = GlfwGamepads[controllerId];
    if (glfwJoystickPresent(glfwGamepadId)) {
        GLFWgamepadstate state;
        glfwGetGamepadState(glfwGamepadId, &state);
        for (const auto& entry : GlfwButtonMap[controllerId]) {
            padState[entry.second] = state.buttons[entry.first];
        }
    }
    isGamepadStateValid[controllerId] = true;
}

void Gamepad::loadSettings() {
    auto keys = Settings::object["keys"];
    for (auto entry : keys.items()) {
        std::string key = entry.key();
        std::string value = entry.value();

        try {
            if (key.find("scancode_", 0) == 0) {
                Scancode scancode = std::stoi(key.substr(9));
                auto cdef = Gamepad::stringToButton.find(value.c_str());
                if (cdef == Gamepad::stringToButton.end()) {
                    LOG_ERR << "Illegal ControllerDef: " << value << "\n";
                    continue;
                }

                setScancode(scancode, cdef->second);
            }
        }
        catch (std::logic_error& e) {
            LOG_ERR << "Error Parsing Scancode '" << key << "': "
                << e.what() << "\n";
        }
    }
}

void Gamepad::writeSettings() {
    Settings::erase("keys");
    for (auto entry : kbdMap) {
        std::stringstream ss;
        ss << "scancode_" << std::to_string(entry.first);
        Settings::setIn({ "keys", ss.str() },
            buttonToString.at(entry.second));
    }
}

bool Gamepad::readGamepadState(Gamepad::Button btn) {
    int controllerId = btn % 16;
    if (!isGamepadStateValid[controllerId]) {
        refreshGamepadState(controllerId);
    }
    return padState[btn];
}

bool Gamepad::isPressed(Gamepad::Button btn) {
    TriStateBool e = emuState[btn];
    return e.isUndefined() ? (kbdState[btn] || readGamepadState(btn)) : e;
}

void Gamepad::setScancode(Scancode sc, Button btn) {
    kbdMap[sc] = btn;
    revKbdMap[btn] = sc;
}

void Gamepad::onKeydown(Gamepad::Scancode sc, bool pressed) {
    if (waitingFor != Button::None) {
        setScancode(sc, waitingFor);
        waitingFor = Button::None;
        return;
    }

    auto btn = kbdMap.find(sc);
    auto input = btn == kbdMap.end() ? Button::None : btn->second;

    kbdState[input] = pressed;
}

void Gamepad::onWaitForInput(Gamepad::Button button) {
    waitingFor = button;
}
