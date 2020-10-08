#pragma once

#include <functional>
#include <map>
#include <variant>
#include <filesystem>
#include <imgui.h>

#include "core/gui/gui.hpp"
#include "core/gui/filebrowser.hpp"
#include "core/gui/opengl_surface.hpp"
#include "core/gui/opengl.hpp"
#include "core/gui/notifications.hpp"
#include "core/recents.hpp"
#include "core/util.hpp"
#include "inputs.hpp"
#include "IconsMaterialDesign.h"

#define ICON_FONT "assets/" ## FONT_ICON_FILE_NAME_FAR

namespace Gui {

    template<class EmuType>
    class Manager {
    public:
        std::list<std::filesystem::path> recentFiles;

        Gui::Window handle{nullptr};

        ImFont* defaultFont;
        ImFont* sansFont;
        ImFont* monoFont;

        void pushMonoFont() {
            ImGui::PushFont(monoFont);
        }

        void pushHighlightText() {
            ImGui::PushStyleColor(ImGuiCol_Text, Gui::HIGHLIGHT_TEXT_COLOR);
        }

        std::shared_ptr<Gui::Surface> screenSurface;

        template<class ElementType>
        class WithLifecycle {
        public:
            Manager& manager;

            WithLifecycle(
                Manager& manager_,
                ElementType& self,
                const char* _key,
                const char* _title,
                std::function<void(ElementType&, EmuType&)> renderFn,
                std::function<void(ElementType&, EmuType&)> initFn,
                std::function<void(ElementType&, EmuType&)> teardownFn
            ) : m_renderFn(renderFn)
                , manager(manager_)
                , m_self(self)
                , m_initFn(initFn)
                , m_teardownFn(teardownFn)
                , key(_key)
                , title(_title) {}

            const char* key;
            const char* title;

            virtual void init(EmuType& emu) {
                if (m_initFn)
                    m_initFn(m_self, emu);
            }

            virtual void render(EmuType& emu) {
                m_renderFn(m_self, emu);
            }

            virtual void teardown(EmuType& emu) {
                if (m_teardownFn)
                    m_teardownFn(m_self, emu);
            }

        protected:
            std::function<void(ElementType&, EmuType&)> m_renderFn;
            std::function<void(ElementType&, EmuType&)> m_initFn;
            std::function<void(ElementType&, EmuType&)> m_teardownFn;

            ElementType& m_self;
        };

        class Window : public WithLifecycle<Window> {
        public:
            void init(EmuType& emu) {
                m_show = Settings::get(this->key, false);
                WithLifecycle<Window>::init(emu);
            }

            void teardown(EmuType& emu) {
                Settings::set(this->key, m_show);
                WithLifecycle<Window>::teardown(emu);
            }

            bool* show() {
                return &m_show;
            }

            Window(
                Manager& pManager,
                const char* key,
                const char* title,
                std::function<void(Window&, EmuType&)> renderFn,
                std::function<void(Window&, EmuType&)> initFn = nullptr,
                std::function<void(Window&, EmuType&)> teardownFn = nullptr
            ) : WithLifecycle<Window>(pManager, *this, key, title, renderFn, initFn, teardownFn) {}

        private:
            bool m_show = true;
        };

        class Dialog : public WithLifecycle<Dialog> {
        public:
            Dialog(
                Manager& pManager,
                const char* key,
                const char* title,
                std::function<void(Dialog&, EmuType&)> renderFn,
                std::function<void(Dialog&, EmuType&)> initFn = nullptr,
                std::function<void(Dialog&, EmuType&)> teardownFn = nullptr,
                std::function<bool(Dialog&, EmuType&)> shouldCloseFn = nullptr,
                std::function<void(Dialog&, EmuType&)> onClosedFn = nullptr
            ) : WithLifecycle<Dialog>(pManager, *this, key, title, renderFn, initFn, teardownFn)
              , m_shouldCloseFn(shouldCloseFn)
              , m_onClosedFn(onClosedFn) {}

            std::function<bool(Dialog&, EmuType&)> m_shouldCloseFn;
            std::function<void(Dialog&, EmuType&)> m_onClosedFn;

            bool notifiedOpen = false;

            bool shouldClose(EmuType& emu) { return m_shouldCloseFn && m_shouldCloseFn(*this, emu); }
    
            void onClosed(EmuType& emu) { if (m_onClosedFn) { m_onClosedFn(*this, emu); } }

            void open() {
                ImGui::OpenPopup(this->title);
            }

            void notifyOpen() {
                notifiedOpen = true;
            }

            void render(EmuType& emu) {
                if (notifiedOpen) {
                    open();
                    notifiedOpen = false;
                }

                bool open = true;
                if (ImGui::BeginPopupModal(this->title, &open, ImGuiWindowFlags_AlwaysAutoResize)) {

                    WithLifecycle<Dialog>::render(emu);

                    if (!open || shouldClose(emu)) {
                        ImGui::CloseCurrentPopup();
                        onClosed(emu);
                    }
                    ImGui::EndPopup();
                }
            }
        };

