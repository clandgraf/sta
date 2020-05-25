#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>

uint8_t* readFile(std::ifstream& file);

uint8_t* readFile(const char* path);

uint8_t* readFile(const std::filesystem::path& path);
