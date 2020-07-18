#pragma once

#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "inputs.hpp"
#include "util.hpp"

static const char* glsl_version = "#version 130";

static GLFWwindow* window = nullptr;

namespace Gui {

    using Texture = unsigned int;

    using Shader = unsigned int;

    using Program = unsigned int;

    Texture createTexture() {
        Texture texture;
        glGenTextures(1, &texture);
        return texture;
    }

    void freeTexture(Texture texture) {
        glDeleteTextures(1, &texture);
    }

    void uploadTextureData(Texture texture, int width, int height, const void* data) {
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    }

    bool initShader(Shader& shader, unsigned int type, const char* source) {
        shader = glCreateShader(type);
        glShaderSource(shader, 1, &source, NULL);
        glCompileShader(shader);

        int  success;
        char log[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, log);
            LOG_ERR << log << "\n";
            return false;
        }

        return true;
    }

    bool initProgram(Program& program, std::vector<Shader> shaders) {
        program = glCreateProgram();
        for (const auto& shader: shaders) {
            glAttachShader(program, shader);
        }
        glLinkProgram(program);
        
        int success;
        char log[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, log);
            LOG_ERR << log << "\n";
            return false;
        }
    
        return true;
    }

    class Surface {

    public:
        static bool init() {
            if (!initShader(VertexShader, GL_VERTEX_SHADER, VertexShaderSource)
                || !initShader(FragmentShader, GL_FRAGMENT_SHADER, FragmentShaderSource)) {
                return false;
            }

            return initProgram(Program, {VertexShader, FragmentShader});
        }

    private:
        static const char* VertexShaderSource;
        static const char* FragmentShaderSource;

        static Shader VertexShader;
        static Shader FragmentShader;
        static Program Program;

    public:
        Surface(size_t width, size_t height) : m_width(m_width), m_height(height) {
            m_data = new Palette::Color[m_width * m_height];
            m_texture = createTexture();
        }

        ~Surface() {
            freeTexture(m_texture);
            delete[] m_data;
        }

        void setPixel(int x, int y, Palette::Color color) {
            m_data[y * m_width + x] = color;
        }

        void render() {
            uploadTextureData(m_texture, m_width, m_height, m_data);

            
        }

    private:
        size_t m_width;
        size_t m_height;
        Palette::Color* m_data;
        Texture m_texture;
    };
}

const char* Gui::Surface::VertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\n";

const char* Gui::Surface::FragmentShaderSource = 
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main() {\n"
    "    FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\n";

Gui::Shader Gui::Surface::VertexShader = 0;
Gui::Shader Gui::Surface::FragmentShader = 0;
Gui::Program Gui::Surface::Program = 0;

void setWindowClosing(bool closing) {
    if (window) {
        glfwSetWindowShouldClose(window, closing);
    }
}

bool Gui::isWindowClosing() {
    return glfwWindowShouldClose(window);
}

void Gui::pollEvents() {
    Input::resetMenuRequest();
    return glfwPollEvents();
}

bool isFullscreen() {
    return glfwGetWindowMonitor(window) != nullptr;
}

void Gui::swapBuffers() {
    glfwSwapBuffers(window);
}

#include "keynames.hpp"

const char* getKeyName(int scancode) {
    return KEY_NAMES.at(scancode);
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

void updateInputs(GLFWwindow* window, int key, int scancode, int action, int mods) {
    Input::dispatchInput(key, action == GLFW_PRESS);
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
            width = Settings::get("window-width", int(.66f * float(mode->width)));
            height = Settings::get("window-height", int(.66f * float(mode->height)));
        }
    } else {
        width = Settings::get("window-width", 1920);
        height = Settings::get("window-height", 1200);
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

    Input::setScancode(GLFW_KEY_ESCAPE, Input::Menu);
    glfwSetKeyCallback(window, updateInputs);

    return true;
}

void teardownWindow() {
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    Settings::set("window-width", width);
    Settings::set("window-height", height);

    glfwDestroyWindow(window);
    glfwTerminate();
}
