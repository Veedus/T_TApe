#pragma once
#include "../imgui/imgui.h"
#include "../Utils/SettingsContext.hpp"
#include "../RenderEngine/Renderer.hpp"

struct RcsSettings
{
private:
    static constexpr const char* enabledId = "rcs.enabled";
    static constexpr const char* verticalPowerId = "rcs.verticalPower";
    static constexpr const char* horizontalPowerId = "rcs.horizontalPower";

    bool _enabled = false;
    float _verticalPower = 0.5f;
    float _horizontalPower = 0.5f;

public:
    bool isEnabled() const {
        return _enabled;
    }

    float getVerticalPower() const {
        return _verticalPower;
    }

    float getHorizontalPower() const {
        return _horizontalPower;
    }

    void render() {
        if (ImGui::BeginTabItem("RCS Settings")) {

            ImGui::Checkbox("Enabled##RCS", &_enabled);

            Renderer::renderImguiFloatValue("Vertical power", "RCS", &_verticalPower, 0.0f, 1.0f, 0.01f, 0.1f);
            Renderer::renderImguiFloatValue("Horizontal power", "RCS", &_horizontalPower, 0.0f, 1.0f, 0.01f, 0.1f);

            ImGui::EndTabItem();
        }
    }

    void load(const SettingsContext& settingsContext) {
        settingsContext.loadBool(enabledId, _enabled);
        settingsContext.loadFloat(verticalPowerId, _verticalPower);
        settingsContext.loadFloat(horizontalPowerId, _horizontalPower);
    }

    void save(SettingsContext& settingsContext) const {
        settingsContext.set(enabledId, _enabled);
        settingsContext.set(verticalPowerId, _verticalPower);
        settingsContext.set(horizontalPowerId, _horizontalPower);
    }
};
