#include <imgui.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "gui_filebrowser.hpp"

static const int BUFFER_LEN = 0xff;

namespace fs = std::filesystem;
static fs::path fileBrowserCurrentDir;
static char fileBrowserInputBuffer[BUFFER_LEN];
static std::vector<fs::path> files;
static std::vector<const char *> filesCstr;
static int fileBrowserSelection = 0;

static void updateChildren() {
    strncpy(fileBrowserInputBuffer, fileBrowserCurrentDir.string().c_str(), BUFFER_LEN - 1);

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

void updateFromPath(const fs::path& p) {
    fileBrowserCurrentDir = p;
    updateChildren();
}

void updateFromString(const char* p) {
    fileBrowserCurrentDir = p;
    updateChildren();
}

bool ImGui_FileBrowser(fs::path& selectedFile) {
    if (ImGui::Button("Up")) {
        updateFromPath(fileBrowserCurrentDir.parent_path());
    }
    ImGui::SameLine(); 
    if (ImGui::InputText("##Path", fileBrowserInputBuffer, IM_ARRAYSIZE(fileBrowserInputBuffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
        fs::path tmpPath = fileBrowserInputBuffer;
        if (fs::exists(tmpPath)) {
            if (fs::is_directory(tmpPath)) {
                updateFromPath(tmpPath);
            } else {
                selectedFile = tmpPath;
                return true;
            }
        } else {
            updateFromPath(fileBrowserCurrentDir);
        }
    }

    ImGui::PushItemWidth(-1);
    if (ImGui::ListBox("##Files", &fileBrowserSelection, filesCstr.data(), filesCstr.size(), 10)) {
        fs::path selectedPath = files[fileBrowserSelection];
        if (fs::is_directory(selectedPath)) {
            updateFromPath(selectedPath);
        } else {
            selectedFile = selectedPath;
            return true;
        }
    }
    ImGui::PopItemWidth();

    return false;
}

void ImGui_FileBrowser_Init() {
#if defined(_WIN32) || defined(WIN32) 
    char* base = getenv("USERPROFILE");
#else
    char* base = getenv("HOME");
#endif

    updateFromString(base);
}
