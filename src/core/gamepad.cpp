#include "core/gamepad.hpp"
#include "core/util.hpp"

namespace Gamepad {
    const int ButtonTypeBegin = Up;
    const int ButtonTypeEnd = Select + 1;

    const int ButtonBegin = Up_0;
    const int ButtonEnd = None;

    const std::array<int, 5> GlfwGamepads = {
        GLFW_JOYSTICK_1, GLFW_JOYSTICK_2, GLFW_JOYSTICK_3, GLFW_JOYSTICK_4, GLFW_JOYSTICK_5
    };

    const std::map<ButtonType, std::string> buttonTypeToLabel = {
        { Up,       "Up"     },
        { Down,     "Down"   },
        { Left,     "Left"   },
        { Right,    "Right"  },
        { ButtonA,  "A"      },
        { ButtonB,  "B"      },
        { ButtonX,  "X"      },
        { ButtonY,  "Y"      },
        { ButtonL1, "L1"     },
        { ButtonR1, "R1"     },
        { ButtonL2, "L2"     },
        { ButtonR2, "R2"     },
        { ButtonL3, "L3"     },
        { ButtonR3, "R3"     },
        { Start,    "Start"  },
        { Select,   "Select" }
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

    const std::map<std::string, Button> stringToButton = {
        { "Up_0",       Up_0,       }, { "Up_1",       Up_1,       }, { "Up_2",      Up_2,        }, { "Up_3",       Up_3,       }, { "Up_4",       Up_4,       },
        { "Down_0",     Down_0,     }, { "Down_1",     Down_1,     }, { "Down_2",    Down_2,      }, { "Down_3",     Down_3,     }, { "Down_4",     Down_4,     },
        { "Left_0",     Left_0,     }, { "Left_1",     Left_1,     }, { "Left_2",    Left_2,      }, { "Left_3",     Left_3,     }, { "Left_4",     Left_4,     },
        { "Right_0",    Right_0,    }, { "Right_1",    Right_1,    }, { "Right_2",   Right_2,     }, { "Right_3",    Right_3,    }, { "Right_4",    Right_4,    },
        { "ButtonA_0",  ButtonA_0,  }, { "ButtonA_1",  ButtonA_1,  }, { "ButtonA_2", ButtonA_2,   }, { "ButtonA_3",  ButtonA_3,  }, { "ButtonA_4",  ButtonA_4,  },
        { "ButtonB_0",  ButtonB_0,  }, { "ButtonB_1",  ButtonB_1,  }, { "ButtonB_2", ButtonB_2,   }, { "ButtonB_3",  ButtonB_3,  }, { "ButtonB_4",  ButtonB_4,  },
        { "ButtonX_0",  ButtonX_0,  }, { "ButtonX_1",  ButtonX_1,  }, { "ButtonX_2", ButtonX_2,   }, { "ButtonX_3",  ButtonX_3,  }, { "ButtonX_4",  ButtonX_4,  },
        { "ButtonY_0",  ButtonY_0,  }, { "ButtonY_1",  ButtonY_1,  }, { "ButtonY_2", ButtonY_2,   }, { "ButtonY_3",  ButtonY_3,  }, { "ButtonY_4",  ButtonY_4,  },
        { "ButtonL1_0", ButtonL1_0, }, { "ButtonL1_1", ButtonL1_1, }, { "ButtonL1_2", ButtonL1_2, }, { "ButtonL1_3", ButtonL1_3, }, { "ButtonL1_4", ButtonL1_4, },
        { "ButtonR1_0", ButtonR1_0, }, { "ButtonR1_1", ButtonR1_1, }, { "ButtonR1_2", ButtonR1_2, }, { "ButtonR1_3", ButtonR1_3, }, { "ButtonR1_4", ButtonR1_4, },
        { "ButtonL2_0", ButtonL2_0, }, { "ButtonL2_1", ButtonL2_1, }, { "ButtonL2_2", ButtonL2_2, }, { "ButtonL2_3", ButtonL2_3, }, { "ButtonL2_4", ButtonL2_4, },
        { "ButtonR2_0", ButtonR2_0, }, { "ButtonR2_1", ButtonR2_1, }, { "ButtonR2_2", ButtonR2_2, }, { "ButtonR2_3", ButtonR2_3, }, { "ButtonR2_4", ButtonR2_4, },
        { "ButtonL3_0", ButtonL3_0, }, { "ButtonL3_1", ButtonL3_1, }, { "ButtonL3_2", ButtonL3_2, }, { "ButtonL3_3", ButtonL3_3, }, { "ButtonL3_4", ButtonL3_4, },
        { "ButtonR3_0", ButtonR3_0, }, { "ButtonR3_1", ButtonR3_1, }, { "ButtonR3_2", ButtonR3_2, }, { "ButtonR3_3", ButtonR3_3, }, { "ButtonR3_4", ButtonR3_4, },
        { "Start_0",    Start_0,    }, { "Start_1",    Start_1,    }, { "Start_2",   Start_2,     }, { "Start_3",    Start_3,    }, { "Start_4",    Start_4,    },
        { "Select_0",   Select_0,   }, { "Select_1",   Select_1,   }, { "Select_2",  Select_2,    }, { "Select_3",   Select_3,   }, { "Select_4",   Select_4,   },
    };

    const std::map<Button, std::string> buttonToString = {
    { Up_0,       "Up_0"       }, { Up_1,       "Up_1"       }, { Up_2,       "Up_2"       }, { Up_3,       "Up_3"       }, { Up_4,       "Up_4"       },
    { Down_0,     "Down_0"     }, { Down_1,     "Down_1"     }, { Down_2,     "Down_2"     }, { Down_3,     "Down_3"     }, { Down_4,     "Down_4"     },
    { Left_0,     "Left_0"     }, { Left_1,     "Left_1"     }, { Left_2,     "Left_2"     }, { Left_3,     "Left_3"     }, { Left_4,     "Left_4"     },
    { Right_0,    "Right_0"    }, { Right_1,    "Right_1"    }, { Right_2,    "Right_2"    }, { Right_3,    "Right_3"    }, { Right_4,    "Right_4"    },
    { ButtonA_0,  "ButtonA_0"  }, { ButtonA_1,  "ButtonA_1"  }, { ButtonA_2,  "ButtonA_2"  }, { ButtonA_3,  "ButtonA_3"  }, { ButtonA_4,  "ButtonA_4"  },
    { ButtonB_0,  "ButtonB_0"  }, { ButtonB_1,  "ButtonB_1"  }, { ButtonB_2,  "ButtonB_2"  }, { ButtonB_3,  "ButtonB_3"  }, { ButtonB_4,  "ButtonB_4"  },
    { ButtonX_0,  "ButtonX_0"  }, { ButtonX_1,  "ButtonX_1"  }, { ButtonX_2,  "ButtonX_2"  }, { ButtonX_3,  "ButtonX_3"  }, { ButtonX_4,  "ButtonX_4"  },
    { ButtonY_0,  "ButtonY_0"  }, { ButtonY_1,  "ButtonY_1"  }, { ButtonY_2,  "ButtonY_2"  }, { ButtonY_3,  "ButtonY_3"  }, { ButtonY_4,  "ButtonY_4"  },
    { ButtonL1_0, "ButtonL1_0" }, { ButtonL1_1, "ButtonL1_1" }, { ButtonL1_2, "ButtonL1_2" }, { ButtonL1_3, "ButtonL1_3" }, { ButtonL1_4, "ButtonL1_4" },
    { ButtonR1_0, "ButtonR1_0" }, { ButtonR1_1, "ButtonR1_1" }, { ButtonR1_2, "ButtonR1_2" }, { ButtonR1_3, "ButtonR1_3" }, { ButtonR1_4, "ButtonR1_4" },
    { ButtonL2_0, "ButtonL2_0" }, { ButtonL2_1, "ButtonL2_1" }, { ButtonL2_2, "ButtonL2_2" }, { ButtonL2_3, "ButtonL2_3" }, { ButtonL2_4, "ButtonL2_4" },
    { ButtonR2_0, "ButtonR2_0" }, { ButtonR2_1, "ButtonR2_1" }, { ButtonR2_2, "ButtonR2_2" }, { ButtonR2_3, "ButtonR2_3" }, { ButtonR2_4, "ButtonR2_4" },
    { ButtonL3_0, "ButtonL3_0" }, { ButtonL3_1, "ButtonL3_1" }, { ButtonL3_2, "ButtonL3_2" }, { ButtonL3_3, "ButtonL3_3" }, { ButtonL3_4, "ButtonL3_4" },
    { ButtonR3_0, "ButtonR3_0" }, { ButtonR3_1, "ButtonR3_1" }, { ButtonR3_2, "ButtonR3_2" }, { ButtonR3_3, "ButtonR3_3" }, { ButtonR3_4, "ButtonR3_4" },
    { Start_0,    "Start_0"    }, { Start_1,    "Start_1"    }, { Start_2,    "Start_2"    }, { Start_3,    "Start_3"    }, { Start_4,    "Start_4"    },
    { Select_0,   "Select_0"   }, { Select_1,   "Select_1"   }, { Select_2,   "Select_2"   }, { Select_3,   "Select_3"   }, { Select_4,   "Select_4"   }
    };

    const std::map<Button, std::string> buttonToLabel = {
        { Up_0,       "Up"        }, { Up_1,       "Up"        }, { Up_2,       "Up"        }, { Up_3,       "Up"        }, { Up_4,       "Up"        },
        { Down_0,     "Down"      }, { Down_1,     "Down"      }, { Down_2,     "Down"      }, { Down_3,     "Down"      }, { Down_4,     "Down"      },
        { Left_0,     "Left"      }, { Left_1,     "Left"      }, { Left_2,     "Left"      }, { Left_3,     "Left"      }, { Left_4,     "Left"      },
        { Right_0,    "Right"     }, { Right_1,    "Right"     }, { Right_2,    "Right"     }, { Right_3,    "Right"     }, { Right_4,    "Right"     },
        { ButtonA_0,  "Button A"  }, { ButtonA_1,  "Button A"  }, { ButtonA_2,  "Button A"  }, { ButtonA_3,  "Button A"  }, { ButtonA_4,  "Button A"  },
        { ButtonB_0,  "Button B"  }, { ButtonB_1,  "Button B"  }, { ButtonB_2,  "Button B"  }, { ButtonB_3,  "Button B"  }, { ButtonB_4,  "Button B"  },
        { ButtonX_0,  "Button X"  }, { ButtonX_1,  "Button X"  }, { ButtonX_2,  "Button X"  }, { ButtonX_3,  "Button X"  }, { ButtonX_4,  "Button X"  },
        { ButtonY_0,  "Button B"  }, { ButtonY_1,  "Button Y"  }, { ButtonY_2,  "Button Y"  }, { ButtonY_3,  "Button Y"  }, { ButtonY_4,  "Button Y"  },
        { ButtonL1_0, "Button L1" }, { ButtonL1_1, "Button L1" }, { ButtonL1_2, "Button L1" }, { ButtonL1_3, "Button L1" }, { ButtonL1_4, "Button L1" },
        { ButtonR1_0, "Button R1" }, { ButtonR1_1, "Button R1" }, { ButtonR1_2, "Button R1" }, { ButtonR1_3, "Button R1" }, { ButtonR1_4, "Button R1" },
        { ButtonL2_0, "Button L2" }, { ButtonL2_1, "Button L2" }, { ButtonL2_2, "Button L2" }, { ButtonL2_3, "Button L2" }, { ButtonL2_4, "Button L2" },
        { ButtonR2_0, "Button R2" }, { ButtonR2_1, "Button R2" }, { ButtonR2_2, "Button R2" }, { ButtonR2_3, "Button R2" }, { ButtonR2_4, "Button R2" },
        { ButtonL3_0, "Button L3" }, { ButtonL3_1, "Button L3" }, { ButtonL3_2, "Button L3" }, { ButtonL3_3, "Button L3" }, { ButtonL3_4, "Button L3" },
        { ButtonR3_0, "Button R3" }, { ButtonR3_1, "Button R3" }, { ButtonR3_2, "Button R3" }, { ButtonR3_3, "Button R3" }, { ButtonR3_4, "Button R3" },
        { Start_0,    "Start"     }, { Start_1,    "Start"     }, { Start_2,    "Start"     }, { Start_3,    "Start"     }, { Start_4,    "Start"     },
        { Select_0,   "Select"    }, { Select_1,   "Select"    }, { Select_2,   "Select"    }, { Select_3,   "Select"    }, { Select_4,   "Select"    },
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
