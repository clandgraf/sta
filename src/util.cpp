#include "util.hpp"

uint8_t* readFile(std::ifstream& file) {
    file.seekg(0, file.end);
    size_t length = file.tellg();
    file.seekg(0, file.beg);

    uint8_t* data = new uint8_t[length];
    file.read((char*)(data), length);

    return data;
}

uint8_t* readFile(const char* path) {
    std::ifstream file(path, std::ios::binary);
    uint8_t* data = readFile(file);
    file.close();
    return data;
}

uint8_t* readFile(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);
    uint8_t* data = readFile(file);
    file.close();
    return data;
}
