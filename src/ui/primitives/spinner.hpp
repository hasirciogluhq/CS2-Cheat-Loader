#pragma once
#include <cmath>

#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::p {

// Indeterminate loading spinner (Loader2 in shadcn).
inline void spinner(float radius = 8.f, float thickness = 2.f) {
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy({radius * 2.f, radius * 2.f});
    const float t = static_cast<float>(ImGui::GetTime());
    const float a0 = t * 6.f;
    auto* dl = ImGui::GetWindowDrawList();
    dl->PathArcTo({pos.x + radius, pos.y + radius}, radius - thickness * 0.5f, a0, a0 + 4.2f, 24);
    dl->PathStroke(ImGui::GetColorU32(theme::c.foreground), 0, thickness);
}

}  // namespace ui::p