        std::map<std::string, std::shared_ptr<Window>> windows;
        std::map<std::string, std::shared_ptr<Dialog>> dialogs;

        std::shared_ptr<Window> window(
            const char* key,
            const char* title,
            std::function<void(Window&, EmuType&)> renderFn,
            std::function<void(Window&, EmuType&)> initFn = nullptr,
            std::function<void(Window&, EmuType&)> teardownFn = nullptr
        ) {
            if (windows.find(key) != windows.end()) {
                return nullptr;
            }

            auto w = std::make_shared<Window>(*this, key, title, renderFn, initFn, teardownFn);
            windows[key] = w;
            return w;
        }

        std::shared_ptr<Dialog> dialog(
            const char* key,
            const char* title,
            std::function<void(Dialog&, EmuType&)> renderFn,
            std::function<void(Dialog&, EmuType&)> initFn = nullptr,
            std::function<void(Dialog&, EmuType&)> teardownFn = nullptr,
            std::function<bool(Dialog&, EmuType&)> shouldCloseFn = nullptr,
            std::function<void(Dialog&, EmuType&)> onClosedFn = nullptr
        ) {
            if (dialogs.find(key) != dialogs.end()) {
                return nullptr;
            }

            auto w = std::make_shared<Dialog>(*this, key, title, renderFn, initFn, teardownFn, shouldCloseFn, onClosedFn);
            dialogs[key] = w;
            return w;
        }

        using Action = typename std::function<void(EmuType&)>;

        using Checkbox = typename std::function<bool&(EmuType&)>;

        struct CheckboxAction {
            Checkbox checkbox;
            Action action;
        };

        using MenuItem = typename std::variant<Action, Checkbox, CheckboxAction>;

        std::map<std::string, std::map<std::string, MenuItem>> menubar;

        void action(const std::string& menu, const std::string& label, Action item) {
            menubar[menu][label] = item;
        }

        void checkbox(const std::string& menu, const std::string& label, Action item) {
            menubar[menu][label] = item;
        }

        void action(const std::string& menu, const std::string& label, Checkbox checkbox, Action action) {
            MenuItem item = CheckboxAction{checkbox, action};
            menubar[menu][label] = item;
        }

        bool init(EmuType& emu, const char* windowTitle, bool fullscreen) {
            ImGui_FileBrowser_Init();

            if (!(handle = initWindow(windowTitle, fullscreen))) {
                return false;
            }

            if (!Surface::init()) {
                return false;
            }

            screenSurface = std::make_shared<Gui::Surface>(256, 240);
            for (int y = 0; y < screenSurface->m_height; y++) {
                for (int x = 0; x < screenSurface->m_width; x++) {
                    screenSurface->setPixel(x, y, { 0, 0, 0 });
                }
            }

            Input::loadSettings();
            recentFiles = Util::loadRecentFiles("recentFiles");

            for (auto& dialog : dialogs) {
                dialog.second->init(emu);
            }

            for (auto& window : windows) {
                window.second->init(emu);
            }

            return initImGUI();
        }

        bool initImGUI() {
            // Setup Dear ImGui context
            IMGUI_CHECKVERSION();
            ImGui::CreateContext();
            ImGuiIO& io = ImGui::GetIO(); (void)io;
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
            //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

            // Setup Dear ImGui style
            ImGui::StyleColorsDark();

            // Setup Platform/Renderer bindings
            ImGui_Impl_Init(handle);

            sansFont = io.Fonts->AddFontFromFileTTF("assets/SourceSansPro-Regular.ttf", 24.0f);
            ImFontConfig config;
            config.MergeMode = true;
            static const ImWchar icon_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
            io.Fonts->AddFontFromFileTTF("assets/MaterialIcons-Regular.ttf", 20.0f, &config, icon_ranges);
            monoFont = io.Fonts->AddFontFromFileTTF("assets/SourceCodePro-Medium.ttf", 20.0f);
            io.Fonts->Build();

            return true;
        }

        void teardownImGui() {
            ImGui_Impl_Shutdown();
            ImGui::DestroyContext();
        }

        void teardown(EmuType& emu) {
            for (auto& window : windows) {
                window.second->teardown(emu);
            }

            for (auto& dialog : dialogs) {
                dialog.second->teardown(emu);
            }

            Util::writeRecentFiles(recentFiles, "recentFiles");
            Input::writeSettings();

            teardownImGui();
            screenSurface = nullptr;
            Surface::teardown();
            teardownWindow(handle);
        }

        void setPixel(unsigned int x, unsigned int y, unsigned int v) {
            screenSurface->setPixel(x, y, Palette::DEFAULT[v & 0x3f]);
        }

        std::function<void(unsigned int, unsigned int, unsigned int)> getSetPixelFn() {
            return [this](unsigned int x, unsigned int y, unsigned int v){ 
                this->setPixel(x, y, v); 
            };
        }

