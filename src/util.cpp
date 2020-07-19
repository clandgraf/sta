#include "util.hpp"

#include <cstring>

std::shared_ptr<Palette> Palette::fromFile(const std::filesystem::path& p) {
    return nullptr;
}

const Palette Palette::DEFAULT{ 
    std::array<Palette::Color, 64>{{
        {  84,  84,  84 },
        {   0,  30, 116 },
        {   8,  16, 144 },
        {  48,   0, 136 },
        {  68,   0, 100 },
        {  92,   0,  48 },
        {  84,   4,   0 },
        {  60,  24,   0 },
        {  32,  42,   0 },
        {   8,  58,   0 },
        {   0,  64,   0 },
        {   0,  60,   0 },
        {   0,  50,  60 },
        {   0,   0,   0 },
        {   0,   0,   0 },
        {   0,   0,   0 },

        { 152, 150, 152 },
        {   8,  76, 196 },
        {  48,  50, 236 },
        {  92,  30, 228 },
        { 136,  20, 176 },
        { 160,  20, 100 },
        { 152,  34,  32 },
        { 120,  60,   0 },
        {  84,  90,   0 },
        {  40, 114,   0 },
        {   8, 124,   0 },
        {   0, 118,  40 },
        {   0, 102, 120 },
        {   0,   0,   0 },
        {   0,   0,   0 },
        {   0,   0,   0 },

        { 236, 238, 236 },
        {  76, 154, 236 },
        { 120, 124, 236 },
        { 176,  98, 236 },
        { 228,  84, 236 },
        { 236,  88, 180 },
        { 236, 106, 100 },
        { 212, 136,  32 },
        { 160, 170,   0 },
        { 116, 196,   0 },
        {  76, 208,  32 },
        {  56, 204, 108 },
        {  56, 180, 204 },
        {  60,  60,  60 },
        {   0,   0,   0 },
        {   0,   0,   0 },

        { 236, 238, 236 },
        { 168, 204, 236 },
        { 188, 188, 236 },
        { 212, 178, 236 },
        { 236, 174, 236 },
        { 236, 174, 212 },
        { 236, 180, 176 },
        { 228, 196, 144 },
        { 204, 210, 120 },
        { 180, 222, 120 },
        { 168, 226, 144 },
        { 152, 226, 180 },
        { 160, 214, 228 },
        { 160, 162, 160 },
        {   0,   0,   0 },
        {   0,   0,   0 },
    }}
};

Palette::Palette(std::array<Color, 64> colors) : m_data(colors) {}

Palette::Color Palette::operator[](unsigned int v) const {
    return m_data[v];
}

static int _cliIndex(int ac, char** av, const char* param) {
    for (int i = 1; i < ac; i++) {
        if (strcmp(av[i], param) == 0) {
            return i;
        }
    }

    return 0;
}

bool CliArguments::flag(int ac, char** av, const char* param) {
    return _cliIndex(ac, av, param) > 0;
}

char* CliArguments::value(int ac, char** av, const char* param) {
    int i = _cliIndex(ac, av, param) + 1;
    if (i < ac && i > 1) {
        return av[i];
    }

    return nullptr;
}

uint8_t* readFile(std::ifstream& file, size_t* length) {
    file.seekg(0, file.end);
    size_t len = file.tellg();
    file.seekg(0, file.beg);

    uint8_t* data = new uint8_t[len];
    file.read((char*)(data), len);

    if (length) {
        *length = len;
    }
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

namespace sm = StreamManipulators;

sm::HexOutput::HexOutput(int value, int width) 
    : m_value(value), m_width(width) {}


void sm::HexOutput::print(std::ostream& out) const {
    out << "0x" 
        << std::hex << std::setw(m_width) << std::setfill('0')
        << m_value;
}

sm::HexOutput sm::hex(uint8_t value) {
    return {value, 2};
}

sm::HexOutput sm::hex(uint16_t value) {
    return {value, 4};
}

using json = nlohmann::json;

json Settings::object;

void Settings::read() {
    std::ifstream settingsFile("settings.json");
    if (!settingsFile.is_open()) {
        LOG_MSG << "settings.json could not be opened.\n";
        object = json::object();
        return;
    }

    object = json::parse(settingsFile);
    settingsFile.close();
}

void Settings::write() {
    std::ofstream settingsFile("settings.json");
    if (!settingsFile.is_open()) {
        LOG_MSG << "settings.json could not be opened.\n";
        return;
    }
    settingsFile << object;
    settingsFile.close();
}

void Settings::erase(const char* str) {
    if (object.contains(str)) {
        object.erase(str);
    }
}
