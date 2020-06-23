#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <json.hpp>

#define LOG_ERR (std::cerr << "[ERROR] "<< __FILE__ << ":" << __LINE__ << " ")
#define LOG_MSG (std::cerr << "[INFO]  "<< __FILE__ << ":" << __LINE__ << " ")

namespace CliArguments {
    bool flag(int ac, char** av, const char* param);
    char* value(int ac, char** av, const char* param);
}

template<class T>
auto operator<<(std::ostream& os, T const& t) -> decltype(t.print(os), os)
{
    t.print(os);
    return os;
}

namespace StreamManipulators {
    class HexOutput {
    private:
        int m_value;
        int m_width;
    public:
        HexOutput(int value, int width);
        void print(std::ostream&) const;
    };

    HexOutput hex(uint8_t v);
    HexOutput hex(uint16_t v);
}

uint8_t* readFile(std::ifstream& file, size_t* len = nullptr);

uint8_t* readFile(const char* path);

uint8_t* readFile(const std::filesystem::path& path);

namespace Settings {
    extern nlohmann::json object;

    template<typename V>
    V get(const char* str, const V& value) {
        return object.value(str, value);
    }

    template<typename V>
    void set(const char* str, const V& value) {
        object[str] = value;
    }

    void read();
    void write();
}
