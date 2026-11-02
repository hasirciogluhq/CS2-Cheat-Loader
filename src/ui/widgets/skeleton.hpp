#pragma once
#include <cmath>

#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Skeleton> placeholder (pulsing). width <=0 fills.
inline void skeleton(float width, float height, float rounding = -1.f) {
    using namespace theme;
    const float w = width > 0.f ? width : ImGui::GetContentRegionAvail().x;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy({w, height});
    const float pulse = 0.5f + 0.5f * std::sin(static_cast<float>(ImGui::GetTime()) * 3.f);
    ImGui::GetWindowDrawList()->AddRectFilled(pos, {pos.x + w, pos.y + height},
                                              ImGui::GetColorU32(mul_alpha(c.muted, 0.5f + 0.5f * pulse)),
                                              rounding < 0.f ? m.radius : rounding);
}

}  // namespace ui::w
