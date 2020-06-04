#pragma once

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

static const char* glsl_version = "#version 130";

static GLFWwindow* window = nullptr;

void setWindowClosing(bool closing) {
    if (window) {
        glfwSetWindowShouldClose(window, closing);
    }
}

bool isWindowClosing() {
    return glfwWindowShouldClose(window);
}

bool isFullscreen() {
    return glfwGetWindowMonitor(window) != nullptr;
}

void toggleFullscreen() {
    if (isFullscreen()) {
        glfwSetWindowMonitor(
            window, nullptr,
            50, 50, 1920, 1200,
            GLFW_DONT_CARE);
        glfwSwapInterval(1);
    } else {
        GLFWmonitor* monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(
            window, monitor,
            0, 0, mode->width, mode->height,
            GLFW_DONT_CARE);
        glfwSwapInterval(1);
    }
}

void ImGui_Impl_NewFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
}

void ImGui_Impl_RenderDrawData(ImDrawData* draw_data) {
    ImGui_ImplOpenGL3_RenderDrawData(draw_data);
}

void ImGui_Impl_Init() {
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void ImGui_Impl_Shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
}

static void glfw_error_callback(int error, const char* description)
{
    std::cerr << "Glfw Error " << error << ": " << description << "\n";
}

bool initWindow(const char* title, bool fullscreen) {
    glfwSetErrorCallback(glfw_error_callback);

    if (!glfwInit()) {
        return false;
    }

    // Set GL Version
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

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
        } else {
            width = int(.66f * float(mode->width));
            height = int(.66f * float(mode->height));
        }
    } else {
        width = 1920;
        height = 1200;
    }

    window = glfwCreateWindow(
        width, height, 
        "staNES", 
        fullscreen ? monitor : nullptr, 
        nullptr
    );
    if (window == NULL) {
        return false;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup GLAD
    bool err = gladLoadGL() == 0;
    if (err) {
        std::cerr << "Failed to initialize OpenGL loader!\n";
        return false;
    }

    return true;
}

void teardownWindow() {
    glfwDestroyWindow(window);
    glfwTerminate();
}
