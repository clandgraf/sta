#include <imgui.h>
#include "gui_opengl.hpp"

#include "gui.hpp"
#include "rom.hpp"
#include "mem.hpp"
#include "emu.hpp"
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

static bool showEmuState = true;
static bool showMemoryView = true;
static bool showRomInfo = true;
static bool showDisassembly = true;

static void renderMenuBar(Disassembler& disasm) {
    bool openRom = false;

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open")) {
                openRom = true;
            }
            
            ImGui::Separator();
            
            if (ImGui::MenuItem("Exit")) {
                setWindowClosing(true);
            }
            
            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Disassembler")) {
            if (ImGui::MenuItem("Absolute Labels", nullptr, disasm.m_showAbsoluteLabels)) {
                disasm.m_showAbsoluteLabels = !disasm.m_showAbsoluteLabels;
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
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (openRom) {
        ImGui::OpenPopup("Open ROM");
    }
}

static void renderRomInfo(Emu& emu) {
    if (emu.isInitialized() && showRomInfo) {
        if (ImGui::Begin("ROM Info", &showRomInfo)) {
            ImGui::Text("Mapper: %d, %s", emu.m_cart->mapper_id, mappers[emu.m_cart->mapper_id]);
            ImGui::Text("PRG ROM #: %d", emu.m_cart->prg_size());
            ImGui::Text("CHR ROM #: %d", emu.m_cart->chr_size());
        }
        ImGui::End();
    }
}

static void renderEmuState(Emu& emu) {
    if (emu.isInitialized() && showEmuState) {
        if (ImGui::Begin("Emu State", &showEmuState)) {
            if (ImGui::Button(ICON_MD_PLAY_ARROW)) {
                emu.stepOperation();
            }

            ImGui::PushFont(monoFont);
            ImGui::Text("PC: %04x  Carry:    %01x", emu.m_pc, emu.m_f_carry);
            ImGui::Text("SP: %02x    Zero:     %01x", emu.m_sp, emu.m_f_zero);
            ImGui::Text("A:  %02x    IRQ:      %01x", emu.m_r_a, emu.m_f_irq); 
            ImGui::Text("X:  %02x    Decimal:  %01x", emu.m_r_x, emu.m_f_decimal);
            ImGui::Text("Y:  %02x    Overflow: %01x", emu.m_r_y, emu.m_f_overflow);
            ImGui::Text("          Negative: %01x", emu.m_f_negative);
            ImGui::Text("CPU Cycles: %d", emu.getCycleCount());
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
                for (uint8_t i = 0; i < emu.m_cart->prg_size(); i++) {
                    snprintf(title, 10, "PRG %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(emu.m_cart->prg_banks[i], 0x4000, 0x8000 + 0x4000 * i);  // TODO only true for NROM
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                for (uint8_t i = 0; i < emu.m_cart->chr_size(); i++) {
                    snprintf(title, 10, "CHR %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(emu.m_cart->chr_banks[i], 0x2000, 0x2000 * i);
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
            Cart* cart = Cart::fromFile(selectedFile);
            if (cart) {
                emu.init(cart);
                open = false;
            }
            
        }
        ImGui::EndPopup();
    }

    if (!open) {
        ImGui::CloseCurrentPopup();
    }
}

int16_t lastOpcode = -1;
std::string disassembledLine{""};

void renderDisassembly(Emu& emu, Disassembler& disasm) {
    using DisasmSegmentSptr = std::shared_ptr<DisasmSegment>;

    if (emu.getMode() == Emu::Mode::EXEC) {
        uint16_t address = emu.getOpcodeAddress();
        DisasmSegmentSptr segment = disasm.disasmSegment(address);

        if (ImGui::Begin("Disassembly", &showDisassembly)) {
            ImGui::PushFont(monoFont);
            for (auto& entry: segment->m_lines) {
                auto& line = entry.second;
                if (line.offset == address) {
                    ImGui::PushStyleColor(ImGuiCol_Text, highlight_text_color);
                    ImGui::Text(line.repr.c_str());
                    ImGui::PopStyleColor();
                } else {
                    ImGui::Text(line.repr.c_str());
                }
            }
            ImGui::PopFont();
        }
        ImGui::End();
    }
}

void doUi(Emu& emu, Disassembler& disasm) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_Impl_NewFrame();
    ImGui::NewFrame();

    // Render Views
    ImGui::PushFont(sansFont);

    // ImGui::ShowDemoWindow();

    renderMenuBar(disasm);
    renderOpenRomDialog(emu);
    renderEmuState(emu);
    renderRomInfo(emu);
    renderMemoryView(emu);
    renderDisassembly(emu, disasm);
    ImGui::PopFont();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_Impl_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
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

bool initUi(bool fullscreen) {
    ImGui_FileBrowser_Init();

    if (!initWindow(WINDOW_TITLE, fullscreen)) {
        return false;
    }

    return initImGUI(window);
}

void teardownImGui() {
    ImGui_Impl_Shutdown();
    ImGui::DestroyContext();
}

void teardownUi() {
    teardownImGui();
    teardownWindow();
}
