#pragma once
#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Avatar> with initials fallback.
inline void avatar(const char* initials, float size = 32.f) {
    using namespace theme;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    ImGui::Dummy({size, size});
    auto* dl = ImGui::GetWindowDrawList();
    const ImVec2 ctr{pos.x + size * 0.5f, pos.y + size * 0.5f};
    dl->AddCircleFilled(ctr, size * 0.5f, ImGui::GetColorU32(c.muted), 32);
    const float fs = size * 0.38f;
    ImGui::PushFont(fonts::medium, fs);
    const ImVec2 ts = ImGui::CalcTextSize(initials);
    dl->AddText(fonts::medium, fs, {ctr.x - ts.x * 0.5f, ctr.y - ts.y * 0.5f}, ImGui::GetColorU32(c.foreground), initials);
    ImGui::PopFont();
}

}  // namespace ui::w
