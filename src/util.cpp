#include "util.hpp"

#include <cstring>

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
