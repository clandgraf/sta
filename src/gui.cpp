#include <imgui.h>
#include <sstream>

#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
#include "ppu.hpp"
#include "disasm.hpp"
#include "mappers.hpp"
#include "IconsMaterialDesign.h"

#include <iostream>
#include <cstdio>
#include <filesystem>

#define _CRT_SECURE_NO_WARNINGS 1
#include <imgui_memory_editor.h>

#include "gui_filebrowser.hpp"

#include "defs.hpp"

namespace fs = std::filesystem;

static ImFont* defaultFont;
static ImFont* sansFont;
static ImFont* monoFont;

static MemoryEditor mem_edit;

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
static ImVec4 highlight_text_color = ImVec4(0.89f, 0.0f, 0.0f, 1.00f);

static std::list<fs::path> recentFiles;

static bool patternTableLoaded = false;

static bool showEmuState = true;
static bool showMemoryView = true;
static bool showRomInfo = true;
static bool showDisassembly = true;
static bool showPatternTable = true;
static bool showControls = false;

#include "gui_opengl.hpp"

static std::shared_ptr<Gui::Surface> patternTableSurface;

static std::shared_ptr<Gui::Surface> screenSurface;

Palette::Color colors[4] = {
    { 0xff, 0xff, 0xff },
    { 0xbb, 0xbb, 0xbb },
    { 0x77, 0x77, 0x77 },
    { 0x00, 0x00, 0x00 },
};

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

