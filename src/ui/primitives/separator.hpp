#pragma once
#include "imgui.h"
#include "ui/primitives/layout.hpp"
#include "ui/theme.hpp"

namespace ui::p {

// Horizontal rule (1px, border color).
inline void separator() {
    ImGui::PushStyleColor(ImGuiCol_Separator, theme::c.border);
    ImGui::Separator();
    ImGui::PopStyleColor();
}

// Vertical rule for use between SameLine items.
inline void vseparator(float height = 16.f) {
    const ImVec2 p = ImGui::GetCursorScreenPos();
    ImGui::Dummy({1.f, height});
    ImGui::GetWindowDrawList()->AddLine(p, {p.x, p.y + height}, ImGui::GetColorU32(theme::c.border));
}

}  // namespace ui::p
