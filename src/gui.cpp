#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "gui.hpp"
#include "rom.hpp"

#include <iostream>
#include <cstdio>

#define _CRT_SECURE_NO_WARNINGS 1
#include <imgui_memory_editor.h>

static const char* glsl_version = "#version 130";

static GLFWwindow* window;

static ImFont* defaultFont;
static ImFont* sansFont;
static ImFont* monoFont;

static MemoryEditor mem_edit;

static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

static bool showMemoryView = true;
static bool showRomInfo = true;

void renderMenuBar() {
    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Open", "CTRL+O")) {}
            ImGui::Separator();
            if (ImGui::MenuItem("Exit", "CTRL+Q")) {
                glfwSetWindowShouldClose(window, true);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("View")) {
            if (ImGui::MenuItem("Rom Info", nullptr, nullptr, !showRomInfo)) {
                showRomInfo = !showRomInfo;
            }

            if (ImGui::MenuItem("Memory", nullptr, nullptr, !showMemoryView)) {
                showMemoryView = !showMemoryView;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void renderRomInfo(cart* cart) {
    if (cart && showRomInfo) {
        if (ImGui::Begin("ROM Info", &showRomInfo)) {
            ImGui::Text("Mapper: %d, %s", cart->mapper_id, "foo");
            ImGui::Text("PRG ROM #: %d", cart->prg_size());
            ImGui::Text("CHR ROM #: %d", cart->chr_size());
        }
        ImGui::End();
    }
}

void renderMemoryView(cart* cart) {
    // Create Memory View
    if (cart && showMemoryView) {
        if (ImGui::Begin("Memory", &showMemoryView)) {
            ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
            char title[10];
            if (ImGui::BeginTabBar("MyTabBar", tab_bar_flags)) {
                for (uint8_t i = 0; i < cart->prg_size(); i++) {
                    snprintf(title, 10, "PRG %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(cart->prg_banks[i], 0x4000, 0x4000 * i);
                        ImGui::PopFont();
                        ImGui::EndTabItem();
                    }
                }

                for (uint8_t i = 0; i < cart->chr_size(); i++) {
                    snprintf(title, 10, "CHR %d", i);
                    if (ImGui::BeginTabItem(title)) {
                        ImGui::PushFont(monoFont);
                        mem_edit.DrawContents(cart->chr_banks[i], 0x2000, 0x2000 * i);
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

void doUi(cart* cart) {
    // Poll and handle events (inputs, window resize, etc.)
    // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
    // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application.
    // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application.
    // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    // Render Views
    ImGui::PushFont(sansFont);
    renderMenuBar();
    renderRomInfo(cart);
    renderMemoryView(cart);
    ImGui::PopFont();

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

GLFWwindow* initGL() {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        return nullptr;
    }

    // Create GL Context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    GLFWwindow* window = glfwCreateWindow(1920, 1200, "Dear ImGui GLFW+OpenGL3 example", NULL, NULL);
    if (window == NULL) {
        return nullptr;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup GLAD
    bool err = gladLoadGL() == 0;
    if (err) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return nullptr;
    }

    return window;
}

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
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    defaultFont = io.Fonts->AddFontDefault();
    sansFont = io.Fonts->AddFontFromFileTTF("assets/SourceSansPro-Regular.ttf", 24.0f);
    monoFont = io.Fonts->AddFontFromFileTTF("assets/SourceCodePro-Medium.ttf", 20.0f);
    if (!monoFont) {
        std::cerr << "Failed to load Monospaced-Font!\n";
        return false;
    }

    return true;
}

bool initUi() {
    window = initGL();
    if (!window) {
        return false;
    }
    return initImGUI(window);
}

void teardownImGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void teardownGL() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void teardownUi() {
    teardownImGUI();
    teardownGL();
}

bool isUiClosing() {
    return glfwWindowShouldClose(window);
}