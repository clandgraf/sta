#include <imgui.h>

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

static bool patternTableLoaded = false;

static bool showEmuState = true;
static bool showMemoryView = true;
static bool showRomInfo = true;
static bool showDisassembly = true;
static bool showPatternTable = true;
static bool showControls = false;

#include "gui_opengl.hpp"

static Gui::TextureType patternTableTexture;

uint8_t colors[4][3] = {
    { 0xff, 0xff, 0xff },
    { 0xbb, 0xbb, 0xbb },
    { 0x77, 0x77, 0x77 },
    { 0x00, 0x00, 0x00 },
};

static void refreshPatternTable(Emu& emu) {
    // 256 x 128

    uint8_t* data = new uint8_t[2 /*tables*/ * 3 /*colors*/ * 128 * 128];

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

                    size_t textureOff = (textureY * 256 + textureX) * 3;
                    data[textureOff + 0] = colors[pxl][0];
                    data[textureOff + 1] = colors[pxl][1];
                    data[textureOff + 2] = colors[pxl][2];
                }
            }
        }
    }

    Gui::uploadTextureData(patternTableTexture, 256, 128, data);
    delete[] data;
}

static void initPatternTable() {
    uint8_t* data = new uint8_t[2 /*tables*/ * 3 /*colors*/ * 128 * 128];

    for (int i = 0; i < 2 * 128 * 128; i++) {
        data[i * 3 + 0] = 0x00;
        data[i * 3 + 1] = 0x00;
        data[i * 3 + 2] = 0x00;
    }

    Gui::uploadTextureData(patternTableTexture, 256, 128, data);
}

static void renderMenuBar(Emu& emu) {
    bool openRom = false;
    bool setupControllers = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                openRom = true;
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
            std::shared_ptr<Cart> cart = Cart::fromFile(selectedFile);
            if (cart) {
                emu.init(cart);
                open = false;
            }
            
        }
        if (!open) {
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}

static void renderSetupControllersDialog() {
    bool open = true;
    if (ImGui::BeginPopupModal("Setup Controllers", &open, ImGuiWindowFlags_AlwaysAutoResize)) {
        

        if (!open) {
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
            ImGui::Image((void*)(intptr_t)patternTableTexture, ImVec2(512, 256));
        }
        ImGui::End();
    }
}

static void renderControls(EmuInputs& inputs) {
    if (showControls) {
        if (ImGui::Begin("Controls", &showControls)) {
            ImGui::Checkbox("Up", &(inputs.d_up));
            ImGui::SameLine();
            ImGui::Checkbox("Left", &(inputs.d_left));
            ImGui::SameLine();
            ImGui::Checkbox("A", &(inputs.btn_a));
            ImGui::SameLine();
            ImGui::Checkbox("Start", &(inputs.start));

            ImGui::Checkbox("Down", &(inputs.d_down));
            ImGui::SameLine();
            ImGui::Checkbox("Right", &(inputs.d_right));
            ImGui::SameLine();
            ImGui::Checkbox("B", &(inputs.btn_b));
            ImGui::SameLine();
            ImGui::Checkbox("Select", &(inputs.select));
        }
        ImGui::End();
    }
}

static void renderFrame() {
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Gui::runFrame(Emu& emu) {
    EmuInputs inputs = getInputs();
    if (inputs.escape) {
        emu.m_isStepping = true;
    } else {
        emu.stepFrame();
        renderFrame();
    }
}

void Gui::runUi(Emu& emu) {
    static EmuInputs emuInputs;

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
    renderControls(emuInputs);
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
    ImGui_FileBrowser_Init();

    if (!initWindow(WINDOW_TITLE, fullscreen)) {
        return false;
    }

    showMemoryView = Settings::get("debugger-view-memory", true);
    showEmuState = Settings::get("debugger-view-state", true);
    showDisassembly = Settings::get("debugger-view-disassembly", true);
    showRomInfo = Settings::get("debugger-view-rominfo", true);
    showControls = Settings::get("debugger-view-controls", true);

    patternTableTexture = Gui::createTexture();
    initPatternTable();

    return initImGUI(window);
}

static void teardownImGui() {
    ImGui_Impl_Shutdown();
    ImGui::DestroyContext();
}

void Gui::teardownUi() {
    freeTexture(patternTableTexture);

    Settings::set("debugger-view-memory", showMemoryView);
    Settings::set("debugger-view-state", showEmuState);
    Settings::set("debugger-view-disassembly", showDisassembly);
    Settings::set("debugger-view-rominfo", showRomInfo);
    Settings::set("debugger-view-controls", showControls);

    teardownImGui();
    teardownWindow();
}
