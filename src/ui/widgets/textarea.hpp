#pragma once
#include <cstddef>

#include "imgui.h"
#include "ui/fonts.hpp"
#include "ui/theme.hpp"

namespace ui::w {

// shadcn <Textarea>. width <0 fills.
inline bool textarea(const char* id, char* buf, size_t size, float height = 80.f, float width = -1.f) {
    using namespace theme;
    ImGui::PushFont(fonts::regular, m.font_base);
    ImGui::PushStyleColor(ImGuiCol_FrameBg, with_alpha(c.input, 0.08f));
    ImGui::PushStyleColor(ImGuiCol_Border, c.input);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.f);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, m.radius);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, {m.space_3 - 2.f, m.space_2});
    const bool changed = ImGui::InputTextMultiline(id, buf, size, {width < 0.f ? -FLT_MIN : width, height});
    ImGui::PopStyleVar(3);
    ImGui::PopStyleColor(2);
    ImGui::PopFont();
    return changed;
}

template <size_t N>
bool textarea(const char* id, char (&buf)[N], float height = 80.f) { return textarea(id, buf, N, height); }

}  // namespace ui::w
