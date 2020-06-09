#include "util.hpp"

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
