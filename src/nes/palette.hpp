#pragma once

#include <cstdint>
#include <memory>
#include <filesystem>
#include <array>

struct Palette {
    struct Color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
    };

    static std::shared_ptr<Palette> fromFile(const std::filesystem::path& p);

    static const Palette DEFAULT;

    Palette(std::array<Color, 64>);

    Color operator[](unsigned int v) const;

private:
    std::array<Color, 64> m_data;
};

