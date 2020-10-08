#include <json.hpp>

#include "util.hpp"
#include "recents.hpp"

namespace fs = std::filesystem;
using Recents = Util::Recents;

void Util::addRecent(Recents& recents, const fs::path& p) {
    recents.remove(p);
    recents.push_front(p);
    while (recents.size() > 10) {
        recents.pop_back();
    }
}

Recents Util::loadRecentFiles(const std::string& key) {
    Recents recents;
    
    nlohmann::json recentsJson = Settings::object[key];
    if (recentsJson.is_null()) {
        return recents;
    }
    if (!recentsJson.is_array()) {
        LOG_ERR << "Configuration Error: recentFiles should be array!\n";
        return recents;
    }

    for (auto entry : recentsJson) {
        if (!entry.is_string()) {
            LOG_ERR << "Configuration Error: recentFiles entry should be string\n";
            continue;
        }

        std::string s = entry;
        const fs::path path{ s };
        if (fs::exists(path)) {
            addRecent(recents, path);
        }
    }
    return recents;
}

void Util::writeRecentFiles(Recents& recents, const std::string& key) {
    Settings::object[key] = std::vector<std::string>();
    for (auto it = recents.rbegin(); it != recents.rend(); it++) {
        Settings::object["recentFiles"].push_back(it->string());
    }
}
