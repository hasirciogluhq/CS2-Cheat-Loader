#pragma once
#include <cstring>

#include "imgui.h"
#include "ui/theme.hpp"

namespace ui::p {

inline void spacer(float h = 8.f) { ImGui::Dummy({0.f, h}); }
inline void gap_x(float w) { ImGui::Dummy({w, 0.f}); ImGui::SameLine(0.f, 0.f); }
inline void same_line(float gap = -1.f) { ImGui::SameLine(0.f, gap < 0.f ? theme::m.inline_gap : gap); }
inline float avail_width() { return ImGui::GetContentRegionAvail().x; }

// Width of one of `n` equal columns that fit in the available width.
inline float column_width(int n) {
    return (avail_width() - theme::m.inline_gap * static_cast<float>(n - 1)) / static_cast<float>(n);
}

// End of the visible part of an ImGui label ("Name##id" -> "Name").
inline const char* label_end(const char* label) {
    const char* e = std::strstr(label, "##");
    return e ? e : label + std::strlen(label);
}

}  // namespace ui::p
