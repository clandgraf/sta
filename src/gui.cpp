#include <imgui.h>
#include <sstream>

#include "gui.hpp"
#include "gui/gui_window.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "disasm.hpp"
#include "IconsMaterialDesign.h"

#include <iostream>
#include <cstdio>
#include <filesystem>

#include "gui/gui_filebrowser.hpp"

#include "defs.hpp"

namespace fs = std::filesystem;

static ImFont* defaultFont;
static ImFont* sansFont;
static ImFont* monoFont;

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static ImVec4 highlight_text_color = ImVec4(0.89f, 0.0f, 0.0f, 1.00f);

static std::list<fs::path> recentFiles;

#include "gui_opengl.hpp"

static std::shared_ptr<Gui::Surface> screenSurface;

void Gui::pushMonoFont() {
    ImGui::PushFont(monoFont);
}

void Gui::pushHighlightText() {
    ImGui::PushStyleColor(ImGuiCol_Text, highlight_text_color);
}

static void addRecent(const fs::path& p) {
    recentFiles.remove(p);
    recentFiles.push_front(p);
    while (recentFiles.size() > 10) {
        recentFiles.pop_back();
    }
}

namespace Gui {
    struct Notification {
        std::string m_message;
        unsigned int m_time;

        Notification(const std::string& message)
            : m_message(message)
            , m_time(150) {}
    };


    struct {
    private:
        std::vector<Notification> notifications;
    public:
        void add(const std::string& message) {
            notifications.push_back(message);
            while (notifications.size() > 5) {
                notifications.pop_back();
            }
        }

        void update() {
            for (auto& n: notifications) {
                --n.m_time;
            }
             notifications.erase(
               std::remove_if(notifications.begin(), notifications.end(),
                              [](const auto& n){ return n.m_time == 0; }), 
               notifications.end());
        }

        void render() {
            update();

            if (notifications.size() == 0) {
                return;
            }
            const float DISTANCE = 10.0f;
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 window_pos = ImVec2(DISTANCE, io.DisplaySize.y - DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration 
                                         | ImGuiWindowFlags_AlwaysAutoResize 
                                         | ImGuiWindowFlags_NoSavedSettings 
                                         | ImGuiWindowFlags_NoFocusOnAppearing 
                                         | ImGuiWindowFlags_NoNav
                                         | ImGuiWindowFlags_NoInputs
                                         | ImGuiWindowFlags_NoBackground;

            if (ImGui::Begin("Notifications", nullptr, windowFlags)) {
                for (const auto& n: notifications) {
                    ImGui::Text(n.m_message.c_str());
                }
            }
            ImGui::End();
        }
    } Notifications;
}

static void Gui::addNotification(const std::string& message) {
    Gui::Notifications.add(message);
}

static void loadRecentFiles() {
    nlohmann::json recentFiles = Settings::object["recentFiles"];
    if (recentFiles.is_null()) {
        return;
    }
    if (!recentFiles.is_array()) {
        LOG_ERR << "Configuration Error: recentFiles should be array!\n";
        return;
    }

    for (auto entry : recentFiles) {
        if (!entry.is_string()) {
            LOG_ERR << "Configuration Error: recentFiles entry should be string\n";
            continue;
        }
        std::string s = entry;
        const fs::path path{s};
        if (fs::exists(path)) {
            addRecent(path);
        }
    }
}

static void writeRecentFiles() {
    Settings::object["recentFiles"] = std::vector<std::string>();
    for (auto it = recentFiles.rbegin(); it != recentFiles.rend(); it++) {
        Settings::object["recentFiles"].push_back(it->string());
    }
}

static bool openFile(Emu& emu, const fs::path& p) {
    std::shared_ptr<Cart> cart = Cart::fromFile(p);
    if (cart) {
        addRecent(p);
        emu.init(cart);
        return true;
    }
    return false;
}

static void renderMenuBar(Emu& emu) {
    bool openRom = false;
    bool setupControllers = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                openRom = true;
            }

