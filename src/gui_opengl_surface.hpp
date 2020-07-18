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
        void render();

        Texture getTexture();

    private:
        size_t m_width;
        size_t m_height;
        Palette::Color* m_data;
        Texture m_texture;
    };
}