        void renderFrame() {
            int display_w, display_h;
            glfwGetFramebufferSize((GLFWwindow*) handle._, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(CLEAR_COLOR.x, CLEAR_COLOR.y, CLEAR_COLOR.z, CLEAR_COLOR.w);
            glClear(GL_COLOR_BUFFER_BIT);

            screenSurface->upload();
            screenSurface->render(display_w, display_h);
        }

        void renderMenu(const std::string& menu, EmuType& emu) {
            for (auto& entry : menubar[menu]) {
                auto title = entry.first.c_str(); 
                auto item = entry.second;
                if (std::holds_alternative<Action>(item)) {
                    if (ImGui::MenuItem(title)) {
                        std::get<Action>(item)(emu);
                    }
                } else if (std::holds_alternative<Checkbox>(item)) {
                    bool& v = std::get<Checkbox>(item)(emu);
                    if (ImGui::MenuItem(title, nullptr, v)) {
                        v = !v;
                    }
                } else if (std::holds_alternative<CheckboxAction>(item)) {
                    bool& v = std::get<CheckboxAction>(item).checkbox(emu);
                    if (ImGui::MenuItem(title, nullptr, v)) {
                        v = !v;
                        std::get<CheckboxAction>(item).action(emu);
                    }
                }
            }
        }

        void renderMenuBar(EmuType& emu) {
            bool showOpenFile = false;
            
            if (ImGui::BeginMainMenuBar()) {
                if (ImGui::BeginMenu("File")) {
                    if (ImGui::MenuItem("Open")) {
                        showOpenFile = true;
                    }

                    if (ImGui::BeginMenu("Recent")) {
                        if (recentFiles.empty()) {
                            ImGui::MenuItem("<none>");
                        }
                        else {
                            std::filesystem::path path;
                            for (auto p : recentFiles) {
                                if (ImGui::MenuItem(p.string().c_str())) {
                                    path = p;
                                }
                            }
                            if (!path.empty()) {
                                openFile(emu, path);
                            }
                        }
                        ImGui::EndMenu();
                    }

                    if (menubar["File"].size() > 0) {
                        ImGui::Separator();
                    }

                    renderMenu("File", emu);

                    ImGui::Separator();

                    if (ImGui::MenuItem("Exit")) {
                        setWindowClosing(handle, true);
                    }

                    ImGui::EndMenu();
                }

                for (auto& menuEntry: menubar) {
                    std::string menuName = menuEntry.first;
                    if (menuName != "File" && menuName != "View") {
                        if (ImGui::BeginMenu(menuName.c_str())) {
                            renderMenu(menuName, emu);
                            ImGui::EndMenu();
                        }
                    }
                }

                if (ImGui::BeginMenu("View")) {
                    if (ImGui::MenuItem("Fullscreen", nullptr, isFullscreen(handle))) {
                        toggleFullscreen(handle);
                    }

                    ImGui::Separator();

                    for (auto& window : windows) {
                        auto w = window.second;
                        ImGui::MenuItem(w->title, nullptr, w->show());
                    }

                    ImGui::EndMenu();
                }
                ImGui::EndMainMenuBar();
            }

            if (showOpenFile) {
                ImGui::OpenPopup("Open File");
            }
        }

        bool openFile(EmuType& emu, const std::filesystem::path& path) {
            bool ret = emu.init(path);
            if (ret) {
                Util::addRecent(recentFiles, path);
            }
            return ret;
        }

        void renderOpenRomDialog(EmuType& emu) {
            std::filesystem::path selectedFile;
            bool open = true;
            if (ImGui::BeginPopupModal("Open ROM", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
                if (ImGui_FileBrowser(selectedFile)) {
                    if (openFile(emu, selectedFile)) {
                        open = false;
                    }
                }
                if (!open) {
                    ImGui::CloseCurrentPopup();
                }
                ImGui::EndPopup();
            }
        }

        void runUi(EmuType& emu) {
            // Poll and handle events (inputs, window resize, etc.)
            // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
            // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
            // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
            // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.

            // Start the Dear ImGui frame
            ImGui_Impl_NewFrame();
            ImGui::NewFrame();

            // Render Views
            ImGui::PushFont(sansFont);

            // ImGui::ShowDemoWindow();

            renderMenuBar(emu);
            renderOpenRomDialog(emu);

            for (auto& dialog : dialogs) {
                dialog.second->render(emu);
            }

            for (auto& window : windows) {
                window.second->render(emu);
            }

            Gui::renderNotifications();
            ImGui::PopFont();

            // Rendering
            ImGui::Render();
            renderFrame();
            ImGui_Impl_RenderDrawData(ImGui::GetDrawData());
        }

        void runFrame(EmuType& emu) {
            const Input::State& inputs = Input::getState();
            if (inputs.openMenu) {
                emu.m_isStepping = true;
            }
            else {
                emu.stepFrame();
                renderFrame();
            }
        }

        void setTitle(const char* title) {
            Gui::setTitle(handle, title);
        }

        bool isWindowClosing() {
            return Gui::isWindowClosing(handle);
        }

        void swapBuffers() {
            Gui::swapBuffers(handle);
        }
    };
}
