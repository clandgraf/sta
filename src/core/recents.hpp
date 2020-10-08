#pragma once

#include <filesystem>
#include <list>

namespace Util {
    using Recents = std::list<std::filesystem::path>;
    
    void addRecent(Recents&, const std::filesystem::path&);
    Recents loadRecentFiles(const std::string& key);
    void writeRecentFiles(Recents&, const std::string& key);
}
