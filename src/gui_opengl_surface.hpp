#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "util.hpp"

namespace Gui {

    using Texture = unsigned int;

    using Shader = unsigned int;

    using Program = unsigned int;

    class Surface {

    public:
        static bool init();
        static void teardown();

    private:
        static const char* VertexShaderSource;
        static const char* FragmentShaderSource;

        static unsigned int vbo, vao, ebo;

        static Program Program;

    public:
        Surface(size_t width, size_t height);
        ~Surface();

        void setPixel(int x, int y, Palette::Color color);
        void upload();
        void render(int displayWidth, int displayHeight);

        Texture getTexture();

        const size_t m_width;
        const size_t m_height;

    private:
        Palette::Color* m_data;
        Texture m_texture;
    };
}