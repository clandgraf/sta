#pragma once

#include <map>
#include <array>
#include <string>
#include <GLFW/glfw3.h>

namespace Gamepad {
    using Scancode = int;

    enum Button {
        Up_0, Up_1, Up_2, Up_3, Up_4,
        Down_0, Down_1, Down_2, Down_3, Down_4,
        Left_0, Left_1, Left_2, Left_3, Left_4,
        Right_0, Right_1, Right_2, Right_3, Right_4,
        ButtonA_0, ButtonA_1, ButtonA_2, ButtonA_3, ButtonA_4,
        ButtonB_0, ButtonB_1, ButtonB_2, ButtonB_3, ButtonB_4,
        ButtonX_0, ButtonX_1, ButtonX_2, ButtonX_3, ButtonX_4,
        ButtonY_0, ButtonY_1, ButtonY_2, ButtonY_3, ButtonY_4,
        ButtonL1_0, ButtonL1_1, ButtonL1_2, ButtonL1_3, ButtonL1_4,
        ButtonR1_0, ButtonR1_1, ButtonR1_2, ButtonR1_3, ButtonR1_4,
        ButtonL2_0, ButtonL2_1, ButtonL2_2, ButtonL2_3, ButtonL2_4,
        ButtonR2_0, ButtonR2_1, ButtonR2_2, ButtonR2_3, ButtonR2_4,
        ButtonL3_0, ButtonL3_1, ButtonL3_2, ButtonL3_3, ButtonL3_4,
        ButtonR3_0, ButtonR3_1, ButtonR3_2, ButtonR3_3, ButtonR3_4,
        Start_0, Start_1, Start_2, Start_3, Start_4,
        Select_0, Select_1, Select_2, Select_3, Select_4,

        None,
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

    const std::array<const std::map<int, Button>, 5> InstanceMap = {
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
            { GLFW_GAMEPAD_BUTTON_START,        Start_4    },
            { GLFW_GAMEPAD_BUTTON_BACK,         Select_4   },
        })
    };

    extern bool isPressed(Gamepad::Button btn);
    extern void reset()
    extern void onKeydown(Scancode, bool pressed);
    extern void setScancode(Scancode, Button);
    extern void onWaitForInput(Button button);
}
