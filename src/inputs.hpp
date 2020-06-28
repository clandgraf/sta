#pragma once

#include <map>
#include <vector>
#include <unordered_set>

namespace Input {
    using Scancode = int;

    enum ControllerDef {
        None,
        Menu,
        Up_0, Up_1,
        Down_0, Down_1,
        Left_0, Left_1,
        Right_0, Right_1,
        ButtonA_0, ButtonA_1,
        ButtonB_0, ButtonB_1,
        Start_0, Start_1,
        Select_0, Select_1,
    };

    const std::map<ControllerDef, const char*> defToString = {
        { Menu,      "Menu"      },
        { Up_0,      "Up_0"      },
        { Up_1,      "Up_1"      },
        { Down_0,    "Down_0"    },
        { Down_1,    "Down_1"    },
        { Left_0,    "Left_0"    },
        { Left_1,    "Left_1"    },
        { Right_0,   "Right_0"   },
        { Right_1,   "Right_1"   },
        { ButtonA_0, "ButtonA_0" },
        { ButtonA_1, "ButtonA_1" },
        { ButtonB_0, "ButtonB_0" },
        { ButtonB_1, "ButtonB_1" },
        { Start_0,   "Start_0"   },
        { Start_1,   "Start_1"   },
        { Select_0,  "Select_0"  },
        { Select_1,  "Select_1"  },
    };

    const std::map<ControllerDef, const char*> defToLabel = {
        { Menu,      "Menu"      },
        { Up_0,      "Up"      },
        { Up_1,      "Up"      },
        { Down_0,    "Down"    },
        { Down_1,    "Down"    },
        { Left_0,    "Left"    },
        { Left_1,    "Left"    },
        { Right_0,   "Right"   },
        { Right_1,   "Right"   },
        { ButtonA_0, "Button A" },
        { ButtonA_1, "Button A" },
        { ButtonB_0, "Button B" },
        { ButtonB_1, "Button B" },
        { Start_0,   "Start"   },
        { Start_1,   "Start"   },
        { Select_0,  "Select"  },
        { Select_1,  "Select"  },
    };

    const std::map<const char*, ControllerDef> stringToDef = {
        { "Menu",      Menu,       },
        { "Up_0",      Up_0,       },
        { "Up_1",      Up_1,       },
        { "Down_0",    Down_0,     },
        { "Down_1",    Down_1,     },
        { "Left_0",    Left_0,     },
        { "Left_1",    Left_1,     },
        { "Right_0",   Right_0,    },
        { "Right_1",   Right_1,    },
        { "ButtonA_0", ButtonA_0,  },
        { "ButtonA_1", ButtonA_1,  },
        { "ButtonB_0", ButtonB_0,  },
        { "ButtonB_1", ButtonB_1,  },
        { "Start_0",   Start_0,    },
        { "Start_1",   Start_1,    },
        { "Select_0",  Select_0,   },
        { "Select_1",  Select_1,   },
    };

    const std::vector<ControllerDef> inputsGeneral = {
        Menu,
    };

    const std::vector<ControllerDef> inputsController0 = {
        Up_0,
        Down_0,
        Left_0,
        Right_0,
        ButtonA_0,
        ButtonB_0,
        Start_0,
        Select_0,
    };

    const std::vector<ControllerDef> inputsController1 = {
        Up_1,
        Down_1,
        Left_1,
        Right_1,
        ButtonA_1,
        ButtonB_1,
        Start_1,
        Select_1,
    };

    struct Controller {
        bool d_up = false;
        bool d_down = false;
        bool d_left = false;
        bool d_right = false;
        bool start = false;
        bool select = false;
        bool btn_a = false;
        bool btn_b = false;
    };

    struct State {
        Input::Controller input0;
        Input::Controller input1;

        bool openMenu = false;
    };

    void dispatchInput(Scancode scancode, bool pressed);
    void waitForInput(ControllerDef def);
    ControllerDef getWaitingForInput();
    void setScancode(Scancode scancode, ControllerDef def);
    void clearButton(ControllerDef def);
    const std::unordered_set<Scancode>& getScancodes(ControllerDef);
    const Input::State& getState();
    void resetMenuRequest();
    void loadSettings();
    void writeSettings();
}
