#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>

uint8_t* readFile(std::ifstream& file, size_t* len = nullptr);

uint8_t* readFile(const char* path);

uint8_t* readFile(const std::filesystem::path& path);
