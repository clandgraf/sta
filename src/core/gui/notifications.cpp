#include <string>
#include <vector>
#include <imgui.h>

#include "core/gui/notifications.hpp"

namespace Gui {
    struct Notification {
        std::string m_message;
        unsigned int m_time;

        Notification(const std::string& message)
            : m_message(message)
            , m_time(150) {}
    };

    struct {
    private:
        std::vector<Notification> notifications;
    public:
        void add(const std::string& message) {
            notifications.push_back(message);
            while (notifications.size() > 5) {
                notifications.pop_back();
            }
        }

        void update() {
            for (auto& n : notifications) {
                --n.m_time;
            }
            notifications.erase(
                std::remove_if(notifications.begin(), notifications.end(),
                    [](const auto& n) { return n.m_time == 0; }),
                notifications.end());
        }

        void render() {
            update();

            if (notifications.size() == 0) {
                return;
            }
            const float DISTANCE = 10.0f;
            ImGuiIO& io = ImGui::GetIO();
            ImVec2 window_pos = ImVec2(DISTANCE, io.DisplaySize.y - DISTANCE);
            ImVec2 window_pos_pivot = ImVec2(0.0f, 1.0f);
            ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
            ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDecoration
                | ImGuiWindowFlags_AlwaysAutoResize
                | ImGuiWindowFlags_NoSavedSettings
                | ImGuiWindowFlags_NoFocusOnAppearing
                | ImGuiWindowFlags_NoNav
                | ImGuiWindowFlags_NoInputs
                | ImGuiWindowFlags_NoBackground;

            if (ImGui::Begin("Notifications", nullptr, windowFlags)) {
                for (const auto& n : notifications) {
                    ImGui::Text(n.m_message.c_str());
                }
            }
            ImGui::End();
        }
    } Notifications;
}

void Gui::addNotification(const std::string& message) {
    Gui::Notifications.add(message);
}

void Gui::updateNotifications() {
    Gui::Notifications.update();
}

void Gui::renderNotifications() {
    Gui::Notifications.render();
}