#pragma once
#include "../Math/Vector2d.hpp"
#include "../Game/Camera.hpp"
#include "../Game/PlayerManager.hpp"
#include "../RenderEngine/Renderer.hpp"
#include "Settings.hpp"
#include "AimbotSettings.hpp"

class Esp
{
private:
    Esp() {}
    ~Esp() {}

    bool isValidTarget(Player* player, const EspSettings& settings) const {
        if (!player || !player->isValid() || player->isDead()
            || !player->isEnemy() || !player->isInRange(settings.getEspRangeInMeters())) {
            return false;
        }
        return true;
    }

    float calculateDynamicSize(float baseSize, float minSize, float distance, float maxDistance) const {
        float size = baseSize * (1.0f - distance / maxDistance);
        return std::max(size, minSize);
    }

    bool getScreenPosition(Vector3d worldPosition, Vector2d& screenPosition) const {
        return Camera::getInstance().worldToScreen(worldPosition, screenPosition);
    }

    void renderTracers(Player* player, const EspSettings& settings, bool visible) const {
        Vector2d headScreenPosition;
        Vector2d originScreenPosition;

        if (!getScreenPosition(player->getHeadPosition(), headScreenPosition) ||
            !getScreenPosition(player->getPosition(), originScreenPosition)) {
            return;
        }

        float height = (headScreenPosition.y - originScreenPosition.y) + 5.0f;
        float width = height * 0.1;

        Vector2d enemyPosition = Vector2d(originScreenPosition.x - width / 2, originScreenPosition.y);
        Vector2d tracersPosition = Vector2d(1920 / 2, 1080);

        Color tracersColor = visible ? settings.tracersColorVisible() : settings.tracersColor();
        Renderer::drawLine(tracersPosition, enemyPosition, tracersColor, settings.tracersThickness());
    }

    void renderFOV(const AimbotSettings& aimbot, const EspSettings& settings) const {
        float fieldOfView = aimbot.getFieldOfView();
        Vector2d centerScreenPosition = Vector2d(1920 / 2, 1080 / 2);
        Renderer::drawFovCircle(centerScreenPosition, fieldOfView * 8, 360.0f, settings.fovColor(), 1.0f);
    }

    void renderEspBox(Player* player, const EspSettings& settings, bool visible) const {
        Vector2d headScreenPosition;
        Vector2d originScreenPosition;

        if (!getScreenPosition(player->getHeadPosition(), headScreenPosition) ||
            !getScreenPosition(player->getPosition(), originScreenPosition)) {
            return;
        }

        float height = (headScreenPosition.y - originScreenPosition.y) + 5.0f;
        float width = height * 0.6;

        Vector2d rectPosition = Vector2d(originScreenPosition.x - width / 2, originScreenPosition.y);
        Vector2d size = Vector2d(width, height);

        Color espBoxColor = visible ? settings.espBoxColorVisible() : settings.espBoxColor();
        Renderer::drawRectangleOutline(rectPosition, size, espBoxColor, 2.0f);
    }

    void renderHealthBar(Player* player, const EspSettings& settings, bool visible) const {
        Vector2d headScreenPosition;
        Vector2d originScreenPosition;

        if (!getScreenPosition(player->getHeadPosition(), headScreenPosition) ||
            !getScreenPosition(player->getPosition(), originScreenPosition)) {
            return;
        }

        float height = 6.0f;
        float width = 40.0f;

        Vector2d rectPosition = Vector2d(originScreenPosition.x - width / 2, headScreenPosition.y - 10.0f);
        Vector2d size = Vector2d(width, height);

        float fill = (float)player->getHealth() / (float)player->getMaxHealth();
        Renderer::drawBorderedFillRectangle(rectPosition, size, Color::lerp(Color(1.0, 0.0, 0.0), Color(0.0, 1.0, 0.0), fill),
            Color(), 2.0f, fill);

        fill = (float)player->getShield() / (float)player->getMaxShield();
        Renderer::drawBorderedFillRectangle(Vector2d(rectPosition.x, rectPosition.y - (height + 3)), size,
            Color::lerp(Color(1.0, 1.0, 1.0), Color(0.0, 0.0, 1.0), fill), Color(), 2.0f, fill);
    }

public:
    static Esp& getInstance() {
        static Esp instance;
        return instance;
    }

    Esp(const Esp&) = delete;
    Esp& operator=(const Esp&) = delete;

    void render() const {
        const EspSettings& settings = Settings::getInstance().getEspSettings();
        const AimbotSettings& aimbot = Settings::getInstance().getAimbotSettings();

        if (settings.drawFov()) {
            renderFOV(aimbot, settings);
        }

        for (auto player : PlayerManager::getInstance()) {
            bool visible = player->isVisible();
            if (!isValidTarget(player, settings)) {
                continue;
            }
            if (settings.isTracersEnabled()) {
                renderTracers(player, settings, visible);
            }
            if (settings.isEspEnabled()) {
                if (settings.isEspBoxEnabled()) {
                    renderEspBox(player, settings, visible);
                }
                if (settings.isEspHpShealdsEnabled()) {
                    renderHealthBar(player, settings, visible);
                }
            }
        }
    }
};
