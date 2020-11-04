#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <sstream>

#include "core/gamepad.hpp"
#include "core/util.hpp"
#include "core/gui/opengl.hpp"
#include "core/gui/notifications.hpp"

static const char* glsl_version = "#version 130";

namespace Gui {
    bool m_isEscapePressed = false;
}

GLFWwindow* w(Gui::Window win) { return (GLFWwindow*) win._; }

void Gui::setWindowClosing(Window window, bool closing) {
    if (window._) {
        glfwSetWindowShouldClose(w(window), closing);
    }
}

bool Gui::isWindowClosing(Window window) {
    return glfwWindowShouldClose(w(window));
}

void Gui::pollEvents() {
    return glfwPollEvents();
}

bool Gui::isFullscreen(Window window) {
    return glfwGetWindowMonitor(w(window)) != nullptr;
}

void Gui::swapBuffers(Window window) {
    glfwSwapBuffers(w(window));
}

void Gui::toggleFullscreen(Window window) {
    GLFWwindow* win{w(window)};
    if (isFullscreen(window)) {
        glfwSetWindowMonitor(
            win, nullptr,
            50, 50, 1920, 1200,
            GLFW_DONT_CARE);
        glfwSwapInterval(1);
    }
    else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(
            win, monitor,
            0, 0, mode->width, mode->height,
            GLFW_DONT_CARE);
        glfwSwapInterval(1);
    }
}

void Gui::ImGui_Impl_NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void Gui::ImGui_Impl_RenderDrawData(ImDrawData* draw_data) {
    ImGui_ImplOpenGL3_RenderDrawData(draw_data);
}

void Gui::ImGui_Impl_Init(Gui::Window window) {
    ImGui_ImplGlfw_InitForOpenGL(w(window), true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void Gui::ImGui_Impl_Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

static void glfw_joystick_callback(int jid, int event) {
    std::stringstream ss;
    if (event == GLFW_CONNECTED) {
        ss << "Gamepad " << jid << " connected.";
        if (!glfwJoystickIsGamepad(jid))
        {
            ss << " No gamepad.";
        }
        Gui::addNotification(ss.str());
    }
    else if (event == GLFW_DISCONNECTED) {
        ss << "Gamepad " << jid << " disconnected.";
        Gui::addNotification(ss.str());
    }
}

static void glfw_error_callback(int error, const char* description) {
    LOG_ERR << "Glfw Error " << error << ": " << description << "\n";
}

bool Gui::isEscapePressed() {
    bool v = m_isEscapePressed;
    m_isEscapePressed = false;
    return v;
}

void updateInputs(GLFWwindow* window, int key, int scancode, int action, int mods) {
    switch (action) {
    case GLFW_PRESS:
        if (key == GLFW_KEY_ESCAPE) {
            Gui::m_isEscapePressed = true;
        }

        Gamepad::onKeydown(key, true);
        break;
    case GLFW_RELEASE:
        Gamepad::onKeydown(key, false);
        break;
    }
}

Gui::Window Gui::initWindow(const char* title, bool fullscreen) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        return Window{nullptr};
    }

    // Set GL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create Window
    GLFWmonitor* monitor = nullptr;
    const GLFWvidmode* mode = nullptr;
    unsigned int width = 0;
    unsigned int height = 0;

    monitor = glfwGetPrimaryMonitor();
    mode = glfwGetVideoMode(monitor);

    if (monitor && mode) {
        if (fullscreen) {
            width = mode->width;
            height = mode->height;
        }
        else {
            width = Settings::get("window-width", int(.66f * float(mode->width)));
            height = Settings::get("window-height", int(.66f * float(mode->height)));
        }
    }
    else {
        width = Settings::get("window-width", 1920);
        height = Settings::get("window-height", 1200);
    }

    GLFWwindow* win = glfwCreateWindow(
        width, height,
        "staNES",
        fullscreen ? monitor : nullptr,
        nullptr
    );
    if (win == nullptr) {
        return Window{nullptr};
    }
    glfwMakeContextCurrent(win);
    glfwSwapInterval(1); // Enable vsync

    // Setup GLAD
    bool err = gladLoadGL() == 0;
    if (err) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return Window{nullptr};
    }

    glfwSetKeyCallback(win, updateInputs);
    glfwSetJoystickCallback(glfw_joystick_callback);

    return Window{win};
}

void Gui::setTitle(Window window, const char* title) {
    glfwSetWindowTitle(w(window), title);
}

void Gui::teardownWindow(Window window) {
    int width, height;
    glfwGetWindowSize(w(window), &width, &height);
    Settings::set("window-width", width);
    Settings::set("window-height", height);

    glfwDestroyWindow(w(window));
    glfwTerminate();
}
