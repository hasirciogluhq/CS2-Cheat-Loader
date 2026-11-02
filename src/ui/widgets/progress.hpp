#pragma once
#include <algorithm>

#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Progress>; value in [0,1]. width <= 0 fills available.
inline void progress(float value, float width = 0.f, float height = 6.f) {
    using namespace theme;
    const float w = width > 0.f ? width : ImGui::GetContentRegionAvail().x;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy({w, height});
    auto* dl = ImGui::GetWindowDrawList();
    dl->AddRectFilled(pos, {pos.x + w, pos.y + height}, ImGui::GetColorU32(with_alpha(c.primary, 0.2f)), 999.f);
    const float fw = w * std::clamp(value, 0.f, 1.f);
    if (fw > 0.5f) dl->AddRectFilled(pos, {pos.x + fw, pos.y + height}, ImGui::GetColorU32(c.primary), 999.f);
}

}  // namespace ui::w
