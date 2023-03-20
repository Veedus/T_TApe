#pragma once
#include "../Game/LocalPlayer.hpp"
#include "Settings.hpp"
#include "../Math/QAngle.hpp"

class Rcs {
private:
    QAngle _previousPunch;

    Rcs() : _previousPunch(QAngle::zero()) {}
    ~Rcs() {}

    void recoilControl(const RcsSettings& settings, QAngle& weponPunch, const QAngle& playerAngle) const {
        QAngle punchValue = weponPunch - _previousPunch;
        punchValue.x *= settings.getVerticalPower();
        punchValue.y *= settings.getHorizontalPower();

        QAngle newAngle = playerAngle - punchValue;
        LocalPlayer::getInstance().setViewAngle(newAngle);
    }

public:

    static Rcs& getInstance() {
        static Rcs instance;
        return instance;
    }

    Rcs(const Rcs&) = delete;
    Rcs& operator=(const Rcs&) = delete;

    void update() {

        if (!LocalPlayer::getInstance().isValid() || LocalPlayer::getInstance().isKnocked() || LocalPlayer::getInstance().isDead()) {
            _previousPunch = QAngle(0, 0);
            return;
        }

        const RcsSettings& settings = Settings::getInstance().getRcsSettings();
        QAngle weponPunch = LocalPlayer::getInstance().getWeaponPunch();

        if (LocalPlayer::getInstance().isInAttack() && settings.isEnabled()) {
            QAngle playerAngle = LocalPlayer::getInstance().getViewAngle();
            recoilControl(settings, weponPunch, playerAngle);
        }

        _previousPunch = weponPunch;
    }
};