            if (ImGui::BeginMenu("Recent")) {
                if (recentFiles.empty()) {
                    ImGui::MenuItem("<none>");
                } else {
                    fs::path path;
                    for (auto p: recentFiles) {
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

            if (ImGui::MenuItem("Setup Controllers")) {
                setupControllers = true;
            }
            
            if (ImGui::MenuItem("Reset")) {
                emu.reset();
            }

            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit")) {
                setWindowClosing(true);
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Debugger")) {
            if (ImGui::MenuItem("Log State", nullptr, emu.m_logState)) {
                emu.m_logState = !emu.m_logState;
            }

            for (auto& window: Gui::Window::Entries) {
                Gui::Window& w = *window.second;
                if (w.hasActions()) {
                    if (ImGui::BeginMenu(w.title)) {
                        for (const auto& action: w) {
                            if (ImGui::MenuItem(action.c_str(), nullptr)) {
                                w.runAction(action.c_str(), emu);
                            }
                        }
                        ImGui::EndMenu();
                    }
                }
            }

            if (ImGui::MenuItem("Break on Interrupts", nullptr, emu.m_breakOnInterrupt)) {
                emu.m_breakOnInterrupt = !emu.m_breakOnInterrupt;
            }

            if (ImGui::MenuItem("Absolute Labels", nullptr, emu.m_disassembler->m_showAbsoluteLabels)) {
                emu.m_disassembler->m_showAbsoluteLabels = !emu.m_disassembler->m_showAbsoluteLabels;
                emu.m_disassembler->refresh();
            }

            if (ImGui::MenuItem("Display Cartridge Addresses", nullptr, emu.m_disassembler->m_translateCartSpace)) {
                emu.m_disassembler->m_translateCartSpace = !emu.m_disassembler->m_translateCartSpace;
                emu.m_disassembler->refresh();
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Fullscreen", nullptr, isFullscreen())) {
                toggleFullscreen();
            }

            ImGui::Separator();

            for (auto& window: Gui::Window::Entries) {
                auto w = window.second;
                ImGui::MenuItem(w->title, nullptr, w->show());
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (openRom) {
        ImGui::OpenPopup("Open ROM");
    }

    if (setupControllers) {
        ImGui::OpenPopup("Setup Controllers");
    }
}

static void renderOpenRomDialog(Emu& emu) {
    fs::path selectedFile;
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

struct ButtonWidgetIds {
    std::string add;
    std::string clear;
};

static std::map<Input::ControllerDef, ButtonWidgetIds> buttonWidgetIds;

static void renderControllerButtonPane(Input::ControllerDef button) {
    ImGui::Text(Input::defToLabel.at(button).c_str()); 
    ImGui::SameLine();
    if (ImGui::Button(buttonWidgetIds[button].add.c_str())) {
        Input::waitForInput(button);
    }
    ImGui::SameLine();
    if (ImGui::Button(buttonWidgetIds[button].clear.c_str())) {
        Input::clearButton(button);
    }

    const auto& scancodes = Input::getScancodes(button);
    auto scancode = scancodes.begin();

    if (scancode == scancodes.end()) {
        ImGui::Text("None");
    } else {
        std::stringstream ss;
        for (; scancode != scancodes.end(); scancode++) {
            if (scancode != scancodes.begin())
                ss << ", ";
            const char * name = getKeyName(*scancode);
            ss << name;
        }

        ImGui::Text(ss.str().c_str());
    }
}

static void renderControllerSetupPane(const std::vector<Input::ControllerDef>& controller) {
    auto it = controller.begin();
    for (; it != controller.end(); it++) {
        if (it != controller.begin())
            ImGui::Separator();
        renderControllerButtonPane(*it);
    }
}

static void renderSetupControllersDialog() {
    bool openSetup = true;
    if (ImGui::BeginPopupModal("Setup Controllers", &openSetup, ImGuiWindowFlags_AlwaysAutoResize)) {
        renderControllerSetupPane(Input::inputsGeneral);

        ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
        if (ImGui::BeginTabBar("Controllers", tab_bar_flags)) {
            if (ImGui::BeginTabItem("Controller 1"))
            {
                renderControllerSetupPane(Input::inputsController0);
                ImGui::EndTabItem();
            }
            if (ImGui::BeginTabItem("Controller 2"))
            {
                renderControllerSetupPane(Input::inputsController1);
                ImGui::EndTabItem();
            }
            ImGui::EndTabBar();
        }

        if (Input::getWaitingForInput() != Input::None) {
            ImGui::OpenPopup("Waiting For Key");
        }
        bool openWaiting = true;
        if (ImGui::BeginPopupModal("Waiting For Key", &openWaiting, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::Text("Press a key");

            if (!openWaiting || Input::getWaitingForInput() == Input::None) {
                ImGui::CloseCurrentPopup();
                Input::waitForInput(Input::None);
            }
            ImGui::EndPopup();
        }

        if (!openSetup) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

static void setPixel(unsigned int x, unsigned int y, unsigned int v) {
    screenSurface->setPixel(x, y, Palette::DEFAULT[v & 0x3f]);
}

SetPixelFn Gui::getSetPixelFn() {
    return setPixel;
}

static void renderFrame() {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);

    screenSurface->upload();
    screenSurface->render(display_w, display_h);
}

void Gui::runFrame(Emu& emu) {
    const Input::State& inputs = Input::getState();
    if (inputs.openMenu) {
        emu.m_isStepping = true;
    } else {
        emu.stepFrame();
        renderFrame();
    }
}

void Gui::runUi(Emu& emu) {
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
    renderSetupControllersDialog();
    for (auto& window: Gui::Window::Entries) {
        window.second->render(emu);
    }
    Gui::Notifications.render();
    ImGui::PopFont();

    // Rendering
    ImGui::Render();
    renderFrame();
    ImGui_Impl_RenderDrawData(ImGui::GetDrawData());
}

#define ICON_FONT "assets/" ## FONT_ICON_FILE_NAME_FAR

bool initImGUI(GLFWwindow* window) {
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer bindings
    ImGui_Impl_Init();

    sansFont = io.Fonts->AddFontFromFileTTF("assets/SourceSansPro-Regular.ttf", 24.0f);
    ImFontConfig config;
    config.MergeMode = true;
    static const ImWchar icon_ranges[] = { ICON_MIN_MD, ICON_MAX_MD, 0 };
    io.Fonts->AddFontFromFileTTF("assets/MaterialIcons-Regular.ttf", 20.0f, &config, icon_ranges);
    monoFont = io.Fonts->AddFontFromFileTTF("assets/SourceCodePro-Medium.ttf", 20.0f);
    io.Fonts->Build();

    return true;
}

bool Gui::initUi(Emu& emu, bool fullscreen) {
    static char widgetIdBuffer[256];

    ImGui_FileBrowser_Init();

    if (!initWindow(WINDOW_TITLE, fullscreen)) {
        return false;
    }

    if (!Surface::init()) {
        return false;
    }

    screenSurface = std::make_shared<Gui::Surface>(256, 240);
    for (int y = 0; y < screenSurface->m_height; y++) {
        for (int x = 0; x < screenSurface->m_width; x++) {
            screenSurface->setPixel(x, y, {0, 0, 0});
        }
    }

    for (const auto& label: Input::defToString) {
        std::string addLabel, clearLabel;
        snprintf(widgetIdBuffer, 256, "Add##%s-add", label.second.c_str());
        addLabel = widgetIdBuffer;
        snprintf(widgetIdBuffer, 256, "Clear##%s-clear", label.second.c_str());
        clearLabel = widgetIdBuffer;
        buttonWidgetIds[label.first] = {addLabel, clearLabel};
    }

    Input::loadSettings();
    loadRecentFiles();

    for (auto& window: Gui::Window::Entries) {
        window.second->init(emu);
    }

    return initImGUI(window);
}

static void teardownImGui() {
    ImGui_Impl_Shutdown();
    ImGui::DestroyContext();
}

void Gui::teardownUi(Emu& emu) {
    for (auto& window : Gui::Window::Entries) {
        window.second->teardown(emu);
    }

    writeRecentFiles();
    Input::writeSettings();

    teardownImGui();
    screenSurface = nullptr;
    Surface::teardown();
    teardownWindow();
}
