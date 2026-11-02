#pragma once
#include <algorithm>
#include <cstdio>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Slider>. Fills available width; shows the value on the right when `show_value`.
inline bool slider(const char* id, float& v, float lo, float hi, const char* fmt = "%.0f", bool show_value = true) {
    using namespace theme;
    constexpr float r = 7.f, track_h = 4.f, h = 18.f;
    char txt[32] = {};
    float vw = 0.f;
    if (show_value) {
        std::snprintf(txt, sizeof txt, fmt, v);
        ImGui::PushFont(fonts::regular, m.font_sm);
        vw = ImGui::CalcTextSize(txt).x + m.space_3;
        ImGui::PopFont();
    }
    const float w = std::max(ImGui::GetContentRegionAvail().x - vw, 40.f);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::InvisibleButton(id, {w, h});
    bool changed = false;
    if (ImGui::IsItemActive()) {
        const float t = std::clamp((ImGui::GetIO().MousePos.x - pos.x - r) / (w - 2.f * r), 0.f, 1.f);
        const float nv = lo + t * (hi - lo);
        changed = nv != v;
        v = nv;
    }
    const float t = std::clamp((v - lo) / (hi - lo), 0.f, 1.f);
    const float x0 = pos.x + r, x1 = pos.x + w - r, cx = x0 + t * (x1 - x0), cy = pos.y + h * 0.5f;
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled({pos.x, cy - track_h * 0.5f}, {pos.x + w, cy + track_h * 0.5f}, ImGui::GetColorU32(c.muted), 999.f);
    dl->AddRectFilled({pos.x, cy - track_h * 0.5f}, {cx, cy + track_h * 0.5f}, ImGui::GetColorU32(c.primary), 999.f);
    dl->AddCircleFilled({cx, cy}, r, ImGui::GetColorU32({0.09f, 0.09f, 0.11f, 1.f}), 24);
    dl->AddCircle({cx, cy}, r - 0.5f, ImGui::GetColorU32(ImGui::IsItemActive() || ImGui::IsItemHovered() ? c.ring : c.primary), 24, 1.5f);
    if (show_value) {
        ImGui::SameLine(0.f, m.space_3);
        ImGui::PushFont(fonts::regular, m.font_sm);
        ImGui::PushStyleColor(ImGuiCol_Text, c.muted_foreground);
        ImGui::TextUnformatted(txt);
        ImGui::PopStyleColor();
        ImGui::PopFont();
    }
    return changed;
}

}  // namespace ui::w
