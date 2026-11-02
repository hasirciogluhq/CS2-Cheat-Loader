#pragma once
#include <initializer_list>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <ToggleGroup type="single"> (outlined segmented control).
inline bool toggle_group(const char* id, std::initializer_list<const char*> labels, int& selected) {
    using namespace theme;
    const float h = m.control_md, padx = m.space_4 - 2.f;
    ImGui::PushID(id);
    ImGui::PushFont(fonts::medium, m.font_sm + 0.5f);
    float total = 0.f;
    for (const char* l : labels) total += ImGui::CalcTextSize(l, p::label_end(l), true).x + padx * 2.f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    auto* dl = ImGui::GetWindowDrawList();
    bool changed = false;
    float x = pos.x;
    int i = 0, n = static_cast<int>(labels.size());
    for (const char* l : labels) {
        const float w = ImGui::CalcTextSize(l, p::label_end(l), true).x + padx * 2.f;
        ImGui::SetCursorScreenPos({x, pos.y});
        ImGui::PushID(i);
        if (ImGui::InvisibleButton("##t", {w, h}) && selected != i) { selected = i; changed = true; }
        const bool hov = ImGui::IsItemHovered();
        ImGui::PopID();
        ImDrawFlags rf = i == 0 ? ImDrawFlags_RoundCornersLeft : i == n - 1 ? ImDrawFlags_RoundCornersRight : ImDrawFlags_RoundCornersNone;
        if (selected == i || hov)
            dl->AddRectFilled({x, pos.y}, {x + w, pos.y + h}, ImGui::GetColorU32(selected == i ? c.accent : with_alpha(c.accent, 0.5f)), m.radius, rf);
        if (i > 0) dl->AddLine({x, pos.y}, {x, pos.y + h}, ImGui::GetColorU32(c.input));
        dl->AddText({x + padx, pos.y + (h - ImGui::GetFontSize()) * 0.5f},
                    ImGui::GetColorU32(selected == i ? c.accent_foreground : c.muted_foreground), l, p::label_end(l));
        x += w;
        ++i;
    }
    ImGui::PopFont();
    dl->AddRect(pos, {pos.x + total, pos.y + h}, ImGui::GetColorU32(c.input), m.radius);
    ImGui::SetCursorScreenPos(pos);
    ImGui::Dummy({total, h});
    ImGui::PopID();
    return changed;
}

// shadcn <Toggle>: a button that stays pressed.
inline bool toggle_button(const char* label, bool& pressed) {
    using namespace theme;
    ImGui::PushFont(fonts::medium, m.font_sm + 0.5f);
    const float w = ImGui::CalcTextSize(label, p::label_end(label), true).x + (m.space_4 - 2.f) * 2.f;
    ImGui::PushStyleColor(ImGuiCol_Button, pressed ? c.accent : ImVec4{0, 0, 0, 0});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, c.accent);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, c.accent);
    ImGui::PushStyleColor(ImGuiCol_Text, pressed ? c.accent_foreground : c.muted_foreground);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m.radius);
    const bool clicked = ImGui::Button(label, {w, m.control_md});
    if (clicked) pressed = !pressed;
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);
    ImGui::PopFont();
    return clicked;
}

}  // namespace ui::w