static void addNotification(const std::string& message) {
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

static void refreshPatternTable(Emu& emu) {
    for (int table = 0; table < 2; table++) {
        for (int tile = 0; tile < 256; tile++) {
            uint16_t offset = (table ? 0x1000 : 0) + tile * 0x10;
            for (int row = 0; row < 8; row++) {
                uint8_t p0 = emu.m_cart->readb_ppu(offset + row + 0);
                uint16_t p1 = emu.m_cart->readb_ppu(offset + row + 8);
                for (int col = 0; col < 8; col++) {
                    uint8_t pxl = ((p0 >> (7 - col)) & 1) | (((p1 >> (7 - col)) & 1) << 1);

                    size_t textureX = ((table ? 128 : 0) + (tile % 16) * 8 + col);
                    size_t textureY = ((tile / 16) * 8 + row);

                    patternTableSurface->setPixel(textureX, textureY, colors[pxl]);
                }
            }
        }
    }

    patternTableSurface->upload();
}

static void initPatternTable() {
    for (int y = 0; y < 128; y++) {
        for (int x = 0; x < 2 * 128; x++) {
            patternTableSurface->setPixel(x, y, {0, 0, 0});
        }
    }

    patternTableSurface->upload();
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

            if (ImGui::MenuItem("Refresh Pattern Table", nullptr)) {
                if (emu.isInitialized()) {
                    refreshPatternTable(emu);
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

            if (ImGui::MenuItem("Emu State", nullptr, showEmuState)) {
                showEmuState = !showEmuState;
            }

            if (ImGui::MenuItem("Rom Info", nullptr, showRomInfo)) {
                showRomInfo = !showRomInfo;
            }

            if (ImGui::MenuItem("Disassembly", nullptr, showDisassembly)) {
                showDisassembly = !showDisassembly;
            }

            if (ImGui::MenuItem("Memory", nullptr, showMemoryView)) {
                showMemoryView = !showMemoryView;
            }

            if (ImGui::MenuItem("Controls", nullptr, showControls)) {
                showControls = !showControls;
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

static void renderRomInfo(Emu& emu) {
    if (emu.isInitialized() && showRomInfo) {
        if (ImGui::Begin("ROM Info", &showRomInfo)) {
            ImGui::Text("File: %s", emu.m_cart->m_name.c_str());
            ImGui::Text("Mapper: %d, %s", emu.m_cart->m_mapperId, mappers[emu.m_cart->m_mapperId]);
            ImGui::Text("PRG ROM #: %d", emu.m_cart->prg_size());
            ImGui::Text(emu.m_cart->m_useChrRam ? "CHR RAM #: %d" : "CHR ROM #: %d", emu.m_cart->chr_size());
        }
        ImGui::End();
    }
}

static void ImGui_AttachTooltip(const char* str) {
    if (ImGui::IsItemHovered()) {
        ImGui::BeginTooltip();
        ImGui::TextUnformatted(str);
        ImGui::EndTooltip();
    }
}

static void renderStateControl(const char* icon, const char* tooltip, std::function<void()> action) {
    if (ImGui::Button(icon)) {
        action();
    }
    ImGui_AttachTooltip(tooltip);
}

static void renderEmuState(Emu& emu) {
    if (emu.isInitialized() && showEmuState) {
        if (ImGui::Begin("Emu State", &showEmuState)) {
            renderStateControl(ICON_MD_PLAY_ARROW, "Continue", [&emu]{ emu.m_isStepping = false; });
            ImGui::SameLine();
            renderStateControl(ICON_MD_ARROW_FORWARD, "Step Operation", [&emu]{ emu.stepOperation(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_ARROW_UPWARD, "Step Out", [&emu] { emu.stepOut(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_CAMERA, "Step Frame", [&emu] { emu.stepFrame(); });
            ImGui::SameLine();
            renderStateControl(ICON_MD_SKIP_NEXT, "Step Scanline", [&emu] { emu.stepScanline(); });

            ImGui::PushFont(monoFont);
            ImGui::Text("CPU Cycles: %d", emu.getCycleCount());
            ImGui::Text("PC: %04x  Carry:    %01x", emu.m_pc, emu.m_f_carry);
            ImGui::Text("SP: %02x    Zero:     %01x", emu.m_sp, emu.m_f_zero);
            ImGui::Text("A:  %02x    IRQ:      %01x", emu.m_r_a, emu.m_f_irq); 
            ImGui::Text("X:  %02x    Decimal:  %01x", emu.m_r_x, emu.m_f_decimal);
            ImGui::Text("Y:  %02x    Overflow: %01x", emu.m_r_y, emu.m_f_overflow);
            ImGui::Text("          Negative: %01x", emu.m_f_negative);
            ImGui::Separator();
            ImGui::Text("PPU Cycles: %d", emu.m_ppu->getCycleCount());
            ImGui::Text("PPU Scanline: %d", emu.m_ppu->m_scanline);
            ImGui::PopFont();
        }
        ImGui::End();
    }
}

static void renderMemoryView(Emu& emu) {
    // Create Memory View
    if (emu.isInitialized() && showMemoryView) {
        if (ImGui::Begin("Memory", &showMemoryView)) {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            char title[10];
            if (ImGui::BeginTabBar("Memory Tabbar", tab_bar_flags)) {
                if (ImGui::BeginTabItem("RAM")) {
                    ImGui::PushFont(monoFont);
                    mem_edit.DrawContents(emu.m_mem->m_internalRam, 0x800, 0x0000);
                    ImGui::PopFont();
                    ImGui::EndTabItem();
                }
                for (uint8_t i = 0; i < emu.m_cart->prg_size(); i++) {
                    snprintf(title, 10, "PRG %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(emu.m_cart->m_prgBanks[i], 0x4000, 0x8000 + 0x4000 * i);  // TODO only true for NROM
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                for (uint8_t i = 0; i < emu.m_cart->chr_size(); i++) {
                    snprintf(title, 10, "CHR %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(emu.m_cart->chr(i), 0x2000, 0x2000 * i);
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                ImGui::EndTabBar();
            }
        }
        ImGui::End();
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

static int16_t lastOpcode = -1;
static std::string disassembledLine{""};

static void renderDisassembly(Emu& emu) {
    using DisasmSegmentSptr = std::shared_ptr<DisasmSegment>;

    if (emu.getMode() != Emu::Mode::RESET) {
        uint16_t address = emu.getOpcodeAddress();
        DisasmSegmentSptr segment = emu.m_disassembler->disasmSegment(address);

        if (ImGui::Begin("Disassembly", &showDisassembly)) {
            ImGui::PushFont(monoFont);
            for (auto& entry: segment->m_lines) {
                auto& line = entry.second;

                bool isBreakpoint = emu.isBreakpoint(line.offset);
                static char buffer[0xff];
                snprintf(buffer, 0xff, "%s###%04x_brk", isBreakpoint ? ">" : " ", line.offset);
                if (ImGui::Selectable(buffer, isBreakpoint)) {
                    emu.toggleBreakpoint(line.offset);
                }
                ImGui::SameLine();
                if (line.offset == address) {
                    ImGui::PushStyleColor(ImGuiCol_Text, highlight_text_color);
                    ImGui::Text(line.repr.c_str());
                    ImGui::PopStyleColor();
                } else {
                    ImGui::Text(line.repr.c_str());
                }
            }
            ImGui::PopFont();

            if (ImGui::Button("continue...")) {
                emu.m_disassembler->continueSegment(segment);
            }
        }
        ImGui::End();
    }
}

static void renderPatternTable() {
    if (showPatternTable) {
        if (ImGui::Begin("Pattern Table", &showPatternTable)) {
            ImGui::Image((void*)(intptr_t)patternTableSurface->getTexture(), ImVec2(512, 256));
        }
        ImGui::End();
    }
}

static void renderControls(Input::Controller& input0, Input::Controller& input1) {
    if (showControls) {
        if (ImGui::Begin("Controls", &showControls)) {
            ImGui::Text("Controller 1");
            ImGui::Checkbox("Up", &(input0.d_up));
            ImGui::SameLine();
            ImGui::Checkbox("Left", &(input0.d_left));
            ImGui::SameLine();
            ImGui::Checkbox("A", &(input0.btn_a));
            ImGui::SameLine();
            ImGui::Checkbox("Start", &(input0.start));

            ImGui::Checkbox("Down", &(input0.d_down));
            ImGui::SameLine();
            ImGui::Checkbox("Right", &(input0.d_right));
            ImGui::SameLine();
            ImGui::Checkbox("B", &(input0.btn_b));
            ImGui::SameLine();
            ImGui::Checkbox("Select", &(input0.select));

            ImGui::Separator();

            ImGui::Text("Controller 2");
            ImGui::Checkbox("Up", &(input1.d_up));
            ImGui::SameLine();
            ImGui::Checkbox("Left", &(input1.d_left));
            ImGui::SameLine();
            ImGui::Checkbox("A", &(input1.btn_a));
            ImGui::SameLine();
            ImGui::Checkbox("Start", &(input1.start));

            ImGui::Checkbox("Down", &(input1.d_down));
            ImGui::SameLine();
            ImGui::Checkbox("Right", &(input1.d_right));
            ImGui::SameLine();
            ImGui::Checkbox("B", &(input1.btn_b));
            ImGui::SameLine();
            ImGui::Checkbox("Select", &(input1.select));
        }
        ImGui::End();
    }
}

static void setPixel(unsigned int x, unsigned int y, unsigned int v) {
    screenSurface->setPixel(x, y, Palette::DEFAULT[v]);
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
    screenSurface->render();
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
    static Input::Controller controller0;
    static Input::Controller controller1;

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
    renderEmuState(emu);
    renderRomInfo(emu);
    renderMemoryView(emu);
    renderDisassembly(emu);
    renderPatternTable();
    renderControls(controller0, controller1);
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

bool Gui::initUi(bool fullscreen) {
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

    showMemoryView = Settings::get("debugger-view-memory", true);
    showEmuState = Settings::get("debugger-view-state", true);
    showDisassembly = Settings::get("debugger-view-disassembly", true);
    showRomInfo = Settings::get("debugger-view-rominfo", true);
    showControls = Settings::get("debugger-view-controls", true);

    patternTableSurface = std::make_shared<Gui::Surface>(2 * 128, 128);
    initPatternTable();

    return initImGUI(window);
}

static void teardownImGui() {
    ImGui_Impl_Shutdown();
    ImGui::DestroyContext();
}

void Gui::teardownUi() {
    Settings::set("debugger-view-memory", showMemoryView);
    Settings::set("debugger-view-state", showEmuState);
    Settings::set("debugger-view-disassembly", showDisassembly);
    Settings::set("debugger-view-rominfo", showRomInfo);
    Settings::set("debugger-view-controls", showControls);

    writeRecentFiles();
    Input::writeSettings();

    teardownImGui();
    patternTableSurface = nullptr;
    screenSurface = nullptr;
    Surface::teardown();
    teardownWindow();
}
