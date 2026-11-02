#pragma once
#include <initializer_list>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <RadioGroup> (vertical). Returns true when selection changed.
inline bool radio_group(const char* id, int& selected, std::initializer_list<const char*> labels) {
    using namespace theme;
    bool changed = false;
    ImGui::PushID(id);
    int i = 0;
    for (const char* l : labels) {
        ImGui::PushID(i);
        constexpr float d = 16.f;
        const float h = m.control_sm - 2.f;
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        ImGui::PushFont(fonts::regular, m.font_base);
        const float tw = ImGui::CalcTextSize(l, p::label_end(l), true).x;
        ImGui::InvisibleButton("##r", {d + m.space_2 + tw, h});
        if (ImGui::IsItemClicked() && selected != i) { selected = i; changed = true; }
        const bool hov = ImGui::IsItemHovered(), on = selected == i;
        auto* dl = ImGui::GetWindowDrawList();
        const ImVec2 ctr{pos.x + d * 0.5f, pos.y + h * 0.5f};
        dl->AddCircleFilled(ctr, d * 0.5f, ImGui::GetColorU32(with_alpha(c.input, hov ? 0.16f : 0.08f)), 24);
        dl->AddCircle(ctr, d * 0.5f - 0.5f, ImGui::GetColorU32(on ? c.primary : c.input), 24, 1.f);
        if (on) dl->AddCircleFilled(ctr, 4.f, ImGui::GetColorU32(c.primary), 16);
        dl->AddText({pos.x + d + m.space_2, pos.y + (h - ImGui::GetFontSize()) * 0.5f}, ImGui::GetColorU32(c.foreground), l, p::label_end(l));
        ImGui::PopFont();
        ImGui::PopID();
        ++i;
    }
    ImGui::PopID();
    return changed;
}

}  // namespace ui::w
