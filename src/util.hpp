#pragma once

#include <cstdint>
#include <fstream>
#include <filesystem>
#include <iostream>

#define LOG_ERR (std::cerr << "[ERROR] "<< __FILE__ << ":" << __LINE__ << " ")
#define LOG_MSG (std::cerr << "[INFO]  "<< __FILE__ << ":" << __LINE__ << " ")

uint8_t* readFile(std::ifstream& file, size_t* len = nullptr);

uint8_t* readFile(const char* path);

uint8_t* readFile(const std::filesystem::path& path);
