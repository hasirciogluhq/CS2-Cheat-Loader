#pragma once
#include <algorithm>
#include <cstdio>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"
#include "ui/widgets/button.hpp"

namespace ui::w {

// Stepper:  [-] 42 [+]. Returns true when the value changed.
inline bool input_number(const char* id, int& v, int lo, int hi, int step = 1, float width = 140.f) {
    using namespace theme;
    ImGui::PushID(id);
    const int old = v;
    const float bw = m.control_md;
    if (button("-", Variant::Outline, Size::Md, bw)) v = std::max(lo, v - step);
    ImGui::SameLine(0.f, m.space_1);
    const float mid = std::max(width - 2.f * bw - 2.f * m.space_1, 24.f);
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy({mid, m.control_md});
    char txt[32];
    std::snprintf(txt, sizeof txt, "%d", v);
    ImGui::PushFont(fonts::medium, m.font_base);
    const ImVec2 ts = ImGui::CalcTextSize(txt);
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRect(pos, {pos.x + mid, pos.y + m.control_md}, ImGui::GetColorU32(c.input), m.radius);
    dl->AddText(fonts::medium, m.font_base, {pos.x + (mid - ts.x) * 0.5f, pos.y + (m.control_md - ts.y) * 0.5f}, ImGui::GetColorU32(c.foreground), txt);
    ImGui::PopFont();
    ImGui::SameLine(0.f, m.space_1);
    if (button("+", Variant::Outline, Size::Md, bw)) v = std::min(hi, v + step);
    ImGui::PopID();
    return v != old;
}

}  // namespace ui::w
