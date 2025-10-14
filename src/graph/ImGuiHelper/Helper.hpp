#pragma once
#ifndef IMGUI_HELPER_HPP
#define IMGUI_HELPER_HPP

#include <imgui.h>
#include <imgui_internal.h>
#include <nlohmann/json.hpp>

// Helper additions for ImGui small math utilities and drawing icons.
// - Fixes missing ImVec2 operator overloads (convenience only)
// - Provides JSON helpers and a DrawHelper class with a single static DrawIcon

namespace editor
{
    enum class IconType : ImU32 { Flow, Circle, Square, Grid, RoundSquare, Diamond };

    // Add small ImVec2 operator helpers if the ImGui version in use doesn't provide them.
    inline ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x - b.x, a.y - b.y); }
    inline ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2(a.x + b.x, a.y + b.y); }
    inline ImVec2 operator*(const ImVec2& a, float s) { return ImVec2(a.x * s, a.y * s); }
    inline ImVec2 operator*(float s, const ImVec2& a) { return ImVec2(a.x * s, a.y * s); }

    namespace draw
    {
        class DrawHelper
        {
        public:
            // JSON helpers: take const-ref to avoid unnecessary copies.
            static ImColor createColorFromJson(const nlohmann::json& json)
            {
                // Expecting array [r,g,b,a] with floats (0..1)
                return ImColor(
                    json.at(0).get<float>(),
                    json.at(1).get<float>(),
                    json.at(2).get<float>(),
                    json.at(3).get<float>());
            }

            static ImVec2 createVec2FromJson(const nlohmann::json& json)
            {
                // Expecting object {"x":..., "y":...}
                return ImVec2(json.at("x").get<float>(), json.at("y").get<float>());
            }

            // Draw an icon into an ImDrawList inside rect [a,b].
            // - color and innerColor are ImU32 RGBA packed values.
            static void DrawIcon(ImDrawList* drawList, const ImVec2& a, const ImVec2& b, IconType type, bool filled, ImU32 color, ImU32 innerColor)
            {
                if (!drawList)
                    return;

                const ImRect rect(a, b);
                const float rect_x = rect.Min.x;
                const float rect_y = rect.Min.y;
                const float rect_w = rect.Max.x - rect.Min.x;
                const float rect_h = rect.Max.y - rect.Min.y;
                const ImVec2 rect_center((rect.Min.x + rect.Max.x) * 0.5f, (rect.Min.y + rect.Max.y) * 0.5f);
                const float outline_scale = rect_w / 24.0f;
                const int extra_segments = static_cast<int>(2 * outline_scale); // for full circle

                if (type == IconType::Flow)
                {
                    const float origin_scale = rect_w / 24.0f;
                    const float offset_x = 1.0f * origin_scale;
                    const float offset_y = 0.0f * origin_scale;
                    const float margin = (filled ? 2.0f : 2.0f) * origin_scale;
                    const float rounding = 0.1f * origin_scale;
                    const float tip_round = 0.7f; // percentage of triangle edge (for tip)

                    const ImRect canvas(
                        rect.Min.x + margin + offset_x,
                        rect.Min.y + margin + offset_y,
                        rect.Max.x - margin + offset_x,
                        rect.Max.y - margin + offset_y);

                    const float canvas_x = canvas.Min.x;
                    const float canvas_y = canvas.Min.y;
                    const float canvas_w = canvas.Max.x - canvas.Min.x;
                    const float canvas_h = canvas.Max.y - canvas.Min.y;

                    const float left = canvas_x + canvas_w * 0.5f * 0.3f;
                    const float right = canvas_x + canvas_w - canvas_w * 0.5f * 0.3f;
                    const float top = canvas_y + canvas_h * 0.5f * 0.2f;
                    const float bottom = canvas_y + canvas_h - canvas_h * 0.5f * 0.2f;
                    const float center_y = (top + bottom) * 0.5f;

                    const ImVec2 tip_top(canvas_x + canvas_w * 0.5f, top);
                    const ImVec2 tip_right(right, center_y);
                    const ImVec2 tip_bottom(canvas_x + canvas_w * 0.5f, bottom);

                    // Build path
                    drawList->PathLineTo(ImVec2(left, top) + ImVec2(0.0f, rounding));
                    drawList->PathBezierCubicCurveTo(
                        ImVec2(left, top),
                        ImVec2(left, top),
                        ImVec2(left, top) + ImVec2(rounding, 0.0f));

                    drawList->PathLineTo(tip_top);
                    drawList->PathLineTo(tip_top + (tip_right - tip_top) * tip_round);
                    drawList->PathBezierCubicCurveTo(
                        tip_right,
                        tip_right,
                        tip_bottom + (tip_right - tip_bottom) * tip_round);

                    drawList->PathLineTo(tip_bottom);
                    drawList->PathLineTo(ImVec2(left, bottom) + ImVec2(rounding, 0.0f));
                    drawList->PathBezierCubicCurveTo(
                        ImVec2(left, bottom),
                        ImVec2(left, bottom),
                        ImVec2(left, bottom) - ImVec2(0.0f, rounding));

                    if (!filled)
                    {
                        if (innerColor & 0xFF000000u)
                            drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);

                        drawList->PathStroke(color, true, 2.0f * outline_scale);
                    }
                    else
                    {
                        drawList->PathFillConvex(color);
                    }
                }

                if (type == IconType::Circle)
                {
                    const ImVec2 c = rect_center;
                    if (!filled)
                    {
                        const float r = 0.5f * rect_w / 2.0f - 0.5f;
                        if (innerColor & 0xFF000000u)
                            drawList->AddCircleFilled(c, r, innerColor, 12 + extra_segments);
                        drawList->AddCircle(c, r, color, 12 + extra_segments, 2.0f * outline_scale);
                    }
                    else
                    {
                        drawList->AddCircleFilled(c, 0.5f * rect_w / 2.0f, color, 12 + extra_segments);
                    }
                }

                if (type == IconType::Square)
                {
                    if (filled)
                    {
                        const float r = 0.5f * rect_w / 2.0f;
                        const ImVec2 p0 = rect_center - ImVec2(r, r);
                        const ImVec2 p1 = rect_center + ImVec2(r, r);

#if IMGUI_VERSION_NUM > 18101
                        drawList->AddRectFilled(p0, p1, color, 0.0f, ImDrawFlags_RoundCornersAll);
#else
                        drawList->AddRectFilled(p0, p1, color, 0.0f, 15);
#endif
                    }
                    else
                    {
                        const float r = 0.5f * rect_w / 2.0f - 0.5f;
                        const ImVec2 p0 = rect_center - ImVec2(r, r);
                        const ImVec2 p1 = rect_center + ImVec2(r, r);

                        if (innerColor & 0xFF000000u)
                        {
#if IMGUI_VERSION_NUM > 18101
                            drawList->AddRectFilled(p0, p1, innerColor, 0.0f, ImDrawFlags_RoundCornersAll);
#else
                            drawList->AddRectFilled(p0, p1, innerColor, 0.0f, 15);
#endif
                        }

#if IMGUI_VERSION_NUM > 18101
                        drawList->AddRect(p0, p1, color, 0.0f, ImDrawFlags_RoundCornersAll, 2.0f * outline_scale);
#else
                        drawList->AddRect(p0, p1, color, 0.0f, 15, 2.0f * outline_scale);
#endif
                    }
                }

                if (type == IconType::Diamond)
                {
                    const float r = 0.607f * rect_w / 2.0f - 0.5f;
                    const ImVec2 c = rect_center;

                    drawList->PathLineTo(c + ImVec2(0.0f, -r));
                    drawList->PathLineTo(c + ImVec2(r, 0.0f));
                    drawList->PathLineTo(c + ImVec2(0.0f, r));
                    drawList->PathLineTo(c + ImVec2(-r, 0.0f));

                    if (innerColor & 0xFF000000u)
                        drawList->AddConvexPolyFilled(drawList->_Path.Data, drawList->_Path.Size, innerColor);
                    drawList->PathStroke(color, true, 2.0f * outline_scale);
                }
            }

        private:
            // non-instantiable
            DrawHelper() = delete;
            ~DrawHelper() = delete;
        };
    }
}

#endif // IMGUI_HELPER_HPP
