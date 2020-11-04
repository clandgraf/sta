#pragma once

namespace Gui {
    struct Window { 
        void* _;

        bool operator!() {
            return !_;
        }
    };

    Window initWindow(const char* title, bool fullscreen);
    void setWindowClosing(Window window, bool closing);
    bool isWindowClosing(Window window);
    bool isEscapePressed();
    void pollEvents();
    bool isFullscreen(Window window);
    void swapBuffers(Window window);
    void toggleFullscreen(Window window);
    void setTitle(Window window, const char* title);
    void teardownWindow(Window window);

    void ImGui_Impl_NewFrame();
    void ImGui_Impl_RenderDrawData(ImDrawData* draw_data);
    void ImGui_Impl_Init(Window window);
    void ImGui_Impl_Shutdown();
}
