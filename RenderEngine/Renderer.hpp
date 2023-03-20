#pragma once

#include <GL/gl.h>
#include "../imgui/imgui.h"
#include <algorithm>
#include "../Math/Color.hpp"
#include "../Math/Vector2d.hpp"

class Renderer {
private:
    static Vector2d calculatePointOnCircle(const Vector2d& center, float radius, float angle) {
        float x = center.x + radius * cosf(angle * M_PI / 180.0f);
        float y = center.y + radius * sinf(angle * M_PI / 180.0f);
        return Vector2d(x, y);
    }

    static void saveGLState() {
        glPushAttrib(GL_LINE_BIT | GL_CURRENT_BIT);
    }

    static void restoreGLState() {
        glPopAttrib();
    }
public:

  static void drawLine(const Vector2d& start, const Vector2d& end, const Color& color, const float& width) {
      glLineWidth(width);
      glColor3f(color.r, color.g, color.b);

      glBegin(GL_LINES);
          glVertex2f(start.x, start.y);
          glVertex2f(end.x, end.y);
      glEnd();

      glLineWidth(1.0f);
  }

    static void drawRectangleOutline(const Vector2d& position, const Vector2d& size, const Color& color, const float& lineWidth) {

        glLineWidth(lineWidth);
        glColor3f(color.r, color.g, color.b);

        glBegin(GL_LINE_LOOP);
            glVertex2f(position.x, position.y);
            glVertex2f(position.x + size.x, position.y);
            glVertex2f(position.x + size.x, position.y + size.y);
            glVertex2f(position.x, position.y + size.y);
        glEnd();

        glLineWidth(1.0f);
    }

    static void drawFilledRectagle(const Vector2d& position, const Vector2d& size, const Color& color) {

        glColor3f(color.r, color.g, color.b);
        glBegin(GL_QUADS);
            glVertex2f(position.x, position.y);
            glVertex2f(position.x + size.x, position.y);
            glVertex2f(position.x + size.x, position.y + size.y);
            glVertex2f(position.x, position.y + size.y);
        glEnd();
    }

    static void drawBorderedFillRectangle(const Vector2d& position, const Vector2d& size, const Color& fillColor, const Color& borderColor, const float& lineWidth, const float& fill) {
        drawFilledRectagle(position, Vector2d(size.x, size.y), Color(0, 0, 0));
        drawFilledRectagle(position, Vector2d(size.x * fill, size.y), fillColor);
        drawRectangleOutline(position, size, borderColor, lineWidth);
    }

    static bool renderImguiFloatValue(const std::string& label, const std::string& id, float* value, const float& min, const float& max, const float& slowStep, const float& fastStep) {
        const char* format = "%.3f";

        std::string labelId = label + "##" + id;
        bool sliderResult = ImGui::SliderFloat(labelId.c_str(), value, min, max, format);

        ImGui::SameLine();

        ImGui::PushItemWidth(100);
        std::string inputId = "##_" + id + "_" + id + label;
        bool inputResult = ImGui::InputFloat(inputId.c_str(), value, slowStep, fastStep, format);
        ImGui::PopItemWidth();

        if(inputResult) {
            *value = std::clamp(*value, min, max);
        }

        return sliderResult || inputResult;
    }

    static bool renderImguiIntValue(const std::string& label, const std::string& id, int* value, const int& min, const int& max, const int& slowStep, const int& fastStep) {

        std::string labelId = label + "##" + id;
        bool sliderResult = ImGui::SliderInt(labelId.c_str(), value, min, max);

        ImGui::SameLine();

        ImGui::PushItemWidth(100);
        std::string inputId = "##_" + id + "_" + id + label;
        bool inputResult = ImGui::InputInt(inputId.c_str(), value, slowStep, fastStep);
        ImGui::PopItemWidth();

        if(inputResult) {
            *value = std::clamp(*value, min, max);
        }

        return sliderResult || inputResult;
    }

    static void renderImguiColorValue(const std::string& label, const std::string& id, Color& color) {

        ImVec4 imColor = ImVec4(color.r, color.g, color.b, 1.0f);
        std::string labelId = label + "##" + id;
        std::string popoutId = label + "##popout_" + id;

        ImGui::Text("%s", label.c_str());
        ImGui::SameLine();
        if (ImGui::ColorButton(labelId.c_str(), imColor)) {
            ImGui::OpenPopup(popoutId.c_str());
        }

        if (ImGui::BeginPopup(popoutId.c_str())) {
            std::string pickerId = "##" + label + "_picker_" + id;

            if(ImGui::ColorPicker3(pickerId.c_str(), (float*)&imColor)) {
                color.r = imColor.x;
                color.g = imColor.y;
                color.b = imColor.z;
                color.clamp();
            }
            ImGui::EndPopup();
        }
    }

    static void drawFovCircle(const Vector2d& center, const float& radius, const float& fov, const Color& color, const float& lineWidth) {
    int numSegments = 100;
    float segmentAngle = fov / static_cast<float>(numSegments - 1);
    float startAngle = -fov / 2.0f;

    saveGLState();
    glLineWidth(lineWidth);
    glColor3f(color.r, color.g, color.b);

    glBegin(GL_LINE_STRIP);
        for(int i = 0; i < numSegments; i++) {
            float angle = startAngle + i * segmentAngle;
            Vector2d point = calculatePointOnCircle(center, radius, angle);
            glVertex2f(point.x, point.y);
        }
    glEnd();

    restoreGLState();
}
};
