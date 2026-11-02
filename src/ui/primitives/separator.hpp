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

namespace ui::p {

// Divider with centered text, e.g. "OR".
inline void separator_label(const char* text) {
    const float w = ImGui::GetContentRegionAvail().x;
    const float tw = ImGui::CalcTextSize(text).x + 16.f;
    const ImVec2 pos = ImGui::GetCursorScreenPos();
    const float h = ImGui::GetTextLineHeight();
    ImGui::Dummy({w, h});
    auto* dl = ImGui::GetWindowDrawList();
    const ImU32 col = ImGui::GetColorU32(theme::c.border);
    const float y = pos.y + h * 0.5f;
    dl->AddLine({pos.x, y}, {pos.x + (w - tw) * 0.5f, y}, col);
    dl->AddLine({pos.x + (w + tw) * 0.5f, y}, {pos.x + w, y}, col);
    dl->AddText({pos.x + (w - tw) * 0.5f + 8.f, pos.y}, ImGui::GetColorU32(theme::c.muted_foreground), text);
}

}  // namespace ui::p
