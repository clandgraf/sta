#include <functional>
#include <map>

class Emu;

namespace Gui {
    
    template<class ElementType>
    class WithLifecycle {
    public:
        WithLifecycle(
            ElementType& self,
            const char* _key,
            const char* _title,
            std::function<void(ElementType&, Emu&)> renderFn,
            std::function<void(ElementType&, Emu&)> initFn,
            std::function<void(ElementType&, Emu&)> teardownFn
        ) : m_renderFn(renderFn)
          , m_self(self)
          , m_initFn(initFn)
          , m_teardownFn(teardownFn)
          , key(_key)
          , title(_title) {}

        const char* key;
        const char* title;

        virtual void init(Emu& emu) {
            if (m_initFn)
                m_initFn(m_self, emu);
        }

        virtual void render(Emu& emu) {
            m_renderFn(m_self, emu);
        }

        virtual void teardown(Emu& emu) {
            if (m_teardownFn)
                m_teardownFn(m_self, emu);
        }

    protected:
        std::function<void(ElementType&, Emu&)> m_renderFn;
        std::function<void(ElementType&, Emu&)> m_initFn;
        std::function<void(ElementType&, Emu&)> m_teardownFn;

        ElementType& m_self;
    };

    class Window : public WithLifecycle<Window> {
        using Action = std::function<void(Window&, Emu&)>;
        using ActionMapIterator = std::map<std::string, Action>::iterator;

    public:
        static std::map<std::string, std::shared_ptr<Window>> Entries;

        void init(Emu&);
        void teardown(Emu&);

        bool addAction(const char* title, Action a);

        bool* show();

        Window(
            const char* key,
            const char* title,
            std::function<void(Window&, Emu&)> renderFn,
            std::function<void(Window&, Emu&)> initFn,
            std::function<void(Window&, Emu&)> teardownFn
        );

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
        bool m_show = true;

        std::map<std::string, Action> m_actions;
    };

    class Dialog : public WithLifecycle<Dialog> {
    public:
        static std::map<std::string, std::shared_ptr<Dialog>> Entries;

        Dialog(
            const char* key,
            const char* title,
            std::function<void(Dialog&, Emu&)> renderFn,
            std::function<void(Dialog&, Emu&)> initFn,
            std::function<void(Dialog&, Emu&)> teardownFn
        );
    };

    template<class ElementType>
    std::shared_ptr<ElementType> create(
        const char* key,
        const char* title,
        std::function<void(ElementType&, Emu&)> renderFn,
        std::function<void(ElementType&, Emu&)> initFn = nullptr,
        std::function<void(ElementType&, Emu&)> teardownFn = nullptr
    ) {
        if (ElementType::Entries.find(key) != ElementType::Entries.end()) {
            return nullptr;
        }

        auto w = std::make_shared<ElementType>(key, title, renderFn, initFn, teardownFn);
        ElementType::Entries[key] = w;
        return w;
    }
}
