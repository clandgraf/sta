#include <imgui.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "gui_filebrowser.hpp"

namespace fs = std::filesystem;
static fs::path fileBrowserCurrentDir;
static char fileBrowserCurrentDirCstr[0xff];
static std::vector<fs::path> files;
static std::vector<const char *> filesCstr;
static int fileBrowserSelection = 0;

void updatePath() {
    for (const auto& cstr : filesCstr) {
        free((void*)cstr);
    }
    filesCstr.clear();
    files.clear();
    for (const auto& entry : fs::directory_iterator(fileBrowserCurrentDir)) {
        files.push_back(entry.path());
    }

    for (const auto& entry : files) {
        filesCstr.push_back(_strdup(entry.filename().string().c_str()));
    }
}

void updatePathFromPath(const fs::path& p) {
    fileBrowserCurrentDir = p;
    strncpy(fileBrowserCurrentDirCstr, fileBrowserCurrentDir.string().c_str(), 0xff - 1);
    updatePath();
}

bool ImGui_FileBrowser(fs::path& selectedFile) {
    if (ImGui::Button("Up")) {
        updatePathFromPath(fileBrowserCurrentDir.parent_path());
    }
    ImGui::SameLine(); 
    ImGui::InputText("##Path", fileBrowserCurrentDirCstr, IM_ARRAYSIZE(fileBrowserCurrentDirCstr));

    if (ImGui::ListBox("##Files", &fileBrowserSelection, filesCstr.data(), filesCstr.size(), 10)) {
        fs::path selectedPath = files[fileBrowserSelection];
        if (fs::is_directory(selectedPath)) {
            updatePathFromPath(selectedPath);
        } else {
            selectedFile = selectedPath;
            return true;
        }
    }

    return false;
}

void ImGui_FileBrowser_Init() {
#if defined(_WIN32) || defined(WIN32) 
    char* base = getenv("USERPROFILE");
#else
    char* base = getenv("HOME");
#endif

    fileBrowserCurrentDir = base;
    strncpy(fileBrowserCurrentDirCstr, base, 0xff - 1);
    updatePath();
}
