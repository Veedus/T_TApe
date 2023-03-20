#pragma once
#include <cmath>
#include "Vector3d.hpp"
#include "QAngle.hpp"

class AimResolver
{
private:

public:

    static QAngle calculateAngle(const Vector3d& from, const Vector3d& to) {
        Vector3d newDirection = to - from;
        newDirection.normalize();

        float pitch = -asinf(newDirection.z) * (180 / M_PI);
        float yaw = atan2f(newDirection.y, newDirection.x) * (180 / M_PI);

        return QAngle(pitch, yaw);
    }

    static Vector3d getTargetPosition(const Vector3d& targetPosition, const Vector3d& targetVelocity, const float time) {
        return targetPosition + (targetVelocity * time);
    }

    static float getTimeToTarget(const Vector3d& startPosition, const Vector3d& endPosition, const float bulletSpeed) {
        const float distance = (endPosition - startPosition).length();
        return distance / bulletSpeed;
    }

    static float getBasicBulletDrop(const Vector3d& startPosition, const Vector3d& endPosition, const float bulletSpeed, const float bulletDropRate) {
        const float time = getTimeToTarget(startPosition, endPosition, bulletSpeed);
        const float bulletDropRateScaled = bulletDropRate * 750.0f;
        return 0.5f * bulletDropRateScaled * time * time;
    }

    static Vector3d getTargetPosition(const Vector3d& startPosition, const Vector3d& endPosition, const Vector3d& targetVelocity, const float bulletSpeed) {
        const float time = getTimeToTarget(startPosition, endPosition, bulletSpeed);
        return getTargetPosition(endPosition, targetVelocity, time);
    }

    static bool calculateAimRotation(const Vector3d& startPosition, const Vector3d& endPosition, const Vector3d& targetVelocity, const float bulletSpeed, QAngle& result) {
        const Vector3d endPositionAdjusted = getTargetPosition(startPosition, endPosition, targetVelocity, bulletSpeed);
        result = calculateAngle(startPosition, endPositionAdjusted);
        return true;
    }

    static void calculateAimRotationBasicDrop(const Vector3d& startPosition, const Vector3d& endPosition, const Vector3d& targetVelocity, const float bulletSpeed, const float bulletScale, QAngle& result) {
        const float bulletDrop = getBasicBulletDrop(startPosition, endPosition, bulletSpeed, bulletScale);
        const Vector3d endPositionAdjusted(endPosition.x, endPosition.y, endPosition.z + bulletDrop);
        result = calculateAngle(startPosition, endPositionAdjusted);
    }

    static bool calculateAimRotationNew(const Vector3d& start, const Vector3d& targetPosition, const Vector3d& targetVelocity, const float bulletSpeed, const float bulletScale, const int steps, QAngle& result) {
        const float gravity = 750.0f / bulletScale;

        float angle = 0;
        float time = 0.0;
        const float timeStep = 1.0f / steps;

        for(int i = 0; i < steps; i++) {
            const Vector3d predictedPosition = getTargetPosition(targetPosition, targetVelocity, time);
            if(!optimalAngle(start, predictedPosition, bulletSpeed, gravity, angle)){
                continue;
            }

            const Vector3d direction = predictedPosition - start;
            const float horizontalDistance = direction.length2D();
            const float travelTime = horizontalDistance / (bulletSpeed * cosf(angle));

            if(travelTime <= time){
                result.x = -angle  * (180 / M_PI);
                result.y = atan2f(direction.y, direction.x)  * (180 / M_PI);
                return true;
            }

            time += timeStep;
        }

        const Vector3d targetPositionAdjusted = getTargetPosition(start, targetPosition, targetVelocity, bulletSpeed);
        result = calculateAngle(start, targetPositionAdjusted);

        return true;
    }

    static float getTimeToTarget(const Vector3d& start, const Vector3d& end, const float speed, const float gravity) {

        const float horizontalDistance = start.distance2DTo(end);
        const float heightDifference = end.z - start.z;

        return (speed * sqrtf(2 * sinf(M_PI / 4) / gravity)) + (sqrtf(2 * sinf(M_PI / 4) * (sinf(M_PI / 4) - 2 * heightDifference) / gravity));
    }

    static bool optimalAngle(const Vector3d& start, const Vector3d& end, const float speed, const float gravity, float& angle) {

        const float horizontalDistance = start.distance2DTo(end);
        const float heightDifference = end.z - start.z;

        const float root = powf(speed, 4) - gravity * (gravity * powf(horizontalDistance, 2) + 2 * heightDifference * powf(speed, 2));

        if(root < 0.0) {
            return false;
        }

        const float term1 = powf(speed, 2) - sqrt(root);
        const float term2 = gravity * horizontalDistance;

        angle = atanf(term1 / term2);

        return true;
    }

    static bool optimalAngle2(const Vector3d start, const Vector3d end, const float speed, const float gravity, float& angle) {
        const float horizontalDistance = start.distance2DTo(end);
        const float heightDifference = end.z - start.z;
        const float time = horizontalDistance / (speed * cosf(atanf((heightDifference)/horizontalDistance)));
        angle = asinf((1.0f/2.0f)*(gravity*powf(time,2))/speed);
        //float maxHeight = speed*powf(sinf(angle),2)/(2*gravity);
        return true;
    }

    static bool optimalAngle3(const Vector3d& start, const Vector3d& end, const float speed, const float gravity, float& angle) {
        const float horizontalDistance = start.distance2DTo(end);
        const float heightDifference = end.z - start.z;

        const float vSqr = speed * speed;
        const float gSqr = gravity * gravity;
        const float root = vSqr * vSqr - gSqr * (gSqr * horizontalDistance * horizontalDistance + 2 * heightDifference * vSqr);
        if(root < 0.0f) {
            return false;
        }

        const float sqrtRoot = sqrtf(root);
        const float angle1 = atanf((vSqr - sqrtRoot) / (gravity * horizontalDistance));

        if (!std::isnan(angle1) && angle1 >= 0 && angle1 <= M_PI / 2) {
            angle = angle1;
            return true;
        }

        const float angle2 = atanf((vSqr + sqrtRoot) / (gravity * horizontalDistance));

        if (!std::isnan(angle2) && angle2 >= 0 && angle2 <= M_PI / 2) {
            angle = angle2;
            return true;
        }

        return false;
    }
}; 
