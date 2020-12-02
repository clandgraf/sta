#pragma once

namespace Gui {
    struct WindowHandle { 
        void* _;

        bool operator!() {
            return !_;
        }
    };

    WindowHandle initWindow(const char* title, bool fullscreen);
    void setWindowClosing(WindowHandle window, bool closing);
    bool isWindowClosing(WindowHandle window);
    bool isEscapePressed();
    void pollEvents();
    bool isFullscreen(WindowHandle window);
    void swapBuffers(WindowHandle window);
    void toggleFullscreen(WindowHandle window);
    void setTitle(WindowHandle window, const char* title);
    void teardownWindow(WindowHandle window);

    void ImGui_Impl_NewFrame();
    void ImGui_Impl_RenderDrawData(ImDrawData* draw_data);
    void ImGui_Impl_Init(WindowHandle window);
    void ImGui_Impl_Shutdown();
}
