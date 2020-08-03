#include "util.hpp"
#include "emu.hpp"
#include "gui/gui_window.hpp"

std::map<std::string, std::shared_ptr<Gui::Window>> Gui::Window::Entries;

Gui::Window::Window(
    const char* _key,
    const char* _title,
    std::function<void(Window&, Emu&)> renderFn,
    std::function<void(Window&, Emu&)> initFn,
    std::function<void(Window&, Emu&)> teardownFn
)
    : Gui::WithLifecycle<Window>(*this, _key, _title, renderFn, initFn, teardownFn) {}

bool* Gui::Window::show() {
    return &m_show;
}

void Gui::Window::init(Emu& emu) {
    m_show = Settings::get(key, false);
    Gui::WithLifecycle<Window>::init(emu);
}

void Gui::Window::teardown(Emu& emu) {
    Settings::set(key, m_show);
    Gui::WithLifecycle<Window>::teardown(emu);
}

void Gui::Window::runAction(const char* title, Emu& emu) {
    m_actions[title](*this, emu);
}

bool Gui::Window::addAction(const char* title, Action a) {
    if (m_actions.find(title) != m_actions.end()) {
        return false;
    }
    m_actions[title] = a;
    return true;
}

std::map<std::string, std::shared_ptr<Gui::Dialog>> Gui::Dialog::Entries;

Gui::Dialog::Dialog(
    const char* key,
    const char* title,
    std::function<void(Dialog&, Emu&)> renderFn,
    std::function<void(Dialog&, Emu&)> initFn,
    std::function<void(Dialog&, Emu&)> teardownFn
) 
    : Gui::WithLifecycle<Dialog>(*this, key, title, renderFn, initFn, teardownFn) {}
