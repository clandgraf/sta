#include "util.hpp"
#include "emu.hpp"
#include "gui/gui_window.hpp"

std::map<std::string, std::shared_ptr<Gui::Window>> Gui::Windows;

std::shared_ptr<Gui::Window> Gui::Window::createWindow(
    const char* key, 
    const char* title, 
    std::function<void(Window&, Emu&)> renderFn,
    std::function<void(Window&, Emu&)> initFn,
    std::function<void(Window&, Emu&)> teardownFn
) {
    if (Gui::Windows.find(key) != Gui::Windows.end()) {
        return nullptr;
    }

    auto w = std::make_shared<Window>(key, title, renderFn, initFn, teardownFn);
    Windows[key] = w;
    return w;
}

Gui::Window::Window(
    const char* _key, 
    const char* _title, 
    std::function<void(Window&, Emu&)> renderFn,
    std::function<void(Window&, Emu&)> initFn,
    std::function<void(Window&, Emu&)> teardownFn
) 
    : m_renderFn(renderFn)
    , m_initFn(initFn)
    , m_teardownFn(teardownFn)
    , key(_key)
    , title(_title) {}

bool* Gui::Window::show() {
    return &m_show;
}

void Gui::Window::init(Emu& emu) {
    m_show = Settings::get(key, false);
    if (m_initFn)
        m_initFn(*this, emu);
}

void Gui::Window::render(Emu& emu) {
    m_renderFn(*this, emu);
}

void Gui::Window::teardown(Emu& emu) {
    Settings::set(key, m_show);
    if (m_teardownFn) 
        m_teardownFn(*this, emu);
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
