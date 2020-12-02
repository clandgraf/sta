#include "core/gui/manager.hpp"
#include "inputs.hpp"
#include "emu.hpp"
#include "keynames.hpp"

static const char* getKeyName(int scancode) {
    return KEY_NAMES.at(scancode);
}

struct ButtonWidgetIds {
    std::string add;
    std::string clear;
};

static std::map<Input::ControllerDef, ButtonWidgetIds> buttonWidgetIds;

static void renderControllerButtonPane(Input::ControllerDef button) {
    ImGui::Text(Input::defToLabel.at(button).c_str());
    ImGui::SameLine();
    if (ImGui::Button(buttonWidgetIds[button].add.c_str())) {
        Input::waitForInput(button);
    }
    ImGui::SameLine();
    if (ImGui::Button(buttonWidgetIds[button].clear.c_str())) {
        Input::clearButton(button);
    }

    const auto& scancodes = Input::getScancodes(button);
    auto scancode = scancodes.begin();

    if (scancode == scancodes.end()) {
        ImGui::Text("None");
    }
    else {
        std::stringstream ss;
        for (; scancode != scancodes.end(); scancode++) {
            if (scancode != scancodes.begin())
                ss << ", ";
            const char* name = getKeyName(*scancode);
            ss << name;
        }

        ImGui::Text(ss.str().c_str());
    }
}

static void renderControllerSetupPane(const std::vector<Input::ControllerDef>& controller) {
    auto it = controller.begin();
    for (; it != controller.end(); it++) {
        if (it != controller.begin())
            ImGui::Separator();
        renderControllerButtonPane(*it);
    }
}

static bool shouldWaitingForInputClose(Gui::Dialog<Emu>&, Emu&) {
    return Input::getWaitingForInput() == Input::None;
}

static void onWaitingForInputClosed(Gui::Dialog<Emu>&, Emu&) {
    Input::waitForInput(Input::None);
}

static void renderWaitingForInput(Gui::Dialog<Emu>&, Emu&) {
    ImGui::Text("Press a key");
}

static std::shared_ptr<Gui::Dialog<Emu>> waitingForInputDialog;

static void render(Gui::Dialog<Emu>&, Emu& emu) {
    renderControllerSetupPane(Input::inputsGeneral);

    ImGuiTabBarFlags tab_bar_flags = ImGuiTabBarFlags_None;
    if (ImGui::BeginTabBar("Controllers", tab_bar_flags)) {
        if (ImGui::BeginTabItem("Controller 1"))
        {
            renderControllerSetupPane(Input::inputsController0);
            ImGui::EndTabItem();
        }
        if (ImGui::BeginTabItem("Controller 2"))
        {
            renderControllerSetupPane(Input::inputsController1);
            ImGui::EndTabItem();
        }
        ImGui::EndTabBar();
    }

    if (Input::getWaitingForInput() != Input::None) {
        waitingForInputDialog->open();
    }
    
    waitingForInputDialog->render(emu);
}

static void init(Gui::Dialog<Emu>& dialog, Emu&) {
    static char widgetIdBuffer[256];

    for (const auto& label : Input::defToString) {
        std::string addLabel, clearLabel;
        snprintf(widgetIdBuffer, 256, "Add##%s-add", label.second.c_str());
        addLabel = widgetIdBuffer;
        snprintf(widgetIdBuffer, 256, "Clear##%s-clear", label.second.c_str());
        clearLabel = widgetIdBuffer;
        buttonWidgetIds[label.first] = { addLabel, clearLabel };
    }

    waitingForInputDialog = std::make_shared<Gui::Dialog<Emu>>(
        dialog.manager,
        "gui/waiting-for-input",
        "Waiting For Key",
        renderWaitingForInput,
        nullptr,
        nullptr,
        shouldWaitingForInputClose,
        onWaitingForInputClosed
    );
}

static void teardown(Gui::Dialog<Emu>& dialog, Emu&) {
    waitingForInputDialog = nullptr;
}

void createSetupControllers(Gui::Manager<Emu>& manager) {
    auto dialog = manager.dialog(
        "gui/setup-controllers",
        "Setup Controllers",
        render,
        init,
        teardown
    );

    manager.action("File", "Setup Controllers", [=](Emu&){
        dialog->notifyOpen();
    });
}