#include <functional>
#include <map>

class Emu;

namespace Gui {
    class Window;

    using Action = std::function<void(Window&, Emu&)>;

    class Window {
    public:
        static std::shared_ptr<Window> createWindow(
            const char* key, 
            const char* title, 
            std::function<void(Window&, Emu&)> renderFn, 
            std::function<void(Window&, Emu&)> initFn = nullptr, 
            std::function<void(Window&, Emu&)> teardownFn = nullptr
        );

        void init(Emu&);
        void teardown(Emu&);
        void render(Emu&);

        bool addAction(const char* title, Action a);

        bool* show();

        Window(
            const char* key,
            const char* title,
            std::function<void(Window&, Emu&)> renderFn,
            std::function<void(Window&, Emu&)> initFn,
            std::function<void(Window&, Emu&)> teardownFn
        );

        const char* key;
        const char* title;

        using ActionMapIterator = std::map<std::string, Action>::iterator;

        class ActionIterator : public ActionMapIterator {
        public:
            ActionIterator() : ActionMapIterator() {};
            ActionIterator(ActionMapIterator it) : ActionMapIterator(it) {};
            std::string* operator->() { return (std::string* const)&(ActionMapIterator::operator->()->first); }
            std::string operator*() { return ActionMapIterator::operator*().first; }
        };


        ActionIterator begin() { return m_actions.begin(); }
        ActionIterator end() { return m_actions.end(); }

        bool hasActions() { return m_actions.size() > 0; }
        void runAction(const char* title, Emu& emu);

    private:
        std::function<void(Window&, Emu&)> m_renderFn;
        std::function<void(Window&, Emu&)> m_initFn;
        std::function<void(Window&, Emu&)> m_teardownFn;
        bool m_show = true;

        std::map<std::string, Action> m_actions;
    };

    extern std::map<std::string, std::shared_ptr<Window>> Windows;
}
