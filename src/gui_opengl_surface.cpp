#include "gui_opengl_surface.hpp"

const char* Gui::Surface::VertexShaderSource =
    "#version 330 core\n"
    "layout(location = 0) in vec3 aPos;\n"
    "layout(location = 1) in vec2 aTexCoord;\n"
    "out vec2 TexCoord;\n"
    "void main() {\n"
    "  gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "  TexCoord = aTexCoord;"
    "}\n\0";

const char* Gui::Surface::FragmentShaderSource =
    "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in  vec2 TexCoord;\n"
    "uniform sampler2D ourTexture;\n"
    "void main() {\n"
    "    FragColor = texture(ourTexture, TexCoord);\n"
    "}\n\0";

Gui::Program Gui::Surface::Program = 0;
unsigned int Gui::Surface::vao = 0;
unsigned int Gui::Surface::vbo = 0;
unsigned int Gui::Surface::ebo = 0;


static Gui::Texture createTexture() {
    Gui::Texture texture;
    glGenTextures(1, &texture);
    return texture;
}

void freeTexture(Gui::Texture texture) {
    glDeleteTextures(1, &texture);
}

void uploadTextureData(Gui::Texture texture, int width, int height, const void* data) {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
}

static bool initShader(Gui::Shader& shader, unsigned int type, const char* source) {
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

static bool initProgram(Gui::Program& program, std::vector<Gui::Shader> shaders) {
    program = glCreateProgram();
    for (const auto& shader : shaders) {
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

bool Gui::Surface::init() {
    Shader vertexShader = 0;
    Shader fragmentShader = 0;
    bool success = true;

    success = success && initShader(vertexShader, GL_VERTEX_SHADER, VertexShaderSource);
    success = success && initShader(fragmentShader, GL_FRAGMENT_SHADER, FragmentShaderSource);
    if (success) {
        success = success && initProgram(Program, { vertexShader, fragmentShader });
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    if (!success) {
        return false;
    }

    std::vector<float> vertices = {
         0.5f,  0.5f, 0.0f,  1.0f, 0.0f,  // top right
         0.5f, -0.5f, 0.0f,  1.0f, 1.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  0.0f, 1.0f,  // bottom left
        -0.5f,  0.5f, 0.0f,  0.0f, 0.0f,  // top left 
    };

    std::vector<unsigned int> indices = {
        0, 1, 3,
        1, 2, 3,
    };

    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(0 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return true;
}

void Gui::Surface::teardown() {
    glDeleteProgram(Program);
    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);
}

Gui::Surface::Surface(size_t width, size_t height) : m_width(width), m_height(height) {
    m_data = new Palette::Color[m_width * m_height];
    m_texture = createTexture();
}

Gui::Surface::~Surface() {
    freeTexture(m_texture);
    delete[] m_data;
}

void Gui::Surface::setPixel(int x, int y, Palette::Color color) {
    m_data[y * m_width + x] = color;
}

void Gui::Surface::upload() {
    uploadTextureData(m_texture, m_width, m_height, m_data);
}

void Gui::Surface::render() {
    glUseProgram(Program);
    glBindTexture(GL_TEXTURE_2D, m_texture);
    glBindVertexArray(vao);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

Gui::Texture Gui::Surface::getTexture() {
    return m_texture;
}