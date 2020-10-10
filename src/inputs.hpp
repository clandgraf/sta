#pragma once

#include <map>
#include <vector>
#include <unordered_set>

namespace Input {
    using Scancode = int;

    enum ControllerDef {
        None,
        Menu,
        Up_0, Up_1, Up_2, Up_3, Up_4,
        Down_0, Down_1, Down_2, Down_3, Down_4,
        Left_0, Left_1, Left_2, Left_3, Left_4,
        Right_0, Right_1, Right_2, Right_3, Right_4,
        ButtonA_0, ButtonA_1, ButtonA_2, ButtonA_3, ButtonA_4,
        ButtonB_0, ButtonB_1, ButtonB_2, ButtonB_3, ButtonB_4,
        ButtonX_0, ButtonX_1, ButtonX_2, ButtonX_3, ButtonX_4,
        ButtonY_0, ButtonY_1, ButtonY_2, ButtonY_3, ButtonY_4,
        ButtonL_0, ButtonL_1, ButtonL_2, ButtonL_3, ButtonL_4,
        ButtonR_0, ButtonR_1, ButtonR_2, ButtonR_3, ButtonR_4,
        Start_0, Start_1, Start_2, Start_3, Start_4,
        Select_0, Select_1, Select_2, Select_3, Select_4,
    };

    const std::map<ControllerDef, std::string> defToString = {
        { Menu,      "Menu"      },
        { Up_0,      "Up_0"      }, { Up_1,      "Up_1"      }, { Up_2,      "Up_2"      }, { Up_3,      "Up_3"      }, { Up_4,      "Up_4"      },
        { Down_0,    "Down_0"    }, { Down_1,    "Down_1"    }, { Down_2,    "Down_2"    }, { Down_3,    "Down_3"    }, { Down_4,    "Down_4"    },
        { Left_0,    "Left_0"    }, { Left_1,    "Left_1"    }, { Left_2,    "Left_2"    }, { Left_3,    "Left_3"    }, { Left_4,    "Left_4"    },
        { Right_0,   "Right_0"   }, { Right_1,   "Right_1"   }, { Right_2,   "Right_2"   }, { Right_3,   "Right_3"   }, { Right_4,   "Right_4"   },
        { ButtonA_0, "ButtonA_0" }, { ButtonA_1, "ButtonA_1" }, { ButtonA_2, "ButtonA_2" }, { ButtonA_3, "ButtonA_3" }, { ButtonA_4, "ButtonA_4" },
        { ButtonB_0, "ButtonB_0" }, { ButtonB_1, "ButtonB_1" }, { ButtonB_2, "ButtonB_2" }, { ButtonB_3, "ButtonB_3" }, { ButtonB_4, "ButtonB_4" },
        { ButtonX_0, "ButtonX_0" }, { ButtonX_1, "ButtonX_1" }, { ButtonX_2, "ButtonX_2" }, { ButtonX_3, "ButtonX_3" }, { ButtonX_4, "ButtonX_4" },
        { ButtonY_0, "ButtonY_0" }, { ButtonY_1, "ButtonY_1" }, { ButtonY_2, "ButtonY_2" }, { ButtonY_3, "ButtonY_3" }, { ButtonY_4, "ButtonY_4" },
        { ButtonL_0, "ButtonL_0" }, { ButtonL_1, "ButtonL_1" }, { ButtonL_2, "ButtonL_2" }, { ButtonL_3, "ButtonL_3" }, { ButtonL_4, "ButtonL_4" },
        { ButtonR_0, "ButtonR_0" }, { ButtonR_1, "ButtonR_1" }, { ButtonR_2, "ButtonR_2" }, { ButtonR_3, "ButtonR_3" }, { ButtonR_4, "ButtonR_4" },
        { Start_0,   "Start_0"   }, { Start_1,   "Start_1"   }, { Start_2,   "Start_2"   }, { Start_3,   "Start_3"   }, { Start_4,   "Start_4"   },
        { Select_0,  "Select_0"  }, { Select_1,  "Select_1"  }, { Select_2,  "Select_2"  }, { Select_3,  "Select_3"  }, { Select_4,  "Select_4"  },
    };

    const std::map<ControllerDef, std::string> defToLabel = {
        { Menu,      "Menu"     },
        { Up_0,      "Up"       }, { Up_1,      "Up"       }, { Up_2,      "Up"       }, { Up_3,      "Up"       }, { Up_4,      "Up"       },
        { Down_0,    "Down"     }, { Down_1,    "Down"     }, { Down_2,    "Down"     }, { Down_3,    "Down"     }, { Down_4,    "Down"     },
        { Left_0,    "Left"     }, { Left_1,    "Left"     }, { Left_2,    "Left"     }, { Left_3,    "Left"     }, { Left_4,    "Left"     },
        { Right_0,   "Right"    }, { Right_1,   "Right"    }, { Right_2,   "Right"    }, { Right_3,   "Right"    }, { Right_4,   "Right"    },
        { ButtonA_0, "Button A" }, { ButtonA_1, "Button A" }, { ButtonA_2, "Button A" }, { ButtonA_3, "Button A" }, { ButtonA_4, "Button A" },
        { ButtonB_0, "Button B" }, { ButtonB_1, "Button B" }, { ButtonB_2, "Button B" }, { ButtonB_3, "Button B" }, { ButtonB_4, "Button B" },
        { Start_0,   "Start"    }, { Start_1,   "Start"    }, { Start_2,   "Start"    }, { Start_3,   "Start"    }, { Start_4,   "Start"    },
        { Select_0,  "Select"   }, { Select_1,  "Select"   }, { Select_2,  "Select"   }, { Select_3,  "Select"   }, { Select_4,  "Select"   },
    };

    const std::map<std::string, ControllerDef> stringToDef = {
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
