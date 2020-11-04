#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <json.hpp>

#define LOG_ERR (std::cerr << "[ERROR] "<< __FILE__ << ":" << std::dec << __LINE__ << " ")
#define LOG_MSG (std::cerr << "[INFO]  "<< __FILE__ << ":" << std::dec << __LINE__ << " ")

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

class TriStateBool {
    enum class Value {
        Undefined = -1,
        False = false,
        True = true
    };

    Value m_value = Value::Undefined;

public:
    void operator=(bool& v);
    void reset();
    bool isUndefined() const;
    void set(bool value = true, bool defined = true);
    operator bool() const;
};

namespace Settings {
    extern nlohmann::json object;

    template<typename V>
    V get(const char* str) {
        return object.value(str);
    }

    template<typename V>
    V get(const char* str, const V& value) {
        return object.value(str, value);
    }

    template<typename V>
    void set(const char* str, const V& value) {
        object[str] = value;
    }

    template<typename V>
    void setIn(std::vector<std::string> path, const V& value) {
        std::stringstream ss;
        for (auto s: path)
            ss << "/" << s;

        object[nlohmann::json::json_pointer(ss.str())] = value;
    }

    void erase(const char* str);

    void read();
    void write();
}
