#include "core/gamepad.hpp"

namespace Gamepad {
    class TriStateBool {
        enum class Value {
            Undefined = -1,
            False = false,
            True = true
        };

        Value m_value = Value::Undefined;

    public:
        void operator=(bool& v) {
            m_value = v ? Value::True : Value::False;
        }

        void reset() {
            m_value = Value::Undefined;
        }

        bool isUndefined() const {
            return m_value == Value::Undefined;
        }

        operator bool() const {
            return m_value == Value::True;
        }
    };

    bool readGamepadState(Button btn);

    static Button waitingFor = None;

    static std::map<Scancode, Button> kbdMap;
    static std::map<Button, Scancode> revKbdMap;

    static std::map<Button, bool> kbdState;

    static std::map<Button, bool> padState;

    static std::map<Button, TriStateBool> emuState;
}

void Gamepad::reset() {
    
}

bool Gamepad::readGamepadState(Gamepad::Button btn) {
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
