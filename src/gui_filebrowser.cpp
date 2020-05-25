#include <imgui.h>
#include <cstring>
#include <cstdlib>
#include <vector>
#include "gui_filebrowser.hpp"

namespace fs = std::filesystem;

static char fileBrowserPath[0xff];
static char fileBrowserBuffer[0xff];

static std::vector<fs::path> files;
static std::vector<const char *> filesCstr;
static int fileBrowserSelection = 0;

bool ImGui_FileBrowser(fs::path& selectedFile) {
    ImGui::Button("Up");
    ImGui::SameLine(); 
    ImGui::InputText("##Path", fileBrowserBuffer, IM_ARRAYSIZE(fileBrowserBuffer));

    if (ImGui::ListBox("##Files", &fileBrowserSelection, filesCstr.data(), filesCstr.size(), 10)) {
        fs::path selectedPath = files[fileBrowserSelection];
        if (fs::is_directory(selectedPath)) {
            
        } else {
            selectedFile = selectedPath;
            return true;
        }
    }

    return false;
}

void updatePath() {
    fs::path p(fileBrowserBuffer);
    if (fs::is_directory(p)) {
        strncpy(fileBrowserPath, fileBrowserBuffer, 0xff - 1);
        for (const auto& cstr: filesCstr) {
            free((void*)cstr);
        }
        filesCstr.clear();
        files.clear();
        for (const auto& entry : fs::directory_iterator(p)) {
            files.push_back(entry.path());
        }

        for (const auto& entry : files) {
            filesCstr.push_back(_strdup(entry.filename().string().c_str()));
        }
    }
    else {
        // Restore last valid path
        strncpy(fileBrowserBuffer, fileBrowserPath, 0xff - 1);
    }
}

void ImGui_FileBrowser_Init() {
#if defined(_WIN32) || defined(WIN32) 
    char* base = getenv("USERPROFILE");
#else
    char* base = getenv("HOME");
#endif

    strncpy(fileBrowserBuffer, base, 0xff - 1);
    updatePath();
}
